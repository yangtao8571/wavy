#define MY_LOG_LEVEL 3

#include <gtk/gtk.h>
#include "my_debug.h"
#include "my_constant.h"
#include "my_point.h"
#include "my_prototype_line.h"
#include "my_prototype_manager.h"
#include "my_application_state.h"
#include "my_rectangle.h"
#include "my_shape.h"
#include "my_curve.h"
#include "my_vector.h"
#include "my_control_shape_rect.h"
#include "my_action.h"
#include "my_action_manager.h"

#define MY_ACTION_MANAGER_GET_PRIVATE(o)		(G_TYPE_INSTANCE_GET_PRIVATE ((o), MY_TYPE_ACTION_MANAGER, MyActionManagerPrivate))

G_DEFINE_TYPE (MyActionManager, my_action_manager, G_TYPE_OBJECT);
//G_DEFINE_TYPE_WITH_PRIVATE (MyActionManager, my_action_manager, G_TYPE_OBJECT)
 
typedef struct _MyActionManagerPrivate {
	GQueue* 	historyActions;	// 保存所有action
	MyAction*	activeAction;	// 在一次鼠标按下与松开之间存在，按下时创建，松开时结束
	MyAction*	latestFinishedAction;
} MyActionManagerPrivate;

/******************************************************************************
 * static private variable 
 ******************************************************************************/

/******************************************************************************
 * declare static function 
 ******************************************************************************/
static void my_action_manager_init (MyActionManager* self);
static void my_action_manager_finalize (GObject* object);
static void my_action_manager_dispose (GObject* object);
/******************************************************************************
 * implement function 
 ******************************************************************************/
static void my_action_manager_class_init (MyActionManagerClass* klass)
{
	GObjectClass	*g_object_class;
	// Add private structure
	g_type_class_add_private (klass, sizeof (MyActionManagerPrivate));
	// Get the parent gobject class
	g_object_class = G_OBJECT_CLASS(klass);
	
    /* Hook overridable methods */
	
	/* Hook finalization functions */
	g_object_class->dispose = my_action_manager_dispose; /* instance destructor, reverse of init */
	g_object_class->finalize = my_action_manager_finalize; /* class finalization, reverse of class init */
}

// constructor
static void		my_action_manager_init (MyActionManager* self)
{
	MyActionManagerPrivate* priv = MY_ACTION_MANAGER_GET_PRIVATE (self);
	/* Initialize public fields */
	self->limitSize = 20;
	
	/* Initialize private fields */
	priv->historyActions = g_queue_new();
}

static void my_action_manager_finalize (GObject* object)
{  
  /* Always chain up to the parent class; as with dispose(), finalize()
   * is guaranteed to exist on the parent's class virtual function table
   */
	G_OBJECT_CLASS (my_action_manager_parent_class)->finalize (object);
}

static void my_action_manager_dispose (GObject* object)
{
	/* Reverse what was allocated by instance init */
	MyActionManager *self = MY_ACTION_MANAGER (object);
	MyActionManagerPrivate* priv = MY_ACTION_MANAGER_GET_PRIVATE (self);
 
	g_queue_foreach (priv->historyActions, (GFunc) g_object_unref, NULL);
	g_queue_free (priv->historyActions);
	g_object_unref (priv->activeAction);
	g_object_unref (priv->latestFinishedAction);

  /* Always chain up to the parent class; there is no need to check if
   * the parent class implements the dispose() virtual function: it is
   * always guaranteed to do so
   */
	G_OBJECT_CLASS (my_action_manager_parent_class)->dispose (object);
}

MyAction* my_action_manager_peek_nth (MyActionManager *self, guint n) {
	return (MyAction*) g_queue_peek_nth (MY_ACTION_MANAGER_GET_PRIVATE (self)->historyActions, n);
}

/**
 * 取action的顺序为，先active action，若没有则取latestFinishedAction的
 */
