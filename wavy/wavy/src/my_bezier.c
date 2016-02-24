#define MY_LOG_LEVEL 3

#include <gtk/gtk.h>
#include <glib-object.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "my_constant.h"
#include "my_math.h"
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
#include "my_bezier_util.h"
#include "my_bezier.h"

#define MY_BEZIER_GET_PRIVATE(o)		(G_TYPE_INSTANCE_GET_PRIVATE ((o), MY_TYPE_BEZIER, MyBezierPrivate))

G_DEFINE_TYPE (MyBezier, my_bezier, MY_TYPE_SHAPE);
//G_DEFINE_TYPE_WITH_PRIVATE (MyBezier, my_bezier, G_TYPE_OBJECT)
 
typedef struct _MyBezierPrivate {
	void *noop;
} MyBezierPrivate;

/******************************************************************************
 * declare static function 
 ******************************************************************************/
static void 		my_bezier_init (MyBezier* self);
static void 		my_bezier_finalize (GObject* object);
static void 		my_bezier_dispose (GObject* object);
static void 		init_control_shapes	(MyShape* self, MyLayer *layer);
static void 		real_hide_control_shapes	(MyShape* self);
static void 		real_delete_control_shapes	(MyShape *self);
static void 		real_delete_control_shapes_from_layer	(MyShape *self, MyLayer *layer);
static gboolean		real_has_control_shapes	(MyShape *self);
static void			calculate_just_size	(MyShape *self);
static GString	*	my_bezier_to_string	(MyShape *self);
static gboolean		my_bezier_is_chosen	(MyShape *self, gdouble x, gdouble y, gdouble strokeWidth, ApplicationState *appState);
static gboolean		my_bezier_is_chosen_old	(MyShape *self, gdouble x, gdouble y, gdouble strokeWidth, ApplicationState *appState);
static void			my_bezier_draw_self_old	(MyShape* self, ApplicationState *appState);
static void			my_bezier_draw_self	(MyShape* self, ApplicationState *appState);
static void			my_bezier_draw_self_shadow	(MyShape* self, ApplicationState *appState);
static void			my_bezier_shift		(MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData);
static void			my_bezier_shift_self_only		(MyShape *self, gdouble dx, gdouble dy);
static gboolean		is_out_of_canvas	(MyShape *self, ApplicationState *appState);
static void			update_with_scale	(MyShape *self, gdouble scale);
static void			update_shadow_rect	(MyShape *self);
static void			on_shadow_dirty		(MyShape *self);
static void			from_prototype_line	(MyShape *self, MyPrototypeLine *prototype);
static void			to_prototype_line	(MyShape *self, MyPrototypeLine *prototype);


/******************************************************************************
 * implement function 
 ******************************************************************************/

static void my_bezier_class_init (MyBezierClass* klass)
{
	GObjectClass	*g_object_class;
	MyShapeClass	*myShapeClass;
	/* Add private structure */
	g_type_class_add_private (klass, sizeof (MyBezierPrivate));
	/* Get the parent gobject class */
	g_object_class = G_OBJECT_CLASS(klass);
	
    /* Hook overridable methods */
	myShapeClass = MY_SHAPE_CLASS(klass);
	myShapeClass->to_string = my_bezier_to_string;
	myShapeClass->is_chosen = my_bezier_is_chosen_old;
	myShapeClass->draw_self = my_bezier_draw_self;
	myShapeClass->draw_self_shadow = my_bezier_draw_self_shadow;
	myShapeClass->shift = my_bezier_shift;
	myShapeClass->shift_self_only = my_bezier_shift_self_only;
	myShapeClass->is_out_of_canvas = is_out_of_canvas;
	myShapeClass->init_control_shapes = init_control_shapes;
	myShapeClass->hide_control_shapes = real_hide_control_shapes;
	myShapeClass->delete_control_shapes = real_delete_control_shapes;
	myShapeClass->delete_control_shapes_from_layer = real_delete_control_shapes_from_layer;
	myShapeClass->has_control_shapes = real_has_control_shapes;
	myShapeClass->calculate_just_size = calculate_just_size;
	myShapeClass->update_with_scale = update_with_scale;
	myShapeClass->update_shadow_rect = update_shadow_rect;
	myShapeClass->on_shadow_dirty = on_shadow_dirty;
	myShapeClass->from_prototype_line = from_prototype_line;
	myShapeClass->to_prototype_line = to_prototype_line;
	
	/* Hook finalization functions */
	g_object_class->dispose = my_bezier_dispose; /* instance destructor, reverse of init */
	g_object_class->finalize = my_bezier_finalize; /* class finalization, reverse of class init */
}

