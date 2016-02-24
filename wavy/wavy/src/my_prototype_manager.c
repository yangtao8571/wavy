#define MY_LOG_LEVEL 3

#include <assert.h>
#include <gtk/gtk.h>
#include <glib-object.h>
#include "my_constant.h"
#include "my_math.h"
#include "my_prototype_line.h"
#include "my_prototype_manager.h"

#define MY_PROTOTYPE_MANAGER_GET_PRIVATE(o)		(G_TYPE_INSTANCE_GET_PRIVATE ((o), MY_TYPE_PROTOTYPE_MANAGER, MyPrototypeManagerPrivate))

G_DEFINE_TYPE (MyPrototypeManager, my_prototype_manager, G_TYPE_OBJECT);
//G_DEFINE_TYPE_WITH_PRIVATE (MyPrototypeManager, my_prototype_manager, G_TYPE_OBJECT)
 
typedef struct _MyPrototypeManagerPrivate {
	GQueue *standardQueue;
	GQueue *fastQueue;
	MyPrototypeLine *currentPrototype;
} MyPrototypeManagerPrivate;

/******************************************************************************
 * static private variable 
 ******************************************************************************/
 
/******************************************************************************
 * declare static function 
 ******************************************************************************/
static void my_prototype_manager_init (MyPrototypeManager* self);
static void my_prototype_manager_finalize (GObject* object);
static void my_prototype_manager_dispose (GObject* object);
/******************************************************************************
 * implement function 
 ******************************************************************************/
static void my_prototype_manager_class_init (MyPrototypeManagerClass* klass)
{
	GObjectClass	*g_object_class;
	// Add private structure
	g_type_class_add_private (klass, sizeof (MyPrototypeManagerPrivate));
	// Get the parent gobject class
	g_object_class = G_OBJECT_CLASS(klass);
	
    /* Hook overridable methods */
	
	/* Hook finalization functions */
	g_object_class->dispose = my_prototype_manager_dispose; /* instance destructor, reverse of init */
	g_object_class->finalize = my_prototype_manager_finalize; /* class finalization, reverse of class init */
}

// constructor
static void		my_prototype_manager_init (MyPrototypeManager* self)
{
	/* Initialize public fields */
	
	/* Initialize private fields */
	MyPrototypeManagerPrivate* priv = MY_PROTOTYPE_MANAGER_GET_PRIVATE (self);
	priv->standardQueue = g_queue_new ();
	priv->fastQueue = g_queue_new ();
	priv->currentPrototype = NULL;
}

static void my_prototype_manager_finalize (GObject* object)
{  
  /* Always chain up to the parent class; as with dispose(), finalize()
   * is guaranteed to exist on the parent's class virtual function table
   */
	G_OBJECT_CLASS (my_prototype_manager_parent_class)->finalize (object);
}

static void my_prototype_manager_dispose (GObject* object)
{
	/* Reverse what was allocated by instance init */
	MyPrototypeManager *self = MY_PROTOTYPE_MANAGER (object);
	MyPrototypeManagerPrivate* priv = MY_PROTOTYPE_MANAGER_GET_PRIVATE (self);
 
	g_queue_foreach (priv->standardQueue, g_free, NULL);
	g_queue_free (priv->standardQueue);
	g_queue_foreach (priv->fastQueue, g_free, NULL);
	g_queue_free (priv->fastQueue);
	g_free (priv->currentPrototype);

  /* Always chain up to the parent class; there is no need to check if
   * the parent class implements the dispose() virtual function: it is
   * always guaranteed to do so
   */
	G_OBJECT_CLASS (my_prototype_manager_parent_class)->dispose (object);
}


