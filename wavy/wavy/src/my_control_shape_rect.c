#define MY_LOG_LEVEL 3

#include <gtk/gtk.h>
#include <glib-object.h>
#include <stdlib.h>
#include <stdio.h>
#include "my_constant.h"
#include "my_rectangle.h"
#include "my_prototype_line.h"
#include "my_prototype_manager.h"
#include "my_application_state.h"
#include "my_point.h"
#include "my_debug.h"
#include "my_shape.h"
#include "my_vector.h"
#include "my_util.h"
#include "my_control_shape_rect.h"

#define MY_CONTROL_SHAPE_RECT_GET_PRIVATE(o)		(G_TYPE_INSTANCE_GET_PRIVATE ((o), MY_TYPE_CONTROL_SHAPE_RECT, MyControlShapeRectPrivate))

G_DEFINE_TYPE (MyControlShapeRect, my_control_shape_rect, MY_TYPE_SHAPE);
//G_DEFINE_TYPE_WITH_PRIVATE (MyControlShapeRect, my_control_shape_rect, G_TYPE_OBJECT)
 
typedef struct _MyControlShapeRectPrivate {
	int	nothing;
} MyControlShapeRectPrivate;

/******************************************************************************
 * declare static function 
 ******************************************************************************/
static void 		my_control_shape_rect_init (MyControlShapeRect* self);
static void 		my_control_shape_rect_finalize (GObject* object);
static void 		my_control_shape_rect_dispose (GObject* object);
static void			calculate_just_size	(MyShape *self);
static GString	*	real_to_string	(MyShape *self);
static gboolean		real_is_chosen	(MyShape *self, gdouble x, gdouble y, gdouble strokeWidth, ApplicationState *appState);
static void			real_draw_self	(MyShape* self, ApplicationState *appState);
static void			shift					(MyShape *self, gdouble dx, gdouble dy);
static void			shift_to				(MyShape *self, gdouble x, gdouble y);
static void			shift_self_only			(MyShape *self, gdouble dx, gdouble dy);
static void			shift_to_self_only		(MyShape *self, gdouble x, gdouble y);
static void			update_with_scale		(MyShape *self, gdouble scale);
static gboolean		double_click_callback	(MyShape *self, gdouble x, gdouble y, MyMouse mouse, guint keyMask, gpointer userData);
/******************************************************************************
 * implement function 
 ******************************************************************************/
static void my_control_shape_rect_class_init (MyControlShapeRectClass* klass)
{
	GObjectClass	*g_object_class;
	MyShapeClass	*myShapeClass;
	/* Add private structure */
	g_type_class_add_private (klass, sizeof (MyControlShapeRectPrivate));
	/* Get the parent gobject class */
	g_object_class = G_OBJECT_CLASS(klass);
	
    /* Hook overridable methods */
	myShapeClass = MY_SHAPE_CLASS(klass);
	myShapeClass->to_string = real_to_string;
	myShapeClass->is_chosen = real_is_chosen;
	myShapeClass->draw_self = real_draw_self;
	myShapeClass->shift 	= shift;
	myShapeClass->shift_to	= shift_to;
	myShapeClass->shift_self_only 	= shift_self_only;
	myShapeClass->shift_to_self_only 	= shift_to_self_only;
	myShapeClass->update_with_scale = update_with_scale;
	myShapeClass->calculate_just_size = calculate_just_size;
	myShapeClass->double_click_callback = double_click_callback;
	
	/* Hook finalization functions */
	g_object_class->dispose = my_control_shape_rect_dispose; /* instance destructor, reverse of init */
	g_object_class->finalize = my_control_shape_rect_finalize; /* class finalization, reverse of class init */
}

// constructor
static void		my_control_shape_rect_init (MyControlShapeRect* self)
{
	/* Initialize public fields */
	MY_SHAPE (self)->isControlShape	=	TRUE;
	MY_SHAPE (self)->isEditable 	=	FALSE;
	MY_SHAPE (self)->isFocusable	=	FALSE;
	MY_SHAPE (self)->isShadowing	=	FALSE;
	MY_SHAPE (self)->isStrokeWidthChangable	=	FALSE;
	MY_SHAPE (self)->isColorChangable		=	FALSE;
	MY_SHAPE (self)->strokeWidth	=	1.0;
	self->width = 8;
	self->height = 8;
	self->isCanvasControl	=	FALSE;
	self->isAutoShiftSelf	=	TRUE;
	self->isFilling	=	TRUE;
	self->bgR	=	0;
	self->bgG	=	1;
	self->bgB	=	0;
	self->bgA	=	1;
	
	/* Initialize private fields */
	//MyControlShapeRectPrivate* priv = MY_CONTROL_SHAPE_RECT_GET_PRIVATE (self);
}

