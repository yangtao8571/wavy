#define MY_LOG_LEVEL 3

#include <math.h>
#include <gtk/gtk.h>
#include <glib-object.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "my_constant.h"
#include "my_point.h"
#include "my_util.h"
#include "my_rectangle.h"
#include "my_prototype_line.h"
#include "my_prototype_manager.h"
#include "my_application_state.h"
#include "my_debug.h"
#include "my_shape.h"
#include "my_vector.h"
#include "my_control_shape_rect.h"
//#include "cairo_box_blur.h"
//#include "cairo_gaussian_blur.h"
#include "my_curve.h"

#define MY_CURVE_GET_PRIVATE(o)		(G_TYPE_INSTANCE_GET_PRIVATE ((o), MY_TYPE_CURVE, MyCurvePrivate))

G_DEFINE_TYPE (MyCurve, my_curve, MY_TYPE_SHAPE);
//G_DEFINE_TYPE_WITH_PRIVATE (MyCurve, my_curve, G_TYPE_OBJECT)
 
typedef struct _MyCurvePrivate {
	MyPoint*	startPoint;
	MyPoint*	endPoint;
	GQueue* 	points;
	GQueue*		controlShapes;
} MyCurvePrivate;

/******************************************************************************
 * declare static function 
 ******************************************************************************/
static void 		my_curve_init (MyCurve* self);
static void 		my_curve_finalize (GObject* object);
static void 		my_curve_dispose (GObject* object);
static void 		init_control_shapes	(MyShape* self, MyLayer *layer);
static void 		hide_control_shapes	(MyShape* self);
static void 		delete_control_shapes	(MyShape *self);
static void 		delete_control_shapes_from_layer	(MyShape *self, MyLayer *layer);
gboolean			has_control_shapes	(MyShape *self);
static void			calculate_just_size	(MyShape *self);
static GString	*	my_curve_to_string	(MyShape *self);
static gboolean		my_curve_is_chosen	(MyShape *self, gdouble x, gdouble y, gdouble strokeWidth, ApplicationState *appState);
static gboolean		my_curve_is_chosen_old	(MyShape *self, gdouble x, gdouble y, gdouble strokeWidth, ApplicationState *appState);
static void			my_curve_draw_self	(MyShape* self, ApplicationState *appState);
static void			my_curve_draw_self_shadow	(MyShape* self, ApplicationState *appState);
static void			draw_self_shadow_debug	(MyShape* self);
static void			shift				(MyShape *self, gdouble dx, gdouble dy);
static void			shift_self_only		(MyShape *self, gdouble dx, gdouble dy);
static gboolean		is_out_of_canvas	(MyShape *self, ApplicationState *appState);
static void			update_with_scale	(MyShape *self, gdouble scale);
static void			update_shadow_rect	(MyShape *self);
static void			on_shadow_dirty		(MyShape *self);
static void			from_prototype_line	(MyShape *self, MyPrototypeLine *prototype);
static void			to_prototype_line	(MyShape *self, MyPrototypeLine *prototype);

/******************************************************************************
 * implement function 
 ******************************************************************************/