// constructor
static void		my_bezier_init (MyBezier* self)
{
	/* Initialize public fields */
	MY_SHAPE (self)->isShadowing	=	TRUE;
	
	/* Initialize private fields */
}

static void my_bezier_finalize (GObject* object)
{  
  /* Always chain up to the parent class; as with dispose(), finalize()
   * is guaranteed to exist on the parent's class virtual function table
   */
	G_OBJECT_CLASS (my_bezier_parent_class)->finalize (object);
}

static void my_bezier_dispose (GObject* object)
{
	/* Reverse what was allocated by instance init */
	MyBezier *self = MY_BEZIER (object);
	MyBezierPrivate* priv = MY_BEZIER_GET_PRIVATE (self);
 	
	real_delete_control_shapes (self);
	
  /* Always chain up to the parent class; there is no need to check if
   * the parent class implements the dispose() virtual function: it is
   * always guaranteed to do so
   */
	G_OBJECT_CLASS (my_bezier_parent_class)->dispose (object);
}


void			my_bezier_new_with_coordinary	(MyBezier *bez,
												gint id, 
												ApplicationState *appState,
												gdouble x0, 
												gdouble y0,
												gdouble x3, 
												gdouble y3,
												gdouble scale) {
											
	MyShape *shape = MY_SHAPE (bez);
	
	bez->p0.x = x0;
	bez->p0.y = y0;
	bez->p3.x = x3;
	bez->p3.y = y3;
	bez->p1 = my_vector_add (bez->p0, 
				my_vector_scalar_mul (
					my_vector_norm (
						my_vector_perpendicular ( bez->p0)), 
					60));
	bez->p2 = my_vector_add (bez->p3, 
				my_vector_scalar_mul (
					my_vector_norm (
						my_vector_perpendicular ( bez->p3)), 
					60));
	/*
	bez->standardP0.x = bez->p0.x / scale;
	bez->standardP0.y = bez->p0.y / scale;
	bez->standardP1.x = bez->p1.x / scale;
	bez->standardP1.y = bez->p1.y / scale;
	bez->standardP2.x = bez->p2.x / scale;
	bez->standardP2.y = bez->p2.y / scale;
	bez->standardP3.x = bez->p3.x / scale;
	bez->standardP3.y = bez->p3.y / scale;
	my_debug ("scale is %f, 1/scale is %f", scale, 1/scale);
	my_debug ("p0 is %f,%f, standardP0 is %f,%f", bez->p0.x, bez->p0.y, bez->standardP0.x, bez->standardP0.y);
	my_debug ("p1 is %f,%f, standardP1 is %f,%f", bez->p1.x, bez->p1.y, bez->standardP1.x, bez->standardP1.y);
	my_debug ("p2 is %f,%f, standardP2 is %f,%f", bez->p2.x, bez->p2.y, bez->standardP2.x, bez->standardP2.y);
	my_debug ("p3 is %f,%f, standardP3 is %f,%f", bez->p3.x, bez->p3.y, bez->standardP3.x, bez->standardP3.y);
	*/
	shape->id = id;
	shape->appState = appState;
	shape->red = 0.0;
	shape->green = 0.0;
	shape->blue = 0.0;
	shape->alpha = 1.0;
	//shape->strokeWidth = 5.0;
	
	shape->shadowPadding = shape->boxRadius + 0.707 * shape->strokeWidth;
}


void		my_bezier_new			(MyBezier *bez, gint id, ApplicationState *appState) {

	
	MY_SHAPE (bez)->id = id;
	MY_SHAPE (bez)->appState = appState;
}


void		my_bezier_update_point		(MyBezier *self, gint pointNum, gdouble x, gdouble y) {

	MyBezier *bez = self;
	
	switch (pointNum) {
	case 0:
		bez->p0.x = x;
		bez->p0.y = y;
		/*
		bez->standardP0.x = bez->p0.x / MY_SHAPE(self)->appState->scale;
		bez->standardP0.y = bez->p0.y / MY_SHAPE(self)->appState->scale;
		*/
		break;
	case 1:
		bez->p1.x = x;
		bez->p1.y = y;
		/*
		bez->standardP1.x = bez->p1.x / MY_SHAPE(self)->appState->scale;
		bez->standardP1.y = bez->p1.y / MY_SHAPE(self)->appState->scale;
		*/
		break;
	case 2:
		bez->p2.x = x;
		bez->p2.y = y;
		/*
		bez->standardP2.x = bez->p2.x / MY_SHAPE(self)->appState->scale;
		bez->standardP2.y = bez->p2.y / MY_SHAPE(self)->appState->scale;
		*/
		break;
	case 3:
		bez->p3.x = x;
		bez->p3.y = y;
		/*
		bez->standardP3.x = bez->p3.x / MY_SHAPE(self)->appState->scale;
		bez->standardP3.y = bez->p3.y / MY_SHAPE(self)->appState->scale;
		my_debug ("after update standardP3 is %f,%f, p3 is %f,%f", bez->standardP3.x, bez->standardP3.y, bez->p3.x, bez->p3.y);
		*/
		break;
	default:
		break;
	}
}


