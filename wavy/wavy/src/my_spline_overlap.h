#ifndef __MY_SPLINE_OVERLAP_H__
#define __MY_SPLINE_OVERLAP_H__

#define MY_TYPE_SPLINE_OVERLAP                  (my_spline_overlap_get_type ())
#define MY_SPLINE_OVERLAP(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_SPLINE_OVERLAP, MySplineOverlap))
#define MY_IS_SPLINE_OVERLAP(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_SPLINE_OVERLAP))
#define MY_SPLINE_OVERLAP_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), MY_TYPE_SPLINE_OVERLAP, MySplineOverlapClass))
#define MY_IS_SPLINE_OVERLAP_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), MY_TYPE_SPLINE_OVERLAP))
#define MY_SPLINE_OVERLAP_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), MY_TYPE_SPLINE_OVERLAP, MySplineOverlapClass))

typedef struct _MySplineOverlap			MySplineOverlap;
typedef struct _MySplineOverlapClass		MySplineOverlapClass;

struct _MySplineOverlap {
	MySpline	spline;
};

struct _MySplineOverlapClass {
	MySplineClass parent_class;
    /* Overridable methods */
};

/* Constructor */

/* used by MY_TYPE_SPLINE_OVERLAP */
GType my_spline_overlap_get_type (void);

/* Overridable methods wrapper */

/* Non-Overridable Methods */

#endif /*__MY_SPLINE_OVERLAP_H__*/