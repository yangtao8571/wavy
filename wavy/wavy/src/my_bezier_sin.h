#ifndef __MY_BEZIER_SIN_H__
#define __MY_BEZIER_SIN_H__

#define MY_TYPE_BEZIER_SIN                  (my_bezier_sin_get_type ())
#define MY_BEZIER_SIN(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_BEZIER_SIN, MyBezierSin))
#define MY_IS_BEZIER_SIN(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_BEZIER_SIN))
#define MY_BEZIER_SIN_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), MY_TYPE_BEZIER_SIN, MyBezierSinClass))
#define MY_IS_BEZIER_SIN_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), MY_TYPE_BEZIER_SIN))
#define MY_BEZIER_SIN_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), MY_TYPE_BEZIER_SIN, MyBezierSinClass))

typedef struct _MyBezierSin			MyBezierSin;
typedef struct _MyBezierSinClass		MyBezierSinClass;
struct _MyBezierSin {
	MyBezier	bez;
};

struct _MyBezierSinClass {
	MyBezierClass parent_class;
    /* Overridable methods */
};

/* used by MY_TYPE_BEZIER_SIN */
GType my_bezier_sin_get_type (void);

/* Constructor */
void	my_bezier_sin_new_with_coordinary	(MyBezierSin * bez,
												gint id, 
												ApplicationState *appState,
												gdouble x0, gdouble y0,
												gdouble x3, gdouble y3,
												gdouble scale);
												
/* Overridable methods wrapper */

/* Non-Overridable Methods */

#endif /*__MY_BEZIER_SIN_H__*/