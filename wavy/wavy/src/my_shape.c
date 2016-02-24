#define MY_LOG_LEVEL 3

#include <assert.h>
#include <math.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "my_constant.h"
#include "my_math.h"
#include "my_point.h"
#include "my_vector.h"
#include "my_rectangle.h"
#include "my_prototype_line.h"
#include "my_prototype_manager.h"
#include "my_application_state.h"
#include "my_layer.h"
#include "my_debug.h"
#include "my_box_blur.h"
#include "my_util.h"
#include "my_shape.h"
#include "my_picture.h"

#define MY_SHAPE_GET_PRIVATE(o)		(G_TYPE_INSTANCE_GET_PRIVATE ((o), MY_TYPE_SHAPE, MyShapePrivate))

G_DEFINE_TYPE (MyShape, my_shape, G_TYPE_OBJECT);
//G_DEFINE_TYPE_WITH_PRIVATE (MyShape, my_shape, G_TYPE_OBJECT)
 
typedef struct _MyShapePrivate {
	gboolean	dirty;	// whether shadow pixbuf need reproduce
} MyShapePrivate;

/******************************************************************************
 * declare static function 
 ******************************************************************************/
static void my_shape_init (MyShape* self);
static void my_shape_finalize (GObject* object);
static void my_shape_dispose (GObject* object);
static void		draw_self_shadow_debug		(MyShape* self);
/******************************************************************************
 * implement function 
 ******************************************************************************/
static void my_shape_class_init (MyShapeClass* klass)
{
	GObjectClass	*g_object_class;
	/* Add private structure */
	g_type_class_add_private (klass, sizeof (MyShapePrivate));
	/* Get the parent gobject class */
	g_object_class = G_OBJECT_CLASS(klass);
	
    /* Hook overridable methods */
	klass->draw_self = NULL;
	klass->draw_self_shadow = NULL;
	klass->draw_self_shadow_debug = draw_self_shadow_debug;
	klass->to_string = NULL;
	klass->is_chosen = NULL;
	klass->shift = NULL;
	klass->shift_to = NULL;
	klass->shift_self_only = NULL;
	klass->shift_to_self_only = NULL;
	klass->init_control_shapes	= NULL;
	klass->hide_control_shapes	= NULL;
	klass->delete_control_shapes	= NULL;
	klass->delete_control_shapes_from_layer	= NULL;
	klass->calculate_just_size = NULL;
	klass->update_with_scale = NULL;
	klass->update_shadow_rect = NULL;
	klass->on_shadow_dirty	=	NULL;
	klass->to_prototype_line	=	NULL;
	klass->double_click_callback	= NULL;
	
	/* Hook finalization functions */
	g_object_class->dispose = my_shape_dispose; /* instance destructor, reverse of init */
	g_object_class->finalize = my_shape_finalize; /* class finalization, reverse of class init */
}

// constructor
static void		my_shape_init (MyShape* self)
{
	MyShapePrivate* priv;

	/* Initialize public fields */
	//self->value = 0;
	self->timeZone = g_time_zone_new (NULL);
	self->createTimestamp = g_date_time_new_now (self->timeZone);
	/*
	self->justSize.left = G_MAXDOUBLE;
	self->justSize.top = G_MAXDOUBLE;
	self->justSize.right = G_MINDOUBLE;
	self->justSize.bottom = G_MINDOUBLE;
	*/
	self->justSize.x = G_MAXDOUBLE;
	self->justSize.y = G_MAXDOUBLE;
	self->justSize.width = 0;
	self->justSize.height = 0;
	
	self->isControlShape=	FALSE;
	self->isPicture		=	FALSE;
	self->isEnabled		=	TRUE;
	self->isShowable	=	TRUE;
	self->isShowing		=	TRUE;
	self->isFocusable	=	TRUE;
	self->isFocusing	=	FALSE;
	self->isEditable	=	TRUE;
	self->isEditing		=	FALSE;
	self->isDeletable	=	TRUE;
	self->isShiftable	=	TRUE;
	self->isDragable	=	TRUE;
	self->isShadowing	=	FALSE;
	self->isStrokeWidthChangable	=	TRUE;
	self->isColorChangable			=	TRUE;
	self->strokeWidth	=	4;
	self->orignDashes	=	NULL;
	self->dashes		=	NULL;
	self->dashCount		=	0;
	self->dashOffset	=	0;
	self->isUseShiftTo	=	FALSE;
	self->boxRadius		=	2;
	self->isShadowDirty	=	TRUE;
	self->shadowDeltaX	=	4.;
	self->shadowDeltaY	=	4.;
	self->needMarkDirtyWhenShift	=	FALSE;
	self->isDirty		=	TRUE;
 
	/* Initialize private fields */
	priv = MY_SHAPE_GET_PRIVATE (self);
}

