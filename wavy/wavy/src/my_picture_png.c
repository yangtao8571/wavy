#define MY_LOG_LEVEL 3

#include <assert.h>
#include <gtk/gtk.h>
#include <glib-object.h>
#include <stdlib.h>
#include <stdio.h>
#include "my_constant.h"
#include "my_math.h"
#include "my_rectangle.h"
#include "my_prototype_line.h"
#include "my_prototype_manager.h"
#include "my_application_state.h"
#include "my_point.h"
#include "my_debug.h"
#include "my_shape.h"
#include "my_vector.h"
#include "my_control_shape_rect.h"
#include "my_util.h"
#include "my_box_blur.h"
#include "my_picture.h"
#include "my_picture_png.h"


#define MY_PICTURE_PNG_GET_PRIVATE(o)		(G_TYPE_INSTANCE_GET_PRIVATE ((o), MY_TYPE_PICTURE_PNG, MyPicturePngPrivate))

G_DEFINE_TYPE (MyPicturePng, my_picture_png, MY_TYPE_PICTURE);
//G_DEFINE_TYPE_WITH_PRIVATE (MyPicturePng, my_picture_png, G_TYPE_OBJECT)
 
typedef struct _MyPicturePngPrivate {
	void *noop;
} MyPicturePngPrivate;

/******************************************************************************
 * declare static function 
 ******************************************************************************/
static void 		my_picture_png_init 	(MyPicturePng* self);
static void 		my_picture_png_finalize (GObject* object);
static void 		my_picture_png_dispose	(GObject* object);
/******************************************************************************
 * implement function 
 ******************************************************************************/
static void my_picture_png_class_init (MyPicturePngClass* klass)
{
	GObjectClass	*g_object_class;
	MyShapeClass	*myShapeClass;
	/* Add private structure */
	g_type_class_add_private (klass, sizeof (MyPicturePngPrivate));
	/* Get the parent gobject class */
	g_object_class = G_OBJECT_CLASS(klass);
	
    /* Hook overridable methods */
	myShapeClass = MY_SHAPE_CLASS(klass);
	
	/* Hook finalization functions */
	g_object_class->dispose = my_picture_png_dispose; /* instance destructor, reverse of init */
	g_object_class->finalize = my_picture_png_finalize; /* class finalization, reverse of class init */
}

// constructor
static void		my_picture_png_init (MyPicturePng* self)
{
	/* Initialize public fields */
	MY_SHAPE (self)->isStrokeWidthChangable	=	FALSE;
	MY_SHAPE (self)->isColorChangable		=	FALSE;
	MY_SHAPE (self)->isShadowing			=	TRUE;
	
	/* Initialize private fields */
	//MyPicturePngPrivate* priv = MY_PICTURE_PNG_GET_PRIVATE (self);
}

static void my_picture_png_finalize (GObject* object)
{  
	my_debug ("finalizing rect control shape %#x", object);
  /* Always chain up to the parent class; as with dispose(), finalize()
   * is guaranteed to exist on the parent's class virtual function table
   */
	G_OBJECT_CLASS (my_picture_png_parent_class)->finalize (object);
}

static void my_picture_png_dispose (GObject* object)
{
	my_debug ("disposing rect control shape %#x", object);
	
	/* Reverse what was allocated by instance init */

  /* Always chain up to the parent class; there is no need to check if
   * the parent class implements the dispose() virtual function: it is
   * always guaranteed to do so
   */
	G_OBJECT_CLASS (my_picture_png_parent_class)->dispose (object);
}


MyStatus	my_picture_png_init_by_file		(MyPicturePng *self, char *fileName) {

	
	MyPicturePngPrivate* priv = MY_PICTURE_PNG_GET_PRIVATE (self);
	ApplicationState *appState = MY_SHAPE (self)->appState;
	MyPicture *picture = MY_PICTURE (self);
	cairo_status_t crStatus;

	picture->fileName = fileName;
	picture->imageSurface = cairo_image_surface_create_from_png (fileName);
	crStatus = cairo_surface_status (picture->imageSurface);
	switch (crStatus) {
		case CAIRO_STATUS_NO_MEMORY:
			return MY_STATUS_NO_MEMORY;
		case CAIRO_STATUS_FILE_NOT_FOUND:
			return MY_STATUS_FILE_NOT_FOUND;
		case CAIRO_STATUS_READ_ERROR:
			return MY_STATUS_READ_ERROR;
		default: // success
			picture->orignWidth = cairo_image_surface_get_width (picture->imageSurface);
			picture->orignHeight = cairo_image_surface_get_height (picture->imageSurface);
			picture->width = picture->orignWidth * appState->scale;
			picture->height = picture->orignHeight * appState->scale;
			return MY_STATUS_SUCCESS;			
	}
}