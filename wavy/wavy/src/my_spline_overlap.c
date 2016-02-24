#define MY_LOG_LEVEL 3

#include <gtk/gtk.h>
#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <f2c.h>
#include <clapack.h>
#include "GSuperMacro.h"
#include "my_constant.h"
#include "my_point.h"
#include "my_rectangle.h"
#include "my_util.h"
#include "my_prototype_line.h"
#include "my_prototype_manager.h"
#include "my_application_state.h"
#include "my_debug.h"
#include "my_vector.h"
#include "my_shape.h"
#include "my_layer.h"
#include "my_curve.h"
#include "my_control_shape_rect.h"
#include "my_spline.h"
#include "my_spline_overlap.h"

#define MY_SPLINE_OVERLAP_GET_PRIVATE(o)		(G_TYPE_INSTANCE_GET_PRIVATE ((o), MY_TYPE_SPLINE_OVERLAP, MySplineOverlapPrivate))

G_DEFINE_TYPE (MySplineOverlap, my_spline_overlap, MY_TYPE_SPLINE);
//G_DEFINE_TYPE_WITH_PRIVATE (MySplineOverlap, my_spline_overlap, G_TYPE_OBJECT)
 
typedef struct _MySplineOverlapPrivate {	
	gdouble		overlapFillR;	// 半圆填充颜色
	gdouble		overlapFillG;
	gdouble		overlapFillB;
	gdouble		overlapFillA;
	gdouble		overlapOffset;	// 第一个半圆的起点
	gdouble		overlapInterval;// 上一个半圆的中点到下一个半圆的中点之间的距离
	gdouble		overlapRadius;	// 半径
	gint		overlapSide;	// 圆狐方向是否为法线方向
} MySplineOverlapPrivate;

/******************************************************************************
 * declare static function 
 ******************************************************************************/
static void 			my_spline_overlap_init 		(MySplineOverlap* self);
static void 			my_spline_overlap_finalize 	(GObject* object);
static void 			my_spline_overlap_dispose 	(GObject* object);
G_SUPER_METHOD	(void, draw_self,				(MyShape* self, ApplicationState *appState));
static void			draw_self				(MyShape* self, ApplicationState *appState);
G_SUPER_METHOD	(void, draw_self_shadow,		(MyShape* self, ApplicationState *appState));
static void			draw_self_shadow		(MyShape* self, ApplicationState *appState);
G_SUPER_METHOD	(void, draw_self_shadow_debug,	(MyShape* self));
static void			draw_self_shadow_debug	(MyShape* self);
static void			update_shadow_rect		(MyShape *self);
static void			on_shadow_dirty			(MyShape *self);
G_SUPER_METHOD	(void, from_prototype_line,		(MyShape* self, MyPrototypeLine *prototype));
static void			from_prototype_line		(MyShape *self, MyPrototypeLine *prototype);
G_SUPER_METHOD	(void, to_prototype_line,		(MyShape* self, MyPrototypeLine *prototype));
static void			to_prototype_line		(MyShape *self, MyPrototypeLine *prototype);
/******************************************************************************
 * implement function 
 ******************************************************************************/
static void my_spline_overlap_class_init (MySplineOverlapClass* klass)
{
	GObjectClass	*g_object_class;
	MyShapeClass	*myShapeClass;

	/* Add private structure */
	g_type_class_add_private (klass, sizeof (MySplineOverlapPrivate));
	/* Get the parent gobject class */
	g_object_class = G_OBJECT_CLASS(klass);

    /* Hook overridable methods */
	myShapeClass = MY_SHAPE_CLASS(klass);
	G_OVERRIDE_METHOD (myShapeClass, draw_self, draw_self);
	G_OVERRIDE_METHOD (myShapeClass, draw_self_shadow, draw_self_shadow);
	G_OVERRIDE_METHOD (myShapeClass, draw_self_shadow_debug, draw_self_shadow_debug);
	myShapeClass->update_shadow_rect = update_shadow_rect;
	myShapeClass->on_shadow_dirty = on_shadow_dirty;
	G_OVERRIDE_METHOD (myShapeClass, from_prototype_line, from_prototype_line);
	G_OVERRIDE_METHOD (myShapeClass, to_prototype_line, to_prototype_line);
	
	/* Hook finalization functions */
	g_object_class->dispose = my_spline_overlap_dispose; /* instance destructor, reverse of init */
	g_object_class->finalize = my_spline_overlap_finalize; /* class finalization, reverse of class init */
}