static void my_shape_finalize (GObject* object)
{  
  /* Always chain up to the parent class; as with dispose(), finalize()
   * is guaranteed to exist on the parent's class virtual function table
   */
	G_OBJECT_CLASS (my_shape_parent_class)->finalize (object);
}

static void my_shape_dispose (GObject* object)
{
	/* Reverse what was allocated by instance init */
	MyShape *self = MY_SHAPE (object);
	MyShapePrivate* priv = MY_SHAPE_GET_PRIVATE (self);
	g_free (self->orignDashes);
	g_free (self->dashes);
	
  /* Always chain up to the parent class; there is no need to check if
   * the parent class implements the dispose() virtual function: it is
   * always guaranteed to do so
   */
	G_OBJECT_CLASS (my_shape_parent_class)->dispose (object);
}


void		my_shape_draw_self	(MyShape* self, ApplicationState *appState) {
	
	
	gdouble scale = appState->scale;
	
	my_debug ("drawing shape self %#x func %#x ...", self, MY_SHAPE_GET_CLASS (self)->draw_self);
	if (self->isShowable && self->isShowing) {
		cairo_t *cr = appState->cr;
		cairo_save (cr);
		cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
		cairo_set_source_rgba (cr, self->red, self->green, self->blue, self->alpha);
		if (self->isDashing) {
			cairo_set_dash (cr, self->dashes, self->dashCount, self->dashOffset);
		}
		cairo_set_line_width (cr, self->strokeWidth * appState->scale);
		
		MY_SHAPE_GET_CLASS (self)->draw_self(self, appState);
		cairo_restore (cr);
	}
	my_debug ("drawing shape self %#x done.", self);
}

/*
void		my_shape_draw_self_shadow	(MyShape* self, ApplicationState *appState) {
	
	MyShapePrivate* priv = MY_SHAPE_GET_PRIVATE (self);
	cairo_t *cr = appState->cr;
	
	if (self->isShowing && self->isShadowing) {
		cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
		cairo_set_source_rgba (cr, 0., 0., 0., 0.15);
		cairo_set_dash (cr, self->dashes, self->dashCount, self->dashOffset);
		cairo_set_line_width (cr, self->strokeWidth * appState->scale);		
		MY_SHAPE_GET_CLASS (self)->draw_self_shadow (self, appState);
	}
}
*/


