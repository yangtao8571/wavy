#define MY_LOG_LEVEL 3

#include <gtk/gtk.h>
#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <f2c.h>
#include <clapack.h>
#include "GSuperMacro.h"
#include "my_constant.h"
#include "my_point.h"
#include "my_rectangle.h"
#include "my_util.h"
#include "my_prototype_line.h"
#include "my_prototype_manager.h"
#include "my_application_state.h"
#include "my_debug.h"
#include "my_vector.h"
#include "my_shape.h"
#include "my_layer.h"
#include "my_curve.h"
#include "my_control_shape_rect.h"
#include "my_spline.h"

#define MY_SPLINE_GET_PRIVATE(o)		(G_TYPE_INSTANCE_GET_PRIVATE ((o), MY_TYPE_SPLINE, MySplinePrivate))

G_DEFINE_TYPE (MySpline, my_spline, MY_TYPE_CURVE);
//G_DEFINE_TYPE_WITH_PRIVATE (MySpline, my_spline, G_TYPE_OBJECT)
 
typedef struct _MySplinePrivate {
	gboolean	isPointsCompleted;	// 用户给定的点全了 todo: delete
	GQueue* 	splinePoints;		// 插值的点
	MyControlShapeRect*	controlP0;			// 两个控制点，非数据点
	MyControlShapeRect*	controlP1;			// 两个控制点，非数据点
	double		dPt1_x;				// 第1个点的导数
	double		dPt1_y;				// 第1个点的导数
	double		dPtn_x;				// 第n个点的导数
	double		dPtn_y;				// 第n个点的导数
	int			segmentCountPerHemite;
} MySplinePrivate;

/******************************************************************************
 * declare static function 
 ******************************************************************************/
static void 		my_spline_init 		(MySpline* self);
static void 		my_spline_finalize 	(GObject* object);
static void 		my_spline_dispose 	(GObject* object);
static gboolean		is_chosen			(MyShape *shape, gdouble x, gdouble y, gdouble strokeWidth, ApplicationState *appState);
G_SUPER_METHOD (void, calculate_just_size, (MyShape* self));
static void			calculate_just_size	(MyShape *self);
G_SUPER_METHOD (void, shift, (MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData));
static void 		shift				(MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData);
G_SUPER_METHOD (void, init_control_shapes, (MyShape* self, MyLayer *layer));
static void 		init_control_shapes	(MyShape* self, MyLayer *layer);
static void			my_spline_draw_self	(MyShape* self, ApplicationState *appState);
static void			draw_self				(MyShape* self, ApplicationState *appState);
static void			draw_self_shadow		(MyShape* self, ApplicationState *appState);
static void			draw_self_shadow_debug	(MyShape* self);
G_SUPER_METHOD (void, update_with_scale, (MyShape* self, gdouble scale));
static void			update_with_scale		(MyShape *self, gdouble scale);
static gboolean		double_click_callback	(MyShape *self, gdouble x, gdouble y, MyMouse mouse, guint keyMask, gpointer userData);
static void			fresh_dirty				(MyShape *self);
static void			from_prototype_line		(MyShape *self, MyPrototypeLine *prototype);
static void			to_prototype_line		(MyShape *self, MyPrototypeLine *prototype);
/******************************************************************************
 * implement function 
 ******************************************************************************/
static void my_spline_class_init (MySplineClass* klass)
{
	GObjectClass	*g_object_class;
	MyShapeClass	*myShapeClass;

	/* Add private structure */
	g_type_class_add_private (klass, sizeof (MySplinePrivate));
	/* Get the parent gobject class */
	g_object_class = G_OBJECT_CLASS(klass);
	
    /* Hook overridable methods */
	myShapeClass = MY_SHAPE_CLASS(klass);
	myShapeClass->is_chosen					= is_chosen;
	G_OVERRIDE_METHOD (myShapeClass, calculate_just_size, calculate_just_size);
	G_OVERRIDE_METHOD (myShapeClass, shift, shift);
	myShapeClass->draw_self					= draw_self;
	myShapeClass->draw_self_shadow			= draw_self_shadow;
	myShapeClass->draw_self_shadow_debug	= draw_self_shadow_debug;
	G_OVERRIDE_METHOD (myShapeClass, update_with_scale, update_with_scale);
	G_OVERRIDE_METHOD (myShapeClass, init_control_shapes, init_control_shapes);
	myShapeClass->double_click_callback		=	double_click_callback;
	myShapeClass->fresh_dirty				=	fresh_dirty;
	myShapeClass->from_prototype_line		=	from_prototype_line;
	myShapeClass->to_prototype_line			=	to_prototype_line;
	
	/* Hook finalization functions */
	g_object_class->dispose = my_spline_dispose; /* instance destructor, reverse of init */
	g_object_class->finalize = my_spline_finalize; /* class finalization, reverse of class init */
}