static void my_curve_class_init (MyCurveClass* klass)
{
	GObjectClass	*g_object_class;
	MyShapeClass	*myShapeClass;
	/* Add private structure */
	g_type_class_add_private (klass, sizeof (MyCurvePrivate));
	/* Get the parent gobject class */
	g_object_class = G_OBJECT_CLASS(klass);
	
    /* Hook overridable methods */
	myShapeClass = MY_SHAPE_CLASS(klass);
	my_debug ("my_curve_to_string %#x", my_curve_to_string);
	my_debug ("myShapeClass->to_string %#x", myShapeClass->to_string);
	myShapeClass->to_string = my_curve_to_string;
	my_debug ("myShapeClass->to_string %#x", myShapeClass->to_string);
	myShapeClass->is_chosen = my_curve_is_chosen_old;
	myShapeClass->draw_self = my_curve_draw_self;
	myShapeClass->draw_self_shadow = my_curve_draw_self_shadow;
	myShapeClass->draw_self_shadow_debug = draw_self_shadow_debug;
	myShapeClass->is_out_of_canvas = is_out_of_canvas;
	myShapeClass->shift = shift;
	myShapeClass->shift_self_only = shift_self_only;
	myShapeClass->init_control_shapes = init_control_shapes;
	myShapeClass->hide_control_shapes = hide_control_shapes;
	myShapeClass->delete_control_shapes = delete_control_shapes;
	myShapeClass->delete_control_shapes_from_layer = delete_control_shapes_from_layer;
	myShapeClass->has_control_shapes = has_control_shapes;
	myShapeClass->calculate_just_size = calculate_just_size;
	myShapeClass->update_with_scale = update_with_scale;
	myShapeClass->update_shadow_rect = update_shadow_rect;
	myShapeClass->on_shadow_dirty = on_shadow_dirty;
	myShapeClass->from_prototype_line = from_prototype_line;
	myShapeClass->to_prototype_line = to_prototype_line;
	
	/* Hook finalization functions */
	g_object_class->dispose = my_curve_dispose; /* instance destructor, reverse of init */
	g_object_class->finalize = my_curve_finalize; /* class finalization, reverse of class init */
}

// constructor
static void		my_curve_init (MyCurve* self)
{
	MyCurvePrivate* priv = MY_CURVE_GET_PRIVATE (self);
	/* Initialize public fields */
	MY_SHAPE (self)->isShadowing	=	TRUE;
	
	/* Initialize private fields */
	priv->points = g_queue_new();
}

static void my_curve_finalize (GObject* object)
{  
  /* Always chain up to the parent class; as with dispose(), finalize()
   * is guaranteed to exist on the parent's class virtual function table
   */
	G_OBJECT_CLASS (my_curve_parent_class)->finalize (object);
}

static void my_curve_dispose (GObject* object)
{
	/* Reverse what was allocated by instance init */
	MyCurve *self = MY_CURVE (object);
	MyCurvePrivate* priv = MY_CURVE_GET_PRIVATE (self);
 
	g_queue_foreach (priv->points, (GFunc) g_object_unref, NULL); // TODO: 不应该用g_object_unref!!!!!!!???
	g_queue_free (priv->points);
	
	delete_control_shapes (MY_SHAPE (self));
	
  /* Always chain up to the parent class; there is no need to check if
   * the parent class implements the dispose() virtual function: it is
   * always guaranteed to do so
   */
	G_OBJECT_CLASS (my_curve_parent_class)->dispose (object);
}


void		my_curve_append_point (MyCurve* self, MyPoint* point) 
{
	MyCurvePrivate* priv = MY_CURVE_GET_PRIVATE (self);
	MyRectangle *justSize = &MY_SHAPE(self)->justSize;
	g_queue_push_tail(priv->points, point);	
	
	my_shape_expand_just_size (MY_SHAPE (self), point->x, point->y);
}


void		my_curve_insert_point_before		(MyCurve *self, MyPoint *point, MyPoint *willInsert) {


	MyCurvePrivate* priv = MY_CURVE_GET_PRIVATE (self);
	gint i = g_queue_index (priv->points, point);
	g_queue_push_nth (priv->points, willInsert, i);
}


void		my_curve_delete_point				(MyCurve *self, MyPoint *point) {


	MyCurvePrivate* priv = MY_CURVE_GET_PRIVATE (self);
	gint i = g_queue_index (priv->points, point);
	g_queue_pop_nth (priv->points, i);
}


MyPoint *my_curve_peek_nth (MyCurve *self, guint n) {
	MyPoint *point = (MyPoint *) g_queue_peek_nth (MY_CURVE_GET_PRIVATE (self)->points, n);
	return point;
}

guint	my_curve_point_count (MyCurve *self) {
	
	MyCurvePrivate* priv = MY_CURVE_GET_PRIVATE (self);
	return g_queue_get_length(priv->points);
}