gint	my_action_manager_get_act_type	(MyActionManager *self) {

	MyActionManagerPrivate* priv = MY_ACTION_MANAGER_GET_PRIVATE (self);
	if (priv->activeAction) {
		my_debug ("priv activeAction %#x", priv->activeAction);
		my_debug ("priv activeAction actType: %d", priv->activeAction->actType);
		return priv->activeAction->actType;
	}
	if (priv->latestFinishedAction) {
		my_debug ("priv latestFinishedAction %#x", priv->latestFinishedAction);
		my_debug ("priv latestFinishedAction actType: %d", priv->latestFinishedAction->actType);
		return priv->latestFinishedAction->actType;
	}
	return MY_ACTION_TYPE_UNKNOWN;
}

void	my_action_manager_update	(MyActionManager *self, gdouble x, gdouble y, gboolean isMousePressed, gint actChosen, MyShape *hitShape, MyMouse mouseButton) {

	MyActionManagerPrivate* priv = MY_ACTION_MANAGER_GET_PRIVATE (self);
	GTimeSpan timeSpan;

	my_debug ("action manager updating...");
	if (isMousePressed) { // 鼠标处于按下状态
		my_debug ("mouse pressing");
		if (priv->activeAction == NULL) { // 新建一个action
			priv->activeAction = g_object_new (MY_TYPE_ACTION, NULL);
			priv->activeAction->phase = MY_ACTION_PHASE_ACTING;
			priv->activeAction->functionChosen = actChosen;
			priv->activeAction->graphTarget = hitShape;
			if (hitShape) {
				if (hitShape->isControlShape)
					priv->activeAction->actType = MY_ACTION_TYPE_DND;
				else
					priv->activeAction->actType = MY_ACTION_TYPE_EDIT_CLICK;
			}
			else
				priv->activeAction->actType = MY_ACTION_TYPE_CLICK;
			priv->activeAction->startPoint = g_malloc (sizeof(MyPoint));
			priv->activeAction->startPoint->x = x;
			priv->activeAction->startPoint->y = y;
			
			if (actChosen == MY_ACTION_FUNC_SPLINE) {	// 把单击改为SPLINE
				priv->activeAction->actType = MY_ACTION_TYPE_SPLINE;
			} else if (actChosen == MY_ACTION_FUNC_SPLINE_OVERLAP) {	// 把单击改为SPLINE_OVERLAP
				priv->activeAction->actType = MY_ACTION_TYPE_SPLINE_OVERLAP;
			} else if (actChosen == MY_ACTION_FUNC_SPLINE_CYC) {	// 把单击改为SPLINE_CYC
				priv->activeAction->actType = MY_ACTION_TYPE_SPLINE_CYC;
			}
			my_debug ("new activeAction %#x type %d target shape %#x", priv->activeAction, priv->activeAction->actType, hitShape);
		} else { // 已有action,更新状态
			if (priv->activeAction->startPoint->x == x && priv->activeAction->startPoint->y == y) { // 鼠标没动
				
			} else { // 鼠标动了
			
				if (actChosen == MY_ACTION_FUNC_SELECT) { // 选择模式下
					if (priv->activeAction->graphTarget) { // 点中了图形,为拖拽操作
						priv->activeAction->actType = MY_ACTION_TYPE_DND;						
					} else { // 为范围选择操作
						priv->activeAction->actType = MY_ACTION_TYPE_RANGE_SELECT;
					}
				} else if (actChosen == MY_ACTION_FUNC_STRAIGHT_LINE) {
					priv->activeAction->actType = MY_ACTION_TYPE_STRAIGHT_LINE;
				} else if (actChosen == MY_ACTION_FUNC_CURVE) {
					priv->activeAction->actType = MY_ACTION_TYPE_CURVE;
				} else if (actChosen == MY_ACTION_FUNC_BEZIER) {
					priv->activeAction->actType = MY_ACTION_TYPE_BEZIER;
					my_debug ("activeAction  %#x graphTarget %#x", priv->activeAction, priv->activeAction->graphTarget);
				} else if (actChosen == MY_ACTION_FUNC_BEZIER_JM) {
					priv->activeAction->actType = MY_ACTION_TYPE_BEZIER_JM;
				} else if (actChosen == MY_ACTION_FUNC_BEZIER_SIN) {
					priv->activeAction->actType = MY_ACTION_TYPE_BEZIER_SIN;
				}
				if (priv->activeAction->graphTarget
						&& MY_IS_CONTROL_SHAPE_RECT (priv->activeAction->graphTarget)) { // 不论何种模式下,点中了控制图形,为拖拽操作
					priv->activeAction->actType = MY_ACTION_TYPE_DND;
				}
			}
		}
		// 判断是否为双击操作
		if (priv->latestFinishedAction
			&& (priv->latestFinishedAction->actType == MY_ACTION_TYPE_EDIT_CLICK
				|| priv->latestFinishedAction->actType == MY_ACTION_TYPE_DND)) {
			timeSpan = g_date_time_difference (priv->activeAction->createTimestamp, 
											priv->latestFinishedAction->createTimestamp);
			if (timeSpan < 400000) { // 单位微秒,两次单击间隔足够小,则把单击改为双击action
				priv->activeAction->actType = MY_ACTION_TYPE_DOUBLE_EDIT_CLICK;
			}
		}
		my_debug ("activeAction %#x actType is %d target graph %#x", priv->activeAction, priv->activeAction->actType, priv->activeAction->graphTarget);
	} else {	// 鼠标处于松开状态
		my_debug ("mouse released");
		if (priv->activeAction) {
			/*
			if (priv->activeAction->actType == MY_ACTION_TYPE_UNKNOWN) {
				
				if (priv->activeAction->startPoint->x == x && priv->activeAction->startPoint->y == y) { // 鼠标没移动,单击操作结束
					priv->activeAction->actType = MY_ACTION_TYPE_CLICK;
					priv->activeAction->endPoint = priv->activeAction->startPoint;
					priv->activeAction->phase = MY_ACTION_PHASE_FINISHED;
				} else { // 鼠标移动了
					my_info ("Impossible to get here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
				}	
			} else { // 结束操作
				priv->activeAction->endPoint = g_malloc (sizeof(MyPoint));
				priv->activeAction->endPoint->x = x;
				priv->activeAction->endPoint->y = y;
				priv->activeAction->phase = MY_ACTION_PHASE_FINISHED;
			}
			*/
			 // 结束操作
			priv->activeAction->endPoint = g_malloc (sizeof(MyPoint));
			priv->activeAction->endPoint->x = x;
			priv->activeAction->endPoint->y = y;
			priv->activeAction->phase = MY_ACTION_PHASE_FINISHED;
			
			
			if (priv->activeAction->actType == MY_ACTION_TYPE_SPLINE
				|| priv->activeAction->actType == MY_ACTION_TYPE_SPLINE_OVERLAP
				|| priv->activeAction->actType == MY_ACTION_TYPE_SPLINE_CYC) {
				if (mouseButton != MY_MOUSE_RIGHT) {
					priv->activeAction->phase = MY_ACTION_PHASE_ACTING;						
				}
			}
			if (priv->activeAction->actType == MY_ACTION_TYPE_CLICK
				|| priv->activeAction->actType == MY_ACTION_TYPE_EDIT_CLICK) {
				
				if (priv->activeAction->actType == MY_ACTION_TYPE_CLICK) {
					
					if (/*priv->activeAction->functionChosen == MY_ACTION_FUNC_EDIT
						&& */priv->activeAction->graphTarget) {	// 把单击改为编辑单击
						priv->activeAction->actType = MY_ACTION_TYPE_EDIT_CLICK;
					} else if (priv->activeAction->functionChosen == MY_ACTION_FUNC_PICTURE) { // 把单击改为插入图片
						priv->activeAction->actType = MY_ACTION_TYPE_PICTURE;
					}
					/* else if (priv->activeAction->functionChosen == MY_ACTION_FUNC_CURVE_BEZIER) { // 把单击改为贝兹
						if (priv->latestFinishedAction && priv->latestFinishedAction->actType != MY_ACTION_TYPE_CURVE_BEZIER_I
							|| priv->latestFinishedAction == NULL) {
							priv->activeAction->actType = MY_ACTION_TYPE_CURVE_BEZIER_I;
							priv->activeAction->phase = MY_ACTION_PHASE_FINISHED;
						} else {
							priv->activeAction->actType = MY_ACTION_TYPE_CURVE_BEZIER_II;
							priv->activeAction->phase = MY_ACTION_PHASE_FINISHED;
						}
					} else if (priv->activeAction->functionChosen == MY_ACTION_FUNC_CURVE) { // 把单击改为线段
						if (priv->latestFinishedAction && priv->latestFinishedAction->actType != MY_ACTION_TYPE_CURVE_SEGMENT_I
							|| priv->latestFinishedAction == NULL) {
							priv->activeAction->actType = MY_ACTION_TYPE_CURVE_SEGMENT_I;
							priv->activeAction->phase = MY_ACTION_PHASE_FINISHED;
						} else {
							priv->activeAction->actType = MY_ACTION_TYPE_CURVE_SEGMENT_II;
							priv->activeAction->phase = MY_ACTION_PHASE_FINISHED;
						} 
					}*/
				}
			}
			
			
			my_debug ("close activeAction %#x actType is %d target %#x", priv->activeAction, priv->activeAction->actType, priv->activeAction->graphTarget);
			
			// 将操作加入历史记录栈中
			g_queue_push_head (priv->historyActions, priv->activeAction);
			my_debug ("is object class? %d", G_TYPE_IS_OBJECT(MY_TYPE_CURVE));
			my_debug ("is object? %d", G_IS_OBJECT(priv->activeAction));
			priv->latestFinishedAction = g_object_ref (G_OBJECT(priv->activeAction));
			my_debug ("ref done");
			g_object_unref (G_OBJECT(priv->activeAction));
			my_debug ("unref done");
			priv->activeAction = NULL;
		}
	}
	
	// 限制操作历史栈的长度
	if (g_queue_get_length (priv->historyActions) > self->limitSize) {
		MyAction *action = g_queue_pop_tail (priv->historyActions);
		g_object_unref (action);
	}
	my_debug ("action manager update done.");
}

