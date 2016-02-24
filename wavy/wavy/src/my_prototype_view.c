#define MY_LOG_LEVEL 3

#include <assert.h>
#include <gtk/gtk.h>
#include <glib-object.h>
#include "my_constant.h"
#include "my_rectangle.h"
#include "my_point.h"
#include "my_vector.h"
#include "my_application_state.h"
#include "my_shape.h"
#include "my_curve.h"
#include "my_bezier.h"
#include "my_bezier_jm.h"
#include "my_bezier_sin.h"
#include "my_spline.h"
#include "my_spline_cyc.h"
#include "my_spline_overlap.h"
#include "my_prototype_line.h"
#include "my_prototype_view.h"


#define MY_PROTOTYPE_VIEW_GET_PRIVATE(o)		(G_TYPE_INSTANCE_GET_PRIVATE ((o), MY_TYPE_PROTOTYPE_VIEW, MyPrototypeViewPrivate))

G_DEFINE_TYPE (MyPrototypeView, my_prototype_view, G_TYPE_OBJECT);
//G_DEFINE_TYPE_WITH_PRIVATE (MyPrototypeView, my_prototype_view, G_TYPE_OBJECT)
 
typedef struct _MyPrototypeViewPrivate {
	ApplicationState *appState;
} MyPrototypeViewPrivate;

/******************************************************************************
 * static private variable 
 ******************************************************************************/
 
/******************************************************************************
 * declare static function 
 ******************************************************************************/
static void my_prototype_view_init (MyPrototypeView* self);
static void my_prototype_view_finalize (GObject* object);
static void my_prototype_view_dispose (GObject* object);
/******************************************************************************
 * implement function 
 ******************************************************************************/
static void my_prototype_view_class_init (MyPrototypeViewClass* klass)
{
	GObjectClass	*g_object_class;
	// Add private structure
	g_type_class_add_private (klass, sizeof (MyPrototypeViewPrivate));
	// Get the parent gobject class
	g_object_class = G_OBJECT_CLASS(klass);
	
    /* Hook overridable methods */
	
	/* Hook finalization functions */
	g_object_class->dispose = my_prototype_view_dispose; /* instance destructor, reverse of init */
	g_object_class->finalize = my_prototype_view_finalize; /* class finalization, reverse of class init */
}

// constructor
static void		my_prototype_view_init (MyPrototypeView* self)
{
	/* Initialize public fields */
	
	/* Initialize private fields */
	MyPrototypeViewPrivate* priv = MY_PROTOTYPE_VIEW_GET_PRIVATE (self);
	
}

static void my_prototype_view_finalize (GObject* object)
{  
  /* Always chain up to the parent class; as with dispose(), finalize()
   * is guaranteed to exist on the parent's class virtual function table
   */
	G_OBJECT_CLASS (my_prototype_view_parent_class)->finalize (object);
}

static void my_prototype_view_dispose (GObject* object)
{
	/* Reverse what was allocated by instance init */
	MyPrototypeView *self = MY_PROTOTYPE_VIEW (object);
	MyPrototypeViewPrivate* priv = MY_PROTOTYPE_VIEW_GET_PRIVATE (self);
 
 

  /* Always chain up to the parent class; there is no need to check if
   * the parent class implements the dispose() virtual function: it is
   * always guaranteed to do so
   */
	G_OBJECT_CLASS (my_prototype_view_parent_class)->dispose (object);
}


void		my_prototype_view_set_application_state		(MyPrototypeView *self, ApplicationState *appState) {
	
	
	MyPrototypeViewPrivate* priv = MY_PROTOTYPE_VIEW_GET_PRIVATE (self);
	priv->appState = appState;
}


ApplicationState *		my_prototype_view_get_application_state		(MyPrototypeView *self) {
	
	
	MyPrototypeViewPrivate* priv = MY_PROTOTYPE_VIEW_GET_PRIVATE (self);
	return priv->appState;
}


