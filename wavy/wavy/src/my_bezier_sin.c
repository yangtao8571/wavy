#define MY_LOG_LEVEL 3

#include <gtk/gtk.h>
#include <glib-object.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "my_constant.h"
#include "my_math.h"
#include "my_point.h"
#include "my_rectangle.h"
#include "my_prototype_line.h"
#include "my_prototype_manager.h"
#include "my_application_state.h"
#include "my_debug.h"
#include "my_shape.h"
#include "my_vector.h"
#include "my_control_shape_rect.h"
#include "my_bezier_util.h"
#include "my_bezier.h"
#include "my_util.h"
#include "my_bezier_sin.h"

#define MY_BEZIER_SIN_GET_PRIVATE(o)		(G_TYPE_INSTANCE_GET_PRIVATE ((o), MY_TYPE_BEZIER_SIN, MyBezierSinPrivate))

G_DEFINE_TYPE (MyBezierSin, my_bezier_sin, MY_TYPE_BEZIER);
//G_DEFINE_TYPE_WITH_PRIVATE (MyBezierSin, my_bezier_sin, G_TYPE_OBJECT)
 
typedef struct _MyBezierSinPrivate {
	gdouble		offset;			// 第一个尖的起点
	gdouble		interval;		// 每个多少距离画一个正弦点
	gdouble		segmentCount;	// 把曲线近似为折线段的线段数
	gdouble		amplitude;		// 振幅
	gdouble		phase;			// 相位
	gdouble		zhouqi;			// 周期
} MyBezierSinPrivate;

/******************************************************************************
 * declare static function 
 ******************************************************************************/
static void 		my_bezier_sin_init (MyBezierSin* self);
static void 		my_bezier_sin_finalize (GObject* object);
static void 		my_bezier_sin_dispose (GObject* object);
static GString	*	my_bezier_sin_to_string	(MyShape *self);
static void			my_bezier_sin_draw_self	(MyShape* self, ApplicationState *appState);
static void			draw_self_shadow	(MyShape* self, ApplicationState *appState);
static void			update_with_scale	(MyShape *self, gdouble scale);
static void			update_shadow_rect	(MyShape *self);
static void			on_shadow_dirty		(MyShape *self);
static void			from_prototype_line	(MyShape *self, MyPrototypeLine *prototype);
static void			to_prototype_line	(MyShape *self, MyPrototypeLine *prototype);


/******************************************************************************
 * implement function 
 ******************************************************************************/
static void my_bezier_sin_class_init (MyBezierSinClass* klass)
{
	MyShapeClass	*myShapeClass = MY_SHAPE_CLASS(klass);
	GObjectClass	* g_object_class = G_OBJECT_CLASS(klass);
	/* Add private structure */
	g_type_class_add_private (klass, sizeof (MyBezierSinPrivate));
	/* Get the parent gobject class */
    /* Hook overridable methods */
	//myShapeClass->to_string = my_bezier_sin_to_string;
	myShapeClass->draw_self = my_bezier_sin_draw_self;
	myShapeClass->draw_self_shadow = draw_self_shadow;
	myShapeClass->update_shadow_rect = update_shadow_rect;
	myShapeClass->on_shadow_dirty = on_shadow_dirty;
	myShapeClass->from_prototype_line = from_prototype_line;
	myShapeClass->to_prototype_line = to_prototype_line;
	
	/* Hook finalization functions */
	g_object_class->dispose = my_bezier_sin_dispose; /* instance destructor, reverse of init */
	g_object_class->finalize = my_bezier_sin_finalize; /* class finalization, reverse of class init */
}

// constructor
static void		my_bezier_sin_init (MyBezierSin* self)
{
	/* Initialize public fields */
//	MY_SHAPE (self)->isShadowing = FALSE;
	
	/* Initialize private fields */
}

static void my_bezier_sin_finalize (GObject* object)
{  
  /* Always chain up to the parent class; as with dispose(), finalize()
   * is guaranteed to exist on the parent's class virtual function table
   */
	G_OBJECT_CLASS (my_bezier_sin_parent_class)->finalize (object);
}

static void my_bezier_sin_dispose (GObject* object)
{
	/* Reverse what was allocated by instance init */
	MyBezierSin *self = MY_BEZIER_SIN (object);
	MyBezierSinPrivate* priv = MY_BEZIER_SIN_GET_PRIVATE (self);
 		
  /* Always chain up to the parent class; there is no need to check if
   * the parent class implements the dispose() virtual function: it is
   * always guaranteed to do so
   */
	G_OBJECT_CLASS (my_bezier_sin_parent_class)->dispose (object);
}


void		my_bezier_sin_new_with_coordinary	(MyBezierSin *bezSin,
												gint id, 
												ApplicationState *appState,
												gdouble x0, gdouble y0,
												gdouble x3, gdouble y3,
												gdouble scale) {
										
													
	MyBezierSinPrivate* priv = MY_BEZIER_SIN_GET_PRIVATE (bezSin);
	my_bezier_new_with_coordinary (MY_BEZIER(bezSin), id, appState, x0, y0, x3, y3, scale);
	
	priv->offset = 0;
	priv->interval = 3;	// 测试效果：6->不规则线，2->近似正弦
	priv->segmentCount = 20;
	priv->amplitude = 10;
	priv->phase = 0;
	priv->zhouqi = 2*PI * 5;
	
	MY_SHAPE (bezSin)->shadowPadding += priv->amplitude;
}