// constructor
static void		my_spline_init (MySpline* self)
{
	MySplinePrivate* priv = MY_SPLINE_GET_PRIVATE (self);

	/* Initialize public fields */
	MY_SHAPE (self)->needMarkDirtyWhenShift	=	TRUE;
 
	/* Initialize private fields */
	priv->segmentCountPerHemite	=	20;
	priv->splinePoints = g_queue_new();
}

static void my_spline_finalize (GObject* object)
{  
  /* Always chain up to the parent class; as with dispose(), finalize()
   * is guaranteed to exist on the parent's class virtual function table
   */
	G_OBJECT_CLASS (my_spline_parent_class)->finalize (object);
}


static void my_spline_dispose (GObject* object)
{
	/* Reverse what was allocated by instance init */
	MySpline *self = MY_SPLINE (object);
	MySplinePrivate* priv = MY_SPLINE_GET_PRIVATE (self);
	g_queue_foreach (priv->splinePoints, (GFunc) g_free, NULL);
	g_queue_free (priv->splinePoints);
 
  /* Always chain up to the parent class; there is no need to check if
   * the parent class implements the dispose() virtual function: it is
   * always guaranteed to do so
   */
	G_OBJECT_CLASS (my_spline_parent_class)->dispose (object);
}


static gboolean		is_chosen		(MyShape *shape, gdouble x, gdouble y, gdouble extraChosenPadding, ApplicationState *appState) {


	gboolean isInRect;
	MySplinePrivate* priv = MY_SPLINE_GET_PRIVATE (shape);
	gdouble radius = shape->strokeWidth + extraChosenPadding;
	isInRect = my_util_is_point_in_path (priv->splinePoints, x, y, radius);
	return isInRect;
}


guint		my_spline_get_points_count	(MySpline *self) {

	
	MySplinePrivate* priv = MY_SPLINE_GET_PRIVATE (self);
	guint c = g_queue_get_length (priv->splinePoints);
	return c;
}


MyPoint	*	my_spline_peek_nth			(MySpline *self, guint i) {
	

	MySplinePrivate* priv = MY_SPLINE_GET_PRIVATE (self);	
	MyPoint *point = (MyPoint *) g_queue_peek_nth (priv->splinePoints, i);
	return point;
}