static GString	*	my_bezier_to_string	(MyShape *self) {

	guint i, pointCount;
	MyPoint *point;
	char buf[50] = {0};
	GString * gstring = g_string_new (NULL);
	return gstring;
}

static gboolean		my_bezier_is_chosen_old		(MyShape *shape, gdouble x, gdouble y, gdouble strokeWidth, ApplicationState *appState) {

	MyBezier *bez = MY_BEZIER (shape);
	MyVector q0, q1;
	gdouble t;
	int i;
	gdouble segmentCount = 20;
	
	q0 = calculateBezierPoint (0, bez->p0, bez->p1, bez->p2, bez->p3);
	for(i = 1; i <= segmentCount; i++) {
		t = i / segmentCount;
		q1 = calculateBezierPoint(t, bez->p0, bez->p1, bez->p2, bez->p3);
		
		if (my_util_is_point_inside_segment (q0.x, q0.y, q1.x, q1.y, x, y, strokeWidth))
			return TRUE;
		
		q0 = q1;
	}
	return FALSE;
}

static gboolean		my_bezier_is_chosen		(MyShape *self, gdouble x, gdouble y, gdouble strokeWidth, ApplicationState *appState) {

	MyBezier *bez = MY_BEZIER (self);
	gboolean isChosen;
	cairo_t *cr = appState->cr;
	
	if (2 * strokeWidth > self->strokeWidth)
		return my_bezier_is_chosen_old (self, x, y, strokeWidth, appState);
	
	cairo_move_to (cr, bez->p0.x, bez->p0.y);
	cairo_curve_to (cr, 
		bez->p1.x, bez->p1.y, 
		bez->p2.x, bez->p2.y,
		bez->p3.x, bez->p3.y);
	
	isChosen = cairo_in_stroke (cr, x, y);
	my_debug ("bezier %#x is chosen? %d", isChosen);
	cairo_new_path (cr);
	return isChosen;
}

static void		my_bezier_draw_self		(MyShape* self, ApplicationState *appState) {
	
	MyBezier *bez = MY_BEZIER (self);
	cairo_t *cr = appState->cr;
	
	cairo_move_to (cr, 
		bez->p0.x + appState->orignX, 
		bez->p0.y + appState->orignY);
	cairo_curve_to (cr, 
		bez->p1.x + appState->orignX, 
		bez->p1.y + appState->orignY, 
		bez->p2.x + appState->orignX, 
		bez->p2.y + appState->orignY,
		bez->p3.x + appState->orignX, 
		bez->p3.y + appState->orignY);
	cairo_stroke (cr);
}


static void			my_bezier_draw_self_shadow	(MyShape* self, ApplicationState *appState) {

	
	MyBezier *bez = MY_BEZIER (self);
	cairo_t *cr = appState->cr;
	gdouble scale = appState->scale;
	
	cairo_move_to (cr, 
		bez->p0.x / scale - self->shadowX, 
		bez->p0.y / scale - self->shadowY);
	cairo_curve_to (cr, 
		bez->p1.x / scale - self->shadowX, 
		bez->p1.y / scale - self->shadowY, 
		bez->p2.x / scale - self->shadowX, 
		bez->p2.y / scale - self->shadowY,
		bez->p3.x / scale - self->shadowX, 
		bez->p3.y / scale - self->shadowY);
	cairo_stroke (cr);
}