// constructor
static void		my_spline_overlap_init (MySplineOverlap* self)
{
	MySplineOverlapPrivate* priv = MY_SPLINE_OVERLAP_GET_PRIVATE (self);

	/* Initialize public fields */
 
	/* Initialize private fields */
	priv->overlapRadius = 10;
}

static void my_spline_overlap_finalize (GObject* object)
{  
  /* Always chain up to the parent class; as with dispose(), finalize()
   * is guaranteed to exist on the parent's class virtual function table
   */
	G_OBJECT_CLASS (my_spline_overlap_parent_class)->finalize (object);
}


static void my_spline_overlap_dispose (GObject* object)
{
	/* Reverse what was allocated by instance init */
	MySplineOverlap *self = MY_SPLINE_OVERLAP (object);
	MySplineOverlapPrivate* priv = MY_SPLINE_OVERLAP_GET_PRIVATE (self);

  /* Always chain up to the parent class; there is no need to check if
   * the parent class implements the dispose() virtual function: it is
   * always guaranteed to do so
   */
	G_OBJECT_CLASS (my_spline_overlap_parent_class)->dispose (object);
}


static void		draw_self		(MyShape* self, ApplicationState *appState) {

	
	MySplineOverlapPrivate* priv = MY_SPLINE_OVERLAP_GET_PRIVATE (self);
	MySpline *spline = MY_SPLINE (self);
	MySplineOverlap *splineOverlap = MY_SPLINE_OVERLAP (self);

	cairo_t *cr = appState->cr;
	
	gdouble currentX, currentY, theLastX, theLastY,
			segmentLen, // 当前两点间线段的长度
			totalLen, 	// 当前point到线起点的总路程
			jmStartLen, // 下一个修饰图应出现的位置,即线的起点到下一个修饰图的起点的路程
			jmEndLen, 	// 下一个修饰图应出现的位置,即线的起点到下一个修饰图的终点的路程
			jmDrawnCount, 	// 已画修饰图的个数
			beyondLastPointLen,
			jmOffset,
			jmEdgeLen,
			jmInterval;	
	MyPoint jmStartPoint, 	// 修饰图起点
			jmEndPoint, 	// 修饰图终点
			jmPoint;		// 修饰图顶点
	gboolean gotJmStartPoint, 	// 是否找到一个修饰图的起点
			gotJmEndPoint;		// 是否找到修饰图的起点所对的终点

	gdouble lx, ly, cx, cy;
	gdouble interX0, interY0, interX1, interY1;	// 曲线与圆交点
	gdouble segInterX0, segInterY0, segInterX1, segInterY1;	// 线段与圆交点
	int interCount;	// 线段与圆交点个数
	int totalInterCount;
	gboolean foundP0, foundP1;	// P0,P1两个交点都找到就满足画半圆
	guint indexP0, // 交点所在线段端点索引。第一个交点所在线段的后端点
		indexP1;	// 第二个交点所在线段的前端点
	guint indexCircleCenter;	// 圆心所在的线段索引，0-based
	guint i, j, pointCount;
	gdouble circleCenterX, circleCenterY;
	gdouble angleStart, angleEnd;
	gdouble overlapRadius;
	int hitCenter;	// 命中圆心个数

	G_SUPER (draw_self, (self, appState));

	totalLen = 0;
	jmDrawnCount = 0;
	jmOffset = (priv->overlapOffset + priv->overlapRadius) * MY_SHAPE(self)->appState->scale;
	overlapRadius = priv->overlapRadius * MY_SHAPE(self)->appState->scale;
	jmInterval = priv->overlapInterval * MY_SHAPE(self)->appState->scale;
	jmStartLen = jmOffset;
	
	pointCount = my_spline_get_points_count (spline);
	if (pointCount < 2) {
		return;
	}
	
	//cairo_set_line_join (cr, CAIRO_LINE_JOIN_BEVEL);
	hitCenter = 0;
	theLastX = my_spline_peek_nth (spline, 0)->x;
	theLastY = my_spline_peek_nth (spline, 0)->y;
	for (i = 1; i < pointCount; i++) {
		currentX = my_spline_peek_nth (spline, i)->x;
		currentY = my_spline_peek_nth (spline, i)->y;
		segmentLen = sqrt ((currentX - theLastX) * (currentX - theLastX) + (currentY - theLastY) * (currentY - theLastY) );
		totalLen += segmentLen;

		while (TRUE) {
			if (totalLen >= jmStartLen) { // 距离满足圆心
				indexCircleCenter = i - 1;
				beyondLastPointLen = jmStartLen - (totalLen - segmentLen);
				jmStartPoint = my_util_part_segment (theLastX, theLastY, currentX, currentY, segmentLen, beyondLastPointLen);
				circleCenterX = jmStartPoint.x;
				circleCenterY = jmStartPoint.y;
				
				foundP0 = FALSE;
				foundP1 = FALSE;

				// 求圆与曲线交点
				totalInterCount = 0;
				lx = my_spline_peek_nth (spline, 0)->x;
				ly = my_spline_peek_nth (spline, 0)->y;
				for (j = 1; j < pointCount; j++) {
					if (foundP0 && foundP1) {
						break;
					}
					cx = my_spline_peek_nth (spline, j)->x;
					cy = my_spline_peek_nth (spline, j)->y;
					interCount = my_util_segment_inter_circle (lx, ly, cx, cy, circleCenterX, circleCenterY, overlapRadius, 
													&segInterX0, &segInterY0, &segInterX1, &segInterY1);
					if (interCount == 2) {
						if (j - 1 == indexCircleCenter) { // 两个交点恰在圆心线段上
							interX0 = segInterX0;
							interY0 = segInterY0;
							interX1 = segInterX1;
							interY1 = segInterY1;
							indexP0 = j;
							indexP1 = j - 1;
							foundP0 = TRUE;
							foundP1 = TRUE;
						} else if (j - 1 < indexCircleCenter) { // 两个交点在圆心线段前面
							interX0 = segInterX0;
							interY0 = segInterY0;
							indexP0 = j;
							foundP0 = TRUE;
						} else { // 两个交点在圆心线段后面
							if (!foundP1) {
								interX1 = segInterX0;
								interY1 = segInterY0;
								indexP1 = j - 1;
								foundP1 = TRUE;
							}
						}
					} else if (interCount == 1) {
						if (j - 1 == indexCircleCenter) { // 交点恰在圆心线段上
							if (my_util_segment_length (lx, ly, circleCenterX, circleCenterY) 
									>= my_util_segment_length (lx, ly, segInterX0, segInterY0)) { // 交点在圆心前面
								
								interX0 = segInterX0;
								interY0 = segInterY0;
								indexP0 = j;
								foundP0 = TRUE;
							} else { // 交点在圆心后面
								if (!foundP1) {
									interX1 = segInterX0;
									interY1 = segInterY0;
									indexP1 = j - 1;
									foundP1 = TRUE;
								}
							}
						} else if (j - 1 < indexCircleCenter) { // 交点在圆心线段前面
							interX0 = segInterX0;
							interY0 = segInterY0;
							indexP0 = j;
							foundP0 = TRUE;
						} else { // 交点在圆心线段后面
							if (!foundP1) {
								interX1 = segInterX0;
								interY1 = segInterY0;
								indexP1 = j - 1;
								foundP1 = TRUE;
							}
						}
					}

					totalInterCount += interCount;
					lx = cx;
					ly = cy;
				} // for end

				// 满足画半圆条件
				if (foundP0 && foundP1) {
					cairo_move_to (cr, interX0 + appState->orignX, interY0 + appState->orignY);
					for (j = indexP0; j <= indexP1; j++) {
						cairo_line_to (cr, my_spline_peek_nth (spline, j)->x + appState->orignX, 
											my_spline_peek_nth (spline, j)->y + appState->orignY);
					}
					cairo_line_to (cr, interX1 + appState->orignX, interY1 + appState->orignY);
					angleStart = atan2 (interY1 - circleCenterY, interX1 - circleCenterX);
					angleEnd = atan2 (interY0 - circleCenterY, interX0 - circleCenterX);
					if (priv->overlapSide) {	
						cairo_arc (cr, circleCenterX + appState->orignX,
										circleCenterY + appState->orignY,
										overlapRadius, 
										angleStart,
										angleEnd);
					} else {
						cairo_arc_negative (cr, circleCenterX + appState->orignX,
										circleCenterY + appState->orignY,
										overlapRadius, 
										angleStart,
										angleEnd);
					}
					cairo_close_path (cr);
					cairo_fill (cr);
				}
				hitCenter ++;
				jmStartLen = jmOffset + hitCenter * jmInterval;
			} else {
				break;
			}
		} // while end
		theLastX = currentX;
		theLastY = currentY;
	} // for end
}