static GString	*	my_bezier_sin_to_string	(MyShape *self) {

	guint i, pointCount;
	MyPoint *point;
	char buf[50] = {0};
	GString * gstring = g_string_new (NULL);
	return gstring;
}


static void		my_bezier_sin_draw_self		(MyShape* self, ApplicationState *appState) {
	
	MyBezier *bez = MY_BEZIER (self);
	cairo_t *cr = appState->cr;
	MyBezierSinPrivate* priv = MY_BEZIER_SIN_GET_PRIVATE (self);
	
	
	gdouble currentX, currentY, theLastX, theLastY,
			segmentLen, // 当前两点间线段的长度
			totalLen, 	// 当前point到线起点的总距离
			sinStartLen, // 下一个正弦点应出现的位置. 线的起点到下一个正弦点的起点的距离
			sinDrawnCount, 	// 已画正弦点的个数
			beyondLastPointLen,
			offset,
			interval,
			amplitude,
			zhouqi,
			phase;
	MyPoint sinPoint;	// 正弦点
	
	MyVector q0, q1;
	gdouble t;
	int i;
	MyVector segmentUnitV,		// 线段的单位向量
			segmentV,
			perpUnitV,
			sinV;
	
	totalLen = 0;
	sinDrawnCount = 0;
	offset = priv->offset * self->appState->scale;
	interval = priv->interval * self->appState->scale;
	amplitude = priv->amplitude * self->appState->scale;
	zhouqi = priv->zhouqi * self->appState->scale;
	phase = priv->phase * self->appState->scale;
	sinStartLen = offset;
	
	q0 = calculateBezierPoint (0, bez->p0, bez->p1, bez->p2, bez->p3);
	theLastX = q0.x;
	theLastY = q0.y;

	cairo_save (cr);		
	cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);

	for (i = 1; i <= priv->segmentCount; i++) {
		t = i / priv->segmentCount;
		q1 = calculateBezierPoint(t, bez->p0, bez->p1, bez->p2, bez->p3);
		currentX = q1.x;
		currentY = q1.y;
		segmentV = my_vector_new (theLastX, theLastY, currentX, currentY);
		segmentUnitV = my_vector_norm (segmentV);
		perpUnitV = my_vector_perpendicular (segmentUnitV);
		segmentLen = sqrt ((currentX - theLastX) * (currentX - theLastX) + (currentY - theLastY) * (currentY - theLastY));
		totalLen += segmentLen;
		while (TRUE) {
			if (totalLen >= sinStartLen) { // 距离满足
				beyondLastPointLen = sinStartLen - (totalLen - segmentLen);
				sinPoint = my_util_part_segment (theLastX, theLastY, currentX, currentY, segmentLen, beyondLastPointLen);
				sinV = my_vector_scalar_mul (perpUnitV, amplitude * sin(2*PI/zhouqi*sinStartLen + phase));
					
				cairo_line_to (cr, sinPoint.x + sinV.x + appState->orignX, sinPoint.y + sinV.y + appState->orignY);
					
				sinDrawnCount++;
				sinStartLen = offset + sinDrawnCount * interval;					
			} else {
				break;
			}
		}
		theLastX = currentX;
		theLastY = currentY;
	}	
	//cairo_set_dash (cr, NULL, 0, 0); // no dash with angle
	cairo_stroke (cr);
	
	cairo_restore (cr);
}


