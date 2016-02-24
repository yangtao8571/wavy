#define MY_LOG_LEVEL 3

#include <gtk/gtk.h>
#include <glib-object.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "f-pixbuf-utils.h"
#include "gdk-pixbuf-rotate.h"
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
#include "my_layer.h"
#include "my_util.h"
#include "my_box_blur.h"
#include "my_picture.h"


#define MY_PICTURE_GET_PRIVATE(o)		(G_TYPE_INSTANCE_GET_PRIVATE ((o), MY_TYPE_PICTURE, MyPicturePrivate))

G_DEFINE_TYPE (MyPicture, my_picture, MY_TYPE_SHAPE);
//G_DEFINE_TYPE_WITH_PRIVATE (MyPicture, my_picture, G_TYPE_OBJECT)
 
typedef struct _MyPicturePrivate {
	GdkPixbuf	*orignPixbuf;
	GdkPixbuf	*pixbuf;
} MyPicturePrivate;

/******************************************************************************
 * declare static function 
 ******************************************************************************/
static void 		my_picture_init 	(MyPicture* self);
static void 		my_picture_finalize (GObject* object);
static void 		my_picture_dispose	(GObject* object);
static GString	*	to_string			(MyShape *self);
static gboolean		is_chosen			(MyShape *self, gdouble x, gdouble y, gdouble strokeWidth);
static void		draw_self			(MyShape* self, ApplicationState *appState);
static void		draw_self_shadow	(MyShape* self, ApplicationState *appState);
static void		shift				(MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData);
static void		shift_self_only		(MyShape *self, gdouble dx, gdouble dy);
static void		reposition_control_shapes		(MyShape *self);
static void 		init_control_shapes				(MyShape* self, MyLayer *layer);
static void 		hide_control_shapes				(MyShape* self);
static void 		delete_control_shapes			(MyShape *self);
static void 		delete_control_shapes_from_layer(MyShape *self, MyLayer *layer);
static void		update_with_scale				(MyShape *self, gdouble scale);
static void		calculate_just_size				(MyShape *self);
static void		on_shadow_dirty					(MyShape *self);
static void		update_shadow_rect				(MyShape *self);
static void		from_prototype_line				(MyShape *self, MyPrototypeLine *prototype);
static void		to_prototype_line				(MyShape *self, MyPrototypeLine *prototype);
/******************************************************************************
 * implement function 
 ******************************************************************************/
static void my_picture_class_init (MyPictureClass* klass)
{
	GObjectClass	*g_object_class;
	MyShapeClass	*myShapeClass;
	/* Add private structure */
	g_type_class_add_private (klass, sizeof (MyPicturePrivate));
	/* Get the parent gobject class */
	g_object_class = G_OBJECT_CLASS(klass);
	
    /* Hook overridable methods */
	myShapeClass = MY_SHAPE_CLASS(klass);
	myShapeClass->to_string = to_string;
	myShapeClass->is_chosen = is_chosen;
	myShapeClass->draw_self = draw_self;
	myShapeClass->draw_self_shadow = draw_self_shadow;
	myShapeClass->shift 	= shift;
	myShapeClass->shift_self_only 	= shift_self_only;
	myShapeClass->init_control_shapes = init_control_shapes;
	myShapeClass->hide_control_shapes = hide_control_shapes;
	myShapeClass->delete_control_shapes = delete_control_shapes;
	myShapeClass->delete_control_shapes_from_layer = delete_control_shapes_from_layer;
	myShapeClass->update_with_scale = update_with_scale;
	myShapeClass->calculate_just_size = calculate_just_size;
	myShapeClass->update_shadow_rect = update_shadow_rect;
	myShapeClass->on_shadow_dirty = on_shadow_dirty;
	myShapeClass->from_prototype_line = from_prototype_line;
	myShapeClass->to_prototype_line = to_prototype_line;
	
	/* Hook finalization functions */
	g_object_class->dispose = my_picture_dispose; /* instance destructor, reverse of init */
	g_object_class->finalize = my_picture_finalize; /* class finalization, reverse of class init */
}

// constructor
static void		my_picture_init (MyPicture* self)
{
	/* Initialize public fields */
	MY_SHAPE (self)->isPicture				=	TRUE;
	MY_SHAPE (self)->isStrokeWidthChangable	=	FALSE;
	MY_SHAPE (self)->isColorChangable		=	FALSE;
	MY_SHAPE (self)->isShadowing			=	TRUE;
	MY_SHAPE (self)->shadowPadding			=	MY_SHAPE (self)->boxRadius;
	
	/* Initialize private fields */
	//MyPicturePrivate* priv = MY_PICTURE_GET_PRIVATE (self);
}

static void my_picture_finalize (GObject* object)
{  
	my_debug ("finalizing rect control shape %#x", object);
  /* Always chain up to the parent class; as with dispose(), finalize()
   * is guaranteed to exist on the parent's class virtual function table
   */
	G_OBJECT_CLASS (my_picture_parent_class)->finalize (object);
}

static void my_picture_dispose (GObject* object)
{
	MyPicture *self = MY_PICTURE (object);
	MyPicturePrivate* priv = MY_PICTURE_GET_PRIVATE (self);

	my_debug ("disposing rect control shape %#x", object);
	
	/* Reverse what was allocated by instance init */
	if (priv->orignPixbuf)
		g_object_unref (priv->orignPixbuf);
	if (priv->pixbuf)
		g_object_unref (priv->pixbuf);

  /* Always chain up to the parent class; there is no need to check if
   * the parent class implements the dispose() virtual function: it is
   * always guaranteed to do so
   */
	G_OBJECT_CLASS (my_picture_parent_class)->dispose (object);
}