static void			shift		(MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {

	
	G_SUPER (shift, (self, source, dx, dy, mouse, keyMask, userData));
	my_shape_fresh_dirty (self);
	my_shape_calculate_just_size (self);
	my_shape_shadow_mark_dirty (self);
}


static void		calculate_just_size		(MyShape *self) {
	

	MySplinePrivate* priv = MY_SPLINE_GET_PRIVATE (self);
	guint i, pointCount;
	MyPoint *point;
	pointCount = g_queue_get_length (priv->splinePoints);

	if (my_curve_point_count (MY_CURVE (self)) < 3) {
		G_SUPER (calculate_just_size, (self));
	} else {
		for (i = 0; i < pointCount; i++) {
			point = (MyPoint *) g_queue_peek_nth (priv->splinePoints, i);
			my_shape_expand_just_size (self, point->x, point->y);
		}
	}
}


static void 		init_control_shapes		(MyShape* self, MyLayer *layer) {


	MyCurve *curve = MY_CURVE(self);
	MyPoint *point;
	guint len;
	MySplinePrivate* priv = MY_SPLINE_GET_PRIVATE (self);

	G_SUPER (init_control_shapes, (self, layer));
		
	/*
	len = my_curve_point_count (curve);
	if (len >= 2) {
		
		if (priv->controlP0) {	// 已有控制图形, 显示他们	
			if (MY_SHAPE (priv->controlP0)->isShowable) {
				MY_SHAPE (priv->controlP0)->isShowing = TRUE;
				MY_SHAPE (priv->controlP1)->isShowing = TRUE;
			}		
		} else {	// 没有, 新建控制图形
			point = my_curve_peek_nth (curve, 0);
			priv->controlP0 = (MyControlShapeRect *) g_object_new (MY_TYPE_CONTROL_SHAPE_RECT, NULL);
			MY_SHAPE (priv->controlP0)->appState = self->appState;
			priv->controlP0->x = point->x - 4;
			priv->controlP0->y = point->y - 4;
			priv->controlP0->target = self;		
			my_layer_add_head_shape (layer, MY_SHAPE (priv->controlP0));
			
			point = my_curve_peek_nth (curve, len - 1);
			priv->controlP1 = (MyControlShapeRect *) g_object_new (MY_TYPE_CONTROL_SHAPE_RECT, NULL);
			MY_SHAPE (priv->controlP1)->appState = self->appState;
			priv->controlP1->x = point->x - 4;
			priv->controlP1->y = point->y - 4;
			priv->controlP1->target = self;			
			my_layer_add_head_shape (layer, MY_SHAPE (priv->controlP1));
		}
	}	
	*/
}


/**
 * 解线性方程组,求出点P2 .. Pn-1的一阶导数
 * in out	p		输入为点P1 .. Pn,存储p[0]到p[np-1]。输出P2 .. Pn-1点的导数,存储为p[1]到p[np-2]
 * in		np		点的个数, np must > 2
 * in		dPt1	第1个点的导数
 * in		dPtn	第n个点的导数
 */
static int			solve_dPt		(double *p, int np, double dPt1, double dPtn){


	integer n = np - 2;
	integer nrhs = 1;
	double *dl = (double *) g_malloc ((n-1) * sizeof (double));
	double *d = (double *) g_malloc (n * sizeof (double));
	double *du = (double *) g_malloc ((n-1) * sizeof (double));
	double *b = (double *) g_malloc ((np-2) * sizeof (double));
	integer ldb = n;
	integer info;
	int i;

	for (i = 0; i < n-1; i++) {
		dl[i] = 1;
		d[i] = 4;
		du[i] = 1;
	}
	d[i] = 4;

	for (i = 0; i < np-2; i++) {
		b[i] = 3 * (p[i+2] - p[i]);
	}
	b[0] -= dPt1;
	b[np-3] -= dPtn;

	dgtsv_(&n, &nrhs, dl, d, du, b, &ldb, &info);

	for (i = 0; i < np-2; i++) {
		p[i+1] = b[i];
	}

	g_free (dl);
	g_free (d);
	g_free (du);
	g_free (b);
	return info;
}

#define calculateP(ttt,tt,t,p1,dp1,p2,dp2)	((2 * p1 - 2 * p2 + dp1 + dp2) * ttt + (-3 * p1 + 3 * p2 - 2 * dp1 - dp2) * tt + dp1 * t + p1)

static void		append_hermite_points	(MyShape* self, double x1, double y1, double dx1, double dy1,
										double x2, double y2, double dx2, double dy2) {


	MySplinePrivate* priv = MY_SPLINE_GET_PRIVATE (self);
	int segmentCountPerHemite = priv->segmentCountPerHemite;
	double T = 1. / segmentCountPerHemite;
	double t = 0.;
	double tt;
	double ttt;
	int i;
	double x;
	double y;
	MyPoint *point;

	for (i = 0; i <= segmentCountPerHemite; i++, t += T) {
		tt = t * t;
		ttt = tt * t;
		x = calculateP(ttt,tt,t,x1,dx1,x2,dx2);
		y = calculateP(ttt,tt,t,y1,dy1,y2,dy2);
		point = (MyPoint *) g_malloc (sizeof (MyPoint));
		point->x = x;
		point->y = y;
		g_queue_push_tail (priv->splinePoints, point);
	}
}


static void		my_spline_calculate_spline_points	(MySpline *spline) {


	MyShape *self = MY_SHAPE (spline);
	MySplinePrivate* priv = MY_SPLINE_GET_PRIVATE (spline);
	MyPoint *point, *point2;
	guint pointCount = my_curve_point_count (MY_CURVE (spline));
	gdouble *x; // use to store directive of point
	gdouble *y;
	guint i;
	
	g_queue_foreach (priv->splinePoints, (GFunc) g_free, NULL);	// todo: cache or something, dont recalculate points when not dirty
	g_queue_clear (priv->splinePoints);	

	if (pointCount < 3) {	// 小于3个点时，直接拷贝
		for (i = 0; i < pointCount; i++) {
			point = (MyPoint *) my_curve_peek_nth (MY_CURVE (self), i);
			point2 = (MyPoint *) g_malloc (sizeof (MyPoint));
			point2->x = point->x;
			point2->y = point->y;
			g_queue_push_tail (priv->splinePoints, point2);
		}
	} else {	// 计算所有点
		x = (gdouble*)g_malloc(pointCount * sizeof(gdouble));
		y = (gdouble*)g_malloc(pointCount * sizeof(gdouble));
		for (i = 0; i < pointCount; i++) {
			point = (MyPoint *) my_curve_peek_nth (MY_CURVE (self), i);
			x[i] = point->x;
			y[i] = point->y;
		}

		if (priv->controlP0) {	// 用户指定初始导数
			priv->dPt1_x = 5 * (priv->controlP0->x - ((MyPoint *) my_curve_peek_nth (MY_CURVE (self), 0))->x);
			priv->dPt1_y = 5 * (priv->controlP0->y - ((MyPoint *) my_curve_peek_nth (MY_CURVE (self), 0))->y);
			priv->dPtn_x = 5 * (-priv->controlP1->x + ((MyPoint *) my_curve_peek_nth (MY_CURVE (self), my_curve_point_count (MY_CURVE (self)) - 1))->x);
			priv->dPtn_y = 5 * (-priv->controlP1->y + ((MyPoint *) my_curve_peek_nth (MY_CURVE (self), my_curve_point_count (MY_CURVE (self)) - 1))->y);
		}

		solve_dPt (x, pointCount, priv->dPt1_x, priv->dPtn_x);
		solve_dPt (y, pointCount, priv->dPt1_y, priv->dPtn_y);
		x[0] = priv->dPt1_x;
		y[0] = priv->dPt1_y;
		x[pointCount-1] = priv->dPtn_x;
		y[pointCount-1] = priv->dPtn_y;

		for (i = 0; i < pointCount - 1; i ++) {			
			point = (MyPoint *) my_curve_peek_nth (MY_CURVE (self), i);
			point2 = (MyPoint *) my_curve_peek_nth (MY_CURVE (self), i+1);
			append_hermite_points (self, 
								point->x,
								point->y,
								x[i],
								y[i],
								point2->x,
								point2->y,
								x[i+1],
								y[i+1]);
		}
		g_free(x);
		g_free(y);
	}
}


static void		draw_self		(MyShape* self, ApplicationState *appState) {

	
	MySplinePrivate* priv = MY_SPLINE_GET_PRIVATE (self);
	MySpline *spline = MY_SPLINE (self);
	MyPoint *point, *point2;
	cairo_t *cr = appState->cr;
	guint pointCount = my_curve_point_count (MY_CURVE (self));
	guint i;

	if (pointCount < 2) {	// do nothing

	} else if (pointCount == 2) {	// only draw straight line
		
		point = (MyPoint *) my_curve_peek_nth (MY_CURVE (self), 0);
		point2 = (MyPoint *) my_curve_peek_nth (MY_CURVE (self), 1);
		cairo_line_to (cr, point->x + appState->orignX,
							point->y + appState->orignY);
		cairo_line_to (cr, point2->x + appState->orignX,
							point2->y + appState->orignY);		
		cairo_stroke (cr);

	} else {	// really cubic spline
		// now draw it
		cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);
		pointCount = g_queue_get_length (priv->splinePoints);
		for (i = 0; i < pointCount; i++) {
			point = (MyPoint *) g_queue_peek_nth (priv->splinePoints, i);
			cairo_line_to (cr, point->x + appState->orignX, 
								point->y + appState->orignY);
		}
		cairo_stroke (cr);
	}
	my_debug ("draw spline %#x done.", self);
}