static GString	*	my_curve_to_string	(MyShape *self) {

	guint i, pointCount;
	MyPoint *point;
	char buf[50] = {0};
	GString * gstring = g_string_new (NULL);
	
	my_debug ("my_curve to_string...");
	
	sprintf(buf, "%d", self->id);
	g_string_append (gstring, "<curve id=\"");
	g_string_append (gstring, buf);
	g_string_append (gstring, "\" paths=\"");
	
	for (i = 0, pointCount = my_curve_point_count (MY_CURVE(self)); i < pointCount; i++) {
		point = my_curve_peek_nth (MY_CURVE(self), i);
		my_debug("point %#x", point);
		
		sprintf(buf, "%f", point->x);
		my_debug("buf: %s", buf);
		g_string_append_len (gstring, buf, strlen(buf));
		
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "%f", point->y);
		my_debug("buf: %s", buf);
		g_string_append (gstring, ",");
		g_string_append_len (gstring, buf, strlen(buf));
		g_string_append (gstring, " ");
	}
	
	g_string_append (gstring, "\" />");
	return gstring;
}


static gboolean		my_curve_is_chosen_old	(MyShape *shape, gdouble x, gdouble y, gdouble extraChosenPadding, ApplicationState *appState) {

	gboolean isInRect;
	MyCurvePrivate* priv = MY_CURVE_GET_PRIVATE (shape);
	gdouble radius = shape->strokeWidth + extraChosenPadding;
	isInRect = my_util_is_point_in_path (priv->points, x, y, radius);
	return isInRect;
}


static gboolean		my_curve_is_chosen	(MyShape *self, gdouble x, gdouble y, gdouble strokeWidth, ApplicationState *appState) {

	
	guint k, pointCount;
	MyCurve *curve = MY_CURVE (self);
	MyPoint *point;
	cairo_t *cr = appState->cr;
	gboolean isChosen;
	
	if (2 * strokeWidth > self->strokeWidth)
		return my_curve_is_chosen_old (self, x, y, strokeWidth, appState);
	
	for (k = 0, pointCount = my_curve_point_count (curve); k < pointCount; k++) {
		point = my_curve_peek_nth (curve, k);		
		cairo_line_to (cr, point->x, point->y);
	}
	isChosen = cairo_in_stroke (cr, x, y);
	cairo_new_path (cr);
	my_debug ("curve in stroke ? %d", isChosen);
	return isChosen;
}