void		my_shape_draw_self_shadow	(MyShape* self, ApplicationState *appState) {
	
	
	MyShapePrivate* priv = MY_SHAPE_GET_PRIVATE (self);
	cairo_t *copy = appState->cr;
	cairo_t *c;
	cairo_surface_t *sur;
	cairo_t *cr_window;
	int cut_width_byte;
	int cut_height_byte;
	int dig_extra_left_byte;
	int dig_extra_top_byte;
	int dig_extra_right_byte;
	int dig_extra_bottom_byte;
	int width;
	int height;
	int stride;
	int blockCountX;
	int blockCountY;
	int blockIndexX;
	int blockIndexY;
	int blockX;
	int blockY;
	int block_width_byte;
	int block_height_byte;
	int block_inner_left_byte;
	int block_inner_top_byte;
	int block_inner_right_byte;
	int block_inner_bottom_byte;
	cairo_format_t format;
	unsigned char *tmpBluredSurfaceData;
	unsigned char *surfaceData;
	
		gdouble scale = self->appState->scale;
		gdouble sx;
		gdouble sy;
		gdouble fi;
		gdouble realCenterX;
		gdouble realCenterY;
		gdouble centerX;
		gdouble centerY;
		gdouble scaledRotatedCenterX;
		gdouble scaledRotatedCenterY;
		gdouble translateX;
		gdouble translateY;
				int blurTimes = 1;	// 各方向blur次数

	my_debug ("my_shape_draw_self_shadow ...");

	if (self->isShowing && self->isShadowing) {
		
		if (self->isShadowDirty) { // dirty,需要重新blur shadow
			MY_SHAPE_GET_CLASS (self)->on_shadow_dirty (self);
		}
		
		MY_SHAPE_GET_CLASS (self)->update_shadow_rect (self);		
		
		if (self->isShadowDirty) { 
			
			// let sub class draw shadow
			
			if (self->shadowSurface) {
				cairo_surface_destroy (self->shadowSurface);
			}
			width = self->shadowOrignWidth;
			height = self->shadowOrignHeight;
			sur = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 
								width,
								height);
			self->shadowSurface = sur;
			stride = cairo_image_surface_get_stride (sur);
			self->shadowStride = stride;
			c = cairo_create (sur);
			appState->cr = c;
			cairo_set_source_rgb (c, 0.4, 0.4, 0.4);
			if (self->isDashing) {
				cairo_set_dash (c, self->orignDashes, self->dashCount, self->dashOffset);
			}
			cairo_set_line_width (c, self->strokeWidth);
			
			MY_SHAPE_GET_CLASS (self)->draw_self_shadow (self, appState);
			
			cairo_destroy (c);
			appState->cr = copy;
			// BEGIN: for debug
#ifdef MY_GUI_DEBUG
			cairo_set_source_rgba (copy, self->red, self->green, self->blue, self->alpha);
			cairo_set_dash (copy, self->orignDashes, self->dashCount, self->dashOffset);
			cairo_set_line_width (copy, self->strokeWidth);
			MY_SHAPE_GET_CLASS (self)->draw_self_shadow_debug (self);
#endif  // END: for debug


			// let us blur
			
			/*
			width = cairo_image_surface_get_width (sur);
			height = cairo_image_surface_get_height (sur);
			*/
			format = cairo_image_surface_get_format (sur);
//			assert (format == CAIRO_FORMAT_ARGB32);

			surfaceData = cairo_image_surface_get_data (sur);
			if (stride * height <= appState->shadowBufSize) { // buf够大,不用分块blur
				memset (appState->shadowBuf, 0, appState->shadowBufSize);
				my_box_blur_horizontal	(surfaceData, appState->shadowBuf, width, height, stride, self->boxRadius);
				my_box_blur_horizontal	(appState->shadowBuf, surfaceData, width, height, stride, self->boxRadius);
				my_box_blur_vertical	(surfaceData, appState->shadowBuf, width, height, stride, self->boxRadius);
				my_box_blur_vertical	(appState->shadowBuf, surfaceData, width, height, stride, self->boxRadius);
				
			} else { // buf太小,需要分块blur
				tmpBluredSurfaceData = g_malloc0( sizeof(unsigned char) * stride * height);
				memset (appState->shadowBuf, 0, appState->shadowBufSize);
				memset (appState->shadowBuf2, 0, appState->shadowBufSize);
				dig_extra_left_byte		= self->boxRadius * 4 * blurTimes;
				dig_extra_top_byte		= self->boxRadius * blurTimes;
				dig_extra_right_byte	= self->boxRadius * 4 * blurTimes;
				dig_extra_bottom_byte	= self->boxRadius * blurTimes;
				cut_width_byte	= appState->shadowBufStride - dig_extra_left_byte - dig_extra_right_byte;
				cut_height_byte	= appState->shadowBufHeight - dig_extra_top_byte - dig_extra_bottom_byte;
				assert (cut_width_byte > 0);
				assert (cut_width_byte > 0);
				blockCountX = ceil ((double) stride / cut_width_byte);
				blockCountY = ceil ((double) height / cut_height_byte);
				
				for (blockIndexY = 0; blockIndexY < blockCountY; blockIndexY++) {
					for (blockIndexX = 0; blockIndexX < blockCountX; blockIndexX++) {

						my_util_block_position_in_buffer (surfaceData,
												stride,
												height,
												blockIndexX,
												blockIndexY,
												cut_width_byte,
												cut_height_byte,
												dig_extra_left_byte,
												dig_extra_top_byte,
												dig_extra_right_byte,
												dig_extra_bottom_byte,
												&blockX,
												&blockY,
												&block_width_byte,
												&block_height_byte,
												&block_inner_left_byte,
												&block_inner_top_byte,
												&block_inner_right_byte,
												&block_inner_bottom_byte);
												
						my_util_memcpy_box_to_continuous (surfaceData,
												stride,
												height,
												blockX,
												blockY,
												block_width_byte,
												block_height_byte,
												appState->shadowBuf,
												0);
						// 每个方向必须严格blur 2次
						my_box_blur_horizontal	(appState->shadowBuf, appState->shadowBuf2, block_width_byte / 4, block_height_byte, block_width_byte, self->boxRadius);
						my_box_blur_horizontal	(appState->shadowBuf2, appState->shadowBuf, block_width_byte / 4, block_height_byte, block_width_byte, self->boxRadius);
						my_box_blur_vertical	(appState->shadowBuf, appState->shadowBuf2, block_width_byte / 4, block_height_byte, block_width_byte, self->boxRadius);
						my_box_blur_vertical	(appState->shadowBuf2, appState->shadowBuf, block_width_byte / 4, block_height_byte, block_width_byte, self->boxRadius);

						my_util_memcpy_box_to_box (appState->shadowBuf,
												block_width_byte,
												block_height_byte,
												block_inner_left_byte,
												block_inner_top_byte,
												block_width_byte - block_inner_left_byte - block_inner_right_byte,
												block_height_byte - block_inner_top_byte - block_inner_bottom_byte,
												tmpBluredSurfaceData,
												blockX + block_inner_left_byte,
												blockY + block_inner_top_byte,
												stride,
												height);
					}
				}
				memcpy (surfaceData, tmpBluredSurfaceData, sizeof(unsigned char) * stride * height);
				g_free (tmpBluredSurfaceData);
			}
			self->isShadowDirty = FALSE;
		}
		
		// let us draw the shadow surface finally
		
		if (self->isPicture) {
			/*
			sx = self->shadowWidth / self->shadowOrignWidth;
			sy = self->shadowHeight / self->shadowOrignHeight;
			*/
			sx = MY_PICTURE (self)->width / MY_PICTURE (self)->orignWidth;
			sy = MY_PICTURE (self)->height / MY_PICTURE (self)->orignHeight;
		} else {
//			sx = 1.;
//			sy = 1.;
//			fi = 0;
			sx = scale;
			sy = scale;
		}
		fi = self->angle;//45*PI/180;
		
//		gdouble centerX = self->shadowX + self->shadowDeltaX + appState->orignX + self->shadowWidth / 2;
//		gdouble centerY = self->shadowY + self->shadowDeltaY + appState->orignY + self->shadowHeight / 2;
		centerX = self->shadowX + self->shadowOrignWidth / 2 + self->shadowDeltaX + appState->orignX / sx;	// 标准值
		centerY = self->shadowY + self->shadowOrignHeight / 2 + self->shadowDeltaY + appState->orignY / sy;
		scaledRotatedCenterX = (cos (fi) * sx * centerX - sin (fi) * sy * centerY);		// 缩放旋转后值
		scaledRotatedCenterY = (sin (fi) * sx * centerX + cos (fi) * sy * centerY);
		translateX = sx * centerX - scaledRotatedCenterX;
		translateY = sy * centerY - scaledRotatedCenterY;
		cr_window = gdk_cairo_create (appState->pixmap);
		cairo_save (cr_window);

		cairo_translate (cr_window, translateX, translateY);
		cairo_rotate (cr_window, fi);
		cairo_scale (cr_window, sx, sy);
		/*
		cairo_rectangle (cr_window,
						self->shadowX + self->shadowDeltaX + appState->orignX / scale,	// 原点不能scale加倍
						self->shadowY + self->shadowDeltaY + appState->orignY / scale,
						self->shadowOrignWidth,
						self->shadowOrignHeight);
		cairo_clip (cr_window);
		 */
		cairo_set_source_surface (cr_window, 
						self->shadowSurface, 
					centerX - self->shadowOrignWidth / 2,
					centerY - self->shadowOrignHeight / 2);
		cairo_save (cr_window);
		cairo_set_operator (cr_window, CAIRO_OPERATOR_MULTIPLY);
		cairo_paint (cr_window);
		cairo_restore (cr_window);
		// BEGIN: for debug
#ifdef MY_GUI_DEBUG
		cairo_set_line_width (cr_window, 1. / scale);
		cairo_set_source_rgb (cr_window, 1., 1., 0.); // 黄框为shadow rect
		cairo_rectangle (cr_window, 
						self->shadowX + self->shadowDeltaX + appState->orignX / scale, 
						self->shadowY + self->shadowDeltaY + appState->orignY / scale,
						self->shadowOrignWidth,
						self->shadowOrignHeight);
		cairo_stroke (cr_window);
#endif  // END: for debug		
		
		cairo_restore (cr_window);
		
		// BEGIN: for debug
#ifdef MY_GUI_DEBUG
		cairo_set_line_width (cr_window, 1.);
		cairo_set_source_rgb (cr_window, 0., 0., 1.); // 蓝框为 just rect at scale 1.0 
		cairo_rectangle (cr_window, 
						self->justSize.x / scale + appState->orignX, 
						self->justSize.y / scale + appState->orignY,
						self->justSize.width / scale,
						self->justSize.height / scale);
		cairo_stroke (cr_window);		
		cairo_set_source_rgb (cr_window, 0., 0., 1.); // 蓝框为 just rect
		cairo_rectangle (cr_window, 
						self->justSize.x + appState->orignX, 
						self->justSize.y + appState->orignY,
						self->justSize.width,
						self->justSize.height);
		cairo_stroke (cr_window);		
		cairo_set_source_rgb (cr_window, 1., 1., 0.); // 黄框为 shadow rect at scale 1.0 
		cairo_rectangle (cr_window, 
						self->shadowX + self->shadowDeltaX + appState->orignX, 
						self->shadowY + self->shadowDeltaY + appState->orignY,
						self->shadowOrignWidth,
						self->shadowOrignHeight);
		cairo_stroke (cr_window);
#endif  // END: for debug
		
		cairo_destroy (cr_window);
		
		/*
		GdkPixmap *pixmap = gdk_pixmap_new (appState->drawingArea->window,
						   100,
						   100,
						   -1);
		GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, 100, 100);
		unsigned char *pixels = gdk_pixbuf_get_pixels (pixbuf);
		int rowstride = gdk_pixbuf_get_rowstride (pixbuf);
		memset (pixels, 0, rowstride * 100);
		gdk_draw_pixbuf (pixmap,
					appState->drawingArea->style->white_gc,
					pixbuf,
					0,
					0,
					0,
					0, 
					100,
					100,
					GDK_RGB_DITHER_NORMAL, 0, 0);
		*/
		
		/*
		GdkGC *gc = gdk_gc_new (appState->drawingArea->window);
		my_debug ("gc %#x", gc);
		gdk_draw_drawable (appState->pixmap,
			appState->drawingArea->style->fg_gc[gtk_widget_get_state (appState->drawingArea)],
			pixmap,
			0, 
			0,
			0,
			0,
			-1,
			-1);
		*/
	}
}