MyStatus	my_picture_init_by_file		(MyPicture *self, char *fileName) {

	
	MyPicturePrivate* priv = MY_PICTURE_GET_PRIVATE (self);
	GError *err = NULL;
	self->fileName = fileName;
	priv->orignPixbuf = gdk_pixbuf_new_from_file (fileName, &err);
	if (priv->orignPixbuf) {
		self->width = self->orignWidth = gdk_pixbuf_get_width (priv->orignPixbuf);
		self->height = self->orignHeight = gdk_pixbuf_get_height (priv->orignPixbuf);
		/*
		self->centerX = self->x + self->width / 2;
		self->centerY = self->y + self->height / 2;
		*/
		self->rotateSideLen = sqrt( self->width*self->width + self->height*self->height );
		/*
		self->rotateX = self->centerX - self->rotateSideLen / 2;
		self->rotateY = self->centerY - self->rotateSideLen / 2;
		*/
		self->rotateX = self->x + self->width / 2 - self->rotateSideLen / 2;
		self->rotateY = self->y + self->height / 2 - self->rotateSideLen / 2;
		
		my_debug ("rotateSideLen %f", self->rotateSideLen);
		my_debug ("picture width,height %f,%f", self->width, self->height);
		
		// pixbuf convert to cairo surface
		self->imageSurface = f_pixbuf_to_cairo_surface (priv->orignPixbuf);
		
		return MY_STATUS_SUCCESS;
	} else {
		MyStatus myStatus;
		switch (err->code) {
			case G_FILE_ERROR_NOENT:
				myStatus = MY_STATUS_FILE_NOT_FOUND;
				break;
			case G_FILE_ERROR_ACCES:
				myStatus = MY_STATUS_PERMISSION_ERROR;
				break;
			case GDK_PIXBUF_ERROR_INSUFFICIENT_MEMORY:
				myStatus = MY_STATUS_NO_MEMORY;
				break;
			default:
				if (err->domain == G_FILE_ERROR) {
					myStatus = MY_STATUS_READ_ERROR;
				} else if (err->domain == GDK_PIXBUF_ERROR) {
					myStatus = MY_STATUS_GDK_PIXBUF_ERROR;
				}
				break;
		}
		g_error_free (err);
		return myStatus;
	}
}


void	my_picture_init_by_pixbuf		(MyPicture *self, GdkPixbuf *pixbuf) {

	
	MyPicturePrivate *priv = MY_PICTURE_GET_PRIVATE (self);
	priv->orignPixbuf = pixbuf;
	if (priv->orignPixbuf) {
		self->width = self->orignWidth = gdk_pixbuf_get_width (priv->orignPixbuf);
		self->height = self->orignHeight = gdk_pixbuf_get_height (priv->orignPixbuf);
		self->rotateSideLen = sqrt( self->width*self->width + self->height*self->height );
		self->rotateX = self->x + self->width / 2 - self->rotateSideLen / 2;
		self->rotateY = self->y + self->height / 2 - self->rotateSideLen / 2;
		
		// pixbuf convert to cairo surface
		self->imageSurface = f_pixbuf_to_cairo_surface (priv->orignPixbuf);
	}
}


static GString	*	to_string	(MyShape *self) {
	return NULL;
}


static gboolean		is_chosen	(MyShape *self, gdouble x, gdouble y, gdouble strokeWidth) {


	gboolean isInRect;
	MyPicture *picture = MY_PICTURE(self);
	gdouble x0, y0, x2, y2, x4, y4, x6, y6;

	if (self->isShowing) {
		if (my_util_is_point_in_rect (picture->x, picture->y, picture->width, picture->height, x, y)) {
			isInRect = TRUE;
		} else {
			isInRect = FALSE;
		}
		x0 = picture->p0ControlRect->x;
		y0 = picture->p0ControlRect->y;
		x2 = picture->p2ControlRect->x;
		y2 = picture->p2ControlRect->y;
		x4 = picture->p4ControlRect->x;
		y4 = picture->p4ControlRect->y;
		x6 = picture->p6ControlRect->x;
		y6 = picture->p6ControlRect->y;
		isInRect = my_util_is_point_inside_rect (	
										x, y,
										x0, y0,
										x2, y2,
										x4, y4,
										x6, y6);
	} else {
		isInRect = FALSE;
	}
	my_debug ("picture %#x is chosen? %d", self, isInRect);
	return isInRect;
}


static void			draw_self	(MyShape* self, ApplicationState *appState) {

	
	MyPicture *picture = MY_PICTURE(self);
	MyPicturePrivate* priv = MY_PICTURE_GET_PRIVATE (picture);
	cairo_t *cr_window;
	gdouble scale = self->appState->scale;
	gdouble sx;
	gdouble sy;
	gdouble totalSx;
	gdouble totalSy;
	gdouble fi;
	gdouble realCenterX;
	gdouble realCenterY;
	gdouble centerX;
	gdouble centerY;
	gdouble scaledRotatedCenterX;
	gdouble scaledRotatedCenterY;
	gdouble translateX;
	gdouble translateY;
/*
	cairo_t *cr_window = gdk_cairo_create (appState->pixmap);
	cairo_save (cr_window);
	cairo_rectangle (cr_window,
					picture->x + appState->orignX,
					picture->y + appState->orignY,
					picture->orignWidth * self->appState->scale,
					picture->orignHeight * self->appState->scale);
	cairo_clip (cr_window);
	cairo_set_source_surface (cr_window, 
							priv->imageSurface, 
							picture->x + appState->orignX,
							picture->y + appState->orignY);
	cairo_paint (cr_window);
	cairo_restore (cr_window);
*/
	/*
	sx = scale;
	sy = scale;
	*/
	sx = picture->width / picture->orignWidth;
	sy = picture->height / picture->orignHeight;
	fi = self->angle;
	realCenterX = picture->x + picture->width / 2 + appState->orignX;
	realCenterY = picture->y + picture->height / 2 + appState->orignY;
	centerX = realCenterX / sx;	// 原图中心
	centerY = realCenterY / sy;
	scaledRotatedCenterX = (cos (fi) * sx * centerX - sin (fi) * sy * centerY);		// 缩放旋转后值
	scaledRotatedCenterY = (sin (fi) * sx * centerX + cos (fi) * sy * centerY);
	translateX = realCenterX - scaledRotatedCenterX;
	translateY = realCenterY - scaledRotatedCenterY;
//	cr_window = gdk_cairo_create (appState->pixmap);
	cr_window = appState->cr_window;
	cairo_save (cr_window);

	cairo_translate (cr_window, translateX, translateY);
	cairo_rotate (cr_window, fi);
	cairo_scale (cr_window, sx, sy);

	/*
	cairo_set_source_surface (cr_window, 
					picture->imageSurface, 
					picture->x / scale + appState->orignX / scale, 
					picture->y / scale + appState->orignY / scale);
	*/
	cairo_set_source_surface (cr_window, 
					picture->imageSurface, 
					centerX - picture->orignWidth / 2,
					centerY - picture->orignHeight / 2);
					
	cairo_paint (cr_window);
	cairo_restore (cr_window);
}