static void			draw_self_shadow	(MyShape* self, ApplicationState *appState) {
	

	
	MySplineOverlapPrivate* priv = MY_SPLINE_OVERLAP_GET_PRIVATE (self);
	MySpline *spline = MY_SPLINE (self);
	MySplineOverlap *splineOverlap = MY_SPLINE_OVERLAP (self);
	gdouble scale = appState->scale;

	cairo_t *cr = appState->cr;
	
	gdouble currentX, currentY, theLastX, theLastY,
			segmentLen, // 当前两点间线段的长度
			totalLen, 	// 当前point到线起点的总路程
			jmStartLen, // 下一个修饰图应出现的位置,即线的起点到下一个修饰图的起点的路程
			jmEndLen, 	// 下一个修饰图应出现的位置,即线的起点到下一个修饰图的终点的路程
			jmDrawnCount, 	// 已画修饰图的个数
			beyondLastPointLen,
			jmOffset,
			jmEdgeLen,
			jmInterval;	
	MyPoint jmStartPoint, 	// 修饰图起点
			jmEndPoint, 	// 修饰图终点
			jmPoint;		// 修饰图顶点
	gboolean gotJmStartPoint, 	// 是否找到一个修饰图的起点
			gotJmEndPoint;		// 是否找到修饰图的起点所对的终点

	gdouble lx, ly, cx, cy;
	gdouble interX0, interY0, interX1, interY1;	// 曲线与圆交点
	gdouble segInterX0, segInterY0, segInterX1, segInterY1;	// 线段与圆交点
	int interCount;	// 线段与圆交点个数
	int totalInterCount;
	gboolean foundP0, foundP1;	// P0,P1两个交点都找到就满足画半圆
	guint indexP0, // 交点所在线段端点索引。第一个交点所在线段的后端点
		indexP1;	// 第二个交点所在线段的前端点
	guint indexCircleCenter;	// 圆心所在的线段索引，0-based
	guint i, j, pointCount;
	gdouble circleCenterX, circleCenterY;
	gdouble angleStart, angleEnd;
	gdouble overlapRadius;
	int hitCenter;	// 命中圆心个数

	G_SUPER (draw_self_shadow, (self, appState));

	totalLen = 0;
	jmDrawnCount = 0;
	jmOffset = (priv->overlapOffset + priv->overlapRadius) * MY_SHAPE(self)->appState->scale;
	overlapRadius = priv->overlapRadius * MY_SHAPE(self)->appState->scale;
	jmInterval = priv->overlapInterval * MY_SHAPE(self)->appState->scale;
	jmStartLen = jmOffset;
	
	pointCount = my_spline_get_points_count (spline);
	if (pointCount < 2) {
		return;
	}
	
	//cairo_set_line_join (cr, CAIRO_LINE_JOIN_BEVEL);
	hitCenter = 0;
	theLastX = my_spline_peek_nth (spline, 0)->x;
	theLastY = my_spline_peek_nth (spline, 0)->y;
	for (i = 1; i < pointCount; i++) {
		currentX = my_spline_peek_nth (spline, i)->x;
		currentY = my_spline_peek_nth (spline, i)->y;
		segmentLen = sqrt ((currentX - theLastX) * (currentX - theLastX) + (currentY - theLastY) * (currentY - theLastY) );
		totalLen += segmentLen;

		while (TRUE) {
			if (totalLen >= jmStartLen) { // 距离满足圆心
				indexCircleCenter = i - 1;
				beyondLastPointLen = jmStartLen - (totalLen - segmentLen);
				jmStartPoint = my_util_part_segment (theLastX, theLastY, currentX, currentY, segmentLen, beyondLastPointLen);
				circleCenterX = jmStartPoint.x;
				circleCenterY = jmStartPoint.y;
				
				foundP0 = FALSE;
				foundP1 = FALSE;

				// 求圆与曲线交点
				totalInterCount = 0;
				lx = my_spline_peek_nth (spline, 0)->x;
				ly = my_spline_peek_nth (spline, 0)->y;
				for (j = 1; j < pointCount; j++) {
					if (foundP0 && foundP1) {
						break;
					}
					cx = my_spline_peek_nth (spline, j)->x;
					cy = my_spline_peek_nth (spline, j)->y;
					interCount = my_util_segment_inter_circle (lx, ly, cx, cy, circleCenterX, circleCenterY, overlapRadius, 
													&segInterX0, &segInterY0, &segInterX1, &segInterY1);
					if (interCount == 2) {
						if (j - 1 == indexCircleCenter) { // 两个交点恰在圆心线段上
							interX0 = segInterX0;
							interY0 = segInterY0;
							interX1 = segInterX1;
							interY1 = segInterY1;
							indexP0 = j;
							indexP1 = j - 1;
							foundP0 = TRUE;
							foundP1 = TRUE;
						} else if (j - 1 < indexCircleCenter) { // 两个交点在圆心线段前面
							interX0 = segInterX0;
							interY0 = segInterY0;
							indexP0 = j;
							foundP0 = TRUE;
						} else { // 两个交点在圆心线段后面
							if (!foundP1) {
								interX1 = segInterX0;
								interY1 = segInterY0;
								indexP1 = j - 1;
								foundP1 = TRUE;
							}
						}
					} else if (interCount == 1) {
						if (j - 1 == indexCircleCenter) { // 交点恰在圆心线段上
							if (my_util_segment_length (lx, ly, circleCenterX, circleCenterY) 
									>= my_util_segment_length (lx, ly, segInterX0, segInterY0)) { // 交点在圆心前面
								
								interX0 = segInterX0;
								interY0 = segInterY0;
								indexP0 = j;
								foundP0 = TRUE;
							} else { // 交点在圆心后面
								if (!foundP1) {
									interX1 = segInterX0;
									interY1 = segInterY0;
									indexP1 = j - 1;
									foundP1 = TRUE;
								}
							}
						} else if (j - 1 < indexCircleCenter) { // 交点在圆心线段前面
							interX0 = segInterX0;
							interY0 = segInterY0;
							indexP0 = j;
							foundP0 = TRUE;
						} else { // 交点在圆心线段后面
							if (!foundP1) {
								interX1 = segInterX0;
								interY1 = segInterY0;
								indexP1 = j - 1;
								foundP1 = TRUE;
							}
						}
					}

					totalInterCount += interCount;
					lx = cx;
					ly = cy;
				} // for end

				// 满足画半圆条件
				if (foundP0 && foundP1) {
					cairo_move_to (cr, interX0 / scale - self->shadowX, interY0 / scale - self->shadowY);
					for (j = indexP0; j <= indexP1; j++) {
						cairo_line_to (cr, my_spline_peek_nth (spline, j)->x / scale - self->shadowX, 
											my_spline_peek_nth (spline, j)->y / scale - self->shadowY);
					}
					cairo_line_to (cr, interX1 / scale - self->shadowX, interY1 / scale - self->shadowY);
					angleStart = atan2 (interY1 - circleCenterY, interX1 - circleCenterX);
					angleEnd = atan2 (interY0 - circleCenterY, interX0 - circleCenterX);
					if (priv->overlapSide) {	
						cairo_arc (cr, circleCenterX / scale - self->shadowX,
										circleCenterY / scale - self->shadowY,
										overlapRadius, 
										angleStart,
										angleEnd);
					} else {
						cairo_arc_negative (cr, circleCenterX / scale - self->shadowX,
										circleCenterY / scale - self->shadowY,
										overlapRadius, 
										angleStart,
										angleEnd);
					}
					cairo_close_path (cr);
					cairo_fill (cr);
				}
				hitCenter ++;
				jmStartLen = jmOffset + hitCenter * jmInterval;
			} else {
				break;
			}
		} // while end
		theLastX = currentX;
		theLastY = currentY;
	} // for end
}