static void		draw_self_shadow_debug		(MyShape* self) {

	
	MyBezier *bez = MY_BEZIER (self);
	cairo_t *cr = self->appState->cr;
	gdouble scale = self->appState->scale;
	
	cairo_move_to (cr, 
		bez->p0.x / scale + self->appState->orignX, 
		bez->p0.y / scale + self->appState->orignY);
	cairo_curve_to (cr, 
		bez->p1.x / scale + self->appState->orignX,
		bez->p1.y / scale + self->appState->orignY, 
		bez->p2.x / scale + self->appState->orignX,
		bez->p2.y / scale + self->appState->orignY,
		bez->p3.x / scale + self->appState->orignX,
		bez->p3.y / scale + self->appState->orignY);
	cairo_stroke (cr);
}


static gboolean		is_out_of_canvas	(MyShape *self, ApplicationState *appState) {

	MyBezier *bez = MY_BEZIER (self);
	
	gdouble bezRectX = my_min (my_min (my_min (bez->p0.x, bez->p1.x), bez->p2.x), bez->p3.x);
	gdouble bezRectY = my_min (my_min (my_min (bez->p0.y, bez->p1.y), bez->p2.y), bez->p3.y);
	gdouble maxX = my_max ( my_max (my_max (bez->p0.x, bez->p1.x), bez->p2.x), bez->p3.x);
	gdouble maxY = my_max ( my_max (my_max (bez->p0.y, bez->p1.y), bez->p2.y), bez->p3.y);
	
	my_debug ("invoking my_util_is_rect_intersection(%f,%f,%f,%f,%f,%f,%f,%f)", 
			bezRectX, bezRectY, maxX - bezRectX, maxY - bezRectY, 1, 1, appState->canvasWidth, appState->canvasHeight);
	return !my_util_is_rect_intersection (bezRectX, bezRectY, maxX - bezRectX, maxY - bezRectY, 
										0, 0, appState->canvasWidth, appState->canvasHeight);
}