static void		draw_self_shadow	(MyShape* self, ApplicationState *appState) {

	
	gdouble scale = appState->scale;
	cairo_t *cr = appState->cr;
	MyPicture *picture = MY_PICTURE(self);
	MyPicturePrivate* priv = MY_PICTURE_GET_PRIVATE (picture);

	cairo_mask_surface (cr, 
						picture->imageSurface, 
						self->shadowPadding,//picture->x / (picture->width / picture->orignWidth - self->shadowX), 
						self->shadowPadding//picture->y / (picture->height / picture->orignHeight - self->shadowY)
						);
}


static void			draw_self_for_pixbuf_old	(MyShape* self, ApplicationState *appState) {

	
	MyPicture *picture = MY_PICTURE(self);
	MyPicturePrivate* priv = MY_PICTURE_GET_PRIVATE (picture);
	GdkRectangle rect;
	GdkPixbuf *pixbuf;	
		gdouble m[4];
		
	my_debug ("drawing picture %#x ...", self);

	if (self->angle > 0.034 || self->angle < -0.034) {
		if (priv->pixbuf)
			g_object_unref (priv->pixbuf);
		if (!(priv->pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, picture->rotateSideLen, picture->rotateSideLen))) {
			my_debug ("NULL pointer return from gdk_pixbuf_new()");
		}
		gdk_pixbuf_fill (priv->pixbuf, 0);
		
		/*
		gdouble angle = 30./180*PI;
		gdk_pixbuf_rotate_martix_fill_for_rotation(m, angle);
		*/
		gdk_pixbuf_rotate_martix_fill_for_rotation(m, self->angle);
		gdk_pixbuf_rotate_matrix_mult_number (m, appState->scale);
		gdk_pixbuf_rotate (priv->pixbuf,
							picture->rotateSideLen/2,
							picture->rotateSideLen/2,
							m,
							priv->orignPixbuf,
							picture->orignWidth/2,
							picture->orignHeight/2,
							&rect.x, 
							&rect.y, 
							&rect.width, 
							&rect.height,
							GDK_INTERP_NEAREST);
							
		gdk_draw_pixbuf (appState->pixmap,
						appState->drawingArea->style->white_gc,
						priv->pixbuf,
						0,
						0,
						picture->rotateX + appState->orignX,
						picture->rotateY + appState->orignY,
						-1,
						-1,
						GDK_RGB_DITHER_NORMAL, 0, 0);
		
		my_debug ("result rect %d,%d,%d,%d", rect.x, rect.y, rect.width, rect.height);
		
	} else if (self->appState->scale != 1.) {
		
		pixbuf = gdk_pixbuf_scale_simple (priv->orignPixbuf,
										picture->width,
										picture->height,
										GDK_INTERP_BILINEAR);
		if (pixbuf) {
			if (priv->pixbuf)
				g_object_unref (priv->pixbuf);
			priv->pixbuf = pixbuf;
		}
		gdk_draw_pixbuf (appState->pixmap,
				appState->drawingArea->style->white_gc,
				priv->pixbuf,
				0,
				0,
				picture->x + appState->orignX,
				picture->y + appState->orignY, 
				-1,
				-1,
				GDK_RGB_DITHER_NORMAL, 0, 0);		
		
	} else if (priv->orignPixbuf) {
		gdk_draw_pixbuf (appState->pixmap,
				appState->drawingArea->style->white_gc,
				priv->orignPixbuf,
				0,
				0,
				picture->x + appState->orignX,
				picture->y + appState->orignY, 
				picture->orignWidth,
				picture->orignHeight,
				GDK_RGB_DITHER_NORMAL, 0, 0);
	}
	my_debug ("drawing picture %#x done.", self);
}


/*
static void		draw_self_shadow_for_pixbuf_old	(MyShape* self, ApplicationState *appState) {

	MyPicture *picture = MY_PICTURE(self);
	MyPicturePrivate* priv = MY_PICTURE_GET_PRIVATE (picture);
	GdkPixbuf *pixbuf = priv->orignPixbuf;
  int n_channels = gdk_pixbuf_get_n_channels (pixbuf);

  
	int bps = gdk_pixbuf_get_bits_per_sample (pixbuf);
	
	int width = gdk_pixbuf_get_width (pixbuf);
	int height = gdk_pixbuf_get_height (pixbuf);
	int stride = gdk_pixbuf_get_rowstride (pixbuf);
	int channels = gdk_pixbuf_get_n_channels (pixbuf);
	unsigned char *boxData = gdk_pixbuf_get_pixels (pixbuf);
	unsigned char* tmpData = (unsigned char* ) g_malloc0( sizeof(unsigned char) * stride * height);
	
	my_debug ("drawing picture shadow %#x ...", self);
  g_assert (gdk_pixbuf_get_colorspace (pixbuf) == GDK_COLORSPACE_RGB);
  g_assert (gdk_pixbuf_get_bits_per_sample (pixbuf) == 8);
  g_assert (gdk_pixbuf_get_has_alpha (pixbuf));
  g_assert (n_channels == 4);
	my_debug ("bps %d, has alpha %d, channels %d", bps, gdk_pixbuf_get_has_alpha (pixbuf),
		gdk_pixbuf_get_n_channels (pixbuf));
	
	my_box_blur_horizontal (boxData, tmpData, width, height, stride, 3);
	my_box_blur_horizontal (tmpData, boxData, width, height, stride, 3);
	my_box_blur_horizontal (boxData, tmpData, width, height, stride, 3);
	my_box_blur_vertical (tmpData, boxData, width, height, stride, 3);
	my_box_blur_vertical (boxData, tmpData, width, height, stride, 3);
	my_box_blur_vertical (tmpData, boxData, width, height, stride, 3);
	g_free (tmpData);
}
*/