void					my_prototype_view_draw_widget		(MyPrototypeView *self, 
															MyPrototypeLine *prototype, 
															GdkWindow *window, 
															gdouble width, 
															gdouble height) {
	
	
	MyPrototypeViewPrivate* priv = MY_PROTOTYPE_VIEW_GET_PRIVATE (self);
	MyCurve *standardStraightLine;
	MyPoint *point;
	
	switch (prototype->style) {
	case MY_PROTOTYPE_STYLE_STRAIGHT_LINE:
		assert (width >= 20.);
		assert (height >= 20.);		
		standardStraightLine = (MyCurve *) g_object_new (MY_TYPE_CURVE, NULL);
		point = (MyPoint *) g_malloc (sizeof(MyPoint));	//	start point of segment
		point->x = 10.;
		point->y = 10.;
		my_curve_append_point (standardStraightLine, point);
		point = g_malloc (sizeof(MyPoint));				//	end point of segment
		point->x = width - 10.;
		point->y = height - 10.;
		my_curve_append_point (standardStraightLine, point);
		
		my_shape_from_prototype_line (MY_SHAPE (standardStraightLine), prototype);
		priv->appState->cr = gdk_cairo_create (window);
		MY_SHAPE (standardStraightLine)->appState = priv->appState;
		my_shape_draw_self (MY_SHAPE (standardStraightLine), priv->appState);
		break;
	}
}


