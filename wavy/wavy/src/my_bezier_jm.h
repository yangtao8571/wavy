#ifndef __MY_BEZIER_JM_H__
#define __MY_BEZIER_JM_H__

#define MY_TYPE_BEZIER_JM                  (my_bezier_jm_get_type ())
#define MY_BEZIER_JM(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_BEZIER_JM, MyBezierJm))
#define MY_IS_BEZIER_JM(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_BEZIER_JM))
#define MY_BEZIER_JM_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), MY_TYPE_BEZIER_JM, MyBezierJmClass))
#define MY_IS_BEZIER_JM_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), MY_TYPE_BEZIER_JM))
#define MY_BEZIER_JM_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), MY_TYPE_BEZIER_JM, MyBezierJmClass))

typedef struct _MyBezierJm			MyBezierJm;
typedef struct _MyBezierJmClass		MyBezierJmClass;
struct _MyBezierJm {
	MyBezier	bez;
};

struct _MyBezierJmClass {
	MyBezierClass parent_class;
    /* Overridable methods */
};

/* Constructor */
void	my_bezier_jm_new_with_coordinary	(MyBezierJm * bez,
												gint id, 
												ApplicationState *appState,
												gdouble x0, gdouble y0,
												gdouble x3, gdouble y3,
												gdouble scale);

/* used by MY_TYPE_BEZIER_JM */
GType my_bezier_jm_get_type (void);

/* Overridable methods wrapper */

/* Non-Overridable Methods */

#endif /*__MY_BEZIER_JM_H__*/