static void		draw_self_shadow_debug		(MyShape* self) {
}


GString	*	my_shape_to_string	(MyShape *self) {
	my_debug ("to_string method %#x", MY_SHAPE_GET_CLASS (self)->to_string);
	return MY_SHAPE_GET_CLASS (self)->to_string(self);
}


gboolean	my_shape_is_chosen	(MyShape *self, gdouble x, gdouble y, gdouble extraChosenPadding, ApplicationState *appState) {

	
	return MY_SHAPE_GET_CLASS (self)->is_chosen(self, x, y, extraChosenPadding, appState);
}


void		my_shape_shift		(MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {
	
	if (self->isShiftable) {
		self->justSize.x += dx;
		self->justSize.y += dy;
		MY_SHAPE_GET_CLASS (self)->shift (self, source, dx, dy, mouse, keyMask, userData);
	}
}


void		my_shape_shift_to				(MyShape *self, MyShape *source, gdouble x, gdouble y, MyMouse mouse, guint keyMask, gpointer userData) {
	
	if (self->isShiftable) {
		self->justSize.x = x;
		self->justSize.y = y;
		MY_SHAPE_GET_CLASS (self)->shift_to (self, source, x, y, mouse, keyMask, userData);
	}
}


void		my_shape_shift_self_only		(MyShape *self, gdouble dx, gdouble dy) {
	
	if (self->isShiftable) {
		self->justSize.x += dx;
		self->justSize.y += dy;
		MY_SHAPE_GET_CLASS (self)->shift_self_only (self, dx, dy);
	}
}


void		my_shape_shift_to_self_only		(MyShape *self, gdouble x, gdouble y) {
	
	if (self->isShiftable) {
		self->justSize.x = x;
		self->justSize.y = y;
		MY_SHAPE_GET_CLASS (self)->shift_to_self_only (self, x, y);
	}
}


gboolean 	my_shape_is_out_of_canvas		(MyShape *self, ApplicationState *appState) {

	return MY_SHAPE_GET_CLASS (self)->is_out_of_canvas (self, appState);
}


void 		my_shape_draw_dash_rectangle	(MyShape* self, ApplicationState *appState) {

	my_debug ("draw dash rectangle of shape %#x...", self);
	/*
	if (self->isShowable && self->isShowing && self->isFocusable && self->isFocusing) {
		gdk_draw_rectangle (pixmap,
							appState->gcDash,
							FALSE,
							self->justSize.x,
							self->justSize.y,
							self->justSize.width,
							self->justSize.height);
	}
	*/
	my_debug ("draw dash rectangle of shape %#x done.", self);
}


void 		my_shape_init_control_shapes	(MyShape* self, MyLayer *layer) {

	my_debug ("init control shapes of shape %#x of layer %#x...", self, layer);
	MY_SHAPE_GET_CLASS (self)->init_control_shapes (self, layer);
	my_debug ("init control shapes done.");
}


void 		my_shape_hide_control_shapes	(MyShape* self) {


	my_debug ("hide control shapes of shape %#x ...", self);
	MY_SHAPE_GET_CLASS (self)->hide_control_shapes (self);
	my_debug ("hide control shapes done.");
}


void 		my_shape_delete_control_shapes	(MyShape *self) {


	if (self->isEditable) {
		my_debug ("deleting control shapes of shape %#x ...", self);
		MY_SHAPE_GET_CLASS (self)->delete_control_shapes (self);
	}
	my_debug ("deleting control shapes of shape %#x done.", self);
}


void 		my_shape_delete_control_shapes_from_layer	(MyShape *self, MyLayer *layer) {

	if (self->isEditable) {
		my_debug ("delete control shapes of shape %#x from layer %#x...", self, layer);
		MY_SHAPE_GET_CLASS (self)->delete_control_shapes_from_layer (self, layer);
	}
	my_debug ("delete control shapes of shape %#x from layer %#x done.", self, layer);
}


gboolean 		my_shape_has_control_shapes	(MyShape *self) {


	return MY_SHAPE_GET_CLASS (self)->has_control_shapes (self);
}


void		my_shape_expand_just_size		(MyShape *self, gdouble x, gdouble y) {


	MyRectangle *justSize = &self->justSize;
	
	if (x < justSize->x) {
		if (justSize->width > 0) {
			justSize->width += (justSize->x - x);
		} else {
			justSize->width = 1;
		}
		justSize->x = x;
	}
	if (x > justSize->x + justSize->width) {
		justSize->width = (x - justSize->x);
	}
	if (y < justSize->y) {
		if (justSize->height > 0) {
			justSize->height += (justSize->y - y);
		} else {
			justSize->height = 1;
		}
		justSize->y = y;
	}
	if (y > justSize->y + justSize->height) {
		justSize->height = (y - justSize->y);
	}
}


void		my_shape_calculate_just_size		(MyShape *self) {
	
	self->justSize.x = G_MAXDOUBLE;
	self->justSize.y = G_MAXDOUBLE;
	self->justSize.width = 0;
	self->justSize.height = 0;
	MY_SHAPE_GET_CLASS (self)->calculate_just_size (self);
}


void		my_shape_set_dash				(MyShape *self, gdouble dashes[], gint dashCount) {

	
	gint z;
	if (dashCount > 0) {
		self->orignDashes = g_malloc (sizeof(gdouble) * dashCount);
		self->dashes = g_malloc (sizeof(gdouble) * dashCount);
		for (z = 0; z < dashCount; z++) {
			self->orignDashes[z] = dashes[z];
			self->dashes[z] = dashes[z] * self->appState->scale;
		}
	}
	self->dashCount = dashCount;
}


void		my_shape_set_dash_by_str		(MyShape *self, gchar *orignDashesStr) {


	gchar **orignDashes = g_strsplit (orignDashesStr, " ", -1);
	gdouble *dashes;
	gint dashCount = 0;
	int i;
	if (orignDashes) {
		i = 0;
		while (orignDashes[i]) {
			i++;
			dashCount++;
		}
		dashes = (gdouble *) g_malloc (sizeof (gdouble) * dashCount);
		for (i = 0; i < dashCount; i++) {
			dashes[i] = my_util_str_to_double (orignDashes[i]);
		}
		my_shape_set_dash (self, dashes, dashCount);
		g_strfreev (orignDashes);
		g_free (dashes);
	}
}


gchar *		my_shape_get_str_from_dash		(MyShape *self) {


	gchar *dashStr;
	gchar **dashesStrArr;
	int i;

	if (self->dashCount <= 0) {
		return NULL;
	} else {
		dashesStrArr = g_malloc (sizeof (gchar *) * (self->dashCount+1));
		for (i = 0; i < self->dashCount; i++) {
			dashesStrArr[i] = my_util_double_to_str (self->orignDashes[i]);
		}
		dashesStrArr[i] = NULL;
		dashStr = g_strjoinv (" ", dashesStrArr);

		my_util_free_str_arr (dashesStrArr);
		g_free (dashesStrArr);
		return dashStr;
	}
}


void		my_shape_update_with_scale		(MyShape *self, gdouble scale) {

	
	gint z;
	MY_SHAPE_GET_CLASS (self)->update_with_scale (self, scale);
	my_debug ("shape %#x update_with_scale ...", self);
	if (self->isShadowing) {
//		my_shape_shadow_mark_dirty (self);	
	}
	if (self->dashCount > 0) {
		for (z = 0; z < self->dashCount; z++) {
			self->dashes[z] = self->orignDashes[z] * scale;
		}
	}
	my_debug ("shape %#x update_with_scale done.", self);
}


gboolean	my_shape_is_dirty				(MyShape *self) {
	

	MyShapePrivate* priv = MY_SHAPE_GET_PRIVATE (self);
	return priv->dirty;
}


void		my_shape_shadow_mark_dirty		(MyShape *self) {

	MyShapePrivate* priv = MY_SHAPE_GET_PRIVATE (self);
	priv->dirty = TRUE;
	self->isShadowDirty = TRUE;
}


void		my_shape_mark_dirty				(MyShape *self) {

	self->isDirty = TRUE;
}


void		my_shape_fresh_dirty			(MyShape *self) {


	if (MY_SHAPE_GET_CLASS (self)->fresh_dirty) {
		MY_SHAPE_GET_CLASS (self)->fresh_dirty (self);
	}
}


void		my_shape_update_shadow_rect		(MyShape *self) {
	
	
	MY_SHAPE_GET_CLASS (self)->update_shadow_rect (self);
}


void		my_shape_from_prototype_line	(MyShape *self, MyPrototypeLine *prototype) {
	

	gint z;
	
	self->red = prototype->red;
	self->green = prototype->green;
	self->blue = prototype->blue;
	self->alpha = prototype->alpha;
	self->strokeWidth = prototype->strokeWidth;
	self->isDashing	=	prototype->isDashing;
	my_shape_set_dash_by_str (self, prototype->orignDashesStr);
	self->dashOffset = prototype->dashOffset;
	/*
	self->orignDashes = g_malloc (sizeof(gdouble) * prototype->dashCount);
	self->dashes = g_malloc (sizeof(gdouble) * prototype->dashCount);
	for (z = 0; z < prototype->dashCount; z++) {
		self->orignDashes[z] = prototype->orignDashes[z];
		self->dashes[z] = prototype->dashes[z];
	}
	self->dashCount = prototype->dashCount;	
	*/

	self->isShadowing = prototype->isShadowing;
	self->shadowDeltaX = prototype->shadowDeltaX;
	self->shadowDeltaY = prototype->shadowDeltaY;
	MY_SHAPE_GET_CLASS (self)->from_prototype_line (self, prototype);	
}


void		my_shape_to_prototype_line		(MyShape *self, MyPrototypeLine *prototype) {
	
	
	gint z;
	prototype->red = self->red;
	prototype->green = self->green;
	prototype->blue = self->blue;
	prototype->alpha = self->alpha;
	prototype->strokeWidth = self->strokeWidth;
	prototype->isDashing	=	self->isDashing;
	my_prototype_line_set_orign_dashes_str (prototype, my_shape_get_str_from_dash (self));
	prototype->dashOffset	=	self->dashOffset;
	/*
	prototype->orignDashes = (gdouble *) g_malloc (sizeof(gdouble) * self->dashCount);
	prototype->dashes = (gdouble *) g_malloc (sizeof(gdouble) * self->dashCount);
	for (z = 0; z < self->dashCount; z++) {
		prototype->orignDashes[z] = self->orignDashes[z];
		prototype->dashes[z] = self->dashes[z];
	}
	prototype->dashCount = self->dashCount;
	*/
	
	prototype->isShadowing = self->isShadowing;
	prototype->shadowDeltaX = self->shadowDeltaX;
	prototype->shadowDeltaY = self->shadowDeltaY;
	MY_SHAPE_GET_CLASS (self)->to_prototype_line (self, prototype);
}