static void my_control_shape_rect_finalize (GObject* object)
{  
	my_debug ("finalizing rect control shape %#x", object);
  /* Always chain up to the parent class; as with dispose(), finalize()
   * is guaranteed to exist on the parent's class virtual function table
   */
	G_OBJECT_CLASS (my_control_shape_rect_parent_class)->finalize (object);
}

static void my_control_shape_rect_dispose (GObject* object)
{
	
	/* Reverse what was allocated by instance init */
	MyControlShapeRect *self = MY_CONTROL_SHAPE_RECT (object);
	MyControlShapeRectPrivate* priv = MY_CONTROL_SHAPE_RECT_GET_PRIVATE (self);

	my_debug ("disposing rect control shape %#x", object);

  /* Always chain up to the parent class; there is no need to check if
   * the parent class implements the dispose() virtual function: it is
   * always guaranteed to do so
   */
	G_OBJECT_CLASS (my_control_shape_rect_parent_class)->dispose (object);
}


MyControlShapeRect *	my_control_shape_rect_new	(ApplicationState *appState,
													gdouble x, 
													gdouble y, 
													gdouble width, 
													gdouble height, 
													MyShape *target, 
													MyControlShapeRectOnShift onShift,
													gpointer userData) {
												
	MyControlShapeRect *rectShape = (MyControlShapeRect *) g_object_new (MY_TYPE_CONTROL_SHAPE_RECT, NULL);
	MY_SHAPE (rectShape)->appState = appState;
	rectShape->x = x;
	rectShape->y = y;
	/*
	rectShape->standardX = (rectShape->x + rectShape->width / 2) / appState->scale - rectShape->width / 2;
	rectShape->standardY = (rectShape->y + rectShape->height / 2) / appState->scale - rectShape->height / 2;
	*/
	rectShape->width = width;
	rectShape->height = height;
	rectShape->target = target;
	rectShape->onShift = onShift;
	rectShape->userData = userData;
	return rectShape;
}


static GString	*	real_to_string	(MyShape *self) {
	return NULL;
}

static gboolean		real_is_chosen	(MyShape *self, gdouble x, gdouble y, gdouble strokeWidth, ApplicationState *appState) {

	gboolean isInRect;
	MyControlShapeRect *rectShape = MY_CONTROL_SHAPE_RECT(self);
	if (self->isShowing) {
		if (my_util_is_point_in_rect (rectShape->x, rectShape->y, rectShape->width, rectShape->height, x, y)) {
			isInRect = TRUE;
		} else {
			isInRect = FALSE;
		}	
	} else
		isInRect = FALSE;
	my_debug ("control rect %#x is chosen? %d", self, isInRect);
	return isInRect;
}

static void			real_draw_self	(MyShape* self, ApplicationState *appState) {

	cairo_t *cr = appState->cr;
	MyControlShapeRect *controlShapeRect = MY_CONTROL_SHAPE_RECT(self);
	
	my_debug ("drawing control rect %#x ...", self);
	cairo_set_dash (cr, self->orignDashes, self->dashCount, self->dashOffset);
	cairo_set_line_width (cr, self->strokeWidth);
	
	cairo_rectangle (cr, 
					controlShapeRect->x + appState->orignX,
					controlShapeRect->y + appState->orignY,
					controlShapeRect->width,
					controlShapeRect->height);
	
	if (controlShapeRect->isFilling) {
		cairo_set_source_rgba (cr, controlShapeRect->bgR, controlShapeRect->bgG, controlShapeRect->bgB, controlShapeRect->bgA);
		cairo_fill_preserve (cr);
	}
	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_stroke (cr);
	
	my_debug ("drawing control rect %#x done.", self);
}