static void			shift		(MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {


	MyPicture *picture = MY_PICTURE (self);
	shift_self_only (self, dx, dy);
	if (picture->p0ControlRect != NULL) {
		my_shape_shift_self_only (picture->p0ControlRect, dx, dy);
		my_shape_shift_self_only (picture->p1ControlRect, dx, dy);
		my_shape_shift_self_only (picture->p2ControlRect, dx, dy);
		my_shape_shift_self_only (picture->p3ControlRect, dx, dy);
		my_shape_shift_self_only (picture->p4ControlRect, dx, dy);
		my_shape_shift_self_only (picture->p5ControlRect, dx, dy);
		my_shape_shift_self_only (picture->p6ControlRect, dx, dy);
		my_shape_shift_self_only (picture->p7ControlRect, dx, dy);
		my_shape_shift_self_only (picture->p8ControlRect, dx, dy);
	}
}


static void			shift_self_only		(MyShape *self, gdouble dx, gdouble dy) {


	MyPicture *picture = MY_PICTURE (self);
	picture->x += dx;
	picture->y += dy;
	/*
	picture->centerX += dx;
	picture->centerY += dy;
	*/
	picture->rotateX += dx;
	picture->rotateY += dy;
}


/**
 * 计算新的中心和高度,当P1 P5控制点在法线方向上移动后
 */
static void			calculate_scaled_position_on_fi_direction_when_move_p1_or_p5	(MyPicture *picture, gdouble dx, gdouble dy, gboolean isP1) {


	gdouble cosA;
	gdouble dHeight;
	MyVector axisv, fiv, dv, projdv2fiv;
	int isGrowing;

	axisv = my_vector_new (0, 0, cos (MY_SHAPE (picture)->angle), sin (MY_SHAPE (picture)->angle));
	fiv = my_vector_perpendicular (axisv);
	dv = my_vector_new (0, 0, dx, dy);
	projdv2fiv = my_vector_project (dv, fiv);
	cosA = my_vector_dot_mul (fiv, projdv2fiv) / (my_vector_length (fiv) * my_vector_length (projdv2fiv));
	if (cosA > 0) {	// fiv与dv同向
		isGrowing = -1;
	} else if (cosA < 0) {	// 反向
		isGrowing = 1;
	} else {
		isGrowing = 0;
	}
	if (!isP1) {
		isGrowing *= -1;
	}

	dHeight = isGrowing * my_vector_length (projdv2fiv);
	picture->x = picture->x + picture->width / 2 + projdv2fiv.x / 2 - picture->width / 2;
	picture->y = picture->y + picture->height / 2 + projdv2fiv.y / 2 - (picture->height + dHeight) / 2;
	picture->height += dHeight;
}


static void			calculate_scaled_position_on_axis_direction_when_move_p3_or_p7	(MyPicture *picture, gdouble dx, gdouble dy, gboolean isP3) {


	gdouble cosA;
	gdouble dWidth;
	MyVector axisv, dv, projdv2axisv;
	int isGrowing;

	axisv = my_vector_new (0, 0, cos (MY_SHAPE (picture)->angle), sin (MY_SHAPE (picture)->angle));
	dv = my_vector_new (0, 0, dx, dy);
	projdv2axisv = my_vector_scalar_mul (axisv, my_vector_dot_mul (dv, axisv) / my_vector_dot_mul (axisv, axisv));
	cosA = my_vector_dot_mul (axisv, projdv2axisv) / (my_vector_length (axisv) * my_vector_length (projdv2axisv));
	if (cosA > 0) {	// axisv与dv同向
		isGrowing = 1;
	} else if (cosA < 0) {	// 反向
		isGrowing = -1;
	} else {
		isGrowing = 0;
	}
	if (!isP3) {
		isGrowing *= -1;
	}

	dWidth = isGrowing * my_vector_length (projdv2axisv);
	picture->x = picture->x + picture->width / 2 + projdv2axisv.x / 2 - (picture->width + dWidth) / 2;
	picture->y = picture->y + picture->height / 2 + projdv2axisv.y / 2 - (picture->height) / 2;
	picture->width += dWidth;
}


static gboolean			onShiftP0		(MyControlShapeRect *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {
	
	
	MyPicture *picture = (MyPicture *) self->userData;
	MyPicturePrivate* priv = MY_PICTURE_GET_PRIVATE (picture);
	ApplicationState *appState = MY_SHAPE (picture)->appState;
	MyVector correctv;
	MyVector dv = my_vector_new (0, 0, dx, dy);
	MyVector diagv = my_vector_new (0, 0, 0, 0);

	my_util_rotate_point (MY_SHAPE (picture)->angle, picture->width, picture->height, &diagv.x, &diagv.y);
	correctv = my_vector_project (dv, diagv);
	calculate_scaled_position_on_fi_direction_when_move_p1_or_p5 (picture, correctv.x, correctv.y, TRUE);
	calculate_scaled_position_on_axis_direction_when_move_p3_or_p7 (picture, correctv.x, correctv.y, FALSE);

	reposition_control_shapes (picture);
	return TRUE;
}


static gboolean			onShiftP1		(MyControlShapeRect *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {

	
	MyPicture *picture = (MyPicture *) self->userData;
	MyPicturePrivate* priv = MY_PICTURE_GET_PRIVATE (picture);
	ApplicationState *appState = MY_SHAPE (picture)->appState;
	calculate_scaled_position_on_fi_direction_when_move_p1_or_p5 (picture, dx, dy, TRUE);

	reposition_control_shapes (picture);
	return TRUE;
}


static gboolean			onShiftP2		(MyControlShapeRect *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {
	

	MyPicture *picture = (MyPicture *) self->userData;
	MyPicturePrivate* priv = MY_PICTURE_GET_PRIVATE (picture);
	ApplicationState *appState = MY_SHAPE (picture)->appState;
	MyVector correctv;
	MyVector dv = my_vector_new (0, 0, dx, dy);
	MyVector diagv = my_vector_new (0, 0, 0, 0);

	my_util_rotate_point (MY_SHAPE (picture)->angle, picture->width, -picture->height, &diagv.x, &diagv.y);
	correctv = my_vector_project (dv, diagv);
	calculate_scaled_position_on_fi_direction_when_move_p1_or_p5 (picture, correctv.x, correctv.y, TRUE);
	calculate_scaled_position_on_axis_direction_when_move_p3_or_p7 (picture, correctv.x, correctv.y, TRUE);

	reposition_control_shapes (picture);
	return TRUE;
}


static gboolean			onShiftP3		(MyControlShapeRect *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {
	
	
	MyPicture *picture = (MyPicture *) self->userData;
	MyPicturePrivate* priv = MY_PICTURE_GET_PRIVATE (picture);
	ApplicationState *appState = MY_SHAPE (picture)->appState;
	calculate_scaled_position_on_axis_direction_when_move_p3_or_p7 (picture, dx, dy, TRUE);

	reposition_control_shapes (picture);
	return TRUE;
}


static gboolean			onShiftP4		(MyControlShapeRect *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {
	
	
	MyPicture *picture = (MyPicture *) self->userData;
	MyPicturePrivate* priv = MY_PICTURE_GET_PRIVATE (picture);
	ApplicationState *appState = MY_SHAPE (picture)->appState;
	MyVector correctv;
	MyVector dv = my_vector_new (0, 0, dx, dy);
	MyVector diagv = my_vector_new (0, 0, 0, 0);

	my_util_rotate_point (MY_SHAPE (picture)->angle, picture->width, picture->height, &diagv.x, &diagv.y);
	correctv = my_vector_project (dv, diagv);
	calculate_scaled_position_on_fi_direction_when_move_p1_or_p5 (picture, correctv.x, correctv.y, FALSE);
	calculate_scaled_position_on_axis_direction_when_move_p3_or_p7 (picture, correctv.x, correctv.y, TRUE);

	reposition_control_shapes (picture);
	return TRUE;
}


static gboolean			onShiftP5		(MyControlShapeRect *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {
	

	MyPicture *picture = (MyPicture *) self->userData;
	MyPicturePrivate* priv = MY_PICTURE_GET_PRIVATE (picture);
	ApplicationState *appState = MY_SHAPE (picture)->appState;
	calculate_scaled_position_on_fi_direction_when_move_p1_or_p5 (picture, dx, dy, FALSE);

	reposition_control_shapes (picture);
	return TRUE;
}


static gboolean			onShiftP6		(MyControlShapeRect *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {
	
	
	MyPicture *picture = (MyPicture *) self->userData;
	MyPicturePrivate* priv = MY_PICTURE_GET_PRIVATE (picture);
	ApplicationState *appState = MY_SHAPE (picture)->appState;
	MyVector correctv;
	MyVector dv = my_vector_new (0, 0, dx, dy);
	MyVector diagv = my_vector_new (0, 0, 0, 0);

	my_util_rotate_point (MY_SHAPE (picture)->angle, picture->width, -picture->height, &diagv.x, &diagv.y);
	correctv = my_vector_project (dv, diagv);
	calculate_scaled_position_on_fi_direction_when_move_p1_or_p5 (picture, correctv.x, correctv.y, FALSE);
	calculate_scaled_position_on_axis_direction_when_move_p3_or_p7 (picture, correctv.x, correctv.y, FALSE);

	reposition_control_shapes (picture);
	return TRUE;
}


static gboolean			onShiftP7		(MyControlShapeRect *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {
	
	
	MyPicture *picture = (MyPicture *) self->userData;
	MyPicturePrivate* priv = MY_PICTURE_GET_PRIVATE (picture);
	ApplicationState *appState = MY_SHAPE (picture)->appState;
	calculate_scaled_position_on_axis_direction_when_move_p3_or_p7 (picture, dx, dy, FALSE);

	reposition_control_shapes (picture);
	return TRUE;
}

// 拖动旋转点回调
static gboolean			onShiftP8		(MyControlShapeRect *self, MyShape *source, gdouble x, gdouble y, MyMouse mouse, guint keyMask, gpointer userData) {

	MyPicture *picture = (MyPicture *) self->userData;
	MyPicturePrivate* priv = MY_PICTURE_GET_PRIVATE (picture);
	ApplicationState *appState = MY_SHAPE (picture)->appState;
	
	//my_debug ("x,y is %f,%f, picture center is %f,%f", x, y, picture->centerX, picture->centerY);
	
	// 计算angle
	/*
	gdouble xToPicCenter = x - picture->centerX;
	gdouble yToPicCenter = y - picture->centerY;
	*/
	gdouble centerX = picture->x + picture->width / 2;
	gdouble centerY = picture->y + picture->height / 2;	
	gdouble xToPicCenter = x + self->width / 2 - centerX;	// 控制点中心与图片中心的连线向量, x为center坐标
	gdouble yToPicCenter = y + self->height / 2 - centerY;
	MyPoint p8Point;

	if (picture->height >= 0) {
		MY_SHAPE (picture)->angle = atan2(yToPicCenter, xToPicCenter) + PI/2;
	} else {
		MY_SHAPE (picture)->angle = atan2(yToPicCenter, xToPicCenter) - PI/2;
	}

	// 增加旋转粘性
	if (MY_SHAPE (picture)->angle < PI/30 && MY_SHAPE (picture)->angle > -PI/30) {
		MY_SHAPE (picture)->angle = 0;
	}

	my_debug ("atan2(%f, %f) is %f", yToPicCenter, xToPicCenter, atan2(yToPicCenter, xToPicCenter));
	my_debug ("angle to be rotate is %f", MY_SHAPE (picture)->angle);
	my_debug ("point relate to picture center is %f,%f", xToPicCenter, yToPicCenter);
	// 移动控制点
	/*
	MyPoint p8Point = my_util_part_segment (picture->centerX, 
							picture->centerY, 
							x, 
							y, 
							sqrt ((x-picture->centerX)*(x-picture->centerX) + (y-picture->centerY)*(y-picture->centerY)),
							picture->height / 2 + 60);	
	*/
	p8Point = my_util_part_segment (centerX, 
											centerY, 
											x, 
											y, 
											sqrt ((x - centerX) * (x - centerX) + (y - centerY) * (y - centerY)),
											picture->height / 2 + 60 * MY_SHAPE (self)->appState->scale);	
	
	my_shape_shift_to_self_only (MY_SHAPE (self), p8Point.x - self->width / 2, p8Point.y - self->height / 2);
	reposition_control_shapes (picture);
	return TRUE;
}


static void		reposition_control_shapes		(MyShape *self) {

	
	gdouble fi = self->angle;
	MyPicture *picture = MY_PICTURE (self);
	MyPoint p8Point;
	gdouble centerX = picture->x + picture->width / 2;
	gdouble centerY = picture->y + picture->height / 2;
	gdouble controlCenterX;
	gdouble controlCenterY;
	
	if (picture->p0ControlRect) {
		picture->p0ControlRect->x = picture->x - 4;
		picture->p0ControlRect->y = picture->y - 4;
		picture->p1ControlRect->x = picture->x + picture->width / 2 - 2;
		picture->p1ControlRect->y = picture->p0ControlRect->y;
		picture->p2ControlRect->x = picture->x + picture->width - 4;
		picture->p2ControlRect->y = picture->p0ControlRect->y;
		picture->p3ControlRect->x = picture->p2ControlRect->x;
		picture->p3ControlRect->y = picture->y + picture->height / 2 - 2;
		picture->p4ControlRect->x = picture->p2ControlRect->x;
		picture->p4ControlRect->y = picture->y + picture->height - 4;
		picture->p5ControlRect->x = picture->p1ControlRect->x;
		picture->p5ControlRect->y = picture->p4ControlRect->y;
		picture->p6ControlRect->x = picture->p0ControlRect->x;
		picture->p6ControlRect->y = picture->p4ControlRect->y;
		picture->p7ControlRect->x = picture->p0ControlRect->x;
		picture->p7ControlRect->y = picture->p3ControlRect->y;
		// 旋转
		my_util_rotate_point_with_center (fi, centerX, centerY, picture->p0ControlRect->x + picture->p0ControlRect->width / 2, picture->p0ControlRect->y + picture->p0ControlRect->height / 2, &controlCenterX, &controlCenterY);
		picture->p0ControlRect->x = controlCenterX -  picture->p0ControlRect->width / 2;
		picture->p0ControlRect->y = controlCenterY -  picture->p0ControlRect->height / 2;
		my_util_rotate_point_with_center (fi, centerX, centerY, picture->p1ControlRect->x + picture->p1ControlRect->width / 2, picture->p1ControlRect->y + picture->p1ControlRect->height / 2, &controlCenterX, &controlCenterY);
		picture->p1ControlRect->x = controlCenterX -  picture->p1ControlRect->width / 2;
		picture->p1ControlRect->y = controlCenterY -  picture->p1ControlRect->height / 2;
		my_util_rotate_point_with_center (fi, centerX, centerY, picture->p2ControlRect->x + picture->p2ControlRect->width / 2, picture->p2ControlRect->y + picture->p2ControlRect->height / 2, &controlCenterX, &controlCenterY);
		picture->p2ControlRect->x = controlCenterX -  picture->p2ControlRect->width / 2;
		picture->p2ControlRect->y = controlCenterY -  picture->p2ControlRect->height / 2;
		my_util_rotate_point_with_center (fi, centerX, centerY, picture->p3ControlRect->x + picture->p3ControlRect->width / 2, picture->p3ControlRect->y + picture->p3ControlRect->height / 2, &controlCenterX, &controlCenterY);
		picture->p3ControlRect->x = controlCenterX -  picture->p3ControlRect->width / 2;
		picture->p3ControlRect->y = controlCenterY -  picture->p3ControlRect->height / 2;
		my_util_rotate_point_with_center (fi, centerX, centerY, picture->p4ControlRect->x + picture->p4ControlRect->width / 2, picture->p4ControlRect->y + picture->p4ControlRect->height / 2, &controlCenterX, &controlCenterY);
		picture->p4ControlRect->x = controlCenterX -  picture->p4ControlRect->width / 2;
		picture->p4ControlRect->y = controlCenterY -  picture->p4ControlRect->height / 2;
		my_util_rotate_point_with_center (fi, centerX, centerY, picture->p5ControlRect->x + picture->p5ControlRect->width / 2, picture->p5ControlRect->y + picture->p5ControlRect->height / 2, &controlCenterX, &controlCenterY);
		picture->p5ControlRect->x = controlCenterX -  picture->p5ControlRect->width / 2;
		picture->p5ControlRect->y = controlCenterY -  picture->p5ControlRect->height / 2;
		my_util_rotate_point_with_center (fi, centerX, centerY, picture->p6ControlRect->x + picture->p6ControlRect->width / 2, picture->p6ControlRect->y + picture->p6ControlRect->height / 2, &controlCenterX, &controlCenterY);
		picture->p6ControlRect->x = controlCenterX -  picture->p6ControlRect->width / 2;
		picture->p6ControlRect->y = controlCenterY -  picture->p6ControlRect->height / 2;
		my_util_rotate_point_with_center (fi, centerX, centerY, picture->p7ControlRect->x + picture->p7ControlRect->width / 2, picture->p7ControlRect->y + picture->p7ControlRect->height / 2, &controlCenterX, &controlCenterY);
		picture->p7ControlRect->x = controlCenterX -  picture->p7ControlRect->width / 2;
		picture->p7ControlRect->y = controlCenterY -  picture->p7ControlRect->height / 2;
		
		p8Point = my_util_part_segment (picture->p5ControlRect->x,
										picture->p5ControlRect->y,
										picture->p1ControlRect->x,
										picture->p1ControlRect->y,
										picture->height,
										(picture->height >= 0 ? 
													picture->height + 60 * self->appState->scale : 
													picture->height - 60 * self->appState->scale));
		picture->p8ControlRect->x = p8Point.x;
		picture->p8ControlRect->y = p8Point.y;
	}
}


static void 		init_control_shapes		(MyShape* self, MyLayer *layer) {
	

	MyPicture *picture = MY_PICTURE (self);
	MyPoint p8Point;
	
	if (picture->p0ControlRect == NULL) {// 没有, 新建控制图形	
		picture->p0ControlRect = (MyControlShapeRect *) g_object_new (MY_TYPE_CONTROL_SHAPE_RECT, NULL);
		picture->p0ControlRect->isFilling = FALSE;
		picture->p0ControlRect->isAutoShiftSelf = FALSE;
		picture->p0ControlRect->x = picture->x - 4;
		picture->p0ControlRect->y = picture->y - 4;
		picture->p0ControlRect->target = self;
		picture->p0ControlRect->userData = picture;
		picture->p0ControlRect->onShift = onShiftP0;
		MY_SHAPE (picture->p0ControlRect)->appState = self->appState;
		
		picture->p1ControlRect = (MyControlShapeRect *) g_object_new (MY_TYPE_CONTROL_SHAPE_RECT, NULL);
		picture->p1ControlRect->isFilling = FALSE;
		picture->p1ControlRect->isAutoShiftSelf = FALSE;
		picture->p1ControlRect->x = picture->x + picture->width / 2 - 2;
		picture->p1ControlRect->y = picture->p0ControlRect->y;
		picture->p1ControlRect->target = self;
		picture->p1ControlRect->userData = picture;
		picture->p1ControlRect->onShift = onShiftP1;
		MY_SHAPE (picture->p1ControlRect)->appState = self->appState;
		
		picture->p2ControlRect = (MyControlShapeRect *) g_object_new (MY_TYPE_CONTROL_SHAPE_RECT, NULL);
		picture->p2ControlRect->isFilling = FALSE;
		picture->p2ControlRect->isAutoShiftSelf = FALSE;
		picture->p2ControlRect->x = picture->x + picture->width - 4;
		picture->p2ControlRect->y = picture->p0ControlRect->y;
		picture->p2ControlRect->target = self;
		picture->p2ControlRect->userData = picture;
		picture->p2ControlRect->onShift = onShiftP2;
		MY_SHAPE (picture->p2ControlRect)->appState = self->appState;
		
		picture->p3ControlRect = (MyControlShapeRect *) g_object_new (MY_TYPE_CONTROL_SHAPE_RECT, NULL);
		picture->p3ControlRect->isFilling = FALSE;
		picture->p3ControlRect->isAutoShiftSelf = FALSE;
		picture->p3ControlRect->x = picture->p2ControlRect->x;
		picture->p3ControlRect->y = picture->y + picture->height / 2 - 2;
		picture->p3ControlRect->target = self;
		picture->p3ControlRect->userData = picture;
		picture->p3ControlRect->onShift = onShiftP3;
		MY_SHAPE (picture->p3ControlRect)->appState = self->appState;
		
		picture->p4ControlRect = (MyControlShapeRect *) g_object_new (MY_TYPE_CONTROL_SHAPE_RECT, NULL);
		picture->p4ControlRect->isFilling = FALSE;
		picture->p4ControlRect->isAutoShiftSelf = FALSE;
		picture->p4ControlRect->x = picture->p2ControlRect->x;
		picture->p4ControlRect->y = picture->y + picture->height - 4;
		picture->p4ControlRect->target = self;
		picture->p4ControlRect->userData = picture;
		picture->p4ControlRect->onShift = onShiftP4;
		MY_SHAPE (picture->p4ControlRect)->appState = self->appState;
		
		picture->p5ControlRect = (MyControlShapeRect *) g_object_new (MY_TYPE_CONTROL_SHAPE_RECT, NULL);
		picture->p5ControlRect->isFilling = FALSE;
		picture->p5ControlRect->isAutoShiftSelf = FALSE;
		picture->p5ControlRect->x = picture->p1ControlRect->x;
		picture->p5ControlRect->y = picture->p4ControlRect->y;
		picture->p5ControlRect->target = self;
		picture->p5ControlRect->userData = picture;
		picture->p5ControlRect->onShift = onShiftP5;
		MY_SHAPE (picture->p5ControlRect)->appState = self->appState;
		
		picture->p6ControlRect = (MyControlShapeRect *) g_object_new (MY_TYPE_CONTROL_SHAPE_RECT, NULL);
		picture->p6ControlRect->isFilling = FALSE;
		picture->p6ControlRect->isAutoShiftSelf = FALSE;
		picture->p6ControlRect->x = picture->p0ControlRect->x;
		picture->p6ControlRect->y = picture->p4ControlRect->y;
		picture->p6ControlRect->target = self;
		picture->p6ControlRect->userData = picture;
		picture->p6ControlRect->onShift = onShiftP6;
		MY_SHAPE (picture->p6ControlRect)->appState = self->appState;
		
		picture->p7ControlRect = (MyControlShapeRect *) g_object_new (MY_TYPE_CONTROL_SHAPE_RECT, NULL);
		picture->p7ControlRect->isFilling = FALSE;
		picture->p7ControlRect->isAutoShiftSelf = FALSE;
		picture->p7ControlRect->x = picture->p0ControlRect->x;
		picture->p7ControlRect->y = picture->p3ControlRect->y;
		picture->p7ControlRect->target = self;
		picture->p7ControlRect->userData = picture;
		picture->p7ControlRect->onShift = onShiftP7;
		MY_SHAPE (picture->p7ControlRect)->appState = self->appState;
		
		picture->p8ControlRect = (MyControlShapeRect *) g_object_new (MY_TYPE_CONTROL_SHAPE_RECT, NULL);
		picture->p8ControlRect->isFilling = FALSE;
		picture->p8ControlRect->isAutoShiftSelf = FALSE;
		p8Point = my_util_part_segment (picture->p5ControlRect->x,
												picture->p5ControlRect->y,
												picture->p1ControlRect->x,
												picture->p1ControlRect->y,
												picture->height,
												picture->height + 60 * self->appState->scale);
		picture->p8ControlRect->x = p8Point.x;
		picture->p8ControlRect->y = p8Point.y;
		picture->p8ControlRect->target = self;
		picture->p8ControlRect->userData = picture;
		picture->p8ControlRect->onShift = onShiftP8;
		MY_SHAPE (picture->p8ControlRect)->isUseShiftTo = TRUE;
		MY_SHAPE (picture->p8ControlRect)->appState = self->appState;
		
		my_layer_add_head_shape (layer, MY_SHAPE (picture->p0ControlRect));
		my_layer_add_head_shape (layer, MY_SHAPE (picture->p1ControlRect));
		my_layer_add_head_shape (layer, MY_SHAPE (picture->p2ControlRect));
		my_layer_add_head_shape (layer, MY_SHAPE (picture->p3ControlRect));
		my_layer_add_head_shape (layer, MY_SHAPE (picture->p4ControlRect));
		my_layer_add_head_shape (layer, MY_SHAPE (picture->p5ControlRect));
		my_layer_add_head_shape (layer, MY_SHAPE (picture->p6ControlRect));
		my_layer_add_head_shape (layer, MY_SHAPE (picture->p7ControlRect));
		my_layer_add_head_shape (layer, MY_SHAPE (picture->p8ControlRect));
		
		// todo:delete
		/*
		MY_SHAPE (picture->p0ControlRect)->isShowing = FALSE;
		MY_SHAPE (picture->p1ControlRect)->isShowing = FALSE;
		MY_SHAPE (picture->p2ControlRect)->isShowing = FALSE;
		MY_SHAPE (picture->p3ControlRect)->isShowing = FALSE;
		MY_SHAPE (picture->p4ControlRect)->isShowing = FALSE;
		MY_SHAPE (picture->p5ControlRect)->isShowing = FALSE;
		MY_SHAPE (picture->p6ControlRect)->isShowing = FALSE;
		MY_SHAPE (picture->p7ControlRect)->isShowing = FALSE;
		*/
	} else {
	
		MY_SHAPE (picture->p0ControlRect)->isShowing = TRUE;
		MY_SHAPE (picture->p1ControlRect)->isShowing = TRUE;
		MY_SHAPE (picture->p2ControlRect)->isShowing = TRUE;
		MY_SHAPE (picture->p3ControlRect)->isShowing = TRUE;
		MY_SHAPE (picture->p4ControlRect)->isShowing = TRUE;
		MY_SHAPE (picture->p5ControlRect)->isShowing = TRUE;
		MY_SHAPE (picture->p6ControlRect)->isShowing = TRUE;
		MY_SHAPE (picture->p7ControlRect)->isShowing = TRUE;
	
		MY_SHAPE (picture->p8ControlRect)->isShowing = TRUE;
	}
}


static void 				hide_control_shapes	(MyShape *self) {


	MyPicture *picture = MY_PICTURE(self);
	my_debug ("hide_control_shapes of picture %#x", self);
	
	MY_SHAPE (picture->p0ControlRect)->isShowing = FALSE;
	MY_SHAPE (picture->p1ControlRect)->isShowing = FALSE;
	MY_SHAPE (picture->p2ControlRect)->isShowing = FALSE;
	MY_SHAPE (picture->p3ControlRect)->isShowing = FALSE;
	MY_SHAPE (picture->p4ControlRect)->isShowing = FALSE;
	MY_SHAPE (picture->p5ControlRect)->isShowing = FALSE;
	MY_SHAPE (picture->p6ControlRect)->isShowing = FALSE;
	MY_SHAPE (picture->p7ControlRect)->isShowing = FALSE;
	MY_SHAPE (picture->p8ControlRect)->isShowing = FALSE;
}

static void 				delete_control_shapes	(MyShape *self) {
	
}

static void 				delete_control_shapes_from_layer	(MyShape *self, MyLayer *layer) {


	MyPicture *picture = MY_PICTURE(self);
	my_layer_delete_shape_only (layer, picture->p0ControlRect);
	my_layer_delete_shape_only (layer, picture->p1ControlRect);
	my_layer_delete_shape_only (layer, picture->p2ControlRect);
	my_layer_delete_shape_only (layer, picture->p3ControlRect);
	my_layer_delete_shape_only (layer, picture->p4ControlRect);
	my_layer_delete_shape_only (layer, picture->p5ControlRect);
	my_layer_delete_shape_only (layer, picture->p6ControlRect);
	my_layer_delete_shape_only (layer, picture->p7ControlRect);
	my_layer_delete_shape_only (layer, picture->p8ControlRect);
	picture->p0ControlRect = NULL;
	picture->p1ControlRect = NULL;
	picture->p2ControlRect = NULL;
	picture->p3ControlRect = NULL;
	picture->p4ControlRect = NULL;
	picture->p5ControlRect = NULL;
	picture->p6ControlRect = NULL;
	picture->p7ControlRect = NULL;
	picture->p8ControlRect = NULL;
}


static void		update_with_scale		(MyShape *self, gdouble scale) {

	
	MyPicture *picture = MY_PICTURE(self);
	MyPicturePrivate* priv = MY_PICTURE_GET_PRIVATE (picture);
	/*
	gdouble oldScale = self->appState->oldScale;
	gdouble centerX = picture->x + picture->width / 2;
	gdouble centerY = picture->y + picture->height / 2;
	
	picture->width = picture->width / oldScale * scale;
	picture->height = picture->height / oldScale * scale;
	picture->x = centerX / oldScale * scale - picture->width / 2;
	picture->y = centerY / oldScale * scale - picture->height / 2;
	
	picture->rotateSideLen = picture->rotateSideLen / oldScale * scale;
	picture->rotateX = centerX / oldScale * scale - picture->rotateSideLen / 2;
	picture->rotateY = centerY / oldScale * scale - picture->rotateSideLen / 2;
	*/
	
	picture->x = picture->x / self->appState->oldScale * scale;
	picture->y = picture->y / self->appState->oldScale * scale;
	picture->width = picture->width / self->appState->oldScale * scale;
	picture->height = picture->height / self->appState->oldScale * scale;
}


static void		calculate_just_size		(MyShape *self) {
	
	
	MyPicture *picture = MY_PICTURE (self);
	gdouble scale = self->appState->scale;
	my_shape_expand_just_size (self, picture->x, picture->y);
	my_shape_expand_just_size (self, 
							picture->x + picture->width, 
							picture->y + picture->height);
}


static void		on_shadow_dirty				(MyShape *self) {
	
	
	gdouble scale = self->appState->scale;
	self->shadowPadding = 2 * self->boxRadius;
	self->shadowOrignWidth = ceil (MY_PICTURE (self)->orignWidth + 2 * self->shadowPadding);
	self->shadowOrignHeight = ceil (MY_PICTURE (self)->orignHeight + 2 * self->shadowPadding);
}


static void		update_shadow_rect			(MyShape *self) {
	
	
	MyPicture *picture = MY_PICTURE (self);
	gdouble sx = picture->width / picture->orignWidth;
	gdouble sy = picture->height / picture->orignHeight;
	gdouble scale = self->appState->scale;

	//if (picture->width >= 0) {
		self->shadowX = (picture->x + picture->width / 2) / sx - self->shadowOrignWidth / 2;
		/*self->shadowX = picture->x / abs (sx) - self->shadowPadding;
		self->shadowWidth = picture->width + 2 * self->shadowPadding * sx;*/
	//} else {
	//	self->shadowX = (picture->x + picture->width / 2) / sx + self->shadowOrignWidth / 2;
		/*self->shadowX = picture->x / abs (sx) + self->shadowPadding;
		self->shadowWidth = picture->width - 2 * self->shadowPadding * sx;*/
	//}
	self->shadowWidth = self->shadowOrignWidth * sx;

	//if (picture->height >= 0) {	
		self->shadowY = (picture->y + picture->height / 2) / sy - self->shadowOrignHeight / 2;
		/*self->shadowY = picture->y / abs (sy) - self->shadowPadding;
		self->shadowHeight = picture->height + 2 * self->shadowPadding * sy;*/
	//} else {
	//	self->shadowY = (picture->y + picture->height / 2) / sy + self->shadowOrignHeight / 2;
		/*self->shadowY = picture->y / abs (sy) + self->shadowPadding;
		self->shadowHeight = picture->height - 2 * self->shadowPadding * sy;*/
	//}
	self->shadowHeight = self->shadowOrignHeight * sy;
}


static void		from_prototype_line		(MyShape *self, MyPrototypeLine *prototype) {
}


void		to_prototype_line		(MyShape *self, MyPrototypeLine *prototype) {
	
	
	prototype->style = MY_PROTOTYPE_STYLE_PICTURE;
	prototype->isShadowing = self->isShadowing;
	prototype->shadowDeltaX = self->shadowDeltaX;
	prototype->shadowDeltaY = self->shadowDeltaY;
}