#define MY_LOG_LEVEL 4

#include <gtk/gtk.h>
#include <glib.h>
#include "my_constant.h"
#include "my_prototype_line.h"
#include "my_prototype_manager.h"
#include "my_application_state.h"
#include "my_rectangle.h"
#include "my_point.h"
#include "my_debug.h"
#include "my_shape.h"
#include "my_curve.h"
#include "my_action.h"

#define MY_ACTION_GET_PRIVATE(o)		(G_TYPE_INSTANCE_GET_PRIVATE ((o), MY_TYPE_ACTION, MyActionPrivate))

G_DEFINE_TYPE (MyAction, my_action, G_TYPE_OBJECT);
//G_DEFINE_TYPE_WITH_PRIVATE (MyAction, my_action, G_TYPE_OBJECT)
/*
typedef struct _MyActionPrivate {
	MyPoint*	startPoint;
	MyPoint*	endPoint;
	gint		length;
	GQueue* 	points;
} MyActionPrivate;
*/
/******************************************************************************
 * declare static function 
 ******************************************************************************/
static void my_action_init (MyAction* self);
static void my_action_finalize (GObject* object);
static void my_action_dispose (GObject* object);
/******************************************************************************
 * implement function 
 ******************************************************************************/
static void my_action_class_init (MyActionClass* klass)
{
	GObjectClass	*g_object_class;
	/* Add private structure */
	//g_type_class_add_private (klass, sizeof (MyActionPrivate));
	/* Get the parent gobject class */
	g_object_class = G_OBJECT_CLASS(klass);
	
    /* Hook overridable methods */
	
	/* Hook finalization functions */
	g_object_class->dispose = my_action_dispose; /* instance destructor, reverse of init */
	g_object_class->finalize = my_action_finalize; /* class finalization, reverse of class init */
}

// constructor
static void		my_action_init (MyAction* self)
{
	/* Initialize public fields */
	//self->value = 0;
	self->timeZone = g_time_zone_new (NULL);
	self->createTimestamp = g_date_time_new_now (self->timeZone);
 
	/* Initialize private fields */
	//MyActionPrivate* priv = MY_ACTION_GET_PRIVATE (self);
}

static void my_action_finalize (GObject* object)
{  
  /* Always chain up to the parent class; as with dispose(), finalize()
   * is guaranteed to exist on the parent's class virtual function table
   */
	G_OBJECT_CLASS (my_action_parent_class)->finalize (object);
}

static void my_action_dispose (GObject* object)
{
	/* Reverse what was allocated by instance init */
 
	  /* Always chain up to the parent class; there is no need to check if
   * the parent class implements the dispose() virtual function: it is
   * always guaranteed to do so
   */
	G_OBJECT_CLASS (my_action_parent_class)->dispose (object);
}

