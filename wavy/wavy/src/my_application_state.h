#ifndef __MY_APPLICATION_STATE_H__
#define __MY_APPLICATION_STATE_H__

typedef struct _MyLayer	MyLayer;

typedef struct _ApplicationState {
	MyLayer		*currentLayer;
	cairo_t		*cr;
	cairo_t		*cr_window;	// 为了而避免有透明度的图片背景变黑是用的
	cairo_surface_t *effectSurface;
//	cairo_t		*backend;	// for drawing drop shadow of shapes	
	GdkPixmap	*pixmap;			/* Backing pixmap for drawing area */
	gdouble		windowWidth;
	gdouble		windowHeight;
	GtkWidget	*drawingArea;
	gdouble		drawingAreaWidth;
	gdouble		drawingAreaHeight;
	gdouble		drawingAreaPadding;
	gdouble		drawingAreaBgR;
	gdouble		drawingAreaBgG;
	gdouble		drawingAreaBgB;
	gdouble		drawingAreaBgA;
	gdouble		canvasX;
	gdouble		canvasY;
	gdouble		canvasWidth;
	gdouble		canvasHeight;
	gdouble		canvasPreviewWidth;
	gdouble		canvasPreviewHeight;
	gdouble 	canvasMinWidth;
	gdouble 	canvasMinHeight;
	/*
	gdouble		canvasStandardX;
	gdouble		canvasStandardY;
	gdouble		canvasStandardWidth;
	gdouble		canvasStandardHeight;
	*/
	gdouble		canvasBgR;
	gdouble		canvasBgG;
	gdouble		canvasBgB;
	gdouble		canvasBgA;
	gchar 		*canvasBgFileName;
	GdkPixbuf	*canvasBgPixbuf;
	cairo_surface_t *canvasBgSurface;
	gdouble		canvasBgX;
	gdouble		canvasBgY;
	gdouble		canvasBgWidth;
	gdouble		canvasBgHeight;
	GQueue		*focusedShapes;
	gdouble		hitWidthRadius;
	gdouble		orignX;
	gdouble		orignY;
	gdouble		scale;
	gdouble		oldScale;
	GtkWidget 	*statusBar;
	GdkCursor 	*cursorMoveNormalShape;
	GdkCursor 	*cursorMoveControlShape;
	unsigned char	*shadowBuf;
	unsigned char	*shadowBuf2;
	gsize			shadowBufSize;
	int 			shadowBufWidth;
	int 			shadowBufHeight;
	int 			shadowBufStride;
} ApplicationState;

typedef enum {
    MY_STATUS_SUCCESS = 0,

    MY_STATUS_NO_MEMORY,
    MY_STATUS_FILE_NOT_FOUND,
    MY_STATUS_READ_ERROR,
    MY_STATUS_WRITE_ERROR,
	MY_STATUS_PERMISSION_ERROR,
	MY_STATUS_GDK_PIXBUF_ERROR,
    MY_STATUS_NULL_POINTER,
	
    MY_STATUS_LAST_STATUS
} MyStatus;

#endif /*__MY_APPLICATION_STATE_H__*/