#ifndef __MY_PICTURE_H__
#define __MY_PICTURE_H__

#define MY_TYPE_PICTURE                  (my_picture_get_type ())
#define MY_PICTURE(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_PICTURE, MyPicture))
#define MY_IS_PICTURE(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_PICTURE))
#define MY_PICTURE_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), MY_TYPE_PICTURE, MyPictureClass))
#define MY_IS_PICTURE_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), MY_TYPE_PICTURE))
#define MY_PICTURE_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), MY_TYPE_PICTURE, MyPictureClass))


typedef struct _MyVector				MyVector;
typedef struct _MyShape				MyShape;
typedef struct _MyControlShapeRect		MyControlShapeRect;

typedef struct _MyPicture			MyPicture;
typedef struct _MyPictureClass		MyPictureClass;

struct _MyPicture {
	MyShape 	parent_instance;
	char *		fileName;
	cairo_surface_t	*imageSurface;
	/*
	gdouble		centerX;
	gdouble		centerY;
	*/
	gdouble		x;
	gdouble		y;
	gdouble		width;
	gdouble		height;
	gdouble		orignWidth;	// 原图尺寸,不变
	gdouble		orignHeight;
	gdouble		rotateX;// 旋转后坐标
	gdouble		rotateY;
	gdouble		rotateSideLen;//旋转区域为正方形,边长
	gboolean	isFlipX;// 水平翻转
	gboolean	isFlipY;// 垂直翻转
	MyVector	axis;	// 轴向量
	MyControlShapeRect	* p0ControlRect;
	MyControlShapeRect	* p1ControlRect;
	MyControlShapeRect	* p2ControlRect;
	MyControlShapeRect	* p3ControlRect;
	MyControlShapeRect	* p4ControlRect;
	MyControlShapeRect	* p5ControlRect;
	MyControlShapeRect	* p6ControlRect;
	MyControlShapeRect	* p7ControlRect;
	MyControlShapeRect	* p8ControlRect;
};
struct _MyPictureClass {
	MyShapeClass	parent_class;
    /* Overridable methods */
};

/* used by MY_TYPE_PICTURE */
GType my_picture_get_type (void);

/* Overridable methods wrapper */
MyStatus		my_picture_init_by_file			(MyPicture *self, char *fileName);
void			my_picture_init_by_pixbuf		(MyPicture *self, GdkPixbuf *pixbuf);

/* Non-Overridable Methods */

#endif /*__MY_PICTURE_H__*/