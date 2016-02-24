#define MY_LOG_LEVEL 3

#include <assert.h>
#include <gtk/gtk.h>
#include <glib-object.h>
#include "my_constant.h"
#include "my_point.h"
#include "my_prototype_line.h"
#include "my_prototype_manager.h"
#include "my_application_state.h"
#include "my_rectangle.h"
#include "my_debug.h"
#include "my_shape.h"
#include "my_vector.h"
#include "my_control_shape_rect.h"
#include "my_layer.h"

#define MY_LAYER_GET_PRIVATE(o)		(G_TYPE_INSTANCE_GET_PRIVATE ((o), MY_TYPE_LAYER, MyLayerPrivate))

G_DEFINE_TYPE (MyLayer, my_layer, G_TYPE_OBJECT);
//G_DEFINE_TYPE_WITH_PRIVATE (MyLayer, my_layer, G_TYPE_OBJECT)
 
typedef struct _MyLayerPrivate {
	GQueue* 	curves;
	GQueue*		shapes;
	MyShape*	selectedShape;
} MyLayerPrivate;

/******************************************************************************
 * static private variable 
 ******************************************************************************/
 static gint latestLayerId = 0;
/******************************************************************************
 * declare static function 
 ******************************************************************************/
static void my_layer_init (MyLayer* self);
static void my_layer_finalize (GObject* object);
static void my_layer_dispose (GObject* object);
/******************************************************************************
 * implement function 
 ******************************************************************************/
static void my_layer_class_init (MyLayerClass* klass)
{
	GObjectClass	*g_object_class;
	// Add private structure
	g_type_class_add_private (klass, sizeof (MyLayerPrivate));
	// Get the parent gobject class
	g_object_class = G_OBJECT_CLASS(klass);
	
    /* Hook overridable methods */
	
	/* Hook finalization functions */
	g_object_class->dispose = my_layer_dispose; /* instance destructor, reverse of init */
	g_object_class->finalize = my_layer_finalize; /* class finalization, reverse of class init */
}

// constructor
static void		my_layer_init (MyLayer* self)
{
	MyLayerPrivate* priv = MY_LAYER_GET_PRIVATE (self);
	/* Initialize public fields */
	self->id = latestLayerId++;
	/* Initialize private fields */
	priv->curves = g_queue_new();
	priv->shapes = g_queue_new();
}

static void my_layer_finalize (GObject* object)
{  
  /* Always chain up to the parent class; as with dispose(), finalize()
   * is guaranteed to exist on the parent's class virtual function table
   */
	G_OBJECT_CLASS (my_layer_parent_class)->finalize (object);
}

static void my_layer_dispose (GObject* object)
{
	/* Reverse what was allocated by instance init */
	MyLayer *self = MY_LAYER (object);
	MyLayerPrivate* priv = MY_LAYER_GET_PRIVATE (self);
 
	g_queue_foreach (priv->shapes, (GFunc) g_object_unref, NULL);
	g_queue_free (priv->shapes);

  /* Always chain up to the parent class; there is no need to check if
   * the parent class implements the dispose() virtual function: it is
   * always guaranteed to do so
   */
	G_OBJECT_CLASS (my_layer_parent_class)->dispose (object);
}

void		my_layer_add_shape		(MyLayer* self, MyShape* shape) {


	MyLayerPrivate* priv = MY_LAYER_GET_PRIVATE (self);
	g_queue_push_tail(priv->shapes, shape);
}

void		my_layer_add_head_shape		(MyLayer* self, MyShape* shape) {


	MyLayerPrivate* priv = MY_LAYER_GET_PRIVATE (self);
	g_queue_push_head(priv->shapes, shape);
}

MyShape* my_layer_peek_nth (MyLayer *self, guint n) {
	return (MyShape*) g_queue_peek_nth (MY_LAYER_GET_PRIVATE (self)->shapes, n);
}