MyShape *	my_action_manager_get_active_graph_target	(MyActionManager *self) {

	MyActionManagerPrivate* priv = MY_ACTION_MANAGER_GET_PRIVATE (self);
	if (priv->activeAction) {
		my_debug ("get active graph target %#x of activeAction %#x", priv->activeAction->graphTarget, priv->activeAction);
		return priv->activeAction->graphTarget;
	}
	my_debug ("no activeAction no target graph");
	return NULL;
}

MyShape *	my_action_manager_get_latest_graph_target	(MyActionManager *self) {

	MyActionManagerPrivate* priv = MY_ACTION_MANAGER_GET_PRIVATE (self);
	if (priv->latestFinishedAction) {
		return g_object_ref (G_OBJECT(priv->latestFinishedAction->graphTarget));
	}
	return NULL;
}

MyShape *	my_action_manager_get_latest_2th_graph_target	(MyActionManager *self) {

	MyActionManagerPrivate* priv = MY_ACTION_MANAGER_GET_PRIVATE (self);
	if (priv->historyActions && g_queue_get_length(priv->historyActions) >= 2) {
		my_debug ("latest 2th graph target%#x", MY_ACTION(g_queue_peek_nth (priv->historyActions, 1))->graphTarget);
		return g_object_ref (G_OBJECT(MY_ACTION(g_queue_peek_nth (priv->historyActions, 1))->graphTarget));
	}
	return NULL;
}

MyAction *	my_action_manager_get_active_action (MyActionManager *self) {

	MyActionManagerPrivate* priv = MY_ACTION_MANAGER_GET_PRIVATE (self);
	return priv->activeAction;
}

MyAction *	my_action_manager_get_latest_action (MyActionManager *self) {

	MyActionManagerPrivate* priv = MY_ACTION_MANAGER_GET_PRIVATE (self);
	return priv->latestFinishedAction;
}


MyAction *	my_action_manager_get_latest_2th_action (MyActionManager *self) {

	
	MyActionManagerPrivate* priv = MY_ACTION_MANAGER_GET_PRIVATE (self);
	return g_queue_peek_nth (priv->historyActions, 1);
}


void		my_action_manager_append_finish_action			(MyActionManager *self) {

	
	MyActionManagerPrivate* priv = MY_ACTION_MANAGER_GET_PRIVATE (self);
	my_action_manager_update (self, 0, 0, TRUE, MY_ACTION_FUNC_SELECT, NULL, MY_MOUSE_LEFT);
	my_action_manager_update (self, 0, 0, FALSE, MY_ACTION_FUNC_SELECT, NULL, MY_MOUSE_LEFT);
}