static void			shift		(MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {

	
	MyControlShapeRect *controlShapeRect = MY_CONTROL_SHAPE_RECT (self);
	my_debug ("control shape %#x shift ...", self);
	

	if (controlShapeRect->isAutoShiftSelf)
		shift_self_only (self, dx, dy);
	
	if (controlShapeRect->onShift) {
		if (controlShapeRect->onShift (controlShapeRect, source, dx, dy, mouse, keyMask, userData)) { // TRUE means complete handled, so we return
			return;
		}
	}
	/*
	// support the very simple case
	if (controlShapeRect->pointTarget) {
		controlShapeRect->pointTarget->x += dx;
		controlShapeRect->pointTarget->y += dy;
	}
	if (controlShapeRect->vectorTarget) {
		controlShapeRect->vectorTarget->x += dx;
		controlShapeRect->vectorTarget->y += dy;
	}
	
	//my_shape_calculate_just_size (controlShapeRect->target);
	*/
}


static void			shift_to		(MyShape *self, MyShape *source, gdouble x, gdouble y, MyMouse mouse, guint keyMask, gpointer userData) {


	MyControlShapeRect *controlShapeRect = MY_CONTROL_SHAPE_RECT (self);
	my_debug ("control shape %#x shift_to ...", self);
	
	if (controlShapeRect->isAutoShiftSelf)
		shift_to_self_only (self, x, y);
	
	if (controlShapeRect->onShift) {
		if (controlShapeRect->onShift (controlShapeRect, source, x, y, mouse, keyMask, userData)) { // TRUE means complete handled, so we return
			return;
		}
	}
	
	/*
	if (controlShapeRect->pointTarget) {
		controlShapeRect->pointTarget->x = x;
		controlShapeRect->pointTarget->y = y;
	}
	if (controlShapeRect->vectorTarget) {
		controlShapeRect->vectorTarget->x = x;
		controlShapeRect->vectorTarget->y = y;
	}
	*/
	my_shape_calculate_just_size (controlShapeRect->target);
}


static void			shift_self_only		(MyShape *self, gdouble dx, gdouble dy) {
	

	MyControlShapeRect *controlShapeRect = MY_CONTROL_SHAPE_RECT (self);
	my_debug ("control shape %#x shift_self_only %f,%f ...", self, dx, dy);

	controlShapeRect->x += dx;
	controlShapeRect->y += dy;
	/*
	controlShapeRect->standardX = (controlShapeRect->x + controlShapeRect->width / 2) / self->appState->scale - controlShapeRect->width / 2;
	controlShapeRect->standardY = (controlShapeRect->y + controlShapeRect->height / 2) / self->appState->scale - controlShapeRect->height / 2;
	*/
	my_debug ("control shape %#x shift_self_only %f,%f", self, dx, dy);
}


static void			shift_to_self_only		(MyShape *self, gdouble x, gdouble y) {

	MyControlShapeRect *controlShapeRect = MY_CONTROL_SHAPE_RECT (self);
	controlShapeRect->x = x;
	controlShapeRect->y = y;
	/*
	controlShapeRect->standardX = (controlShapeRect->x + controlShapeRect->width / 2) / self->appState->scale - controlShapeRect->width / 2;
	controlShapeRect->standardY = (controlShapeRect->y + controlShapeRect->height / 2) / self->appState->scale - controlShapeRect->height / 2;
	*/
	my_debug ("shift_to_self_only rect control shape %#x %f,%f", self, x, y);
}


static void		update_with_scale		(MyShape *self, gdouble scale) {

	MyControlShapeRect *controlShapeRect = MY_CONTROL_SHAPE_RECT (self);
	
	if (!controlShapeRect->isCanvasControl) {
		/*
		my_debug ("control shape %#x standard %f,%f", controlShapeRect->standardX, controlShapeRect->standardY);
		controlShapeRect->x = (controlShapeRect->standardX + controlShapeRect->width / 2) * scale - controlShapeRect->width / 2;
		controlShapeRect->y = (controlShapeRect->standardY + controlShapeRect->height / 2) * scale - controlShapeRect->height / 2;
		*/
		controlShapeRect->x = (controlShapeRect->x + controlShapeRect->width / 2) / self->appState->oldScale * scale - controlShapeRect->width / 2;
		controlShapeRect->y = (controlShapeRect->y + controlShapeRect->height / 2) / self->appState->oldScale * scale - controlShapeRect->height / 2;
	}
}


void		calculate_just_size		(MyShape *self) {
}


static gboolean		double_click_callback	(MyShape *self, gdouble x, gdouble y, MyMouse mouse, guint keyMask, gpointer userData) {


	MyControlShapeRect *rect = MY_CONTROL_SHAPE_RECT (self);
	if (rect->target && MY_SHAPE_GET_CLASS (rect->target)->double_click_callback) {
		MY_SHAPE_GET_CLASS (rect->target)->double_click_callback (rect->target, x, y, mouse, keyMask, self);
	}
}