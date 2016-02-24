#ifndef __MY_SHAPE_H__
#define __MY_SHAPE_H__

#define MY_TYPE_SHAPE                  (my_shape_get_type ())
#define MY_SHAPE(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_SHAPE, MyShape))
#define MY_IS_SHAPE(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_SHAPE))
#define MY_SHAPE_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), MY_TYPE_SHAPE, MyShapeClass))
#define MY_IS_SHAPE_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), MY_TYPE_SHAPE))
#define MY_SHAPE_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), MY_TYPE_SHAPE, MyShapeClass))


typedef struct _MyRectangle			MyRectangle;
typedef struct _ApplicationState	ApplicationState;
typedef struct _MyPrototypeLine		MyPrototypeLine;
typedef struct _MyLayer			MyLayer;
typedef struct _MyShape			MyShape;
typedef struct _MyShapeClass	MyShapeClass;

struct _MyShape {
	GObject 	parent_instance;
	ApplicationState *appState;
	gint		id;
	GdkGC		*gc;
	GTimeZone 	*timeZone;
	GDateTime 	*createTimestamp;
	MyRectangle justSize;
	gboolean	isControlShape;
	gboolean	isPicture;
	gboolean	isEnabled;
	gboolean	isShowable;
	gboolean	isShowing;
	gboolean	isFocusable;
	gboolean	isFocusing;
	gboolean	isEditable;
	gboolean	isEditing;
	gboolean	isDeletable;
	gboolean	isShiftable;
	gboolean	isDragable;
	gboolean	isShadowing;
	gboolean	isStrokeWidthChangable;	// 能否调粗细
	gboolean	isColorChangable;
	gdouble		red;
	gdouble		green;
	gdouble		blue;
	gdouble		alpha;
	gdouble		strokeWidth;
	gboolean	isDashing;
	gdouble		*orignDashes;
	gdouble		*dashes;
	gint		dashCount;
	gdouble		dashOffset;
	gdouble		isUseShiftTo;	// 拖动时调用函数
	GdkPixbuf	*shadowPixbuf;	
	cairo_surface_t *shadowSurface;	// 阴影相关
	gboolean	isShadowDirty;
	gdouble		shadowX;
	gdouble		shadowY;
	gdouble		shadowStride;
	gdouble		shadowWidth;	// shape width + padding
	gdouble		shadowHeight;
	gdouble		shadowOrignWidth;
	gdouble		shadowOrignHeight;
	gdouble		shadowPadding;
	gdouble		shadowDeltaX;
	gdouble		shadowDeltaY;
	gint		boxRadius;
	gdouble		angle;	// x轴到axis轴的弧度矢量,从x轴向y轴为正,初始为0
	gboolean	needMarkDirtyWhenShift;	// 当平移时标记dirty
	gboolean	isDirty;	// todo:delete
};

struct _MyShapeClass {
	GObjectClass	parent_class;
    /* Overridable methods */
	void		(*draw_self)				(MyShape* self, ApplicationState *appState);
	void		(*draw_self_shadow)			(MyShape* self, ApplicationState *appState);
	void		(*draw_self_shadow_debug)	(MyShape* self);
	GString *	(*to_string)				(MyShape *self);
	gboolean	(*is_chosen)				(MyShape *self, gdouble x, gdouble y, gdouble strokeWidth, ApplicationState *appState);
	void		(*shift)					(MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData);
	void		(*shift_to)					(MyShape *self, MyShape *source, gdouble x, gdouble y, MyMouse mouse, guint keyMask, gpointer userData);
	void		(*shift_self_only)			(MyShape *self, gdouble dx, gdouble dy);
	void		(*shift_to_self_only)		(MyShape *self, gdouble x, gdouble y);
	gboolean	(*is_out_of_canvas)			(MyShape *self, ApplicationState *appState);
	void		(*init_control_shapes)		(MyShape* self, MyLayer *layer);
	void		(*hide_control_shapes)		(MyShape *self);
	void 		(*delete_control_shapes)	(MyShape *self);
	void 		(*delete_control_shapes_from_layer)	(MyShape *self, MyLayer *layer);
	gboolean	(*has_control_shapes) 		(MyShape *self);
	void		(*calculate_just_size)		(MyShape *self);
	void		(*update_with_scale)		(MyShape *self, gdouble scale);
	void		(*update_shadow_rect)		(MyShape *self);
	void		(*on_shadow_dirty)			(MyShape *self);
	void		(*from_prototype_line)		(MyShape *self, MyPrototypeLine *prototype);
	void		(*to_prototype_line)		(MyShape *self, MyPrototypeLine *prototype);
	void		(*draw_trumbnail)			(MyShape *self, gdouble width, gdouble height);
	gboolean	(*double_click_callback)	(MyShape *self, gdouble x, gdouble y, MyMouse mouse, guint keyMask, gpointer userData);
	void		(*fresh_dirty)				(MyShape *self);
};

/* used by MY_TYPE_SHAPE */
GType my_shape_get_type (void);

/* Overridable methods wrapper */
void		my_shape_draw_self				(MyShape *self, ApplicationState *appState);
void		my_shape_draw_self_shadow		(MyShape *self, ApplicationState *appState);
GString *	my_shape_to_string				(MyShape *self);
gboolean	my_shape_is_chosen				(MyShape *self, gdouble x, gdouble y, gdouble extraChosenPadding, ApplicationState *appState);
void		my_shape_shift					(MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData);
void		my_shape_shift_to				(MyShape *self, MyShape *source, gdouble x, gdouble y, MyMouse mouse, guint keyMask, gpointer userData);
void		my_shape_shift_self_only		(MyShape *self, gdouble dx, gdouble dy);
void		my_shape_shift_to_self_only		(MyShape *self, gdouble x, gdouble y);
gboolean	my_shape_is_out_of_canvas		(MyShape *self, ApplicationState *appState);
void 		my_shape_init_control_shapes	(MyShape *self, MyLayer *layer);
void		my_shape_hide_control_shapes	(MyShape *self);
void 		my_shape_delete_control_shapes	(MyShape *self);
void 		my_shape_delete_control_shapes_from_layer	(MyShape *self, MyLayer *layer);
gboolean	my_shape_has_control_shapes 	(MyShape *self);
void		my_shape_calculate_just_size	(MyShape *self);
void		my_shape_update_with_scale		(MyShape *self, gdouble scale);
void		my_shape_update_shadow_rect		(MyShape *self);
void		my_shape_on_shadow_dirty		(MyShape *self);
void		my_shape_to_prototype_line		(MyShape *self, MyPrototypeLine *prototype);
void		my_shape_from_prototype_line	(MyShape *self, MyPrototypeLine *prototype);


/* Non-Overridable Methods */
void 		my_shape_draw_dash_rectangle	(MyShape* self, ApplicationState *appState);
void		my_shape_expand_just_size		(MyShape *self, gdouble x, gdouble y);
void		my_shape_set_dash				(MyShape *self, gdouble dashes[], gint dashCount);
void		my_shape_set_dash_by_str		(MyShape *self, gchar *orignDashStr);
gboolean	my_shape_is_dirty				(MyShape *self);
void		my_shape_shadow_mark_dirty		(MyShape *self);
void		my_shape_mark_dirty				(MyShape *self);
void		my_shape_fresh_dirty			(MyShape *self);

#endif /*__MY_SHAPE_H__*/