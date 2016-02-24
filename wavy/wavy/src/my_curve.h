#ifndef __MY_CURVE_H__
#define __MY_CURVE_H__

#define MY_TYPE_CURVE                  (my_curve_get_type ())
#define MY_CURVE(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_CURVE, MyCurve))
#define MY_IS_CURVE(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_CURVE))
#define MY_CURVE_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), MY_TYPE_CURVE, MyCurveClass))
#define MY_IS_CURVE_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), MY_TYPE_CURVE))
#define MY_CURVE_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), MY_TYPE_CURVE, MyCurveClass))

typedef struct _MyCurve			MyCurve;
typedef struct _MyCurveClass	MyCurveClass;
struct _MyCurve {
	MyShape shape;
};

struct _MyCurveClass {
	MyShapeClass parent_class;
    /* Overridable methods */
};

/* used by MY_TYPE_CURVE */
GType my_curve_get_type (void);

/* Overridable methods wrapper */
void	my_curve_append_point	(MyCurve *self, MyPoint *point);

/* Non-Overridable Methods */
MyPoint *	my_curve_peek_nth		(MyCurve *self, guint n);
guint		my_curve_point_count	(MyCurve *self);
void		my_curve_update_last_point	(MyCurve *self, gdouble x, gdouble y);
void		my_curve_insert_point_before(MyCurve *self, MyPoint *point, MyPoint *willInsert);
void		my_curve_delete_point		(MyCurve *self, MyPoint *point);

#endif /*__MY_CURVE_H__*/