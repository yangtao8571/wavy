#ifndef __MY_BEZIER_H__
#define __MY_BEZIER_H__

#define MY_TYPE_BEZIER                  (my_bezier_get_type ())
#define MY_BEZIER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_BEZIER, MyBezier))
#define MY_IS_BEZIER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_BEZIER))
#define MY_BEZIER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), MY_TYPE_BEZIER, MyBezierClass))
#define MY_IS_BEZIER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), MY_TYPE_BEZIER))
#define MY_BEZIER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), MY_TYPE_BEZIER, MyBezierClass))


typedef struct _MyControlShapeRect	MyControlShapeRect;
typedef struct _MyBezier			MyBezier;
typedef struct _MyBezierClass		MyBezierClass;
struct _MyBezier {
	MyShape		shape;
	/*
	MyVector	standardP0;
	MyVector	standardP1;
	MyVector	standardP2;
	MyVector	standardP3;
	*/
	MyVector	p0;
	MyVector	p1;
	MyVector	p2;
	MyVector	p3;	
	MyControlShapeRect	* p0ControlRect;
	MyControlShapeRect	* p1ControlRect;
	MyControlShapeRect	* p2ControlRect;
	MyControlShapeRect	* p3ControlRect;
};

struct _MyBezierClass {
	MyShapeClass parent_class;
    /* Overridable methods */
};

/* used by MY_TYPE_BEZIER */
GType my_bezier_get_type (void);

/* Constructor */
void	my_bezier_new					(MyBezier *bez, gint id, ApplicationState *appState);
void	my_bezier_new_with_coordinary	(MyBezier *bez, gint id, ApplicationState *appState,
										gdouble x0, gdouble y0,
										gdouble x3, gdouble y3,
										gdouble scale);
										
/* Overridable methods wrapper */

/* Non-Overridable Methods */
void		my_bezier_update_point		(MyBezier *self, gint pointNum, gdouble x, gdouble y);

#endif /*__MY_BEZIER_H__*/