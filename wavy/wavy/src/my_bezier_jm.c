#define MY_LOG_LEVEL 3

#include <gtk/gtk.h>
#include <glib-object.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "my_constant.h"
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
#include "my_bezier_jm.h"

#define MY_BEZIER_JM_GET_PRIVATE(o)		(G_TYPE_INSTANCE_GET_PRIVATE ((o), MY_TYPE_BEZIER_JM, MyBezierJmPrivate))

G_DEFINE_TYPE (MyBezierJm, my_bezier_jm, MY_TYPE_BEZIER);
//G_DEFINE_TYPE_WITH_PRIVATE (MyBezierJm, my_bezier_jm, G_TYPE_OBJECT)
 
typedef struct _MyBezierJmPrivate {
	gdouble		jmOffset;	// 第一个尖的起点
	gdouble		jmInterval;	// 上一个尖的起点到下一个尖的起点之间的距离
	gdouble		jmEdgeLen;	// 边长
	gint		jmSide;		// 尖的方向
} MyBezierJmPrivate;

/******************************************************************************
 * declare static function 
 ******************************************************************************/
static void 		my_bezier_jm_init (MyBezierJm* self);
static void 		my_bezier_jm_finalize (GObject* object);
static void 		my_bezier_jm_dispose (GObject* object);
static GString	*	my_bezier_jm_to_string	(MyShape *self);
static void			my_bezier_jm_draw_self	(MyShape* self, ApplicationState *appState);
static void			draw_self_shadow	(MyShape* self, ApplicationState *appState);
static void			update_shadow_rect	(MyShape *self);
static void			on_shadow_dirty		(MyShape *self);
static void			from_prototype_line	(MyShape *self, MyPrototypeLine *prototype);
static void			to_prototype_line	(MyShape *self, MyPrototypeLine *prototype);


/******************************************************************************
 * implement function 
 ******************************************************************************/
static void my_bezier_jm_class_init (MyBezierJmClass* klass)
{
	MyShapeClass	*myShapeClass = MY_SHAPE_CLASS(klass);
	GObjectClass	* g_object_class = G_OBJECT_CLASS(klass);
	/* Add private structure */
	g_type_class_add_private (klass, sizeof (MyBezierJmPrivate));
	/* Get the parent gobject class */
    /* Hook overridable methods */
	//myShapeClass->to_string = my_bezier_jm_to_string;
	myShapeClass->draw_self = my_bezier_jm_draw_self;
	myShapeClass->draw_self_shadow = draw_self_shadow;
	myShapeClass->update_shadow_rect = update_shadow_rect;
	myShapeClass->on_shadow_dirty = on_shadow_dirty;
	myShapeClass->from_prototype_line = from_prototype_line;
	myShapeClass->to_prototype_line = to_prototype_line;
	
	/* Hook finalization functions */
	g_object_class->dispose = my_bezier_jm_dispose; /* instance destructor, reverse of init */
	g_object_class->finalize = my_bezier_jm_finalize; /* class finalization, reverse of class init */
}

// constructor
static void		my_bezier_jm_init (MyBezierJm* self)
{
	/* Initialize public fields */
//	MY_SHAPE (self)->isShadowing = FALSE;
	
	/* Initialize private fields */
}

static void my_bezier_jm_finalize (GObject* object)
{  
  /* Always chain up to the parent class; as with dispose(), finalize()
   * is guaranteed to exist on the parent's class virtual function table
   */
	G_OBJECT_CLASS (my_bezier_jm_parent_class)->finalize (object);
}

static void my_bezier_jm_dispose (GObject* object)
{
	/* Reverse what was allocated by instance init */
	MyBezierJm *self = MY_BEZIER_JM (object);
	MyBezierJmPrivate* priv = MY_BEZIER_JM_GET_PRIVATE (self);
 		
  /* Always chain up to the parent class; there is no need to check if
   * the parent class implements the dispose() virtual function: it is
   * always guaranteed to do so
   */
	G_OBJECT_CLASS (my_bezier_jm_parent_class)->dispose (object);
}


void		my_bezier_jm_new_with_coordinary	(MyBezierJm *bezJm,
												gint id, 
												ApplicationState *appState,
												gdouble x0, gdouble y0,
												gdouble x3, gdouble y3,
												gdouble scale) {
										
													
	MyBezierJmPrivate* priv = MY_BEZIER_JM_GET_PRIVATE (bezJm);
	my_bezier_new_with_coordinary (MY_BEZIER(bezJm), id, appState, x0, y0, x3, y3, scale);
	
	priv->jmOffset = 20;
	priv->jmInterval = 50;
	priv->jmEdgeLen = 10;
	priv->jmSide = -1;
	
	MY_SHAPE (bezJm)->shadowPadding += priv->jmEdgeLen;
}


