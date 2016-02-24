#define MY_LOG_LEVEL 3

#include <gtk/gtk.h>
#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
#include "my_spline_cyc.h"

#define MY_SPLINE_CYC_GET_PRIVATE(o)		(G_TYPE_INSTANCE_GET_PRIVATE ((o), MY_TYPE_SPLINE_CYC, MySplineCycPrivate))

G_DEFINE_TYPE (MySplineCyc, my_spline_cyc, MY_TYPE_CURVE);
//G_DEFINE_TYPE_WITH_PRIVATE (MySplineCyc, my_spline_cyc, G_TYPE_OBJECT)
 
typedef struct _MySplineCycPrivate {
	GQueue* 	splinePoints;		// 插值的点
	int			segmentCountPerHemite;
} MySplineCycPrivate;

/******************************************************************************
 * declare static function 
 ******************************************************************************/
static void 		my_spline_cyc_init 		(MySplineCyc* self);
static void 		my_spline_cyc_finalize 	(GObject* object);
static void 		my_spline_cyc_dispose 	(GObject* object);
static gboolean		is_chosen		(MyShape *shape, gdouble x, gdouble y, gdouble strokeWidth, ApplicationState *appState);
G_SUPER_METHOD (void, calculate_just_size, (MyShape* self));
static void			calculate_just_size	(MyShape *self);
G_SUPER_METHOD (void, shift, (MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData));
static void 		shift				(MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData);
G_SUPER_METHOD (void, init_control_shapes, (MyShape* self, MyLayer *layer));
static void 		init_control_shapes	(MyShape* self, MyLayer *layer);
static void			my_spline_cyc_draw_self	(MyShape* self, ApplicationState *appState);
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
static void my_spline_cyc_class_init (MySplineCycClass* klass)
{
	GObjectClass	*g_object_class;
	MyShapeClass	*myShapeClass;

	/* Add private structure */
	g_type_class_add_private (klass, sizeof (MySplineCycPrivate));
	/* Get the parent gobject class */
	g_object_class = G_OBJECT_CLASS(klass);
	
    /* Hook overridable methods */
	myShapeClass = MY_SHAPE_CLASS(klass);
	myShapeClass->is_chosen = is_chosen;
	G_OVERRIDE_METHOD (myShapeClass, calculate_just_size, calculate_just_size);
	G_OVERRIDE_METHOD (myShapeClass, shift, shift);
	myShapeClass->draw_self = draw_self;
	myShapeClass->draw_self_shadow = draw_self_shadow;
	myShapeClass->draw_self_shadow_debug = draw_self_shadow_debug;
	G_OVERRIDE_METHOD (myShapeClass, update_with_scale, update_with_scale);
	G_OVERRIDE_METHOD (myShapeClass, init_control_shapes, init_control_shapes);
	myShapeClass->double_click_callback	=	double_click_callback;
	myShapeClass->fresh_dirty				=	fresh_dirty;
	myShapeClass->from_prototype_line		=	from_prototype_line;
	myShapeClass->to_prototype_line			=	to_prototype_line;
	
	/* Hook finalization functions */
	g_object_class->dispose = my_spline_cyc_dispose; /* instance destructor, reverse of init */
	g_object_class->finalize = my_spline_cyc_finalize; /* class finalization, reverse of class init */
}

// constructor
static void		my_spline_cyc_init (MySplineCyc* self)
{
	
	MySplineCycPrivate* priv = MY_SPLINE_CYC_GET_PRIVATE (self);

	/* Initialize public fields */
	MY_SHAPE (self)->needMarkDirtyWhenShift	=	TRUE;
 
	/* Initialize private fields */
	priv->splinePoints = g_queue_new();
	priv->segmentCountPerHemite = 20;
}

static void my_spline_cyc_finalize (GObject* object)
{  
  /* Always chain up to the parent class; as with dispose(), finalize()
   * is guaranteed to exist on the parent's class virtual function table
   */
	G_OBJECT_CLASS (my_spline_cyc_parent_class)->finalize (object);
}


static void my_spline_cyc_dispose (GObject* object)
{
	/* Reverse what was allocated by instance init */
	MySplineCyc *self = MY_SPLINE_CYC (object);
	MySplineCycPrivate* priv = MY_SPLINE_CYC_GET_PRIVATE (self);
	g_queue_foreach (priv->splinePoints, (GFunc) g_free, NULL);
	g_queue_free (priv->splinePoints);
 
  /* Always chain up to the parent class; there is no need to check if
   * the parent class implements the dispose() virtual function: it is
   * always guaranteed to do so
   */
	G_OBJECT_CLASS (my_spline_cyc_parent_class)->dispose (object);
}


static gboolean		is_chosen		(MyShape *shape, gdouble x, gdouble y, gdouble extraChosenPadding, ApplicationState *appState) {


	gboolean isInRect;
	MySplineCycPrivate* priv = MY_SPLINE_CYC_GET_PRIVATE (shape);
	gdouble radius = shape->strokeWidth + extraChosenPadding;
	isInRect = my_util_is_point_in_path (priv->splinePoints, x, y, radius);
	return isInRect;
}


