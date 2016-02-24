#ifndef __MY_SPLINE_CYC_H__
#define __MY_SPLINE_CYC_H__

#define MY_TYPE_SPLINE_CYC                  (my_spline_cyc_get_type ())
#define MY_SPLINE_CYC(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_SPLINE_CYC, MySplineCyc))
#define MY_IS_SPLINE_CYC(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_SPLINE_CYC))
#define MY_SPLINE_CYC_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), MY_TYPE_SPLINE_CYC, MySplineCycClass))
#define MY_IS_SPLINE_CYC_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), MY_TYPE_SPLINE_CYC))
#define MY_SPLINE_CYC_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), MY_TYPE_SPLINE_CYC, MySplineCycClass))

typedef struct _MySplineCyc			MySplineCyc;
typedef struct _MySplineCycClass		MySplineCycClass;
struct _MySplineCyc {
	MyCurve	curve;
};

struct _MySplineCycClass {
	MyCurveClass parent_class;
    /* Overridable methods */
};

/* Constructor */

/* used by MY_TYPE_SPLINE_CYC */
GType my_spline_cyc_get_type (void);

/* Overridable methods wrapper */

/* Non-Overridable Methods */
void		my_spline_cyc_set_points_completed	(gboolean isPointsCompleted);
gboolean	my_spline_cyc_get_points_completed	(void);
void		my_spline_cyc_calculate_spline_points	(MySplineCyc *splineCyc);

#endif /*__MY_SPLINE_CYC_H__*/