static GString	*	my_bezier_jm_to_string	(MyShape *self) {

	guint i, pointCount;
	MyPoint *point;
	char buf[50] = {0};
	GString * gstring = g_string_new (NULL);
	return gstring;
}


static void		my_bezier_jm_draw_self		(MyShape* self, ApplicationState *appState) {
	
	
	MyBezier *bez = MY_BEZIER (self);
	cairo_t *cr = appState->cr;
	
	// addition triangles
	MyBezierJmPrivate* priv = MY_BEZIER_JM_GET_PRIVATE (self);
	gdouble currentX, currentY, theLastX, theLastY,
			segmentLen, // 当前两点间线段的长度
			totalLen, 	// 当前point到线起点的总距离
			jmStartLen, // 下一个尖应出现的位置. 线的起点到下一个尖的起点的距离
			jmEndLen, 	// 下一个尖应出现的位置. 线的起点到下一个尖的终点的距离
			jmDrawnCount, 	// 已画尖的个数
			beyondLastPointLen,
			jmOffset,
			jmEdgeLen,
			jmInterval;	
	MyPoint jmStartPoint, 	// 尖起点
			jmEndPoint, 	// 尖终点
			jmPoint;		// 尖顶点
	gboolean gotJmStartPoint, 	// 是否找到一个尖的起点
			gotJmEndPoint;		// 是否找到尖的起点所对的终点
			
	MyVector q0, q1;
	gdouble t;
	int i;
	gdouble segmentCount = 20;	// 把曲线近似为折线段的线段数
	cairo_line_join_t backup = cairo_get_line_join (cr);
	
	// main curve
	cairo_move_to (cr, appState->orignX + bez->p0.x, appState->orignY + bez->p0.y);
	cairo_curve_to (cr, 
		bez->p1.x + appState->orignX, 
		bez->p1.y + appState->orignY, 
		bez->p2.x + appState->orignX, 
		bez->p2.y + appState->orignY,
		bez->p3.x + appState->orignX, 
		bez->p3.y + appState->orignY);
		
	cairo_set_line_join (cr, CAIRO_LINE_JOIN_MITER);
	cairo_stroke (cr);
	
	totalLen = 0;
	jmDrawnCount = 0;
	jmOffset = priv->jmOffset * MY_SHAPE(self)->appState->scale;
	jmEdgeLen = priv->jmEdgeLen * MY_SHAPE(self)->appState->scale;
	jmInterval = priv->jmInterval * MY_SHAPE(self)->appState->scale;
	jmStartLen = jmOffset;
	jmEndLen = jmStartLen + jmEdgeLen;
	gotJmStartPoint = gotJmEndPoint = FALSE;
	
	q0 = calculateBezierPoint (0, bez->p0, bez->p1, bez->p2, bez->p3);
	theLastX = q0.x;
	theLastY = q0.y;
	for (i = 1; i <= segmentCount; i++) {
		t = i / segmentCount;
		q1 = calculateBezierPoint(t, bez->p0, bez->p1, bez->p2, bez->p3);
		currentX = q1.x;
		currentY = q1.y;
		//if (k != 0) { // 从第二个点开始有线段
			segmentLen = sqrt ((currentX - theLastX) * (currentX - theLastX) + (currentY - theLastY) * (currentY - theLastY) );
			totalLen += segmentLen;
			my_debug ("segmentLen %f", segmentLen);
			my_debug ("totalLen %f", totalLen);
			while (TRUE) {
				if (totalLen >= jmStartLen) { // 距离满足尖的起点
					if (gotJmStartPoint == FALSE) {
						beyondLastPointLen = jmStartLen - (totalLen - segmentLen);
						jmStartPoint = my_util_part_segment (theLastX, theLastY, currentX, currentY, segmentLen, beyondLastPointLen);
						my_debug ("start beyondLastPointLen is %f", beyondLastPointLen);
						my_debug ("jmStartPoint is (%f,%f)", jmStartPoint.x, jmStartPoint.y);
						gotJmStartPoint = TRUE;						
					}
				} else {
					break;
				}
				if (totalLen >= jmEndLen) { // 距离满足尖的终点
					if (gotJmStartPoint) { // 也有起点,满足画尖
						beyondLastPointLen = jmEndLen - (totalLen - segmentLen);
						jmEndPoint = my_util_part_segment (theLastX, theLastY, currentX, currentY, segmentLen, beyondLastPointLen);
						my_debug ("end beyondLastPointLen is %f", beyondLastPointLen);
						my_debug ("jmEndPoint is (%f,%f)", jmEndPoint.x, jmEndPoint.y);
						gotJmEndPoint = TRUE;
						
						// 画尖
						jmPoint = my_util_jmline_point (jmStartPoint.x, jmStartPoint.y, jmEndPoint.x, jmEndPoint.y, priv->jmSide);
						my_debug ("arrow point %#x (%f,%f)", jmPoint, jmPoint.x, jmPoint.y);
						
						cairo_move_to (cr, jmStartPoint.x + appState->orignX, jmStartPoint.y + appState->orignY);
						cairo_line_to (cr, jmPoint.x + appState->orignX, jmPoint.y + appState->orignY);
						cairo_line_to (cr, jmEndPoint.x + appState->orignX, jmEndPoint.y + appState->orignY);
						
						gotJmStartPoint = gotJmEndPoint = FALSE;
						jmDrawnCount++;
						jmStartLen = jmOffset + jmDrawnCount * jmInterval;
						jmEndLen = jmStartLen + jmEdgeLen;
					}
				} else {
					break;
				}
			}
		//}
		theLastX = currentX;
		theLastY = currentY;
	}	
	cairo_set_dash (cr, NULL, 0, 0); // no dash with angle
	cairo_set_line_join (cr, backup); // restore it
	cairo_stroke (cr);
}