static void		my_curve_draw_self	(MyShape* self, ApplicationState *appState) {

	
	guint k, pointCount;
	MyCurve *curve = MY_CURVE (self);
	MyPoint *point;
	gdouble currentX, currentY, theLastX, theLastY;	
	cairo_t *cr = appState->cr;
	
/*
// sin curve
#ifndef PI
#define PI 3.14159265358979
#endif
double sx = PI / 10;
int w;
double xx = 300;
cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);
for (w = 0; w < 1000; w++) {
	cairo_line_to (cr, xx + w * sx, 50 + 10 * sin (w * sx / 3));
}
cairo_stroke (cr);
*/

/* 
// gaussian blur
int              w, h;
cairo_surface_t *image = cairo_image_surface_create_from_png ("icon/a.png");
//blur_image_surface (image, 8);
cairo_set_source_surface (cr, image, 0, 0);
w = cairo_image_surface_get_width (image);
h = cairo_image_surface_get_height (image);
cairo_scale (cr, 512.0/w, 512.0/h);
cairo_pattern_set_filter (cairo_get_source (cr), CAIRO_FILTER_NEAREST);
cairo_paint (cr);
cairo_surface_destroy (image);
*/

/*
int              w, h;
cairo_surface_t *image;
image = cairo_image_surface_create_from_png ("icon/a.png");
cairo_set_source_surface (cr, image, 0, 0);
w = cairo_image_surface_get_width (image);
h = cairo_image_surface_get_height (image);
//cairo_translate (cr, 128.0, 128.0);
//cairo_translate (cr, -30.0, -30.0);
//cairo_translate (cr, 40.0, 40.0);
cairo_scale (cr, 2, 2);
//cairo_scale (cr, 512.0/w, 512.0/h);
//cairo_pattern_set_filter (cairo_get_source (cr), CAIRO_FILTER_NEAREST);
cairo_paint (cr);
cairo_surface_destroy (image);
*/

/*
cairo_pattern_t *pat;
pat = cairo_pattern_create_linear (256., 0., 0., 256.);
cairo_pattern_add_color_stop_rgba (pat, 0.,  1, 1, 1, 0);
cairo_pattern_add_color_stop_rgba (pat, 0.25,  0, 1, 0, 0.5);
cairo_pattern_add_color_stop_rgba (pat, 0.50,  1, 1, 1, 0);
cairo_pattern_add_color_stop_rgba (pat, 0.75,  0, 0, 1, 0.5);
cairo_pattern_add_color_stop_rgba (pat, 1.,  1, 1, 1, 0);
cairo_rectangle (cr, 0, 0, 512, 512);
cairo_set_source (cr, pat);
cairo_fill (cr);
cairo_move_to (cr, 100, 200);
cairo_line_to (cr, 300, 400);
cairo_line_to (cr, 400, 500);
cairo_stroke (cr);
cairo_pattern_destroy (pat);
*/

/*
double dashes[] = {50.0,  // ink 
                   10.0,  // skip 
                   10.0,  // ink 
                   10.0   // skip
                  };
int    ndash  = sizeof (dashes)/sizeof(dashes[0]);
double offset = -50.;
cairo_set_dash (cr, dashes, ndash, offset);
*/

/*
	double x=25.6,  y=128.0;
double x1=102.4, y1=230.4,
       x2=153.6, y2=25.6,
       x3=230.4, y3=128.0;

cairo_set_source_rgba (cr, 1, 0.2, 0.2, 0.6);
cairo_move_to (cr, x, y);
cairo_curve_to (cr, x1, y1, x2, y2, x3, y3);

cairo_set_line_width (cr, 30.0);
cairo_stroke (cr);

cairo_set_source_rgba (cr, 1, 0.2, 0.2, 0.6);
cairo_set_line_width (cr, 6.0);
cairo_move_to (cr,x,y);   
cairo_line_to (cr,x1,y1);
cairo_move_to (cr,x2,y2); 
cairo_line_to (cr,x3,y3);
cairo_stroke (cr);
*/

	/*
	if (self->gc == NULL) {
		self->gc = gdk_gc_new (pixmap);
		gdk_gc_copy(self->gc, appState->gcSolid);
		gdk_gc_set_line_attributes (self->gc, 10, GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_MITER);
	}
	*/
	my_debug ("draw curve %#x...", self);
	cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);
	for (k = 0, pointCount = my_curve_point_count (curve); k < pointCount; k++) {
		my_debug ("loop points count: %d", k);
		point = my_curve_peek_nth (curve, k);
		
		cairo_line_to (cr, point->x + appState->orignX, 
							point->y + appState->orignY);
	}
	cairo_stroke (cr);
	my_debug ("draw curve %#x done.", self);
}


static void		my_curve_draw_self_shadow	(MyShape* self, ApplicationState *appState) {

	
	guint k, pointCount;
	MyCurve *curve = MY_CURVE (self);
	MyPoint *point;
	cairo_t *cr = appState->cr;
	gdouble scale = appState->scale;
	
	for (k = 0, pointCount = my_curve_point_count (curve); k < pointCount; k++) {
		point = my_curve_peek_nth (curve, k);
		cairo_line_to (cr,	point->x / scale - self->shadowX,
							point->y / scale - self->shadowY);
	}
	cairo_stroke (cr);
}


static void		draw_self_shadow_debug		(MyShape* self) {
	
		
	guint k, pointCount;
	MyCurve *curve = MY_CURVE (self);
	MyPoint *point;
	cairo_t *cr = self->appState->cr;
	gdouble scale = self->appState->scale;
	
	for (k = 0, pointCount = my_curve_point_count (curve); k < pointCount; k++) {
		point = my_curve_peek_nth (curve, k);
		cairo_line_to (cr,	point->x / scale + self->appState->orignX,
							point->y / scale + self->appState->orignY);
	}
	cairo_stroke (cr);	
}