void				my_prototype_manager_init_prototypes_of_standard_category	(MyPrototypeManager *self) {
	
	
	MyPrototypeLine *standardPrototype = g_object_new (MY_TYPE_PROTOTYPE_LINE, NULL);
	my_prototype_manager_add_prototype (self, MY_PROTOTYPE_CATEGORY_STANDARD, standardPrototype);
	standardPrototype->style = MY_PROTOTYPE_STYLE_STRAIGHT_LINE;
	standardPrototype->red = 1.;
	standardPrototype->green = 0.;
	standardPrototype->blue = 0.;
	standardPrototype->alpha = 1.;
	standardPrototype->strokeWidth = 5.;
	my_prototype_line_set_orign_dashes_str (standardPrototype, "5 2 5 2");
	standardPrototype->dashOffset = 0.;
	standardPrototype->isShadowing = FALSE;
	standardPrototype->shadowDeltaX = 4.;
	standardPrototype->shadowDeltaY = 4.;	
	
	standardPrototype = g_object_new (MY_TYPE_PROTOTYPE_LINE, NULL);
	my_prototype_manager_add_prototype (self, MY_PROTOTYPE_CATEGORY_STANDARD, standardPrototype);
	standardPrototype->style = MY_PROTOTYPE_STYLE_SPLINE;
	standardPrototype->red = 1.;
	standardPrototype->green = 0.;
	standardPrototype->blue = 0.;
	standardPrototype->alpha = 1.;
	standardPrototype->strokeWidth = 5.;
	my_prototype_line_set_orign_dashes_str (standardPrototype, "5 2 5 2");
	standardPrototype->dashOffset = 0.;
	standardPrototype->isShadowing = FALSE;
	standardPrototype->shadowDeltaX = 4.;
	standardPrototype->shadowDeltaY = 4.;
	standardPrototype->segmentCountPerHemite = 20;
	
	standardPrototype = g_object_new (MY_TYPE_PROTOTYPE_LINE, NULL);
	my_prototype_manager_add_prototype (self, MY_PROTOTYPE_CATEGORY_STANDARD, standardPrototype);
	standardPrototype->style = MY_PROTOTYPE_STYLE_SPLINE_CYC;
	standardPrototype->red = 1.;
	standardPrototype->green = 0.;
	standardPrototype->blue = 1.;
	standardPrototype->alpha = 1.;
	standardPrototype->strokeWidth = 5.;
	my_prototype_line_set_orign_dashes_str (standardPrototype, "5 2 5 2");
	standardPrototype->dashOffset = 0.;
	standardPrototype->isShadowing = FALSE;
	standardPrototype->shadowDeltaX = 4.;
	standardPrototype->shadowDeltaY = 4.;	
	standardPrototype->segmentCountPerHemite = 20;
	
	standardPrototype = g_object_new (MY_TYPE_PROTOTYPE_LINE, NULL);
	my_prototype_manager_add_prototype (self, MY_PROTOTYPE_CATEGORY_STANDARD, standardPrototype);
	standardPrototype->style = MY_PROTOTYPE_STYLE_SPLINE_OVERLAP;
	standardPrototype->red = 0.;
	standardPrototype->green = 0.;
	standardPrototype->blue = 0.;
	standardPrototype->alpha = 1.;
	standardPrototype->strokeWidth = 2.;
	my_prototype_line_set_orign_dashes_str (standardPrototype, "5 2 5 2");
	standardPrototype->dashOffset = 0.;
	standardPrototype->isShadowing = FALSE;
	standardPrototype->shadowDeltaX = 4.;
	standardPrototype->shadowDeltaY = 4.;	
	standardPrototype->segmentCountPerHemite = 20;
	standardPrototype->overlapOffset = 1;
	standardPrototype->overlapRadius = 5;
	standardPrototype->overlapInterval = 20;
	standardPrototype->overlapSide = 1;
	
	standardPrototype = g_object_new (MY_TYPE_PROTOTYPE_LINE, NULL);
	my_prototype_manager_add_prototype (self, MY_PROTOTYPE_CATEGORY_STANDARD, standardPrototype);
	standardPrototype->style = MY_PROTOTYPE_STYLE_BEZIER;
	standardPrototype->red = 0.;
	standardPrototype->green = 1.;
	standardPrototype->blue = 0.;
	standardPrototype->alpha = 1.;
	standardPrototype->strokeWidth = 5.;
	my_prototype_line_set_orign_dashes_str (standardPrototype, "5 2 5 2");
	standardPrototype->dashOffset = 0.;
	standardPrototype->isShadowing = FALSE;
	standardPrototype->shadowDeltaX = 4.;
	standardPrototype->shadowDeltaY = 4.;
	
	standardPrototype = g_object_new (MY_TYPE_PROTOTYPE_LINE, NULL);
	my_prototype_manager_add_prototype (self, MY_PROTOTYPE_CATEGORY_STANDARD, standardPrototype);
	standardPrototype->style = MY_PROTOTYPE_STYLE_BEZIER_JM;
	standardPrototype->red = 1.;
	standardPrototype->green = 1.;
	standardPrototype->blue = 0.;
	standardPrototype->alpha = 1.;
	standardPrototype->strokeWidth = 5.;
	my_prototype_line_set_orign_dashes_str (standardPrototype, "5 2 5 2");
	standardPrototype->dashOffset = 0.;
	standardPrototype->isShadowing = FALSE;
	standardPrototype->shadowDeltaX = 4.;
	standardPrototype->shadowDeltaY = 4.;
	standardPrototype->jmOffset = 20;
	standardPrototype->jmInterval = 50;
	standardPrototype->jmEdgeLen = 10;
	standardPrototype->jmSide = -1;
	
	standardPrototype = g_object_new (MY_TYPE_PROTOTYPE_LINE, NULL);
	my_prototype_manager_add_prototype (self, MY_PROTOTYPE_CATEGORY_STANDARD, standardPrototype);
	standardPrototype->style = MY_PROTOTYPE_STYLE_BEZIER_SIN;
	standardPrototype->red = 0.;
	standardPrototype->green = 0.;
	standardPrototype->blue = 1.;
	standardPrototype->alpha = 1.;
	standardPrototype->strokeWidth = 5.;
	my_prototype_line_set_orign_dashes_str (standardPrototype, "5 2 5 2");
	standardPrototype->dashOffset = 0.;
	standardPrototype->isShadowing = FALSE;
	standardPrototype->shadowDeltaX = 4.;
	standardPrototype->shadowDeltaY = 4.;	
	standardPrototype->offset = 0;
	standardPrototype->interval = 3;
	standardPrototype->segmentCount = 20;
	standardPrototype->amplitude = 10;
	standardPrototype->phase = 0;
	standardPrototype->zhouqi = 2*PI * 5;
}