static void			draw_self_shadow	(MyShape* self, ApplicationState *appState) {

	
	MyBezier *bez = MY_BEZIER (self);
	cairo_t *cr = appState->cr;
	gdouble scale = appState->scale;
	MyVector newP0, newP1, newP2, newP3;
	
	MyBezierJmPrivate* priv = MY_BEZIER_JM_GET_PRIVATE (self);
	gdouble currentX, currentY, theLastX, theLastY,
			segmentLen, // 当前两点间线段的长度
			totalLen, 	// 当前point到线起点的总距离
			jmStartLen, // 下一个尖应出现的位置. 线的起点到下一个尖的起点的距离
			jmEndLen, 	// 下一个尖应出现的位置. 线的起点到下一个尖的终点的距离
			jmDrawnCount, 	// 已画尖的个数
			beyondLastPointLen,
			jmOffset,
			jmEdgeLen,
			jmInterval;	
	MyPoint jmStartPoint, 	// 尖起点
			jmEndPoint, 	// 尖终点
			jmPoint;		// 尖顶点
	gboolean gotJmStartPoint, 	// 是否找到一个尖的起点
			gotJmEndPoint;		// 是否找到尖的起点所对的终点
			
	MyVector q0, q1;
	gdouble t;
	int i;
	gdouble segmentCount = 20;
	
	cairo_line_join_t backup = cairo_get_line_join (cr);

	cairo_move_to (cr, 
		bez->p0.x / scale - self->shadowX,
		bez->p0.y / scale - self->shadowY);
	cairo_curve_to (cr, 
		bez->p1.x / scale - self->shadowX,
		bez->p1.y / scale - self->shadowY, 
		bez->p2.x / scale - self->shadowX,
		bez->p2.y / scale - self->shadowY,
		bez->p3.x / scale - self->shadowX,
		bez->p3.y / scale - self->shadowY);
		
	
	// addition triangles
	
	newP0.x = bez->p0.x;
	newP0.y = bez->p0.y;
	newP1.x = bez->p1.x;
	newP1.y = bez->p1.y;
	newP2.x = bez->p2.x;
	newP2.y = bez->p2.y;
	newP3.x = bez->p3.x;
	newP3.y = bez->p3.y;
	
	cairo_set_line_join (cr, CAIRO_LINE_JOIN_MITER);
	cairo_stroke (cr);
	
	totalLen = 0;
	jmDrawnCount = 0;
	jmOffset = priv->jmOffset * MY_SHAPE(self)->appState->scale;
	jmEdgeLen = priv->jmEdgeLen * MY_SHAPE(self)->appState->scale;
	jmInterval = priv->jmInterval * MY_SHAPE(self)->appState->scale;
	jmStartLen = jmOffset;
	jmEndLen = jmStartLen + jmEdgeLen;
	gotJmStartPoint = gotJmEndPoint = FALSE;
	
	q0 = calculateBezierPoint (0, newP0, newP1, newP2, newP3);
	theLastX = q0.x;
	theLastY = q0.y;
	for (i = 1; i <= segmentCount; i++) {
		t = i / segmentCount;
		q1 = calculateBezierPoint(t, newP0, newP1, newP2, newP3);
		currentX = q1.x;
		currentY = q1.y;
		//if (k != 0) { // 从第二个点开始有线段
			segmentLen = sqrt ((currentX - theLastX) * (currentX - theLastX) + (currentY - theLastY) * (currentY - theLastY) );
			totalLen += segmentLen;
			my_debug ("segmentLen %f", segmentLen);
			my_debug ("totalLen %f", totalLen);
			while (TRUE) {
				if (totalLen >= jmStartLen) { // 距离满足尖的起点
					if (gotJmStartPoint == FALSE) {
						beyondLastPointLen = jmStartLen - (totalLen - segmentLen);
						jmStartPoint = my_util_part_segment (theLastX, theLastY, currentX, currentY, segmentLen, beyondLastPointLen);
						my_debug ("start beyondLastPointLen is %f", beyondLastPointLen);
						my_debug ("jmStartPoint is (%f,%f)", jmStartPoint.x, jmStartPoint.y);
						gotJmStartPoint = TRUE;						
					}
				} else {
					break;
				}
				if (totalLen >= jmEndLen) { // 距离满足尖的终点
					if (gotJmStartPoint) { // 也有起点,满足画尖
						beyondLastPointLen = jmEndLen - (totalLen - segmentLen);
						jmEndPoint = my_util_part_segment (theLastX, theLastY, currentX, currentY, segmentLen, beyondLastPointLen);
						my_debug ("end beyondLastPointLen is %f", beyondLastPointLen);
						my_debug ("jmEndPoint is (%f,%f)", jmEndPoint.x, jmEndPoint.y);
						gotJmEndPoint = TRUE;
						
						// 画尖
						jmPoint = my_util_jmline_point (jmStartPoint.x, jmStartPoint.y, jmEndPoint.x, jmEndPoint.y, priv->jmSide);
						my_debug ("arrow point %#x (%f,%f)", jmPoint, jmPoint.x, jmPoint.y);
						
						cairo_move_to (cr, 
									jmStartPoint.x / scale - self->shadowX, 
									jmStartPoint.y / scale - self->shadowY);
						cairo_line_to (cr, 
									jmPoint.x / scale - self->shadowX, 
									jmPoint.y / scale - self->shadowY);
						cairo_line_to (cr, 
									jmEndPoint.x / scale - self->shadowX, 
									jmEndPoint.y / scale - self->shadowY);
						
						gotJmStartPoint = gotJmEndPoint = FALSE;
						jmDrawnCount++;
						jmStartLen = jmOffset + jmDrawnCount * jmInterval;
						jmEndLen = jmStartLen + jmEdgeLen;
					}
				} else {
					break;
				}
			}
		//}
		theLastX = currentX;
		theLastY = currentY;
	}	
	cairo_set_line_join (cr, backup); // restore it
	cairo_set_dash (cr, NULL, 0, 0);
	cairo_stroke (cr);
}