static void			shift		(MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {

	
	G_SUPER (shift, (self, source, dx, dy, mouse, keyMask, userData));
	my_shape_fresh_dirty (self);
	my_shape_calculate_just_size (self);
	my_shape_shadow_mark_dirty (self);
}


static void		calculate_just_size		(MyShape *self) {
	

	MySplineCycPrivate* priv = MY_SPLINE_CYC_GET_PRIVATE (self);
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
	MySplineCycPrivate* priv = MY_SPLINE_CYC_GET_PRIVATE (self);

	G_SUPER (init_control_shapes, (self, layer));
	
}


static int			solve_dPt_n_4		(double *p){


	integer n = 2;
	integer nrhs = 1;
	double dl[1] = {3};
	double d[2] = {15,15};
	double du[1] = {3};
	double b[2];
	integer ldb = n;
	integer info;

	b[0] = 12 * (p[2] - p[0]) - 3 * (p[1] - p[2]);
	b[1] = 12 * (p[3] - p[1]) - 3 * (p[1] - p[2]);

	dgtsv_(&n, &nrhs, dl, d, du, b, &ldb, &info);

	p[0] = -0.25 * (b[0] + b[1]) + 0.75 * (b[0] - b[1]);
	p[1] = b[0];
	p[2] = b[1];
	p[3] = p[0];

	return info;
}


// 列主序方阵
#define posColumnLead(a,n,x,y) a[(x)*(n)+y]
/**
 * 解线性方程组,求出点P1 .. Pn的一阶导数
 * in out	p		输入为点P1 .. Pn,存储p[0]到p[np-1]。输出P1 .. Pn点的导数,存储为p[0]到p[np-1]
 * in		np		点的个数, np must > 5
 */
static int			solve_dPt_cyclic		(double *p, int np){
	
	integer n = np - 2;
	integer nrhs = 1;
	double *a;
	integer lda = n;
	integer *ipiv;
	double *b;
	integer ldb = n;
	integer info;
	int i;
	
	ipiv = (integer *) g_malloc (n * sizeof (integer));
	a = (double *) g_malloc (n * n * sizeof (double));
	memset (a, 0, n*n*sizeof(double));
	posColumnLead (a, n, 0, 0) = 15;
	posColumnLead (a, n, 1, 0) = 4;
	posColumnLead (a, n, n - 1, 0) = -1;
	posColumnLead (a, n, 0, n - 1) = -1;
	posColumnLead (a, n, n - 2, n - 1) = 4;
	posColumnLead (a, n, n - 1, n - 1) = 15;
	for (i = 0; i < n - 2; i++) {
		posColumnLead (a, n, i,     i + 1) = 1;
		posColumnLead (a, n, i + 1, i + 1) = 4;
		posColumnLead (a, n, i + 2, i + 1) = 1;
	}
	
	b = (double *) g_malloc ((np-2) * sizeof (double));
	b[0] = 12 * (p[2] - p[0]) - 3 * (p[1] - p[np-2]);
	b[n-1] = 12 * (p[np-1] - p[np-3]) - 3 * (p[1] - p[np-2]);
	for (i = 1; i <= np-4; i++) {
		b[i] = 3 * (p[i+2] - p[i]);
	}

	dgesv_ (&n, &nrhs, a, &lda, ipiv, b, &ldb, &info);
	
	p[0] = -0.25 * (b[0] + b[n-1]) + 0.75 * (p[1] - p[np-2]);
	p[np-1] = p[0];
	for (i = 0; i < np-2; i++) {
		p[i+1] = b[i];
	}

	g_free (a);
	g_free (ipiv);
	g_free (b);
	return info;
}

#define calculateP(ttt,tt,t,p1,dp1,p2,dp2)	((2 * p1 - 2 * p2 + dp1 + dp2) * ttt + (-3 * p1 + 3 * p2 - 2 * dp1 - dp2) * tt + dp1 * t + p1)

static void		append_hermite_points	(MyShape* self, double x1, double y1, double dx1, double dy1,
										double x2, double y2, double dx2, double dy2) {

											
	MySplineCycPrivate* priv = MY_SPLINE_CYC_GET_PRIVATE (self);
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


static void		my_spline_cyc_calculate_spline_points	(MySplineCyc *splineCyc) {

	
	MyShape *self = MY_SHAPE (splineCyc);
	MySplineCycPrivate* priv = MY_SPLINE_CYC_GET_PRIVATE (splineCyc);
	MyPoint *point, *point2, *point3, *point4;
	guint pointCount = my_curve_point_count (MY_CURVE (self));
	gdouble *x = (gdouble*)g_malloc(pointCount * sizeof(gdouble)); // use to store directive of point
	gdouble *y = (gdouble*)g_malloc(pointCount * sizeof(gdouble));
	guint i;

	if (pointCount <= 2) {
		return;
	}

	// 计算导数
	if (pointCount == 3) {
		
		point = (MyPoint *) my_curve_peek_nth (MY_CURVE (self), 0);
		point2 = (MyPoint *) my_curve_peek_nth (MY_CURVE (self), 2);
		x[0] = 0;
		y[0] = 0;
		x[1] = 0.75 * (point2->x - point->x);
		y[1] = 0.75 * (point2->y - point->y);
		x[2] = 0;
		y[2] = 0;

	} else if (pointCount == 4) {

		for (i = 0; i < pointCount; i++) {
			point = (MyPoint *) my_curve_peek_nth (MY_CURVE (self), i);
			x[i] = point->x;
			y[i] = point->y;
		}		
		solve_dPt_n_4 (x);
		solve_dPt_n_4 (y);

	} else {
		for (i = 0; i < pointCount; i++) {
			point = (MyPoint *) my_curve_peek_nth (MY_CURVE (self), i);
			x[i] = point->x;
			y[i] = point->y;
		}
		solve_dPt_cyclic (x, pointCount);
		solve_dPt_cyclic (y, pointCount);
	}

	// 对每段线段插入hermit点
	g_queue_foreach (priv->splinePoints, (GFunc) g_free, NULL);
	g_queue_clear (priv->splinePoints);
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
	point = (MyPoint *) my_curve_peek_nth (MY_CURVE (self), pointCount - 1);
	point2 = (MyPoint *) my_curve_peek_nth (MY_CURVE (self), 0);
	append_hermite_points (self, 
						point->x,
						point->y,
						x[pointCount - 1],
						y[pointCount - 1],
						point2->x,
						point2->y,
						x[0],
						y[0]);

	g_free (x);
	g_free (y);
}


static void		draw_self		(MyShape* self, ApplicationState *appState) {

	
	MySplineCycPrivate* priv = MY_SPLINE_CYC_GET_PRIVATE (self);
	MySplineCyc *splineCyc = MY_SPLINE_CYC (self);
	MyPoint *point, *point2;
	cairo_t *cr = appState->cr;
	guint pointCount = my_curve_point_count (MY_CURVE (self));
	guint i;
	
		my_debug ("draw spline_cyc %#x...", self);
	if (pointCount <= 1) {	// do nothing

	} else if (pointCount == 2) {	// only draw straight line
		
		point = (MyPoint *) my_curve_peek_nth (MY_CURVE (self), 0);
		point2 = (MyPoint *) my_curve_peek_nth (MY_CURVE (self), 1);
		cairo_line_to (cr, point->x + appState->orignX,
							point->y + appState->orignY);
		cairo_line_to (cr, point2->x + appState->orignX,
							point2->y + appState->orignY);		
		cairo_stroke (cr);

	} else {	// really cyclic cubic spline 
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
	my_debug ("draw spline_cyc %#x done.", self);
}


static void			draw_self_shadow	(MyShape* self, ApplicationState *appState) {

	
	MySplineCycPrivate* priv = MY_SPLINE_CYC_GET_PRIVATE (self);
	MySplineCyc *splineCyc = MY_SPLINE_CYC (self);
	MyPoint *point, *point2;
	gdouble scale = appState->scale;
	cairo_t *cr = appState->cr;
	guint pointCount = my_curve_point_count (MY_CURVE (self));
	guint i;
	
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

	} else {	// really cyclic cubic spline 
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
	
	
	MySplineCycPrivate* priv = MY_SPLINE_CYC_GET_PRIVATE (self);
	MySplineCyc *splineCyc = MY_SPLINE_CYC (self);
	MyPoint *point, *point2;
	ApplicationState *appState = self->appState;
	gdouble scale = appState->scale;
	cairo_t *cr = appState->cr;
	guint pointCount = my_curve_point_count (MY_CURVE (self));
	guint i;
	
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

	} else {	// really cyclic cubic spline 
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

	
	MySplineCycPrivate* priv = MY_SPLINE_CYC_GET_PRIVATE (self);
	MySplineCyc *splineCyc = MY_SPLINE_CYC (self);
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


	my_spline_cyc_calculate_spline_points (MY_SPLINE_CYC (self));
}


static void		from_prototype_line		(MyShape *self, MyPrototypeLine *prototype) {

	
	MySplineCycPrivate* priv = MY_SPLINE_CYC_GET_PRIVATE (self);
	priv->segmentCountPerHemite	=	prototype->segmentCountPerHemite;
}


static void		to_prototype_line		(MyShape *self, MyPrototypeLine *prototype) {	
	
	
	MySplineCycPrivate* priv = MY_SPLINE_CYC_GET_PRIVATE (self);
	prototype->segmentCountPerHemite	=	priv->segmentCountPerHemite;
	prototype->style = MY_PROTOTYPE_STYLE_SPLINE_CYC;
}