static void			shift		(MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {

	
	guint i, len, k, pointCount;
	MyCurve *curve = MY_CURVE (self);
	MyPoint *point; 
	MyShape *controlShape;
	MyCurvePrivate* priv = MY_CURVE_GET_PRIVATE (self);
	MyControlShapeRect *controlShapeRect = (MyControlShapeRect *) source;
	
	if (self == source) {
		for (k = 0, pointCount = my_curve_point_count (curve); k < pointCount; k++) {
			point = my_curve_peek_nth (curve, k);
			point->x += dx;
			point->y += dy;
		}
		if (priv->controlShapes) {	// shift控制图形
			for (i = 0, len = g_queue_get_length (priv->controlShapes); i < len; i++) {
				controlShape = (MyShape *) g_queue_peek_nth (priv->controlShapes, i);
				my_shape_shift_self_only (controlShape, dx, dy);
			}
		}
	} else {		
		if (controlShapeRect->pointTarget) {
			controlShapeRect->pointTarget->x += dx;
			controlShapeRect->pointTarget->y += dy;
		}
	}
}


static void			shift_self_only		(MyShape *self, gdouble dx, gdouble dy) {


	guint i, len, k, pointCount;
	MyCurve *curve = MY_CURVE (self);
	MyPoint *point; 
	MyShape *controlShape;
	
	for (k = 0, pointCount = my_curve_point_count (curve); k < pointCount; k++) {
		point = my_curve_peek_nth (curve, k);
		point->x += dx;
		point->y += dy;
	}
}


static gboolean		is_out_of_canvas	(MyShape *self, ApplicationState *appState) {


	my_shape_calculate_just_size (self);
	return !my_util_is_rect_intersection (self->justSize.x, self->justSize.y, self->justSize.width, self->justSize.height,
										0, 0, appState->canvasWidth, appState->canvasHeight);
}


gboolean	onShiftControl	(MyControlShapeRect *contolShape, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, void *userData) {

	
	my_shape_shift (contolShape->target, source, dx, dy, mouse, keyMask, userData);
	return TRUE;
}


void 				init_control_shapes	(MyShape* self, MyLayer *layer) {
	
	guint i, len;
	MyPoint *point;
	MyControlShapeRect *controlRect;
	MyCurve *curve = MY_CURVE(self);
	MyCurvePrivate* priv = MY_CURVE_GET_PRIVATE (self);
	
	if (priv->controlShapes) {	// 已有控制图形, 显示他们
		for (i = 0, len = g_queue_get_length (priv->controlShapes); i < len; i++) {			
			controlRect = g_queue_peek_nth (priv->controlShapes, i);
			if (MY_SHAPE (controlRect)->isShowable)
				MY_SHAPE (controlRect)->isShowing = TRUE;
		}
	} else {	// 没有, 新建控制图形
		priv->controlShapes = g_queue_new ();
		for (i = 0, len = my_curve_point_count (curve); i < len; i++) {
			point = my_curve_peek_nth (curve, i);
			
			controlRect = g_object_new (MY_TYPE_CONTROL_SHAPE_RECT, NULL);
			MY_SHAPE (controlRect)->appState = self->appState;
			controlRect->x = point->x - 4;
			controlRect->y = point->y - 4;
			controlRect->target = self;
			controlRect->pointTarget = point;
			controlRect->onShift = onShiftControl;
			g_queue_push_tail (priv->controlShapes, controlRect);
			
			my_debug ("add control rect %#x to layer", controlRect);
			my_layer_add_head_shape (layer, controlRect);
		}
	}
}


void 				hide_control_shapes	(MyShape* self) {

	guint i, len;
	MyControlShapeRect *controlRect;
	MyCurvePrivate* priv = MY_CURVE_GET_PRIVATE (self);
	if (priv->controlShapes) {
		for (i = 0, len = g_queue_get_length (priv->controlShapes); i < len; i++) {
			controlRect = g_queue_peek_nth (priv->controlShapes, i);
			MY_SHAPE (controlRect)->isShowing = FALSE;
		}
	}
}

void 				delete_control_shapes	(MyShape *self) {
	
	MyCurvePrivate* priv = MY_CURVE_GET_PRIVATE (self);
	if (priv->controlShapes) {
		//g_queue_foreach (priv->controlShapes, (GFunc) g_object_unref, NULL);
		g_queue_free (priv->controlShapes);
		priv->controlShapes = NULL;
	}
}

void 				delete_control_shapes_from_layer	(MyShape *self, MyLayer *layer) {
	
	guint i, len;
	MyShape *controlShape;
	MyCurvePrivate* priv = MY_CURVE_GET_PRIVATE (self);
	if (priv->controlShapes) {
		for (i = 0, len = g_queue_get_length (priv->controlShapes); i < len; i++) {
			controlShape = g_queue_peek_nth (priv->controlShapes, i);
			my_layer_delete_shape_only (layer, controlShape);
		}
		g_queue_free (priv->controlShapes);
		priv->controlShapes = NULL;
	}
}


gboolean			has_control_shapes			(MyShape *self) {


	MyCurvePrivate* priv = MY_CURVE_GET_PRIVATE (self);
	return priv->controlShapes != NULL;
}


void				calculate_just_size			(MyShape *self) {
	

	MyCurvePrivate* priv = MY_CURVE_GET_PRIVATE (self);
	guint i, pointCount;
	MyPoint *point;
	
	for (i = 0, pointCount = g_queue_get_length (priv->points); i < pointCount; i++) {
		point = g_queue_peek_nth (priv->points, i);
		my_shape_expand_just_size (self, point->x, point->y);
	}
}


void		my_curve_update_last_point	(MyCurve *self, gdouble x, gdouble y) {
	

	MyCurvePrivate* priv = MY_CURVE_GET_PRIVATE (self);
	MyPoint *point;
	guint i = g_queue_get_length (priv->points) - 1;
	g_assert (i >= 0);
	point = (MyPoint *) g_queue_peek_nth (priv->points, i);
	point->x = x;
	point->y = y;
}


static void		update_with_scale		(MyShape *self, gdouble scale) {
	

	MyCurvePrivate* priv = MY_CURVE_GET_PRIVATE (MY_CURVE(self));
	guint i, pointCount;
	MyPoint *point;
	my_debug ("update_with_scale ... self is %#x", self);
	my_debug ("priv is %#x", priv);
	
	for (i = 0, pointCount = g_queue_get_length (priv->points); i < pointCount; i++) {
		point = (MyPoint *) g_queue_peek_nth (priv->points, i);
		point->x = point->x / self->appState->oldScale * scale;
		point->y = point->y / self->appState->oldScale * scale;
	}
}


static void		on_shadow_dirty				(MyShape *self) {
	
	gdouble scale = self->appState->scale;
	self->shadowPadding = 2 * self->boxRadius + 0.707 * self->strokeWidth;
	self->shadowOrignWidth = ceil (self->justSize.width / scale + 2 * self->shadowPadding);
	self->shadowOrignHeight = ceil (self->justSize.height / scale + 2 * self->shadowPadding);
}


static void		update_shadow_rect			(MyShape *self) {
	
	
//	my_shape_calculate_just_size (self);
	
//	self->shadowPadding = self->appState->scale * (self->boxRadius + 0.707 * self->strokeWidth);
//	self->shadowX = self->justSize.x - self->shadowPadding;
//	self->shadowY = self->justSize.y - self->shadowPadding;
//	self->shadowWidth = ceil (self->justSize.width + self->shadowPadding * 2);
//	self->shadowHeight = ceil (self->justSize.height + self->shadowPadding * 2);

	gdouble scale = self->appState->scale;
	self->shadowX = self->justSize.x / scale - self->shadowPadding;
	self->shadowY = self->justSize.y / scale - self->shadowPadding;
	self->shadowWidth = self->justSize.width + 2 * self->shadowPadding * scale;
	self->shadowHeight = self->justSize.height + 2 * self->shadowPadding * scale;
}


static void		from_prototype_line		(MyShape *self, MyPrototypeLine *prototype) {
}


static void		to_prototype_line		(MyShape *self, MyPrototypeLine *prototype) {	
	
	prototype->style = MY_PROTOTYPE_STYLE_STRAIGHT_LINE;
}