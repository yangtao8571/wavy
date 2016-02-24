#ifndef __MY_LAYER_H__
#define __MY_LAYER_H__

#define MY_TYPE_LAYER                  (my_layer_get_type ())
#define MY_LAYER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_LAYER, MyLayer))
#define MY_IS_LAYER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_LAYER))
#define MY_LAYER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), MY_TYPE_LAYER, MyLayerClass))
#define MY_IS_LAYER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), MY_TYPE_LAYER))
#define MY_LAYER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), MY_TYPE_LAYER, MyLayerClass))

typedef struct _MyShape			MyShape;
typedef struct _MyLayer			MyLayer;
typedef struct _MyLayerClass	MyLayerClass;

struct _MyLayer {
	GObject parent_instance;
	gint	id;
	gint	z;		// 层的纵向位置
	gint	latestId; // 最后分配给layer中组件的id
};

struct _MyLayerClass {
	GObjectClass parent_class;
    /* Overridable methods */
};

/* used by MY_TYPE_LAYER */
GType my_layer_get_type (void);

/* Overridable methods wrapper */
void		my_layer_add_shape		(MyLayer* self, MyShape* shape);
void		my_layer_add_head_shape		(MyLayer* self, MyShape* shape);

/* Non-Overridable Methods */
MyShape *	my_layer_peek_nth		(MyLayer *self, guint n);
guint		my_layer_shape_count	(MyLayer *self);
MyShape *	my_layer_pop_shape		(MyLayer *self, MyShape *shape);
void		my_layer_delete_shape	(MyLayer *self, MyShape *shape);
void		my_layer_delete_shape_only		(MyLayer *self, MyShape *shape);
void		my_layer_set_select_shape		(MyLayer *self, MyShape *shape);
MyShape *	my_layer_get_select_shape		(MyLayer *self);
void		my_layer_delete_selected_shape (MyLayer *self);
MyShape *	my_layer_hit_shape					(MyLayer *self, gdouble x, gdouble y, gint selectWidth, ApplicationState *appState);
void		my_layer_clear						(MyLayer *self);
void 		my_layer_delete_all_control_shapes	(MyLayer *self);
void		my_layer_shift_top_shape			(MyLayer *self, MyShape *shape);
void		my_layer_shift_up_shape				(MyLayer *self, MyShape *shape);
void		my_layer_shift_down_shape			(MyLayer *self, MyShape *shape);
void		my_layer_shift_bottom_shape			(MyLayer *self, MyShape *shape);

#endif /*__MY_LAYER_H__*/