static void			my_bezier_shift		(MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {


	MyBezier *bez = MY_BEZIER (self);
	my_debug ("my_bezier_shift ...");
	my_bezier_shift_self_only (self, dx, dy);
	
	if (self == source) {
		if (bez->p1ControlRect != NULL) {
			my_shape_shift_self_only (MY_SHAPE (bez->p0ControlRect), dx, dy);
			my_shape_shift_self_only (MY_SHAPE (bez->p1ControlRect), dx, dy);
			my_shape_shift_self_only (MY_SHAPE (bez->p2ControlRect), dx, dy);
			my_shape_shift_self_only (MY_SHAPE (bez->p3ControlRect), dx, dy);
		}
	} else {
	}
}


static void			my_bezier_shift_self_only		(MyShape *self, gdouble dx, gdouble dy) {


	MyBezier *bez = MY_BEZIER (self);
	my_debug ("my_bezier_shift ...");
	
	my_bezier_update_point (bez, 0, bez->p0.x + dx, bez->p0.y + dy);
	my_bezier_update_point (bez, 1, bez->p1.x + dx, bez->p1.y + dy);
	my_bezier_update_point (bez, 2, bez->p2.x + dx, bez->p2.y + dy);
	my_bezier_update_point (bez, 3, bez->p3.x + dx, bez->p3.y + dy);
}


static gboolean			onShiftP0		(MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {

	MyBezier *bez = MY_BEZIER (MY_CONTROL_SHAPE_RECT (self)->target);
	my_bezier_update_point (bez, 0, bez->p0.x + dx, bez->p0.y + dy);

	return TRUE;
}


static gboolean			onShiftP1		(MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {

	MyBezier *bez = MY_BEZIER (MY_CONTROL_SHAPE_RECT (self)->target);
	my_bezier_update_point (bez, 1, bez->p1.x + dx, bez->p1.y + dy);

	return TRUE;
}


static gboolean			onShiftP2		(MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {

	MyBezier *bez = MY_BEZIER (MY_CONTROL_SHAPE_RECT (self)->target);
	my_bezier_update_point (bez, 2, bez->p2.x + dx, bez->p2.y + dy);

	return TRUE;
}


static gboolean			onShiftP3		(MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {

	MyBezier *bez = MY_BEZIER (MY_CONTROL_SHAPE_RECT (self)->target);
	my_bezier_update_point (bez, 3, bez->p3.x + dx, bez->p3.y + dy);

	return TRUE;
}


static void 			init_control_shapes		(MyShape* self, MyLayer *layer) {
	
	MyBezier *bez = MY_BEZIER(self);
	gdouble controlRectWidth = 8;
	gdouble controlRectHeight = 8;
	
	if (bez->p1ControlRect == NULL) {// 没有, 新建控制图形	
		bez->p1ControlRect = my_control_shape_rect_new (self->appState,
									bez->p1.x - controlRectWidth / 2, 
									bez->p1.y - controlRectHeight / 2, 
									controlRectWidth, 
									controlRectHeight,
									self, 
									onShiftP1, 
									NULL);
		bez->p0ControlRect = my_control_shape_rect_new (self->appState,
									bez->p0.x - controlRectWidth / 2, 
									bez->p0.y - controlRectHeight / 2, 
									controlRectWidth, 
									controlRectHeight,
									self, 
									onShiftP0, 
									NULL);
		bez->p2ControlRect = my_control_shape_rect_new (self->appState,
									bez->p2.x - controlRectWidth / 2, 
									bez->p2.y - controlRectHeight / 2, 
									controlRectWidth, 
									controlRectHeight,
									self, 
									onShiftP2, 
									NULL);
		bez->p3ControlRect = my_control_shape_rect_new (self->appState,
									bez->p3.x - controlRectWidth / 2, 
									bez->p3.y - controlRectHeight / 2, 
									controlRectWidth, 
									controlRectHeight,
									self, 
									onShiftP3, 
									NULL);
		/*
		bez->p1ControlRect = g_object_new (MY_TYPE_CONTROL_SHAPE_RECT, NULL);
		bez->p1ControlRect->x = bez->p1.x - 4;
		bez->p1ControlRect->y = bez->p1.y - 4;
		bez->p1ControlRect->target = self;
		bez->p1ControlRect->onShift = onShiftP1;
		
		bez->p0ControlRect = g_object_new (MY_TYPE_CONTROL_SHAPE_RECT, NULL);
		bez->p0ControlRect->x = bez->p0.x - 4;
		bez->p0ControlRect->y = bez->p0.y - 4;
		bez->p0ControlRect->vectorTarget = &bez->p0;
		bez->p0ControlRect->target = self;
		bez->p0ControlRect->userData = bez;
		bez->p0ControlRect->onShift = onShiftP0;
		
		bez->p2ControlRect = g_object_new (MY_TYPE_CONTROL_SHAPE_RECT, NULL);
		bez->p2ControlRect->x = bez->p2.x - 4;
		bez->p2ControlRect->y = bez->p2.y - 4;
		bez->p2ControlRect->vectorTarget = &bez->p2;
		bez->p2ControlRect->target = self;
		bez->p2ControlRect->userData = bez;
		bez->p2ControlRect->onShift = onShiftP2;
		
		bez->p3ControlRect = g_object_new (MY_TYPE_CONTROL_SHAPE_RECT, NULL);
		bez->p3ControlRect->x = bez->p3.x - 4;
		bez->p3ControlRect->y = bez->p3.y - 4;
		bez->p3ControlRect->vectorTarget = &bez->p3;
		bez->p3ControlRect->target = self;
		bez->p3ControlRect->userData = bez;
		bez->p3ControlRect->onShift = onShiftP3;
		
		*/
		my_layer_add_head_shape (layer, bez->p0ControlRect);
		my_layer_add_head_shape (layer, bez->p1ControlRect);
		my_layer_add_head_shape (layer, bez->p2ControlRect);
		my_layer_add_head_shape (layer, bez->p3ControlRect);
	} else {
		MY_SHAPE (bez->p0ControlRect)->isShowing = TRUE;
		MY_SHAPE (bez->p1ControlRect)->isShowing = TRUE;
		MY_SHAPE (bez->p2ControlRect)->isShowing = TRUE;
		MY_SHAPE (bez->p3ControlRect)->isShowing = TRUE;
	}
}

static void 				real_hide_control_shapes	(MyShape *self) {

	MyBezier *bez = MY_BEZIER(self);
	my_debug ("real_hide_control_shapes of bezier %#x", self);
	
	MY_SHAPE (bez->p0ControlRect)->isShowing = FALSE;
	MY_SHAPE (bez->p1ControlRect)->isShowing = FALSE;
	MY_SHAPE (bez->p2ControlRect)->isShowing = FALSE;
	MY_SHAPE (bez->p3ControlRect)->isShowing = FALSE;
}

static void 				real_delete_control_shapes	(MyShape *self) {
	
	MyBezierPrivate* priv = MY_BEZIER_GET_PRIVATE (self);
	
}

static void 				real_delete_control_shapes_from_layer	(MyShape *self, MyLayer *layer) {

	
	if (MY_BEZIER (self)->p0ControlRect) {
		my_layer_delete_shape_only (layer, MY_BEZIER (self)->p0ControlRect);
		my_layer_delete_shape_only (layer, MY_BEZIER (self)->p1ControlRect);
		my_layer_delete_shape_only (layer, MY_BEZIER (self)->p2ControlRect);
		my_layer_delete_shape_only (layer, MY_BEZIER (self)->p3ControlRect);
		MY_BEZIER (self)->p0ControlRect = NULL;
		MY_BEZIER (self)->p1ControlRect = NULL;
		MY_BEZIER (self)->p2ControlRect = NULL;
		MY_BEZIER (self)->p3ControlRect = NULL;		
	}
}

static gboolean			real_has_control_shapes	(MyShape *self) {
	MyBezierPrivate* priv = MY_BEZIER_GET_PRIVATE (self);
}


static void		calculate_just_size		(MyShape *self) {
	
	
	MyBezier *bez = MY_BEZIER (self);
	my_shape_expand_just_size (self, bez->p0.x, bez->p0.y);
	my_shape_expand_just_size (self, bez->p1.x, bez->p1.y);
	my_shape_expand_just_size (self, bez->p2.x, bez->p2.y);
	my_shape_expand_just_size (self, bez->p3.x, bez->p3.y);	
}


static void		update_with_scale		(MyShape *self, gdouble scale) {

	
	MyBezier *bez = MY_BEZIER (self);
	/*
	my_debug ("before scale p0 %f,%f standard %f,%f", bez->p0.x, bez->p0.y, bez->standardP0.x, bez->standardP0.y);
	my_debug ("before scale p1 %f,%f standard %f,%f", bez->p1.x, bez->p1.y, bez->standardP1.x, bez->standardP1.y);
	my_debug ("before scale p2 %f,%f standard %f,%f", bez->p2.x, bez->p2.y, bez->standardP2.x, bez->standardP2.y);
	my_debug ("before scale p3 %f,%f standard %f,%f", bez->p3.x, bez->p3.y, bez->standardP3.x, bez->standardP3.y);
	
	bez->p0.x = bez->standardP0.x * scale;
	bez->p0.y = bez->standardP0.y * scale;
	bez->p1.x = bez->standardP1.x * scale;
	bez->p1.y = bez->standardP1.y * scale;
	bez->p2.x = bez->standardP2.x * scale;
	bez->p2.y = bez->standardP2.y * scale;
	bez->p3.x = bez->standardP3.x * scale;
	bez->p3.y = bez->standardP3.y * scale;
	*/
	bez->p0.x = bez->p0.x / self->appState->oldScale * scale;
	bez->p0.y = bez->p0.y / self->appState->oldScale * scale;
	bez->p1.x = bez->p1.x / self->appState->oldScale * scale;
	bez->p1.y = bez->p1.y / self->appState->oldScale * scale;
	bez->p2.x = bez->p2.x / self->appState->oldScale * scale;
	bez->p2.y = bez->p2.y / self->appState->oldScale * scale;
	bez->p3.x = bez->p3.x / self->appState->oldScale * scale;
	bez->p3.y = bez->p3.y / self->appState->oldScale * scale;
	my_debug ("after scale p0 %f,%f", bez->p0.x, bez->p0.y);
	my_debug ("after scale p1 %f,%f", bez->p1.x, bez->p1.y);
	my_debug ("after scale p2 %f,%f", bez->p2.x, bez->p2.y);
	my_debug ("after scale p3 %f,%f", bez->p3.x, bez->p3.y);
}


static void		on_shadow_dirty				(MyShape *self) {
	
	
	gdouble scale = self->appState->scale;
	self->shadowPadding = 2 * self->boxRadius + 0.707 * self->strokeWidth;
	self->shadowOrignWidth = ceil (self->justSize.width / scale + 2 * self->shadowPadding);
	self->shadowOrignHeight = ceil (self->justSize.height / scale + 2 * self->shadowPadding);
}


static void		update_shadow_rect			(MyShape *self) {
	
	
	gdouble scale = self->appState->scale;
	self->shadowX = self->justSize.x / scale - self->shadowPadding;
	self->shadowY = self->justSize.y / scale - self->shadowPadding;
	self->shadowWidth = self->justSize.width + 2 * self->shadowPadding * scale;
	self->shadowHeight = self->justSize.height + 2 * self->shadowPadding * scale;
}


static void		from_prototype_line		(MyShape *self, MyPrototypeLine *prototype) {
}


static void		to_prototype_line		(MyShape *self, MyPrototypeLine *prototype) {	
	
	
	prototype->style = MY_PROTOTYPE_STYLE_BEZIER;
}