static void		draw_self_shadow_debug		(MyShape* self) {
	
}


static void		on_shadow_dirty				(MyShape *self) {
	
	
	MySplineOverlapPrivate* priv = MY_SPLINE_OVERLAP_GET_PRIVATE (self);
	gdouble scale = self->appState->scale;
	self->shadowPadding = 2 * self->boxRadius + 0.707 * self->strokeWidth + priv->overlapRadius;
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

	
	MySplineOverlapPrivate* priv = MY_SPLINE_OVERLAP_GET_PRIVATE (self);
	G_SUPER (from_prototype_line, (self, prototype));

	priv->overlapOffset = prototype->overlapOffset;
	priv->overlapRadius = prototype->overlapRadius;
	priv->overlapSide = prototype->overlapSide;
	priv->overlapInterval = prototype->overlapInterval;
}
	

static void		to_prototype_line		(MyShape *self, MyPrototypeLine *prototype) {	
	

	MySplineOverlapPrivate* priv = MY_SPLINE_OVERLAP_GET_PRIVATE (self);
	G_SUPER (to_prototype_line, (self, prototype));

	prototype->style = MY_PROTOTYPE_STYLE_SPLINE_OVERLAP;
	prototype->overlapOffset = priv->overlapOffset;
	prototype->overlapRadius = priv->overlapRadius;
	prototype->overlapSide = priv->overlapSide;
	prototype->overlapInterval = priv->overlapInterval;
}