static void		on_shadow_dirty				(MyShape *self) {
	
	
	MyBezierJmPrivate* priv = MY_BEZIER_JM_GET_PRIVATE (MY_BEZIER_JM (self));
	gdouble scale = self->appState->scale;
	self->shadowPadding = 2 * self->boxRadius + 0.707 * self->strokeWidth + priv->jmEdgeLen;
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
	
	
	MyBezierJmPrivate* priv = MY_BEZIER_JM_GET_PRIVATE (MY_BEZIER_JM (self));
	priv->jmOffset = prototype->jmOffset;
	priv->jmInterval = prototype->jmInterval;
	priv->jmEdgeLen = prototype->jmEdgeLen;
	priv->jmSide = prototype->jmSide;
}


static void		to_prototype_line		(MyShape *self, MyPrototypeLine *prototype) {
	
	
	MyBezierJmPrivate* priv = MY_BEZIER_JM_GET_PRIVATE (MY_BEZIER_JM (self));
	
	prototype->style = MY_PROTOTYPE_STYLE_BEZIER_JM;
	prototype->jmOffset = priv->jmOffset;
	prototype->jmInterval = priv->jmInterval;
	prototype->jmEdgeLen = priv->jmEdgeLen;
	prototype->jmSide = priv->jmSide;
}