void				my_prototype_manager_add_prototype				(MyPrototypeManager *self, MyPrototypeCategory category, MyPrototypeLine *prototype) {
	
	
	MyPrototypeManagerPrivate* priv = MY_PROTOTYPE_MANAGER_GET_PRIVATE (self);
	switch (category) {
	case MY_PROTOTYPE_CATEGORY_STANDARD:
		g_queue_push_tail (priv->standardQueue, prototype);
		break;
	case MY_PROTOTYPE_CATEGORY_FAST:
		g_queue_push_tail (priv->fastQueue, prototype);
		break;
	default:
		assert (0);	
	}
	my_prototype_manager_set_current_prototype (self, prototype);
}


void				my_prototype_manager_remove_prototype				(MyPrototypeManager *self, MyPrototypeCategory category, MyPrototypeLine *prototype) {
	
	
	MyPrototypeManagerPrivate* priv = MY_PROTOTYPE_MANAGER_GET_PRIVATE (self);
	switch (category) {
	case MY_PROTOTYPE_CATEGORY_STANDARD:
		g_queue_remove (priv->standardQueue, prototype);
		g_object_unref (prototype);
		break;
	case MY_PROTOTYPE_CATEGORY_FAST:
		g_queue_remove (priv->fastQueue, prototype);
		g_object_unref (prototype);
		break;
	default:
		assert (0);	
	}
}


void				my_prototype_manager_set_current_prototype		(MyPrototypeManager *self, MyPrototypeLine *prototype) {
	
	
	MyPrototypeManagerPrivate* priv = MY_PROTOTYPE_MANAGER_GET_PRIVATE (self);
	priv->currentPrototype = prototype;
}


MyPrototypeLine *	my_prototype_manager_get_current_prototype 		(MyPrototypeManager *self) {
	
	
	MyPrototypeManagerPrivate* priv = MY_PROTOTYPE_MANAGER_GET_PRIVATE (self);
	return priv->currentPrototype;
}


guint				my_prototype_manager_get_length					(MyPrototypeManager *self, MyPrototypeCategory category) {
	
	
	MyPrototypeManagerPrivate* priv = MY_PROTOTYPE_MANAGER_GET_PRIVATE (self);
	guint expectLength = 0;
	switch (category) {
	case MY_PROTOTYPE_CATEGORY_STANDARD:
		expectLength = g_queue_get_length (priv->standardQueue);
		break;
	case MY_PROTOTYPE_CATEGORY_FAST:
		expectLength = g_queue_get_length (priv->fastQueue);
		break;
	default:
		assert (0);	
	}
	return expectLength;
}


MyPrototypeLine *	my_prototype_manager_peek_nth					(MyPrototypeManager *self, MyPrototypeCategory category, guint n) {
	
	
	MyPrototypeManagerPrivate* priv = MY_PROTOTYPE_MANAGER_GET_PRIVATE (self);
	MyPrototypeLine *want;
	switch (category) {
	case MY_PROTOTYPE_CATEGORY_STANDARD:
		want = g_queue_peek_nth (priv->standardQueue, n);
		break;
	case MY_PROTOTYPE_CATEGORY_FAST:
		want = g_queue_peek_nth (priv->fastQueue, n);
		break;
	default:
		assert (0);	
	}
	return want;
}