void					my_prototype_view_draw_pixmap		(MyPrototypeView *self, 
															MyPrototypeLine *prototype, 
															GdkPixmap *window, 
															gdouble width, 
															gdouble height) {
	
	
	MyPrototypeViewPrivate* priv = MY_PROTOTYPE_VIEW_GET_PRIVATE (self);
	MyCurve *standardStraightLine;
	MyPoint *point;
	MyCurve *curve;
	MyBezier *bez;
	MyBezierJm *bezJm;
	MyBezierSin *bezSin;
	
	switch (prototype->style) {
	case MY_PROTOTYPE_STYLE_STRAIGHT_LINE:
		assert (width >= 20.);
		assert (height >= 20.);		
		standardStraightLine = (MyCurve *) g_object_new (MY_TYPE_CURVE, NULL);
		point = (MyPoint *) g_malloc (sizeof(MyPoint));	//	start point of segment
		point->x = 10.;
		point->y = 10.;
		my_curve_append_point (standardStraightLine, point);
		point = (MyPoint *) g_malloc (sizeof(MyPoint));				//	end point of segment
		point->x = width - 10.;
		point->y = height - 10.;
		my_curve_append_point (standardStraightLine, point);
		MY_SHAPE (standardStraightLine)->appState = priv->appState;
		my_shape_calculate_just_size (MY_SHAPE (standardStraightLine));
		
		my_shape_from_prototype_line (MY_SHAPE (standardStraightLine), prototype);
		priv->appState->cr = gdk_cairo_create (window);
		priv->appState->pixmap = window;
		cairo_set_source_rgb (priv->appState->cr, 1., 1., 1.);
		cairo_paint (priv->appState->cr);
		my_shape_draw_self_shadow (MY_SHAPE (standardStraightLine), priv->appState);
		my_shape_draw_self (MY_SHAPE (standardStraightLine), priv->appState);
		break;
		
	case MY_PROTOTYPE_STYLE_SPLINE:
		assert (width >= 20.);
		assert (height >= 20.);		
		curve = (MyCurve *) g_object_new (MY_TYPE_SPLINE, NULL);
		point = (MyPoint *) g_malloc (sizeof(MyPoint));	//	start point of segment
		point->x = 10.;
		point->y = 10.;
		my_curve_append_point (curve, point);
		point = (MyPoint *) g_malloc (sizeof(MyPoint));
		point->x = 10. + (width - 10. - 10.) / 3;
		point->y = 10. + (height - 10. -10.) * 2 / 3;
		my_curve_append_point (curve, point);
		point = (MyPoint *) g_malloc (sizeof(MyPoint));				//	end point of segment
		point->x = width - 10.;
		point->y = height - 10.;
		my_curve_append_point (curve, point);
		
		MY_SHAPE (curve)->appState = priv->appState;
		my_shape_from_prototype_line (MY_SHAPE (curve), prototype);
		priv->appState->cr = gdk_cairo_create (window);
		priv->appState->pixmap = window;
		cairo_set_source_rgb (priv->appState->cr, 1., 1., 1.);
		cairo_paint (priv->appState->cr);

		my_shape_fresh_dirty (MY_SHAPE (curve));
		my_shape_calculate_just_size (MY_SHAPE (curve));

		my_shape_draw_self_shadow (MY_SHAPE (curve), priv->appState);
		my_shape_draw_self (MY_SHAPE (curve), priv->appState);
		break;
		
	case MY_PROTOTYPE_STYLE_SPLINE_CYC:
		assert (width >= 20.);
		assert (height >= 20.);		
		curve = (MyCurve *) g_object_new (MY_TYPE_SPLINE_CYC, NULL);
		point = (MyPoint *) g_malloc (sizeof(MyPoint));	//	start point of segment
		point->x = 15.;
		point->y = 10.;
		my_curve_append_point (curve, point);
		point = (MyPoint *) g_malloc (sizeof(MyPoint));
		point->x = 10. + (width - 10. - 10.) / 3;
		point->y = 10. + (height - 10. -10.) * 2 / 3;
		my_curve_append_point (curve, point);
		point = (MyPoint *) g_malloc (sizeof(MyPoint));				//	end point of segment
		point->x = width - 10.;
		point->y = height - 10.;
		my_curve_append_point (curve, point);
		
		MY_SHAPE (curve)->appState = priv->appState;
		my_shape_from_prototype_line (MY_SHAPE (curve), prototype);
		priv->appState->cr = gdk_cairo_create (window);
		priv->appState->pixmap = window;
		cairo_set_source_rgb (priv->appState->cr, 1., 1., 1.);
		cairo_paint (priv->appState->cr);

		my_shape_fresh_dirty (MY_SHAPE (curve));
		my_shape_calculate_just_size (MY_SHAPE (curve));

		my_shape_draw_self_shadow (MY_SHAPE (curve), priv->appState);
		my_shape_draw_self (MY_SHAPE (curve), priv->appState);
		break;
		
	case MY_PROTOTYPE_STYLE_SPLINE_OVERLAP:
		
		assert (width >= 20.);
		assert (height >= 20.);		
		curve = (MyCurve *) g_object_new (MY_TYPE_SPLINE_OVERLAP, NULL);
		point = (MyPoint *) g_malloc (sizeof(MyPoint));	//	start point of segment
		point->x = 10.;
		point->y = 10.;
		my_curve_append_point (curve, point);
		point = (MyPoint *) g_malloc (sizeof(MyPoint));
		point->x = 10. + (width - 10. - 10.) / 3;
		point->y = 10. + (height - 10. -10.) * 2 / 3;
		my_curve_append_point (curve, point);
		point = (MyPoint *) g_malloc (sizeof(MyPoint));				//	end point of segment
		point->x = width - 10.;
		point->y = height - 10.;
		my_curve_append_point (curve, point);
		
		MY_SHAPE (curve)->appState = priv->appState;
		my_shape_from_prototype_line (MY_SHAPE (curve), prototype);
		priv->appState->cr = gdk_cairo_create (window);
		priv->appState->pixmap = window;
		cairo_set_source_rgb (priv->appState->cr, 1., 1., 1.);
		cairo_paint (priv->appState->cr);

		my_shape_fresh_dirty (MY_SHAPE (curve));
		my_shape_calculate_just_size (MY_SHAPE (curve));

		my_shape_draw_self_shadow (MY_SHAPE (curve), priv->appState);
		my_shape_draw_self (MY_SHAPE (curve), priv->appState);
		break;

	case MY_PROTOTYPE_STYLE_BEZIER:
		assert (width >= 20.);
		assert (height >= 20.);		
		bez = (MyBezier *) g_object_new (MY_TYPE_BEZIER, NULL);
		my_bezier_new_with_coordinary (bez,
										0,
										priv->appState,
										10.,
										height - 10.,
										width - 10.,
										10.,
										priv->appState->scale);
		MY_BEZIER (bez)->p1.x = width / 4;
		MY_BEZIER (bez)->p1.y = -60.;
		MY_BEZIER (bez)->p2.x = width / 4 * 3;
		MY_BEZIER (bez)->p2.y = height + 60.;
		MY_SHAPE (bez)->appState = priv->appState;
		my_shape_from_prototype_line (MY_SHAPE (bez), prototype);
		my_shape_calculate_just_size (MY_SHAPE (bez));
		priv->appState->cr = gdk_cairo_create (window);
		priv->appState->pixmap = window;
		cairo_set_source_rgb (priv->appState->cr, 1., 1., 1.);
		cairo_paint (priv->appState->cr);
		my_shape_draw_self_shadow (MY_SHAPE (bez), priv->appState);
		my_shape_draw_self (MY_SHAPE (bez), priv->appState);
		break;
	
	case MY_PROTOTYPE_STYLE_BEZIER_JM:
		assert (width >= 20.);
		assert (height >= 20.);		
		bezJm = (MyBezierJm *) g_object_new (MY_TYPE_BEZIER_JM, NULL);
		my_bezier_new_with_coordinary (MY_BEZIER (bezJm),
										0,
										priv->appState,
										10.,
										height - 10.,
										width - 10.,
										10.,
										priv->appState->scale);
		MY_BEZIER (bezJm)->p1.x = width / 4;
		MY_BEZIER (bezJm)->p1.y = -60.;
		MY_BEZIER (bezJm)->p2.x = width / 4 * 3;
		MY_BEZIER (bezJm)->p2.y = height + 60.;
		MY_SHAPE (bezJm)->appState = priv->appState;
		my_shape_from_prototype_line (MY_SHAPE (bezJm), prototype);
		my_shape_calculate_just_size (MY_SHAPE (bezJm));
		priv->appState->cr = gdk_cairo_create (window);
		priv->appState->pixmap = window;
		cairo_set_source_rgb (priv->appState->cr, 1., 1., 1.);
		cairo_paint (priv->appState->cr);
		my_shape_draw_self_shadow (MY_SHAPE (bezJm), priv->appState);
		my_shape_draw_self (MY_SHAPE (bezJm), priv->appState);
		break;
		
	case MY_PROTOTYPE_STYLE_BEZIER_SIN:
		assert (width >= 20.);
		assert (height >= 20.);		
		bezSin = (MyBezierSin *) g_object_new (MY_TYPE_BEZIER_SIN, NULL);
		my_bezier_new_with_coordinary (MY_BEZIER (bezSin),
										0,
										priv->appState,
										10.,
										10.,
										width - 13.,
										height - 13.,
										priv->appState->scale);
		MY_BEZIER (bezSin)->p1.x = width / 4;
		MY_BEZIER (bezSin)->p1.y = height / 4;
		MY_BEZIER (bezSin)->p2.x = width / 4 * 3;
		MY_BEZIER (bezSin)->p2.y = height / 4 * 3;
		MY_SHAPE (bezSin)->appState = priv->appState;
		my_shape_from_prototype_line (MY_SHAPE (bezSin), prototype);
		my_shape_calculate_just_size (MY_SHAPE (bezSin));
		priv->appState->cr = gdk_cairo_create (window);
		priv->appState->pixmap = window;
		cairo_set_source_rgb (priv->appState->cr, 1., 1., 1.);
		cairo_paint (priv->appState->cr);
		my_shape_draw_self_shadow (MY_SHAPE (bezSin), priv->appState);
		my_shape_draw_self (MY_SHAPE (bezSin), priv->appState);
		break;
		
	default:
		assert(0);
	}
}


void					my_prototype_view_fill_in_widget			(MyPrototypeView *self, MyPrototypeLine *prototype, GtkWidget *container, 
																	gdouble containerWidth, gdouble containerHeight, gdouble imageWidth, gdouble imageHeight) {
																		
																		
	
}