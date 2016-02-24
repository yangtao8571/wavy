#ifndef __MY_CONTROL_SHAPE_RECT_H__
#define __MY_CONTROL_SHAPE_RECT_H__

#define MY_TYPE_CONTROL_SHAPE_RECT                  (my_control_shape_rect_get_type ())
#define MY_CONTROL_SHAPE_RECT(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_CONTROL_SHAPE_RECT, MyControlShapeRect))
#define MY_IS_CONTROL_SHAPE_RECT(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_CONTROL_SHAPE_RECT))
#define MY_CONTROL_SHAPE_RECT_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), MY_TYPE_CONTROL_SHAPE_RECT, MyControlShapeRectClass))
#define MY_IS_CONTROL_SHAPE_RECT_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), MY_TYPE_CONTROL_SHAPE_RECT))
#define MY_CONTROL_SHAPE_RECT_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), MY_TYPE_CONTROL_SHAPE_RECT, MyControlShapeRectClass))

typedef struct _MyControlShapeRect			MyControlShapeRect;
typedef struct _MyControlShapeRectClass		MyControlShapeRectClass;

struct _MyControlShapeRect {
	MyShape 	parent_instance;
	/*
	gdouble 	standardX;	// unscaled coordinary
	gdouble		standardY;
	*/
	gdouble		x;
	gdouble		y;
	gdouble		width;
	gdouble		height;
	MyShape *	target;
	MyPoint	*	pointTarget;
	MyVector*	vectorTarget;
	gboolean	isCanvasControl;
	gboolean	isAutoShiftSelf;
	void		*userData;
	gboolean	(*onShift) (MyControlShapeRect *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, void *userData);
	gboolean	isFilling;
	gdouble		bgR;
	gdouble		bgG;
	gdouble		bgB;
	gdouble		bgA;
};
struct _MyControlShapeRectClass {
	MyShapeClass	parent_class;
    /* Overridable methods */
};

/* used by MY_TYPE_CONTROL_SHAPE_RECT */
GType my_control_shape_rect_get_type (void);

/* Constructor */
typedef gboolean (*MyControlShapeRectOnShift) (MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData);

MyControlShapeRect *my_control_shape_rect_new (ApplicationState *appState,
												gdouble x, 
												gdouble y, 
												gdouble width, 
												gdouble height, 
												MyShape *target, 
												MyControlShapeRectOnShift onShift,
												gpointer userData);

/* Overridable methods wrapper */

/* Non-Overridable Methods */

#endif /*__MY_CONTROL_SHAPE_RECT_H__*/