static void		draw_self_shadow	(MyShape* self, ApplicationState *appState) {

	
	MyBezier *bez = MY_BEZIER (self);
	cairo_t *cr = appState->cr;
	gdouble scale = appState->scale;
	
	MyVector newP0, newP1, newP2, newP3;
	MyBezierSinPrivate* priv = MY_BEZIER_SIN_GET_PRIVATE (self);
	
	gdouble currentX, currentY, theLastX, theLastY,
			segmentLen, // 当前两点间线段的长度
			totalLen, 	// 当前point到线起点的总距离
			sinStartLen, // 下一个正弦点应出现的位置. 线的起点到下一个正弦点的起点的距离
			sinDrawnCount, 	// 已画正弦点的个数
			beyondLastPointLen,
			offset,
			interval,
			amplitude,
			zhouqi,
			phase;
	MyPoint sinPoint;	// 正弦点
	
	MyVector q0, q1;
	gdouble t;
	int i;
	MyVector segmentUnitV,		// 线段的单位向量
			segmentV,
			perpUnitV,
			sinV;
	
	cairo_save (cr);
		
	cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);
	
	
	newP0.x = bez->p0.x;
	newP0.y = bez->p0.y;
	newP1.x = bez->p1.x;
	newP1.y = bez->p1.y;
	newP2.x = bez->p2.x;
	newP2.y = bez->p2.y;
	newP3.x = bez->p3.x;
	newP3.y = bez->p3.y;
	
	
	
	totalLen = 0;
	sinDrawnCount = 0;
	offset = priv->offset * self->appState->scale;
	interval = priv->interval * self->appState->scale;
	amplitude = priv->amplitude * self->appState->scale;
	zhouqi = priv->zhouqi * self->appState->scale;
	phase = priv->phase * self->appState->scale;
	sinStartLen = offset;
	
	q0 = calculateBezierPoint (0, newP0, newP1, newP2, newP3);
	theLastX = q0.x;
	theLastY = q0.y;
	for (i = 1; i <= priv->segmentCount; i++) {
		t = i / priv->segmentCount;
		q1 = calculateBezierPoint(t, newP0, newP1, newP2, newP3);
		currentX = q1.x;
		currentY = q1.y;
		my_debug ("theLastXY %f,%f", theLastX, theLastY);
		my_debug ("currentXY %f,%f", currentX, currentY);
		//if (k != 0) { // 从第二个点开始有线段
			segmentV = my_vector_new (theLastX, theLastY, currentX, currentY);
			segmentUnitV = my_vector_norm (segmentV);
			perpUnitV = my_vector_perpendicular (segmentUnitV);
			
			my_debug ("segment vector %f,%f", segmentV.x, segmentV.y);
			my_debug ("segmentUnit vector %f,%f", segmentUnitV.x, segmentUnitV.y);
			my_debug ("perpUnit vector %f,%f", perpUnitV.x, perpUnitV.y);
			
			segmentLen = sqrt ((currentX - theLastX) * (currentX - theLastX) + (currentY - theLastY) * (currentY - theLastY) );
			totalLen += segmentLen;
			my_debug ("segmentLen %f", segmentLen);
			my_debug ("totalLen %f", totalLen);
			while (TRUE) {
				if (totalLen >= sinStartLen) { // 距离满足
					beyondLastPointLen = sinStartLen - (totalLen - segmentLen);
					sinPoint = my_util_part_segment (theLastX, theLastY, currentX, currentY, segmentLen, beyondLastPointLen);
					my_debug ("start beyondLastPointLen is %f", beyondLastPointLen);
					my_debug ("sinPoint is (%f,%f)", sinPoint.x, sinPoint.y);
					
					sinV = my_vector_scalar_mul (perpUnitV, 
												amplitude * sin(2*PI/zhouqi*sinStartLen + phase));
					my_debug ("sin vector %f,%f", sinV.x, sinV.y);
					
					cairo_line_to (cr, 
						(sinPoint.x + sinV.x) / scale - self->shadowX, 
						(sinPoint.y + sinV.y) / scale - self->shadowY);
					
					sinDrawnCount++;
					sinStartLen = offset + sinDrawnCount * interval;					
				} else {
					break;
				}
			}
		//}
		theLastX = currentX;
		theLastY = currentY;
	}	
	//cairo_set_dash (cr, NULL, 0, 0); // no dash with angle
	cairo_stroke (cr);
	
	cairo_restore (cr);
}


static void		on_shadow_dirty				(MyShape *self) {
	
	
	MyBezierSinPrivate* priv = MY_BEZIER_SIN_GET_PRIVATE (MY_BEZIER_SIN (self));
	gdouble scale = self->appState->scale;
	self->shadowPadding = 2 * self->boxRadius + 0.707 * self->strokeWidth + priv->amplitude;
	self->shadowOrignWidth = ceil (self->justSize.width / scale + 2 * self->shadowPadding);
	self->shadowOrignHeight = ceil (self->justSize.height / scale + 2 * self->shadowPadding);
}


static void		update_shadow_rect			(MyShape *self) {
	
	
	gdouble scale = self->appState->scale;
	self->shadowX = self->justSize.x / scale - self->shadowPadding;
	self->shadowY = self->justSize.y / scale - self->shadowPadding;
	self->shadowWidth = self->justSize.width + 2 * self->shadowPadding * scale;
	self->shadowHeight = self->justSize.height + 2 * self->shadowPadding * scale;
}


static void		from_prototype_line		(MyShape *self, MyPrototypeLine *prototype) {
	
	
	MyBezierSinPrivate* priv = MY_BEZIER_SIN_GET_PRIVATE (MY_BEZIER_SIN (self));
	priv->offset = prototype->offset;
	priv->interval = prototype->interval;
	priv->segmentCount = prototype->segmentCount;
	priv->amplitude = prototype->amplitude;
	priv->phase = prototype->phase;
	priv->zhouqi = prototype->zhouqi;
}


static void		to_prototype_line		(MyShape *self, MyPrototypeLine *prototype) {	
	
	
	MyBezierSinPrivate* priv = MY_BEZIER_SIN_GET_PRIVATE (MY_BEZIER_SIN (self));
	prototype->style = MY_PROTOTYPE_STYLE_BEZIER_SIN;
	prototype->offset = priv->offset;
	prototype->interval = priv->interval;
	prototype->segmentCount = priv->segmentCount;
	prototype->amplitude = priv->amplitude;
	prototype->phase = priv->phase;
	prototype->zhouqi = priv->zhouqi;
}