guint	my_layer_shape_count (MyLayer *self) {
	
	MyLayerPrivate* priv = MY_LAYER_GET_PRIVATE (self);
	my_debug ("layer %#x has %u shapes", self, g_queue_get_length(priv->shapes));
	return g_queue_get_length(priv->shapes);
}

MyShape*	my_layer_pop_shape (MyLayer *self, MyShape *shape) {

	gint pos;
	MyLayerPrivate* priv = MY_LAYER_GET_PRIVATE (self);
	
	pos = g_queue_index (priv->shapes, shape);
	if (pos != -1) { // find
		return (MyShape *) g_queue_pop_nth (priv->shapes, pos);
	}
	return NULL;
}

/**
 * 删除层中指定的图形,包括其控制图形
 */
void	my_layer_delete_shape (MyLayer *self, MyShape *shape) {

	MyLayerPrivate* priv = MY_LAYER_GET_PRIVATE (self);
	// 删除控制图形
	my_shape_delete_control_shapes_from_layer (shape, self);
	// 删除图形自己
	my_layer_delete_shape_only (self, shape);
}

void	my_layer_delete_shape_only (MyLayer *self, MyShape *shape) {

	my_debug ("deleting shape only ...");
	// 删除图形自己
	if (shape->isDeletable) {
		MyLayerPrivate* priv = MY_LAYER_GET_PRIVATE (self);
		g_queue_remove (priv->shapes, shape);
		if (shape == priv->selectedShape)
			priv->selectedShape = NULL;
	}
	my_debug ("deleting shape only done.");
}

void	my_layer_delete_selected_shape (MyLayer *self) {

	MyLayerPrivate* priv = MY_LAYER_GET_PRIVATE (self);
	if (priv->selectedShape) {
		my_layer_delete_shape (self, priv->selectedShape);
	}
}

void	my_layer_set_select_shape (MyLayer *self, MyShape *shape) {

	MyLayerPrivate* priv = MY_LAYER_GET_PRIVATE (self);
	priv->selectedShape = shape;
}

MyShape *	my_layer_get_select_shape	(MyLayer *self) {

	MyLayerPrivate* priv = MY_LAYER_GET_PRIVATE (self);
	return priv->selectedShape;
}

void	my_layer_clear		(MyLayer *self) {


	MyLayerPrivate* priv = MY_LAYER_GET_PRIVATE (self);
	g_queue_clear (priv->shapes);
	priv->selectedShape = NULL;
	my_debug ("clear layer done.");
}


MyShape *	my_layer_hit_shape		(MyLayer *self, gdouble x, gdouble y, gint selectWidth, ApplicationState *appState) {

	
	guint shapeCount, j;
	MyShape *shape;
	shapeCount = my_layer_shape_count (self);
	// check control shapes
	for (j = 0; j < shapeCount; j++) {
		shape = my_layer_peek_nth (self, j);
		if (shape->isControlShape) {
			if ( my_shape_is_chosen (shape, x, y, selectWidth, appState) ) {
				my_debug ("hit shape loop count: %d", j);
				my_debug ("hitted shape %#x", shape);
				return shape;
			}
		}
	}
	// check normal shapes
	for (j = shapeCount - 1; j >= 0; j--) {
		shape = my_layer_peek_nth (self, j);
		if (shape->isControlShape)
			break;
		if ( my_shape_is_chosen (shape, x, y, selectWidth, appState) ) {
			my_debug ("hit shape loop count: %d", j);
			my_debug ("hitted shape %#x", shape);
			return shape;
		}
	}
	return NULL;
}


static gboolean 	deleteData		(gpointer data, gpointer user_data) {
	
	
	if (MY_IS_CONTROL_SHAPE_RECT(data)) {
		return TRUE;
	} else {
		return FALSE;
	}
}

