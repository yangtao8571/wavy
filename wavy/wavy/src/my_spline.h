#ifndef __MY_SPLINE_H__
#define __MY_SPLINE_H__

#define MY_TYPE_SPLINE                  (my_spline_get_type ())
#define MY_SPLINE(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_SPLINE, MySpline))
#define MY_IS_SPLINE(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_SPLINE))
#define MY_SPLINE_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), MY_TYPE_SPLINE, MySplineClass))
#define MY_IS_SPLINE_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), MY_TYPE_SPLINE))
#define MY_SPLINE_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), MY_TYPE_SPLINE, MySplineClass))

typedef struct _MySpline			MySpline;
typedef struct _MySplineClass		MySplineClass;
struct _MySpline {
	MyCurve	curve;
};

struct _MySplineClass {
	MyCurveClass parent_class;
    /* Overridable methods */
};

/* Constructor */

/* used by MY_TYPE_SPLINE */
GType my_spline_get_type (void);

/* Overridable methods wrapper */

/* Non-Overridable Methods */
void		my_spline_set_points_completed	(gboolean isPointsCompleted);
gboolean	my_spline_get_points_completed	(void);
void		my_spline_calculate_spline_points	(MySpline *spline);
guint		my_spline_get_points_count	(MySpline *self);
MyPoint	*	my_spline_peek_nth			(MySpline *self, guint i);

#endif /*__MY_SPLINE_H__*/