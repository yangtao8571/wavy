#ifndef __MY_ACTION_MANAGER_H__
#define __MY_ACTION_MANAGER_H__

#define MY_TYPE_ACTION_MANAGER                  (my_action_manager_get_type ())
#define MY_ACTION_MANAGER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_ACTION_MANAGER, MyActionManager))
#define MY_IS_ACTION_MANAGER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_ACTION_MANAGER))
#define MY_ACTION_MANAGER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), MY_TYPE_ACTION_MANAGER, MyActionManagerClass))
#define MY_IS_ACTION_MANAGER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), MY_TYPE_ACTION_MANAGER))
#define MY_ACTION_MANAGER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), MY_TYPE_ACTION_MANAGER, MyActionManagerClass))

typedef struct _MyActionManager			MyActionManager;
typedef struct _MyActionManagerClass	MyActionManagerClass;

struct _MyActionManager {
	GObject parent_instance;
	gint	limitSize;
};

struct _MyActionManagerClass {
	GObjectClass parent_class;
};

/* used by MY_TYPE_ACTION_MANAGER */
GType my_action_manager_get_type (void);

/* Overridable methods wrapper */

/* Non-Overridable Methods */
MyAction*	my_action_manager_peek_nth		(MyActionManager *self, guint n);
void		my_action_manager_update		(MyActionManager *self, gdouble x, gdouble y, gboolean isMousePressed, gint actChosen, MyShape *hitShape, MyMouse mouseButton);
gint		my_action_manager_get_act_type	(MyActionManager *self);
MyShape *	my_action_manager_get_active_graph_target	(MyActionManager *self);
MyShape *	my_action_manager_get_latest_graph_target	(MyActionManager *self);
MyShape *	my_action_manager_get_latest_2th_graph_target	(MyActionManager *self);
MyAction *	my_action_manager_get_active_action 			(MyActionManager *self);
MyAction *	my_action_manager_get_latest_action 			(MyActionManager *self);
MyAction *	my_action_manager_get_latest_2th_action 		(MyActionManager *self);
void		my_action_manager_append_finish_action			(MyActionManager *self);

#endif /*__MY_ACTION_MANAGER_H__*/