static void			draw_self_shadow	(MyShape* self, ApplicationState *appState) {

	
	MySplinePrivate* priv = MY_SPLINE_GET_PRIVATE (self);
	MySpline *spline = MY_SPLINE (self);
	MyPoint *point, *point2;
	cairo_t *cr = appState->cr;
	guint pointCount = my_curve_point_count (MY_CURVE (self));
	guint i;
	gdouble scale = appState->scale;
	
	if (pointCount < 2) {	// do nothing, but must draw something for cairo, or it will crash!!!
		
		point = (MyPoint *) my_curve_peek_nth (MY_CURVE (self), 0);
		cairo_line_to (cr, point->x / scale - self->shadowX,
							point->y / scale - self->shadowY);
		cairo_stroke (cr);

	} else if (pointCount == 2) {	// only draw straight line
		
		point = (MyPoint *) my_curve_peek_nth (MY_CURVE (self), 0);
		point2 = (MyPoint *) my_curve_peek_nth (MY_CURVE (self), 1);
		cairo_line_to (cr, point->x / scale - self->shadowX,
							point->y / scale - self->shadowY);
		cairo_line_to (cr, point2->x / scale - self->shadowX,
							point2->y / scale - self->shadowY);		
		cairo_stroke (cr);

	} else {	// really cubic spline
		// now draw it
		cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);
		pointCount = g_queue_get_length (priv->splinePoints);
		for (i = 0; i < pointCount; i++) {
			point = (MyPoint *) g_queue_peek_nth (priv->splinePoints, i);
			cairo_line_to (cr, point->x / scale - self->shadowX, 
								point->y / scale - self->shadowY);
		}
		cairo_stroke (cr);
	}
}


