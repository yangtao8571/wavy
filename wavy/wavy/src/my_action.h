#ifndef __MY_ACTION_H__
#define __MY_ACTION_H__

#define MY_TYPE_ACTION                  (my_action_get_type ())
#define MY_ACTION(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_ACTION, MyAction))
#define MY_IS_ACTION(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_ACTION))
#define MY_ACTION_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), MY_TYPE_ACTION, MyActionClass))
#define MY_IS_ACTION_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), MY_TYPE_ACTION))
#define MY_ACTION_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), MY_TYPE_ACTION, MyActionClass))

#define MY_ACTION_PHASE_UNSTART		1
#define MY_ACTION_PHASE_ACTING		2
#define MY_ACTION_PHASE_FINISHED	3

#define MY_ACTION_FUNC_SELECT			200
#define MY_ACTION_FUNC_STRAIGHT_LINE	211
#define MY_ACTION_FUNC_CURVE			212
#define MY_ACTION_FUNC_BEZIER			213
#define MY_ACTION_FUNC_BEZIER_JM		214
#define MY_ACTION_FUNC_BEZIER_SIN		215
#define MY_ACTION_FUNC_SPLINE			216
#define MY_ACTION_FUNC_SPLINE_CYC		217
#define MY_ACTION_FUNC_SPLINE_OVERLAP	218
#define MY_ACTION_FUNC_PICTURE			230

#define MY_ACTION_TYPE_UNKNOWN				300
#define MY_ACTION_TYPE_CLICK				301
#define MY_ACTION_TYPE_EDIT_CLICK			302
#define MY_ACTION_TYPE_DOUBLE_EDIT_CLICK	303
#define MY_ACTION_TYPE_DND					304
#define MY_ACTION_TYPE_RANGE_SELECT			305
#define MY_ACTION_TYPE_STRAIGHT_LINE	310
#define MY_ACTION_TYPE_CURVE			311
#define MY_ACTION_TYPE_BEZIER			312
#define MY_ACTION_TYPE_BEZIER_JM		313
#define MY_ACTION_TYPE_BEZIER_SIN		314
#define MY_ACTION_TYPE_SPLINE			315
#define MY_ACTION_TYPE_SPLINE_CYC		316
#define MY_ACTION_TYPE_SPLINE_OVERLAP	317
#define MY_ACTION_TYPE_PICTURE			330

typedef struct _MyAction		MyAction;
typedef struct _MyActionClass	MyActionClass;

struct _MyAction {
	GObject parent_instance;
	gint	id;
	gint	phase;
	gint	functionChosen;	// 已选择的操作功能
	MyShape* graphTarget;	// 操作对象
	gint	actType;		// 高级操作类型
	MyPoint	*startPoint;	
	MyPoint	*endPoint;
	GTimeZone *timeZone;
	GDateTime *createTimestamp;
};

struct _MyActionClass {
	GObjectClass parent_class;
};

/* used by MY_TYPE_ACTION */
GType my_action_get_type (void);

/* Overridable methods wrapper */

/* Non-Overridable Methods */

#endif /*__MY_ACTION_H__*/