void 	my_layer_delete_all_control_shapes	(MyLayer *self) {
	/*
	GQueue *tmpQ;
	guint j, len;
	MyShape *shape;
	MyLayerPrivate* priv = MY_LAYER_GET_PRIVATE (self);
	my_debug ("delete all control shapes ...");
	tmpQ = g_queue_new();
	// 双队列实现删除
	g_queue_push_tail (priv->shapes, NULL);
	while (shape = g_queue_pop_head (priv->shapes)) {
		my_debug ("loop %u shape %#x", j, shape);
		if (MY_IS_CONTROL_SHAPE_RECT(shape)) {
			my_debug ("deleting control shape %#x", shape);
		} else {
			g_queue_push_tail (tmpQ, shape);
		}
	}
	g_queue_clear (priv->shapes);
	priv->shapes = tmpQ;
	my_debug ("delete all control shapes done.");
	*/
	
	GQueue *tmpQ;
	MyLayerPrivate* priv = MY_LAYER_GET_PRIVATE (self);
	my_debug ("deleting all control in shapes queue %#x ...", priv->shapes);
	my_debug ("deleteData function %#x ...", deleteData);
	
	tmpQ = my_util_delete_queue_element (priv->shapes, deleteData, NULL);
	g_queue_clear (priv->shapes);
	priv->shapes = tmpQ;
	
	my_debug ("after delete shapes count is %d", g_queue_get_length (tmpQ));
	my_debug ("deleting all control in shapes queue %#x done.", priv->shapes);
}


void		my_layer_shift_top_shape			(MyLayer *self, MyShape *theShape) {
	
	
	GQueue *shapes = MY_LAYER_GET_PRIVATE (self)->shapes;
	guint shapeCount, j;
	MyShape *shape;
	shapeCount = my_layer_shape_count (self);
	
	assert (theShape);
	for (j = shapeCount - 1; j >= 0; j--) {
		shape = my_layer_peek_nth (self, j);
		if (shape == theShape) {
			g_queue_pop_nth (shapes, j);
			break;
		}
	}
	assert (shape == theShape);
	g_queue_push_tail (shapes, theShape);
}


void		my_layer_shift_up_shape			(MyLayer *self, MyShape *theShape) {
	
	
	GQueue *shapes = MY_LAYER_GET_PRIVATE (self)->shapes;
	guint shapeCount, j;
	MyShape *shape;
	shapeCount = my_layer_shape_count (self);
	
	assert (theShape);
	for (j = shapeCount - 1; j >= 0; j--) {
		shape = my_layer_peek_nth (self, j);
		if (shape == theShape) {
			g_queue_pop_nth (shapes, j);
			break;
		}
	}
	assert (shape == theShape);
	g_queue_push_nth (shapes, theShape, j + 1);
}


void		my_layer_shift_down_shape		(MyLayer *self, MyShape *theShape) {
	
	
	GQueue *shapes = MY_LAYER_GET_PRIVATE (self)->shapes;
	guint shapeCount, j;
	MyShape *shape;
	shapeCount = my_layer_shape_count (self);
	
	assert (theShape);
	for (j = shapeCount - 1; j >= 0; j--) {
		shape = my_layer_peek_nth (self, j);
		if (shape == theShape) {
			g_queue_pop_nth (shapes, j);
			break;
		}
	}
	assert (shape == theShape);
	
	shape = g_queue_peek_nth (shapes, j - 1);
	if (shape->isControlShape) {
		g_queue_push_nth (shapes, theShape, j);		
	} else {
		g_queue_push_nth (shapes, theShape, j - 1);		
	}
}


void		my_layer_shift_bottom_shape		(MyLayer *self, MyShape *theShape) {
	
	
	GQueue *shapes = MY_LAYER_GET_PRIVATE (self)->shapes;
	guint shapeCount, j, controlPos;
	MyShape *shape;
	shapeCount = my_layer_shape_count (self);
	
	assert (theShape);
	for (j = shapeCount - 1; j >= 0; j--) {
		shape = my_layer_peek_nth (self, j);
		if (shape == theShape) {
			g_queue_pop_nth (shapes, j);
		} else {
			if (shape->isControlShape) {
				controlPos = j;
				break;
			}
		}
	}
	
	g_queue_push_nth (shapes, theShape, controlPos + 1);
}
