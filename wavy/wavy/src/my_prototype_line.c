#define MY_LOG_LEVEL 3

#include <assert.h>
#include <gtk/gtk.h>
#include <glib-object.h>
#include "my_constant.h"
#include "my_prototype_line.h"


#define MY_PROTOTYPE_LINE_GET_PRIVATE(o)		(G_TYPE_INSTANCE_GET_PRIVATE ((o), MY_TYPE_PROTOTYPE_LINE, MyPrototypeLinePrivate))

G_DEFINE_TYPE (MyPrototypeLine, my_prototype_line, G_TYPE_OBJECT);
//G_DEFINE_TYPE_WITH_PRIVATE (MyPrototypeLine, my_prototype_line, G_TYPE_OBJECT)
 
typedef struct _MyPrototypeLinePrivate {
	void *noop;
} MyPrototypeLinePrivate;

/******************************************************************************
 * static private variable 
 ******************************************************************************/
 
/******************************************************************************
 * declare static function 
 ******************************************************************************/
static void my_prototype_line_init (MyPrototypeLine* self);
static void my_prototype_line_finalize (GObject* object);
static void my_prototype_line_dispose (GObject* object);
/******************************************************************************
 * implement function 
 ******************************************************************************/
static void my_prototype_line_class_init (MyPrototypeLineClass* klass)
{
	GObjectClass	*g_object_class;
	// Add private structure
	g_type_class_add_private (klass, sizeof (MyPrototypeLinePrivate));
	// Get the parent gobject class
	g_object_class = G_OBJECT_CLASS(klass);
	
    /* Hook overridable methods */
	
	/* Hook finalization functions */
	g_object_class->dispose = my_prototype_line_dispose; /* instance destructor, reverse of init */
	g_object_class->finalize = my_prototype_line_finalize; /* class finalization, reverse of class init */
}

// constructor
static void		my_prototype_line_init (MyPrototypeLine* self)
{
	/* Initialize public fields */
	
	/* Initialize private fields */
	MyPrototypeLinePrivate* priv = MY_PROTOTYPE_LINE_GET_PRIVATE (self);
	
}

static void my_prototype_line_finalize (GObject* object)
{  
  /* Always chain up to the parent class; as with dispose(), finalize()
   * is guaranteed to exist on the parent's class virtual function table
   */
	G_OBJECT_CLASS (my_prototype_line_parent_class)->finalize (object);
}

static void my_prototype_line_dispose (GObject* object)
{
	/* Reverse what was allocated by instance init */
	MyPrototypeLine *self = MY_PROTOTYPE_LINE (object);
	MyPrototypeLinePrivate* priv = MY_PROTOTYPE_LINE_GET_PRIVATE (self);
 
	if (self->orignDashesStr) {
		g_free (self->orignDashesStr);
	}

  /* Always chain up to the parent class; there is no need to check if
   * the parent class implements the dispose() virtual function: it is
   * always guaranteed to do so
   */
	G_OBJECT_CLASS (my_prototype_line_parent_class)->dispose (object);
}


void		my_prototype_line_set_orign_dashes_str		(MyPrototypeLine *self, const char *s) {

	self->orignDashesStr = g_strdup (s);
}