#ifndef __MY_PROTOTYPE_LINE_H__
#define __MY_PROTOTYPE_LINE_H__

#define MY_TYPE_PROTOTYPE_LINE                  (my_prototype_line_get_type ())
#define MY_PROTOTYPE_LINE(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_PROTOTYPE_LINE, MyPrototypeLine))
#define MY_IS_PROTOTYPE_LINE(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_PROTOTYPE_LINE))
#define MY_PROTOTYPE_LINE_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), MY_TYPE_PROTOTYPE_LINE, MyPrototypeLineClass))
#define MY_IS_PROTOTYPE_LINE_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), MY_TYPE_PROTOTYPE_LINE))
#define MY_PROTOTYPE_LINE_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), MY_TYPE_PROTOTYPE_LINE, MyPrototypeLineClass))


typedef struct _MyPrototypeLine		MyPrototypeLine;
typedef struct _MyPrototypeLineClass	MyPrototypeLineClass;


struct _MyPrototypeLine {
	GObject parent_instance;
	
	MyPrototypeStyle	style;
//	GtkWidget	*thumbnailWidget;
	gdouble		red;
	gdouble		green;
	gdouble		blue;
	gdouble		alpha;
	gdouble		strokeWidth;
	gboolean	isDashing;
	gchar	*	orignDashesStr;
	gdouble		dashOffset;
	gboolean	isShadowing;
	gdouble		shadowDeltaX;
	gdouble		shadowDeltaY;
		
	gdouble		jmOffset;	// 第一个尖的起点
	gdouble		jmInterval;	// 上一个尖的起点到下一个尖的起点之间的距离
	gdouble		jmEdgeLen;	// 边长
	gint		jmSide;		// 尖的方向
	
	gdouble		offset;			// 第一个尖的起点
	gdouble		interval;		// 每个多少距离画一个正弦点
	gdouble		segmentCount;	// 把曲线近似为折线段的线段数
	gdouble		amplitude;		// 振幅
	gdouble		phase;			// 相位
	gdouble		zhouqi;			// 周期

	gint		segmentCountPerHemite;	// spline的参数
	gdouble		dPt1_x;
	gdouble		dPt1_y;
	gdouble		dPtn_x;
	gdouble		dPtn_y;

	gdouble		overlapFillR;	// spline超覆线参数。半圆填充颜色
	gdouble		overlapFillG;
	gdouble		overlapFillB;
	gdouble		overlapFillA;
	gdouble		overlapOffset;	// 第一个半圆的起点
	gdouble		overlapInterval;// 上一个半圆的起点到下一个半圆的起点之间的距离
	gdouble		overlapRadius;	// 半径
	gint		overlapSide;	// 方向
	
};

struct _MyPrototypeLineClass {
	GObjectClass parent_class;
    /* Overridable methods */
};

/* used by MY_TYPE_PROTOTYPE_LINE */
GType my_prototype_line_get_type (void);

/* Overridable methods wrapper */

/* Non-Overridable Methods */
void		my_prototype_line_set_orign_dashes_str		(MyPrototypeLine *self, const char *s);

#endif /*__MY_PROTOTYPE_LINE_H__*/