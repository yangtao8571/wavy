#define MY_LOG_LEVEL 3

#include <assert.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <glib-object.h>
#include "my_constant.h"
#include "my_rectangle.h"
#include "my_point.h"
#include "my_vector.h"
#include "my_util.h"
#include "my_application_state.h"
#include "my_shape.h"
#include "my_curve.h"
#include "my_bezier.h"
#include "my_bezier_jm.h"
#include "my_bezier_sin.h"
#include "my_spline.h"
#include "my_spline_cyc.h"
#include "my_prototype_line.h"
#include "my_combobox_options_manager.h"


#define MY_COMBOBOX_OPTIONS_MANAGER_GET_PRIVATE(o)		(G_TYPE_INSTANCE_GET_PRIVATE ((o), MY_TYPE_COMBOBOX_OPTIONS_MANAGER, MyComboboxOptionsManagerPrivate))

G_DEFINE_TYPE (MyComboboxOptionsManager, my_combobox_options_manager, G_TYPE_OBJECT);
//G_DEFINE_TYPE_WITH_PRIVATE (MyComboboxOptionsManager, my_combobox_options_manager, G_TYPE_OBJECT)
 
typedef struct _MyComboboxOptionsManagerPrivate {
	GHashTable *nameOptionsDict;
} MyComboboxOptionsManagerPrivate;

/******************************************************************************
 * static private variable 
 ******************************************************************************/
 
/******************************************************************************
 * declare static function 
 ******************************************************************************/
static void my_combobox_options_manager_init (MyComboboxOptionsManager* self);
static void my_combobox_options_manager_finalize (GObject* object);
static void my_combobox_options_manager_dispose (GObject* object);
/******************************************************************************
 * implement function 
 ******************************************************************************/
static void my_combobox_options_manager_class_init (MyComboboxOptionsManagerClass* klass)
{
	GObjectClass	*g_object_class;
	// Add private structure
	g_type_class_add_private (klass, sizeof (MyComboboxOptionsManagerPrivate));
	// Get the parent gobject class
	g_object_class = G_OBJECT_CLASS(klass);
	
    /* Hook overridable methods */
	
	/* Hook finalization functions */
	g_object_class->dispose = my_combobox_options_manager_dispose; /* instance destructor, reverse of init */
	g_object_class->finalize = my_combobox_options_manager_finalize; /* class finalization, reverse of class init */
}

// constructor
static void		my_combobox_options_manager_init (MyComboboxOptionsManager* self)
{
	/* Initialize public fields */
	
	/* Initialize private fields */
	MyComboboxOptionsManagerPrivate* priv = MY_COMBOBOX_OPTIONS_MANAGER_GET_PRIVATE (self);
	priv->nameOptionsDict = g_hash_table_new (g_str_hash, g_str_equal);

}

static void my_combobox_options_manager_finalize (GObject* object)
{  
  /* Always chain up to the parent class; as with dispose(), finalize()
   * is guaranteed to exist on the parent's class virtual function table
   */
	G_OBJECT_CLASS (my_combobox_options_manager_parent_class)->finalize (object);
}

static void my_combobox_options_manager_dispose (GObject* object)
{
	/* Reverse what was allocated by instance init */
	MyComboboxOptionsManager *self = MY_COMBOBOX_OPTIONS_MANAGER (object);
	MyComboboxOptionsManagerPrivate* priv = MY_COMBOBOX_OPTIONS_MANAGER_GET_PRIVATE (self);
 

	//todo: complete delete


  /* Always chain up to the parent class; there is no need to check if
   * the parent class implements the dispose() virtual function: it is
   * always guaranteed to do so
   */
	G_OBJECT_CLASS (my_combobox_options_manager_parent_class)->dispose (object);
}


void			my_combobox_options_manager_append_option		(MyComboboxOptionsManager *self, gchar *name, MyComboboxOption *option) {

	
	MyComboboxOptionsManagerPrivate* priv = MY_COMBOBOX_OPTIONS_MANAGER_GET_PRIVATE (self);
	GQueue *options = (GQueue *) g_hash_table_lookup (priv->nameOptionsDict, name);
	if (!options) {
		options = g_queue_new ();
		g_hash_table_insert (priv->nameOptionsDict, name, options);
	}
	g_queue_push_tail (options, option);
}


GQueue *			my_combobox_options_manager_get_options			(MyComboboxOptionsManager *self, gchar *name) {

	
	MyComboboxOptionsManagerPrivate* priv = MY_COMBOBOX_OPTIONS_MANAGER_GET_PRIVATE (self);
	GQueue *options = (GQueue *) g_hash_table_lookup (priv->nameOptionsDict, name);
	return options;
}


gint			my_combobox_options_manager_get_option_index			(MyComboboxOptionsManager *self, gchar *name, gint value) {

	
	gint index = -1;
	guint i, count;
	MyComboboxOption *option;
	MyComboboxOptionsManagerPrivate* priv = MY_COMBOBOX_OPTIONS_MANAGER_GET_PRIVATE (self);
	GQueue *options = (GQueue *) g_hash_table_lookup (priv->nameOptionsDict, name);
	if (options) {
		for (i = 0, count = g_queue_get_length (options); i < count; i++) {
			option = g_queue_peek_nth (options, i);
			if (option->value == value) {
				index = i;
				break;
			}
		}
	} else {
		index = -1;
	}
	return index;
}


gint			my_combobox_options_manager_get_option_value	(MyComboboxOptionsManager *self, gchar *name, gint index) {

	
	gint value = -1;
	guint i, count;
	MyComboboxOption *option;
	MyComboboxOptionsManagerPrivate* priv = MY_COMBOBOX_OPTIONS_MANAGER_GET_PRIVATE (self);
	GQueue *options = (GQueue *) g_hash_table_lookup (priv->nameOptionsDict, name);
	if (options) {
		i = index;
		option = g_queue_peek_nth (options, i);
		if (option) {
			value = option->value;
		} else {
			value = -1;
		}		
	} else {
		value = -1;
	}
	return value;
}