static void		draw_self_shadow_debug		(MyShape* self) {
	
	
	MySplinePrivate* priv = MY_SPLINE_GET_PRIVATE (self);
	MySpline *spline = MY_SPLINE (self);
	MyPoint *point, *point2;
	ApplicationState *appState = self->appState;
	cairo_t *cr = appState->cr;
	guint pointCount = my_curve_point_count (MY_CURVE (self));
	guint i;
	gdouble scale = appState->scale;
	
	if (pointCount < 2) {	// do nothing, but must draw something for cairo, or it will crash!!!
		
		point = (MyPoint *) my_curve_peek_nth (MY_CURVE (self), 0);
		cairo_line_to (cr, point->x / scale - self->shadowX,
							point->y / scale - self->shadowY);
		cairo_stroke (cr);

	} else if (pointCount == 2) {	// only draw straight line
		
		point = (MyPoint *) my_curve_peek_nth (MY_CURVE (self), 0);
		point2 = (MyPoint *) my_curve_peek_nth (MY_CURVE (self), 1);
		cairo_line_to (cr, point->x / scale + self->appState->orignX,
							point->y / scale + self->appState->orignY);
		cairo_line_to (cr, point2->x / scale + self->appState->orignX,
							point2->y / scale + self->appState->orignY);		
		cairo_stroke (cr);

	} else {	// really cubic spline
		// now draw it
		cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);
		pointCount = g_queue_get_length (priv->splinePoints);
		for (i = 0; i < pointCount; i++) {
			point = (MyPoint *) g_queue_peek_nth (priv->splinePoints, i);
			cairo_line_to (cr, point->x / scale + self->appState->orignX, 
								point->y / scale + self->appState->orignY);
		}
		cairo_stroke (cr);
	}
}


static void			update_with_scale		(MyShape *self, gdouble scale) {


	G_SUPER (update_with_scale, (self, scale));
	my_shape_fresh_dirty (self);
}


static gboolean			double_click_callback		(MyShape *self, gdouble x, gdouble y, MyMouse mouse, guint keyMask, gpointer userData) {

	
	MySplinePrivate* priv = MY_SPLINE_GET_PRIVATE (self);
	MySpline *spline = MY_SPLINE (self);
	MyControlShapeRect *controlRect = (MyControlShapeRect *) userData;
	MyPoint *newPoint;
	
	if (controlRect) {
		my_shape_delete_control_shapes_from_layer (self, self->appState->currentLayer);
		if (keyMask & GDK_CONTROL_MASK) {	// 删除点
			my_curve_delete_point (MY_CURVE (self), controlRect->pointTarget);
		} else {	// 添加点
			newPoint = (MyPoint *) g_malloc (sizeof (MyPoint));
			newPoint->x = controlRect->pointTarget->x + 20;
			newPoint->y = controlRect->pointTarget->y + 20;
			my_curve_insert_point_before (MY_CURVE (self), controlRect->pointTarget, newPoint);
		}
		my_shape_init_control_shapes (self, self->appState->currentLayer);

		my_shape_fresh_dirty (self);
		my_shape_calculate_just_size (self);
		my_shape_shadow_mark_dirty (self);
	}
}


void		fresh_dirty			(MyShape *self) {


	my_spline_calculate_spline_points (MY_SPLINE (self));
}


static void		from_prototype_line		(MyShape *self, MyPrototypeLine *prototype) {

	
	MySplinePrivate* priv = MY_SPLINE_GET_PRIVATE (self);
	priv->segmentCountPerHemite	=	prototype->segmentCountPerHemite;
	priv->dPt1_x	=	prototype->dPt1_x;
	priv->dPt1_y	=	prototype->dPt1_y;
	priv->dPtn_x	=	prototype->dPtn_x;
	priv->dPtn_y	=	prototype->dPtn_y;
}
	

static void		to_prototype_line		(MyShape *self, MyPrototypeLine *prototype) {	
	

	MySplinePrivate* priv = MY_SPLINE_GET_PRIVATE (self);
	prototype->style = MY_PROTOTYPE_STYLE_SPLINE;
	prototype->segmentCountPerHemite	=	priv->segmentCountPerHemite;
	prototype->dPt1_x	=	priv->dPt1_x;
	prototype->dPt1_y	=	priv->dPt1_y;
	prototype->dPtn_x	=	priv->dPtn_x;
	prototype->dPtn_y	=	priv->dPtn_y;
}