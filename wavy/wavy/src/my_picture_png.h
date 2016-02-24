#ifndef __MY_PICTURE_PNG_H__
#define __MY_PICTURE_PNG_H__

#define MY_TYPE_PICTURE_PNG                  (my_picture_png_get_type ())
#define MY_PICTURE_PNG(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_PICTURE_PNG, MyPicturePng))
#define MY_IS_PICTURE_PNG(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_PICTURE_PNG))
#define MY_PICTURE_PNG_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), MY_TYPE_PICTURE_PNG, MyPicturePngClass))
#define MY_IS_PICTURE_PNG_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), MY_TYPE_PICTURE_PNG))
#define MY_PICTURE_PNG_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), MY_TYPE_PICTURE_PNG, MyPicturePngClass))

typedef struct _MyPicturePng			MyPicturePng;
typedef struct _MyPicturePngClass		MyPicturePngClass;

struct _MyPicturePng {
	MyPicture 	parent_instance;
};
struct _MyPicturePngClass {
	MyPictureClass	parent_class;
    /* Overridable methods */
};

/* used by MY_TYPE_PICTURE_PNG */
GType my_picture_png_get_type (void);

/* Overridable methods wrapper */
MyStatus		my_picture_png_init_by_file	(MyPicturePng *self, char *fileName);

/* Non-Overridable Methods */

#endif /*__MY_PICTURE_PNG_H__*/