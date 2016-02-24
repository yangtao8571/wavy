#ifndef __MY_PROTOTYPE_VIEW_H__
#define __MY_PROTOTYPE_VIEW_H__

#define MY_TYPE_PROTOTYPE_VIEW                  (my_prototype_view_get_type ())
#define MY_PROTOTYPE_VIEW(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_PROTOTYPE_VIEW, MyPrototypeView))
#define MY_IS_PROTOTYPE_VIEW(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_PROTOTYPE_VIEW))
#define MY_PROTOTYPE_VIEW_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), MY_TYPE_PROTOTYPE_VIEW, MyPrototypeViewClass))
#define MY_IS_PROTOTYPE_VIEW_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), MY_TYPE_PROTOTYPE_VIEW))
#define MY_PROTOTYPE_VIEW_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), MY_TYPE_PROTOTYPE_VIEW, MyPrototypeViewClass))



typedef struct _ApplicationState	ApplicationState;
typedef struct _MyCurve				MyCurve;
typedef struct _MyPoint				MyPoint;
typedef struct _MyPrototypeLine		MyPrototypeLine;
typedef struct _MyPrototypeView		MyPrototypeView;
typedef struct _MyPrototypeViewClass	MyPrototypeViewClass;

struct _MyPrototypeView {
	GObject parent_instance;
};

struct _MyPrototypeViewClass {
	GObjectClass parent_class;
    /* Overridable methods */
};

/* used by MY_TYPE_PROTOTYPE_VIEW */
GType my_prototype_view_get_type (void);

/* Overridable methods wrapper */

/* Non-Overridable Methods */

void					my_prototype_view_set_application_state		(MyPrototypeView *self, ApplicationState *appState);
ApplicationState *		my_prototype_view_get_application_state		(MyPrototypeView *self);
void					my_prototype_view_draw_widget				(MyPrototypeView *self, MyPrototypeLine *prototype, GdkWindow *window, gdouble width, gdouble height);
void					my_prototype_view_draw_pixmap				(MyPrototypeView *self, MyPrototypeLine *prototype, GdkPixmap *window, gdouble width, gdouble height);
void					my_prototype_view_fill_in_widget			(MyPrototypeView *self, MyPrototypeLine *prototype, GtkWidget *container, 
																	gdouble containerWidth, gdouble containerHeight, gdouble imageWidth, gdouble imageHeight);

#endif /*__MY_PROTOTYPE_VIEW_H__*/