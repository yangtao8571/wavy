#define MY_LOG_LEVEL 3

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>
#include <math.h>
#include <time.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <glib.h>
#include <glib-object.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "my_constant.h"
#include "my_config.h"
#include "my_math.h"
#include "my_point.h"
#include "my_rectangle.h"
#include "my_prototype_line.h"
#include "my_prototype_manager.h"
#include "my_prototype_view.h"
#include "my_application_state.h"
#include "f-pixbuf-utils.h"
#include "my_util.h"
#include "my_debug.h"
#include "my_shape.h"
#include "my_curve.h"
#include "my_vector.h"
#include "my_control_shape_rect.h"
#include "my_picture.h"
#include "my_picture_png.h"
#include "my_layer.h"
#include "my_action.h"
#include "my_action_manager.h"
#include "my_bezier.h"
#include "my_bezier_jm.h"
#include "my_bezier_sin.h"
#include "my_spline.h"
#include "my_spline_cyc.h"
#include "my_spline_overlap.h"
#include "my_combobox_options_manager.h"
#include "my_properties_view.h"

/* XPM */
static char * WheelbarrowFull_xpm[] = {
"48 48 64 1",
"       c None",
".      c #DF7DCF3CC71B",
"X      c #965875D669A6",
"o      c #71C671C671C6",
"O      c #A699A289A699",
"+      c #965892489658",
"@      c #8E38410330C2",
"#      c #D75C7DF769A6",
"$      c #F7DECF3CC71B",
"%      c #96588A288E38",
"&      c #A69992489E79",
"*      c #8E3886178E38",
"=      c #104008200820",
"-      c #596510401040",
";      c #C71B30C230C2",
":      c #C71B9A699658",
">      c #618561856185",
",      c #20811C712081",
"<      c #104000000000",
"1      c #861720812081",
"2      c #DF7D4D344103",
"3      c #79E769A671C6",
"4      c #861782078617",
"5      c #41033CF34103",
"6      c #000000000000",
"7      c #49241C711040",
"8      c #492445144924",
"9      c #082008200820",
"0      c #69A618611861",
"q      c #B6DA71C65144",
"w      c #410330C238E3",
"e      c #CF3CBAEAB6DA",
"r      c #71C6451430C2",
"t      c #EFBEDB6CD75C",
"y      c #28A208200820",
"u      c #186110401040",
"i      c #596528A21861",
"p      c #71C661855965",
"a      c #A69996589658",
"s      c #30C228A230C2",
"d      c #BEFBA289AEBA",
"f      c #596545145144",
"g      c #30C230C230C2",
"h      c #8E3882078617",
"j      c #208118612081",
"k      c #38E30C300820",
"l      c #30C2208128A2",
"z      c #38E328A238E3",
"x      c #514438E34924",
"c      c #618555555965",
"v      c #30C2208130C2",
"b      c #38E328A230C2",
"n      c #28A228A228A2",
"m      c #41032CB228A2",
"M      c #104010401040",
"N      c #492438E34103",
"B      c #28A2208128A2",
"V      c #A699596538E3",
"C      c #30C21C711040",
"Z      c #30C218611040",
"A      c #965865955965",
"S      c #618534D32081",
"D      c #38E31C711040",
"F      c #082000000820",
"                                                ",
"          .XoO                                  ",
"         +@#$%o&                                ",
"         *=-;#::o+                              ",
"           >,<12#:34                            ",
"             45671#:X3                          ",
"               +89<02qwo                        ",
"e*                >,67;ro                       ",
"ty>                 459@>+&&                    ",
"$2u+                  ><ipas8*                  ",
"%$;=*                *3:.Xa.dfg>                ",
"Oh$;ya             *3d.a8j,Xe.d3g8+             ",
" Oh$;ka          *3d$a8lz,,xxc:.e3g54           ",
"  Oh$;kO       *pd$%svbzz,sxxxxfX..&wn>         ",
"   Oh$@mO    *3dthwlsslszjzxxxxxxx3:td8M4       ",
"    Oh$@g& *3d$XNlvvvlllm,mNwxxxxxxxfa.:,B*     ",
"     Oh$@,Od.czlllllzlmmqV@V#V@fxxxxxxxf:%j5&   ",
"      Oh$1hd5lllslllCCZrV#r#:#2AxxxxxxxxxcdwM*  ",
"       OXq6c.%8vvvllZZiqqApA:mq:Xxcpcxxxxxfdc9* ",
"        2r<6gde3bllZZrVi7S@SV77A::qApxxxxxxfdcM ",
"        :,q-6MN.dfmZZrrSS:#riirDSAX@Af5xxxxxfevo",
"         +A26jguXtAZZZC7iDiCCrVVii7Cmmmxxxxxx%3g",
"          *#16jszN..3DZZZZrCVSA2rZrV7Dmmwxxxx&en",
"           p2yFvzssXe:fCZZCiiD7iiZDiDSSZwwxx8e*>",
"           OA1<jzxwwc:$d%NDZZZZCCCZCCZZCmxxfd.B ",
"            3206Bwxxszx%et.eaAp77m77mmmf3&eeeg* ",
"             @26MvzxNzvlbwfpdettttttttttt.c,n&  ",
"             *;16=lsNwwNwgsvslbwwvccc3pcfu<o    ",
"              p;<69BvwwsszslllbBlllllllu<5+     ",
"              OS0y6FBlvvvzvzss,u=Blllj=54       ",
"               c1-699Blvlllllu7k96MMMg4         ",
"               *10y8n6FjvllllB<166668           ",
"                S-kg+>666<M<996-y6n<8*          ",
"                p71=4 m69996kD8Z-66698&&        ",
"                &i0ycm6n4 ogk17,0<6666g         ",
"                 N-k-<>     >=01-kuu666>        ",
"                 ,6ky&      &46-10ul,66,        ",
"                 Ou0<>       o66y<ulw<66&       ",
"                  *kk5       >66By7=xu664       ",
"                   <<M4      466lj<Mxu66o       ",
"                   *>>       +66uv,zN666*       ",
"                              566,xxj669        ",
"                              4666FF666>        ",
"                               >966666M         ",
"                                oM6668+         ",
"                                  *4            ",
"                                                ",
"                                                "};


static MyConfig *config;

static MyComboboxOptionsManager *comboboxOptionsManager;
static MyPropertiesView *propertiesView;
static GtkWidget *vbox_properties;
static GtkWidget *table_property;
static MyPrototypeLine *propertyPrototype = NULL;
static MyShape *currentPropertyShape = NULL;

static void		freshPropertiesWidget	(MyShape *shape);
static void		turn_off_all_shape_focus_edit	(void);
static void		add_custom_icon_with_name_from_pixbuf	(gchar *iconName, gint size, GdkPixbuf *iconPixbuf);
static MyPicture *		insertPicture_by_file_path		(gchar *filename, GtkWidget *parentWindow);
static void 	toolitem_line_clicked_callback	(GtkButton *widget,
													gpointer   user_data);


//static GdkPixbuf * filePixbuf = NULL;
//static GtkWidget *theButton;	// for test
typedef struct _PrototypePicture {
	char *absPath;
	char *iconName;
	GdkPixbuf *pixbuf;
	GtkWidget *thumbnailWidget;
} PrototypePicture;
static PrototypePicture prototypePicture0;
static PrototypePicture prototypePicture1;
static PrototypePicture prototypePicture2;
static PrototypePicture prototypePicture3;

static enum
{
  COL_LINE_CATEGORY,
  COL_LINE_INDEX,
  COL_LINE_PIXBUF,
  NUM_LINE_COLS
};
static enum {
  COL_PICTURE_PATH,
  COL_PICTURE_DISPLAY_NAME,
  COL_PICTURE_PIXBUF,
  NUM_PICTURE_COLS
};

GtkToolItem *btnSelect;
GtkToolItem *btnLineShape;
GtkWidget *btnPicture;
GtkWidget *dialogMorePicture;
GtkWidget *dialogMoreLine;
GtkWidget *toolitem_picture;
GtkWidget *toolitem_line;
static GtkWidget *popupMenu;

static int line_style_chosen;
static MyPrototypeManager *prototypeManager;
static MyPrototypeView *linePrototypeView;
static PrototypePicture currentPrototypePicture; // TODO:
static GtkWidget *btnStraightLine;	// TODO: delete this ?
static guint chosenPrototypeNth;	// 选择第几个线条
static MyPrototypeCategory chosenPrototypeCategory;	// 选择的线条类别
static GtkToolItem *toolitem_line_0;	// TODO: delete this ?
static MyPrototypeLine *linePrototype0;
static GtkToolItem *toolitem_line_1;	// TODO: delete this ?
static MyPrototypeLine *linePrototype1;
static GtkToolItem *toolitem_line_2;	// TODO: delete this ?
static MyPrototypeLine *linePrototype2;
static GtkToolItem *toolitem_line_3;	// TODO: delete this ?
static MyPrototypeLine *linePrototype3;
static GtkToolItem *toolitem_0;		// TODO: delete this ?
static GtkToolItem *toolitem_1;		// TODO: delete this ?
static GtkToolItem *toolitem_2;		// TODO: delete this ?
static GtkToolItem *toolitem_3;		// TODO: delete this ?
static GtkWidget *toolbar_prototype_line;	// TODO: delete this ?
static GtkWidget *toolbar_prototype_picture;// TODO: delete this ?

static ApplicationState *appState;
static MyActionManager *actionManager;
static MyLayer *layer;
static MyShape *latestNewShape = NULL; // 最新创建的曲线
static gint actChosen = NULL;
static gint actChosenLine = NULL;
static gdouble last_x = -1;
static gdouble last_y = -1; // last point of mouse movment
static MyControlShapeRect *canvasControlP0;
static MyControlShapeRect *canvasControlP1;
static MyControlShapeRect *canvasControlP2;
static MyControlShapeRect *canvasControlP3;
static MyControlShapeRect *canvasControlP4;
static MyControlShapeRect *canvasControlP5;
static MyControlShapeRect *canvasControlP6;
static MyControlShapeRect *canvasControlP7;
static MyControlShapeRect *canvasControlDashBox;
static gdouble		rightClickX,
					rightClickY;	// 最后一次点击鼠标右键的位置
static gint		statusBarMsgStackCount;

static MyShape *currentDndShape;
static GTimer *timer;
static guint32 drawTimestamp;

static gboolean		needUpdateShapeCoodinaryX = FALSE; // 是否需要更新所有图形的x坐标,当调整画布时设为TRUE
static gboolean		needUpdateShapeCoodinaryY = FALSE; // 是否需要更新所有图形的y坐标,当调整画布时设为TRUE


static void		calculateCanvasPositionUpdate (void) {


	guint i, len;
	MyShape *shape;

	gdouble shiftDx = appState->canvasPreviewWidth - appState->canvasWidth;
	gdouble shiftDy = appState->canvasPreviewHeight - appState->canvasHeight;

	gdouble canvasWidthWithPad;
	gdouble canvasHeightWithPad;

	my_debug ("calculateCanvasPositionUpdate ...");
	my_debug ("drawingArea->allocation.width %d", appState->drawingArea->allocation.width);
	my_debug ("drawingArea->allocation.height %d", appState->drawingArea->allocation.height);

	/*
	// 计算standard宽高
	appState->canvasStandardWidth = appState->canvasPreviewWidth / appState->scale;
	appState->canvasStandardHeight = appState->canvasPreviewHeight / appState->scale;
	*/

	// 计算画区宽高
	appState->canvasWidth = appState->canvasPreviewWidth;	// 画布宽高
	appState->canvasHeight = appState->canvasPreviewHeight;
	canvasWidthWithPad = appState->canvasWidth + 2 * appState->drawingAreaPadding;	// 画区所需最小宽高
	canvasHeightWithPad = appState->canvasHeight + 2 * appState->drawingAreaPadding;
	appState->drawingAreaWidth = my_max (canvasWidthWithPad, appState->drawingArea->allocation.width); // 画区实际宽高，取屏幕值与所需最小值中较大的
	appState->drawingAreaHeight = my_max (canvasHeightWithPad, appState->drawingArea->allocation.height);

	// 计算画布居中后的坐标
	if (appState->drawingArea->allocation.width > canvasWidthWithPad) {	// 画布x坐标
		appState->canvasX = ceil ((appState->drawingArea->allocation.width - appState->canvasWidth) / 2) + 0.5; // 为了画出来的边框好看所以加0.5
	} else {
		appState->canvasX = appState->drawingAreaPadding;
	}
	if (appState->drawingArea->allocation.height > canvasHeightWithPad) {	// 画布y坐标
		appState->canvasY = ceil ((appState->drawingArea->allocation.height - appState->canvasHeight) / 2) + 0.5;
	} else {
		appState->canvasY = appState->drawingAreaPadding;
	}
	appState->orignX = appState->canvasX;
	appState->orignY = appState->canvasY;

	my_debug ("newOrign %f,%f", appState->orignX, appState->orignY);
	// 计算画布控制点坐标
	canvasControlP0->x = - canvasControlP0->width;
	canvasControlP0->y = - canvasControlP0->height;
	canvasControlP1->x = appState->canvasWidth / 2;
	canvasControlP1->y = - canvasControlP1->height;
	canvasControlP2->x = appState->canvasWidth;
	canvasControlP2->y = - canvasControlP2->height;
	canvasControlP3->x = canvasControlP2->x;
	canvasControlP3->y = appState->canvasHeight / 2;
	canvasControlP4->x = canvasControlP2->x;
	canvasControlP4->y = appState->canvasHeight;
	canvasControlP5->x = canvasControlP1->x;
	canvasControlP5->y = canvasControlP4->y;
	canvasControlP6->x = canvasControlP0->x;
	canvasControlP6->y = canvasControlP5->y;
	canvasControlP7->x = canvasControlP0->x;
	canvasControlP7->y = canvasControlP3->y;
	canvasControlDashBox->x = 0;
	canvasControlDashBox->y = 0;
	canvasControlDashBox->width = appState->canvasWidth;
	canvasControlDashBox->height = appState->canvasHeight;
	MY_SHAPE (canvasControlDashBox)->isShowing = FALSE;
	// 更新所有图形的坐标
	if (needUpdateShapeCoodinaryX || needUpdateShapeCoodinaryY) {
		shiftDx = needUpdateShapeCoodinaryX ? shiftDx : 0;
		shiftDy = needUpdateShapeCoodinaryY ? shiftDy : 0;
		appState->canvasBgX += shiftDx;
		appState->canvasBgY += shiftDy;
		for (i = 0, len = my_layer_shape_count (layer); i < len; i++) {
			shape = my_layer_peek_nth (layer, i);
			if (shape->isControlShape && MY_CONTROL_SHAPE_RECT (shape)->isCanvasControl)
				continue;

			my_shape_shift_self_only (shape, shiftDx, shiftDy);

			// todo: delete shape if need
			/*
			if (!shape->isControlShape && my_shape_is_out_of_canvas (shape, appState)) {
				my_debug ("shape %#x is out of canvas, deleting...", shape);
				my_layer_delete_shape (layer, shape);
			}
			*/
		}
		needUpdateShapeCoodinaryX = needUpdateShapeCoodinaryY = FALSE;
	}
	my_debug ("calculateCanvasPositionUpdate done.");
}


static void		resizeDrawingArea	(void) {

	
	GtkWidget *widget = appState->drawingArea;
	my_debug ("resizeDrawingArea ...");
	if (appState->pixmap) {
		g_object_unref ((gpointer) appState->pixmap);
	}
	appState->pixmap = gdk_pixmap_new (widget->window,
						   widget->allocation.width,
						   widget->allocation.height,
						   -1);
	if (appState->cr) {
		// todo: unref cr
	}
	appState->cr = gdk_cairo_create (appState->pixmap);
	calculateCanvasPositionUpdate ();
	// 效果surface初始化
	if (appState->effectSurface) {
		cairo_surface_destroy (appState->effectSurface);
	}
	appState->effectSurface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 
								appState->canvasWidth,
								appState->canvasHeight);

	gtk_widget_set_size_request (widget,
								appState->canvasWidth + 2 * appState->drawingAreaPadding,
								appState->canvasHeight + 2 * appState->drawingAreaPadding);

	my_debug ("resizeDrawingArea done.");
}


static void 	updateCanvasSize (gdouble width, gdouble height) {

	
	appState->canvasPreviewWidth = width;
	appState->canvasPreviewHeight = height;
	resizeDrawingArea ();
}

/**
 * draw画布里面的内容
 *
 * isDrawInWindow	TRUE -> 往窗口中画,	
 *					FALSE -> 往导出png中画
 * destSurface		当isDrawInWindow为FALSE时使用
 */
static void		draw_content	(gboolean isDrawInWindow, cairo_surface_t *destSurface) {

	
	guint layerCount, shapeCount, pointCount, i, j, k;
	MyShape *shape;
	cairo_t *copy, *cr_window, *cr_effect, *cr;	
	gdouble copyOrignX;
	gdouble copyOrignY;
	
	if (isDrawInWindow) {
		appState->cr_window = gdk_cairo_create (appState->pixmap);
	} else {
		// 备份appState
		copy = appState->cr;
		copyOrignX = appState->orignX;
		copyOrignY = appState->orignY;

	
		cr = cairo_create (destSurface);
		appState->cr = cr;
		appState->orignX = 0;
		appState->orignY = 0;

		appState->cr_window = appState->cr;
	}
	cr_window = appState->cr_window;
	
	//
	// 重画图形
	//
	for (j = 0, shapeCount = my_layer_shape_count (layer); j < shapeCount; j++) {
		shape = my_layer_peek_nth (layer, j);
		my_shape_draw_self_shadow (shape, appState);
		my_shape_draw_self (shape, appState);
	}	
	// 褪色效果
	/*
	{
		int fadeRight = 600; //pixel
		gdouble da = 1. / 200; // alpha delta
		int x, y, i;
		int stride = cairo_image_surface_get_stride (appState->effectSurface);
		unsigned char *surfaceData = cairo_image_surface_get_data (appState->effectSurface);
		unsigned char *p;

		x = appState->canvasWidth - fadeRight;
		if (x > 0) {
			cairo_surface_flush (appState->effectSurface);

			for (; x < appState->canvasWidth; x++, da++) {
				for (y = 0; y < appState->canvasHeight; y++) {
					p = surfaceData + stride * y + x * 4;
					p[3] -= da;
				}
			}
			cairo_surface_mark_dirty (appState->effectSurface);
		}
	}
	cairo_set_source_surface (cr_window, 
							appState->effectSurface, 
							appState->orignX,
							appState->orignY);
	cairo_save (cr_window);
	cairo_set_operator (cr_window, CAIRO_OPERATOR_OVER);
	cairo_paint (cr_window);
	cairo_restore (cr_window);

	cairo_destroy (cr_effect);
	appState->cr = copy;
	*/

	// todo: seperate class
	// test gradient effect
	if (TRUE) {
		cairo_pattern_t *pat;
		gdouble fadePaddingLeft = 0;
		gdouble fadePaddingTop = 0;
		gdouble fadePaddingRight = 500;
		gdouble fadePaddingBottom = 0;

		if (fadePaddingLeft > 0) {
			pat = cairo_pattern_create_linear (appState->orignX, 
												appState->orignY,
												appState->orignX + fadePaddingLeft,
												appState->orignY);
			cairo_pattern_add_color_stop_rgba (pat, 0., 1, 1, 1, 1); // 左侧不透明,显背景色
			cairo_pattern_add_color_stop_rgba (pat, 1., 1, 1, 1, 0); // 右侧透明,显原色
			cairo_set_source (cr_window, pat);
			cairo_rectangle (cr_window, appState->orignX, 
										appState->orignY,  
										fadePaddingLeft, 
										appState->canvasHeight);
			cairo_fill (cr_window);
			cairo_pattern_destroy (pat);
		}
		if (fadePaddingRight > 0) {
			pat = cairo_pattern_create_linear (appState->orignX + appState->canvasWidth - fadePaddingRight, 
												appState->orignY,
												appState->orignX + appState->canvasWidth,
												appState->orignY);
			cairo_pattern_add_color_stop_rgba (pat, 0., 1, 1, 1, 0); // 左侧透明,显原色
			cairo_pattern_add_color_stop_rgba (pat, 1., 1, 1, 1, 1); // 右侧不透明,显背景色
			cairo_set_source (cr_window, pat);
			cairo_rectangle (cr_window, appState->orignX + appState->canvasWidth - fadePaddingRight, 
										appState->orignY,  
										fadePaddingRight, 
										appState->canvasHeight);
			cairo_fill (cr_window);
			cairo_pattern_destroy (pat);
		}
		if (fadePaddingTop > 0) {
			pat = cairo_pattern_create_linear (appState->orignX, 
												appState->orignY,
												appState->orignX,
												appState->orignY + fadePaddingTop);
			cairo_pattern_add_color_stop_rgba (pat, 0., 1, 1, 1, 1); // 上侧不透明,显背景色
			cairo_pattern_add_color_stop_rgba (pat, 1., 1, 1, 1, 0); // 下侧透明,显原色
			cairo_set_source (cr_window, pat);
			cairo_rectangle (cr_window, appState->orignX, 
										appState->orignY, 
										appState->canvasWidth,
										fadePaddingTop);
			cairo_fill (cr_window);
			cairo_pattern_destroy (pat);
		}
		if (fadePaddingBottom > 0) {
			pat = cairo_pattern_create_linear (appState->orignX, 
												appState->orignY + appState->canvasHeight - fadePaddingBottom,
												appState->orignX,
												appState->orignY + appState->canvasHeight);
			cairo_pattern_add_color_stop_rgba (pat, 0., 1, 1, 1, 0); // 上侧透明,显原色
			cairo_pattern_add_color_stop_rgba (pat, 1., 1, 1, 1, 1); // 下侧不透明,显背景色
			cairo_set_source (cr_window, pat);
			cairo_rectangle (cr_window, appState->orignX, 
										appState->orignY + appState->canvasHeight - fadePaddingBottom,
										appState->canvasWidth,
										fadePaddingTop);
			cairo_fill (cr_window);
			cairo_pattern_destroy (pat);
		}
		// 卷页效果	
		{
		MyPicture *effectPicture;
		/*
		effectPicture = (MyPicture *) g_object_new (MY_TYPE_PICTURE_PNG, NULL);
		MY_SHAPE (effectPicture)->appState = appState;
		my_picture_png_init_by_file (MY_PICTURE_PNG (effectPicture), "../icon/effect-0.png");
		cairo_set_source_surface (cr_window, 
									effectPicture->imageSurface, 
									appState->orignX,
									appState->orignY);
		cairo_paint (cr_window);
		cairo_line_to (cr_window, 
						appState->orignX, 
						appState->orignY);
		cairo_line_to (cr_window, 
						appState->orignX + effectPicture->orignWidth, 
						appState->orignY);
		cairo_line_to (cr_window, 
						appState->orignX, 
						appState->orignY + effectPicture->orignHeight);
		cairo_line_to (cr_window, 
						appState->orignX, 
						appState->orignY);
		cairo_set_source_rgb (cr_window, 1., 1., 1.);
		cairo_fill (cr_window);
		

		effectPicture = (MyPicture *) g_object_new (MY_TYPE_PICTURE_PNG, NULL);
		MY_SHAPE (effectPicture)->appState = appState;
		my_picture_png_init_by_file (MY_PICTURE_PNG (effectPicture), "../icon/effect-1.png");
		cairo_set_source_surface (cr_window, 
									effectPicture->imageSurface, 
									appState->canvasWidth - effectPicture->orignWidth + appState->orignX,
									appState->orignY);
		cairo_paint (cr_window);
		cairo_line_to (cr_window, 
						appState->orignX + appState->canvasWidth - effectPicture->orignWidth,
						appState->orignY);
		cairo_line_to (cr_window, 
						appState->orignX + appState->canvasWidth, 
						appState->orignY);
		cairo_line_to (cr_window, 
						appState->orignX + appState->canvasWidth, 
						appState->orignY + effectPicture->orignHeight);
		cairo_line_to (cr_window, 
						appState->orignX + appState->canvasWidth - effectPicture->orignWidth,
						appState->orignY);
		cairo_set_source_rgb (cr_window, 1., 1., 1.);
		cairo_fill (cr_window);
		*/

		
		effectPicture = (MyPicture *) g_object_new (MY_TYPE_PICTURE_PNG, NULL);
		MY_SHAPE (effectPicture)->appState = appState;
		my_picture_png_init_by_file (MY_PICTURE_PNG (effectPicture), "../icon/effect-2.png");
		cairo_set_source_surface (cr_window, 
									effectPicture->imageSurface, 
									appState->canvasWidth - effectPicture->orignWidth + appState->orignX,
									appState->canvasHeight - effectPicture->orignHeight + appState->orignY);
		cairo_paint (cr_window);
		cairo_save (cr_window);
		cairo_line_to (cr_window, 
						appState->orignX + appState->canvasWidth,
						appState->orignY + appState->canvasHeight - effectPicture->orignHeight);
		cairo_line_to (cr_window, 
						appState->orignX + appState->canvasWidth, 
						appState->orignY + appState->canvasHeight);
		cairo_line_to (cr_window, 
						appState->orignX + appState->canvasWidth - effectPicture->orignWidth, 
						appState->orignY + appState->canvasHeight);
		cairo_line_to (cr_window, 
						appState->orignX + appState->canvasWidth,
						appState->orignY + appState->canvasHeight - effectPicture->orignHeight);
		if (isDrawInWindow) {
			cairo_set_source_rgba (cr_window, 1., 1., 1., 1.);
		} else {
			cairo_set_operator (cr_window, CAIRO_OPERATOR_CLEAR);
		}
		cairo_fill (cr_window);
		cairo_restore (cr_window);
		
		/*
		effectPicture = (MyPicture *) g_object_new (MY_TYPE_PICTURE_PNG, NULL);
		MY_SHAPE (effectPicture)->appState = appState;
		my_picture_png_init_by_file (MY_PICTURE_PNG (effectPicture), "../icon/effect-3.png");
		cairo_set_source_surface (cr_window, 
									effectPicture->imageSurface, 
									appState->orignX,
									appState->canvasHeight - effectPicture->orignHeight + appState->orignY);
		cairo_paint (cr_window);
		cairo_line_to (cr_window, 
						appState->orignX,
						appState->orignY + appState->canvasHeight - effectPicture->orignHeight);
		cairo_line_to (cr_window, 
						appState->orignX, 
						appState->orignY + appState->canvasHeight);
		cairo_line_to (cr_window, 
						appState->orignX + effectPicture->orignWidth, 
						appState->orignY + appState->canvasHeight);
		cairo_line_to (cr_window, 
						appState->orignX,
						appState->orignY + appState->canvasHeight - effectPicture->orignHeight);
		cairo_set_source_rgb (cr_window, 1., 1., 1.);
		cairo_fill (cr_window);
		*/
		}
	}
	// 重画所有控制图形
	if (isDrawInWindow) {
		for (j = 0, shapeCount = my_layer_shape_count (layer); j < shapeCount; j++) {
			shape = my_layer_peek_nth (layer, j);
			if (shape->isControlShape)
				my_shape_draw_self (shape, appState);
		}
	}
	
	if (isDrawInWindow) {
		cairo_destroy (cr_window);
		appState->cr_window = NULL;
	} else {
		// 恢复appState
		appState->cr = copy;
		appState->orignX = copyOrignX;
		appState->orignY = copyOrignY;
		cairo_destroy (cr);
	}
}


static void		redraw (void) {

	
	guint layerCount, shapeCount, pointCount, i, j, k;
	MyShape *shape;
	cairo_t *copy, *cr_window, *cr_effect;

	my_debug ("redrawing...");
	// 重画窗口
	cairo_set_operator (appState->cr, CAIRO_OPERATOR_SOURCE);
	cairo_set_source_rgba (appState->cr,
							appState->drawingAreaBgR,
							appState->drawingAreaBgG,
							appState->drawingAreaBgB,
							appState->drawingAreaBgA);
	cairo_rectangle (appState->cr,
					0,
					0,
					appState->drawingAreaWidth,
					appState->drawingAreaHeight);
	cairo_fill (appState->cr);
	// 重画画布
	cairo_set_source_rgba (appState->cr,
							appState->canvasBgR,
							appState->canvasBgG,
							appState->canvasBgB,
							appState->canvasBgA);
	my_debug ("redrawing canvas position %f,%f,%f,%f", appState->canvasX, appState->canvasY, appState->canvasWidth, appState->canvasHeight);
	cairo_rectangle (appState->cr,
					appState->canvasX,
					appState->canvasY,
					appState->canvasWidth,
					appState->canvasHeight);
	cairo_fill (appState->cr);
	// 重画画布背景图
	if (appState->canvasBgPixbuf) {
		/*
		cairo_save (appState->cr);
		my_debug ("orign %f,%f, canvasBgXY %f,%f", appState->orignX, appState->orignY, appState->canvasBgX, appState->canvasBgY);

		gdk_cairo_set_source_pixbuf (appState->cr,
							 appState->canvasBgPixbuf,
							 appState->orignX + appState->canvasBgX,
							 appState->orignY + appState->canvasBgY);
		cairo_rectangle (appState->cr,
						appState->orignX + appState->canvasBgX,
						appState->orignY + appState->canvasBgY,
						appState->canvasBgWidth,
						appState->canvasBgHeight);
		cairo_clip (appState->cr);

		cairo_paint (appState->cr);
		cairo_restore (appState->cr);
		*/

		gdk_draw_pixbuf (appState->pixmap,
					appState->drawingArea->style->white_gc,
					appState->canvasBgPixbuf,
					0,
					0,
					appState->orignX + appState->canvasBgX,
					appState->orignY + appState->canvasBgY,
					appState->canvasBgWidth,
					appState->canvasBgHeight,
					GDK_RGB_DITHER_NORMAL, 0, 0);

		/*
		GdkPixbuf *scaledPixbuf = gdk_pixbuf_scale_simple (appState->canvasBgPixbuf,
															appState->canvasBgWidth + 200,
															appState->canvasBgHeight + 200,
															GDK_INTERP_BILINEAR);

		gdk_draw_pixbuf (appState->pixmap,
					appState->drawingArea->style->white_gc,
					scaledPixbuf,
					0,
					0,
					appState->orignX + appState->canvasBgX,
					appState->orignY + appState->canvasBgY,
					appState->canvasBgWidth + 200,
					appState->canvasBgHeight + 200,
					GDK_RGB_DITHER_NORMAL, 0, 0);
		*/
	}

	if (appState->canvasBgSurface) {
		cairo_save (appState->cr);
		my_debug ("orign %f,%f, canvasBgXY %f,%f", appState->orignX, appState->orignY, appState->canvasBgX, appState->canvasBgY);
		/*
		cairo_rectangle (appState->cr,
						appState->orignX + appState->canvasBgX,
						appState->orignY + appState->canvasBgY,
						appState->canvasBgWidth,
						appState->canvasBgHeight);
		cairo_clip (appState->cr);
		cairo_set_source_surface (appState->cr,
								appState->canvasBgSurface,
								appState->orignX + appState->canvasBgX,
								appState->orignY + appState->canvasBgY);
		cairo_paint (appState->cr);
		*/
		/*
		int stride = cairo_format_stride_for_width (CAIRO_FORMAT_ARGB32, appState->canvasBgWidth);
		unsigned char * data = cairo_image_surface_get_data (appState->canvasBgSurface);
		cairo_surface_t *surface = cairo_image_surface_create_for_data (data,
											CAIRO_FORMAT_ARGB32,
											appState->canvasBgWidth,
											appState->canvasBgHeight,
											stride);
		cairo_mask_surface (appState->cr, surface, 100, 100);
		*/
		/*
		cairo_set_source_rgba (appState->cr, 0, 0.5, 0.5, 1);
		cairo_mask_surface (appState->cr, appState->canvasBgSurface, 100, 100);
		*/
		/*
		// test, it works
		cairo_surface_t *surf1 = cairo_image_surface_create_from_png("BGP.png");
		cairo_surface_t *img = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 100, 100);
		cairo_t *cr = cairo_create(img);
		cairo_set_source_surface(cr, surf1, 0, 0);
		cairo_paint(cr);
		cairo_surface_flush(img);
		cairo_surface_write_to_png(img, "result1.png");
		*/

		cairo_restore (appState->cr);
	}

	draw_content (TRUE, NULL);

	// 重画窗口4个边界区域
	cairo_set_operator (appState->cr, CAIRO_OPERATOR_SOURCE);
	cairo_set_source_rgba (appState->cr,
							appState->drawingAreaBgR,
							appState->drawingAreaBgG,
							appState->drawingAreaBgB,
							appState->drawingAreaBgA);
	cairo_rectangle (appState->cr,
					0,
					0,
					appState->canvasX,
					appState->drawingArea->allocation.height);
	cairo_rectangle (appState->cr,
					0,
					0,
					appState->drawingArea->allocation.width,
					appState->canvasY);
	cairo_rectangle (appState->cr,
					appState->canvasX +  + appState->canvasWidth,
					0,
					appState->canvasX,
					appState->drawingArea->allocation.height);
	cairo_rectangle (appState->cr,
					0,
					appState->canvasY + appState->canvasHeight,
					appState->drawingArea->allocation.width,
					appState->drawingArea->allocation.height - (appState->canvasY + appState->canvasHeight));
	cairo_fill (appState->cr);
	// 重画画布边框
	cairo_rectangle (appState->cr,
					appState->canvasX,
					appState->canvasY,
					appState->canvasWidth,
					appState->canvasHeight);
	cairo_set_dash (appState->cr, NULL, 0, 0);
	cairo_set_source_rgba (appState->cr, 0, 0, 0, 1.);
	cairo_set_line_width (appState->cr, 1);
	cairo_stroke (appState->cr);
	// 重画所有控制图形
	for (j = 0, shapeCount = my_layer_shape_count (layer); j < shapeCount; j++) {
		shape = my_layer_peek_nth (layer, j);
		if (shape->isControlShape)
			my_shape_draw_self (shape, appState);
	}
	
	// 失效区域
	gtk_widget_queue_draw_area (appState->drawingArea,
						0, 0,
						appState->drawingArea->allocation.width,
						appState->drawingArea->allocation.height);

//  gdk_draw_drawable (appState->drawingArea->window,
//		     appState->drawingArea->style->fg_gc[gtk_widget_get_state (appState->drawingArea)],
//		     appState->pixmap,
//		     0,
//			 0,
//			 0,
//			 0,
//			 appState->drawingArea->allocation.width,
//			 appState->drawingArea->allocation.height);
	my_debug ("redraw done.");
}

/* performance tune try
*/
static int ii = 0;
static void		redrawOnOdd (void) {

	if (ii++ % 2 != 0) { // 神奇的代码，能够使拖动流畅，在有背景图时
		my_debug ("No redrawing...");
		return;
	}

	redraw ();
}


static void		redrawOnFps (int fps) {
	fps = 30;
	
}

/*
 * This event is generated whenever the window changes size, including when it is originally created.
 */
static gboolean		configure_event		(GtkWidget         *widget,
										 GdkEventConfigure *event ) {
	my_debug ("configure_event ...");
	/*
  const char *filename = "screenshot.jpg";

  if (!filePixbuf) {
		// load image from file into pixbuf
	  filePixbuf = gdk_pixbuf_new_from_file (filename, NULL);
	}

  if (!appState->pixmap) {
	appState->pixmap = gdk_pixmap_new (widget->window,
						   widget->allocation.width,
						   widget->allocation.height,
						   -1);

	  // clears the pixmap initially to white
	  gdk_draw_rectangle (appState->pixmap,
					widget->style->white_gc,
				  TRUE,
				  0, 0,
				  widget->allocation.width,
				  widget->allocation.height);

	  // load image from pixbuf into pixmap
	  gdk_draw_pixbuf (appState->pixmap,
					widget->style->white_gc,
					filePixbuf,
					0,
					0,
					0,
					0,
					widget->allocation.width,
					widget->allocation.height,
					GDK_RGB_DITHER_NORMAL, 0, 0);

	}
	*/
	resizeDrawingArea ();
	redraw ();
	return TRUE;
}

/* Redraw the screen from the backing pixmap */
static gboolean		expose_event	( GtkWidget      *widget,
									GdkEventExpose *event ) {

	my_debug ("expose_event, x: %d, y: %d, width: %d, height: %d",
			event->area.x, event->area.y,
		    event->area.width, event->area.height);

	gdk_draw_drawable (widget->window,
		     widget->style->fg_gc[gtk_widget_get_state (widget)],
		     appState->pixmap,
		     event->area.x, event->area.y,
		     event->area.x, event->area.y,
		     event->area.width, event->area.height);

  return TRUE;
}
/*
// 定义线型宽度改变的回调函数
#define DEFINE_WIDTH_ACTIVE_FUNCTION(w) \
	static void width##w##_activated (GtkMenuItem *item, MyActionManager *actionManager) {\
		MyShape *latestShape;\
		latestShape = (MyShape *) g_queue_peek_tail (appState->focusedShapes);\
		if (latestShape) {\
			if (latestShape->isControlShape) {\
				latestShape = MY_CONTROL_SHAPE_RECT (latestShape)->target;\
			}\
			if (latestShape->isStrokeWidthChangable)\
				latestShape->strokeWidth = w;\
			my_shape_shadow_mark_dirty (latestShape);\
			my_shape_mark_dirty (latestShape);\
			redraw ();\
		}\
		my_debug ("click popup menu with shape %#x", latestShape);\
	}
static void		width1_activated		(GtkMenuItem *item, MyActionManager *actionManager) {

	MyShape *latestShape;
	latestShape = (MyShape *) g_queue_peek_tail (appState->focusedShapes);
	my_debug ("click popup menu with shape %#x", latestShape);
	if (latestShape) {
		
		if (latestShape->isControlShape) {
			latestShape = MY_CONTROL_SHAPE_RECT (latestShape)->target;
		}
		
		if (latestShape->isStrokeWidthChangable) 
			latestShape->strokeWidth = 1;
		my_shape_shadow_mark_dirty (latestShape);
		my_shape_mark_dirty (latestShape);
		redraw ();
	}
}
DEFINE_WIDTH_ACTIVE_FUNCTION(1)
DEFINE_WIDTH_ACTIVE_FUNCTION(2)
DEFINE_WIDTH_ACTIVE_FUNCTION(3)
DEFINE_WIDTH_ACTIVE_FUNCTION(4)
DEFINE_WIDTH_ACTIVE_FUNCTION(5)
DEFINE_WIDTH_ACTIVE_FUNCTION(6)
DEFINE_WIDTH_ACTIVE_FUNCTION(7)
DEFINE_WIDTH_ACTIVE_FUNCTION(8)
DEFINE_WIDTH_ACTIVE_FUNCTION(9)
DEFINE_WIDTH_ACTIVE_FUNCTION(10)
DEFINE_WIDTH_ACTIVE_FUNCTION(11)
DEFINE_WIDTH_ACTIVE_FUNCTION(12)
DEFINE_WIDTH_ACTIVE_FUNCTION(13)
DEFINE_WIDTH_ACTIVE_FUNCTION(14)
DEFINE_WIDTH_ACTIVE_FUNCTION(15)
*/


/**
 * 属性面板-输入框回车回调
 */
static void		entry_property_activate_callback		(GtkEntry *entry,
														   gpointer  user_data) {
	const gchar *text = gtk_entry_get_text (entry);
	double d;
	GHashTable *ht = (GHashTable *) user_data;
	MyPropertyInfo *propertyInfo = (MyPropertyInfo *) g_hash_table_lookup (ht, "propertyInfo");
	MyPrototypeLine *prototype = (MyPrototypeLine *) g_hash_table_lookup (ht, "prototype");
	if (propertyInfo->valueType == MY_PROPERTY_VALUE_TYPE_DOUBLE
		|| propertyInfo->valueType == MY_PROPERTY_VALUE_TYPE_INT) {
		d = my_util_str_to_double (text);
	}

	if (memcmp (propertyInfo->id, "red", 3) == 0) {
		prototype->red = d;
	}else if (memcmp (propertyInfo->id, "green", 5) == 0) {
		prototype->green = d;
	}else if (memcmp (propertyInfo->id, "blue", 4) == 0) {
		prototype->blue = d;
	}else if (memcmp (propertyInfo->id, "alpha", 5) == 0) {
		prototype->alpha = d;
	}else if (memcmp (propertyInfo->id, "strokeWidth", 12) == 0) {
		prototype->strokeWidth = d;
	}else if (memcmp (propertyInfo->id, "orignDashesStr", 15) == 0) {
		my_prototype_line_set_orign_dashes_str (prototype, text);
	}else if (memcmp (propertyInfo->id, "dashOffset", 11) == 0) {
		prototype->dashOffset = d;
	}else if (memcmp (propertyInfo->id, "shadowDeltaX", 12) == 0) {
		prototype->shadowDeltaX = d;
	}else if (memcmp (propertyInfo->id, "shadowDeltaY", 12) == 0) {
		prototype->shadowDeltaY = d;
	}else if (memcmp (propertyInfo->id, "jmOffset", 8) == 0) {
		prototype->jmOffset = d;
	}else if (memcmp (propertyInfo->id, "jmInterval", 10) == 0) {
		prototype->jmInterval = d;
	}else if (memcmp (propertyInfo->id, "jmEdgeLen", 9) == 0) {
		prototype->jmEdgeLen = d;
	}else if (memcmp (propertyInfo->id, "jmSide", 6) == 0) {
		prototype->jmSide = d;
	}else if (memcmp (propertyInfo->id, "offset", 6) == 0) {
		prototype->offset = d;
	}else if (memcmp (propertyInfo->id, "interval", 8) == 0) {
		prototype->interval = d;
	}else if (memcmp (propertyInfo->id, "segmentCount", 13) == 0) {
		prototype->segmentCount = d;
	}else if (memcmp (propertyInfo->id, "amplitude", 9) == 0) {
		prototype->amplitude = d;
	}else if (memcmp (propertyInfo->id, "phase", 5) == 0) {
		prototype->phase = d;
	}else if (memcmp (propertyInfo->id, "zhouqi", 6) == 0) {
		prototype->zhouqi = d;
	}else if (memcmp (propertyInfo->id, "segmentCountPerHemite", 21) == 0) {
		prototype->segmentCountPerHemite = d;
	}else if (memcmp (propertyInfo->id, "dPt1_x", 7) == 0) {
		prototype->dPt1_x = d;
	}else if (memcmp (propertyInfo->id, "dPt1_y", 7) == 0) {
		prototype->dPt1_y = d;
	}else if (memcmp (propertyInfo->id, "dPtn_x", 7) == 0) {
		prototype->dPtn_x = d;
	}else if (memcmp (propertyInfo->id, "dPtn_y", 7) == 0) {
		prototype->dPtn_y = d;
	}else if (memcmp (propertyInfo->id, "overlapOffset", 14) == 0) {
		prototype->overlapOffset = d;
	}else if (memcmp (propertyInfo->id, "overlapInterval", 16) == 0) {
		prototype->overlapInterval = d;
	}else if (memcmp (propertyInfo->id, "overlapRadius", 14) == 0) {
		prototype->overlapRadius = d;
	}
	if (currentPropertyShape) {
		my_shape_from_prototype_line (currentPropertyShape, prototype);
		my_shape_fresh_dirty (currentPropertyShape);
		my_shape_calculate_just_size (currentPropertyShape);
		my_shape_shadow_mark_dirty (currentPropertyShape);
		redraw ();
		freshPropertiesWidget (currentPropertyShape);
	}
}

/**
 * 属性面板-下拉回调
 */
static void		combobox_property_changed_callback		(GtkComboBox *widget, gpointer user_data) {

	
	int activeIndex = gtk_combo_box_get_active (widget);
	gint value;
	GHashTable *ht = (GHashTable *) user_data;
	MyPropertyInfo *propertyInfo = (MyPropertyInfo *) g_hash_table_lookup (ht, "propertyInfo");
	MyPrototypeLine *prototype = (MyPrototypeLine *) g_hash_table_lookup (ht, "prototype");
	
	if (memcmp (propertyInfo->id, "isShadowing", 11) == 0) {
		value = my_combobox_options_manager_get_option_value (comboboxOptionsManager, propertyInfo->id, activeIndex);
		prototype->isShadowing = value;
	}else if (memcmp (propertyInfo->id, "isDashing", 9) == 0) {
		value = my_combobox_options_manager_get_option_value (comboboxOptionsManager, propertyInfo->id, activeIndex);
		prototype->isDashing = value;
	}else if (memcmp (propertyInfo->id, "jmSide", 6) == 0) {
		value = my_combobox_options_manager_get_option_value (comboboxOptionsManager, propertyInfo->id, activeIndex);
		prototype->jmSide = value;
	}else if (memcmp (propertyInfo->id, "overlapSide", 12) == 0) {
		value = my_combobox_options_manager_get_option_value (comboboxOptionsManager, propertyInfo->id, activeIndex);
		prototype->overlapSide = value;
	}
	if (currentPropertyShape) {
		my_shape_from_prototype_line (currentPropertyShape, prototype);
		my_shape_shadow_mark_dirty (currentPropertyShape);
		redraw ();
	}
}

/**
 * 属性面板-刷新
 */
static void			freshPropertiesWidget	(MyShape *shape) {

	
	currentPropertyShape = shape;
	if (currentPropertyShape) {
		if (propertyPrototype) {
			g_object_unref (propertyPrototype);
			propertyPrototype = NULL;
		}
		propertyPrototype = (MyPrototypeLine *) g_object_new (MY_TYPE_PROTOTYPE_LINE, NULL);
		my_shape_to_prototype_line (currentPropertyShape, propertyPrototype);
		my_properties_view_clear (propertiesView, vbox_properties);
		my_properties_view_render (propertiesView, vbox_properties, propertyPrototype);
	}
}


/**
 * 切换到选择功能
 */
static void			switchToSelectFunc		(void) {
	

	gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON(btnSelect), TRUE);		
	actChosenLine = MY_ACTION_FUNC_SELECT;
}


static gboolean		button_press_event	( GtkWidget		*widget,
										GdkEventButton	*event,
										gpointer userData)
{
  guint layerCount, curveCount, i, j, len;
  MyShape *shape;
  MyShape *focusShape, *latestShape;
	gdouble x = event->x - appState->orignX;
	gdouble y = event->y - appState->orignY;
	
	MyMouse mouseButton;

  my_debug ("button_press_event");
  
	gtk_widget_grab_focus (appState->drawingArea); // 必需,使drawingarea能收到keyboard event
		
	last_x = x;
	last_y = y;
	// 更新操作状态
	if (event->button == 1) {
		mouseButton = MY_MOUSE_LEFT;
	} else if (event->button == 2) {
		mouseButton = MY_MOUSE_MIDDLE;
	} else if (event->button == 3) {
		mouseButton = MY_MOUSE_RIGHT;
	}
	// 判断是否选中图形
	if (mouseButton == MY_MOUSE_LEFT) {
		shape = my_layer_hit_shape (layer, x, y, appState->hitWidthRadius, appState);
	} else {
		shape = NULL;
	}	
	my_debug ("hitted shape %#x", shape);
	
	my_action_manager_update (actionManager, x, y, TRUE, actChosen, shape, mouseButton);

  if (event->button == 1 && appState->pixmap != NULL) { // mouse left button click
/*
		last_x = x;
		last_y = y;

		// 判断是否选中图形
		shape = my_layer_hit_shape (layer, x, y, appState->hitWidthRadius, appState);
		my_debug ("hitted shape %#x", shape);

		// 更新操作状态
		MyMouse mouseButton;
		if (event->button == 1) {
			mouseButton = MY_MOUSE_LEFT;
		} else if (event->button == 2) {
			mouseButton = MY_MOUSE_MIDDLE;
		} else if (event->button == 3) {
			mouseButton = MY_MOUSE_RIGHT;
		}
		my_action_manager_update (actionManager, x, y, TRUE, actChosen, shape, mouseButton);
*/
		// blur当前所有focused shape
		my_debug ("pressing button bluring all shapes(%u) ...", g_queue_get_length (appState->focusedShapes));
		for (i = 0, len = g_queue_get_length (appState->focusedShapes); i < len; i++) {
			shape = (MyShape *) g_queue_peek_nth (appState->focusedShapes, i);
			shape->isFocusing = FALSE;
			my_shape_hide_control_shapes (shape);
			my_debug ("shape %#x blured isEditable ? %d", shape, shape->isEditable);
		}
		g_queue_clear (appState->focusedShapes);

		switch (my_action_manager_get_act_type (actionManager)) {
		case MY_ACTION_TYPE_UNKNOWN:
			break;
		case MY_ACTION_TYPE_CLICK:
			my_debug ("=============================== pressing button MY_ACTION_TYPE_CLICK =======================================");
			// 重画
			redraw();
			break;
		case MY_ACTION_TYPE_EDIT_CLICK:
			my_debug ("=============================== pressing button MY_ACTION_TYPE_EDIT_CLICK =======================================");
			// focus当前点中的
			if (focusShape = my_action_manager_get_active_graph_target (actionManager)) {
				my_debug ("pressing button focusing shape %#x", focusShape);
				if (focusShape->isFocusable) {
					focusShape->isFocusing = TRUE;

					g_queue_push_tail (appState->focusedShapes, focusShape);
				}
				my_debug ("focus shape %#x isEditable ? %d", focusShape, focusShape->isEditable);
				if (focusShape->isEditable) {
					focusShape->isEditing = TRUE;
					my_debug ("pressing button initing control shapes of shape %#", focusShape);
					my_shape_init_control_shapes (focusShape, layer);
				}
			}
			// 重画
			redraw();
			break;
		case MY_ACTION_TYPE_DND:
			my_debug ("=============================== pressing button MY_ACTION_TYPE_DND =======================================");
			// focus当前点中的
			if (focusShape = my_action_manager_get_active_graph_target (actionManager)) {
				if (focusShape->isControlShape && MY_CONTROL_SHAPE_RECT (focusShape)->target) {
					focusShape = MY_CONTROL_SHAPE_RECT (focusShape)->target;
					if (focusShape->isFocusable) {
						focusShape->isFocusing = TRUE;

						g_queue_push_tail (appState->focusedShapes, focusShape);
					}
					my_debug ("focus shape %#x isEditable ? %d", focusShape, focusShape->isEditable);
					if (focusShape->isEditable) {
						focusShape->isEditing = TRUE;
						my_debug ("pressing button initing control shapes of shape %#", focusShape);
						my_shape_init_control_shapes (focusShape, layer);
					}
				}
			}
			// 重画
			redraw();
			break;
		default:
			break;
		}
  } else if (event->button == 2) { // mouse middle button click
  } else if (event->button == 3) { // mouse right button click
  /*
		my_debug ("right button pressed");
		if (event->type == GDK_BUTTON_PRESS) {
			gtk_menu_popup (GTK_MENU (popupMenu),
						NULL, NULL, NULL, NULL,
						event->button,
						event->time);
		}

		rightClickX = event->x;
		rightClickY = event->y;
	*/
  }
  return TRUE;
}


static gboolean		button_release_event	(GtkWidget      *widget,
											GdkEventButton *event ) {

													
	MyShape *shape, *latestShape, *latest2thShape;
	MyBezier *bez;
	MyCurve	*curve;
	MyAction *action;
	MyAction *action2;
	MyPicture *picture;
	MySpline *spline;
	MySplineOverlap *splineOverlap;
	MySplineCyc *splineCyc;
	MyPoint *point;
	guint i, shapeCount;
	
	gdouble x = event->x - appState->orignX;
	gdouble y = event->y - appState->orignY;

	gboolean isRightClickPopMenu = TRUE;
	

	// 更新操作状态
	MyMouse mouseButton;
	if (event->button == 1) {
		mouseButton = MY_MOUSE_LEFT;
	} else if (event->button == 2) {
		mouseButton = MY_MOUSE_MIDDLE;
	} else if (event->button == 3) {
		mouseButton = MY_MOUSE_RIGHT;
	}
	my_action_manager_update (actionManager, x, y, FALSE, actChosen, NULL, mouseButton);

	my_debug ("release button on %f, %f", x, y);

	if (event->button == 1 && appState->pixmap != NULL) {
		last_x = x;
		last_y = y;
	}

	switch (my_action_manager_get_act_type (actionManager)) {
	case MY_ACTION_TYPE_CLICK:
		/*
		latestShape = my_action_manager_get_latest_graph_target (actionManager);
		my_layer_set_select_shape (layer, latestShape);
		if (latestShape == NULL) { // 点击空白处,清空所有焦点
			turn_off_all_shape_focus_edit ();
		}
		// 重画
		redraw();
		*/
		break;
	case MY_ACTION_TYPE_EDIT_CLICK:
		/*
		my_debug ("release button on MY_ACTION_TYPE_EDIT_CLICK");
		latestShape = my_action_manager_get_latest_graph_target (actionManager);
		my_debug ("latestShape %#x", latestShape);
		if (latestShape->isEditable) {
			latestShape->isEditing = TRUE;
			my_shape_init_control_shapes (latestShape, layer);

			g_queue_push_tail (appState->focusedShapes, latestShape);
		}
		// 重画
		redraw();
		*/
		my_debug ("release button on MY_ACTION_TYPE_EDIT_CLICK type");
		// 显示属性
		latestShape = my_action_manager_get_latest_graph_target (actionManager);
		freshPropertiesWidget (latestShape);
		break;
	case MY_ACTION_TYPE_DOUBLE_EDIT_CLICK:
		my_debug ("release button on MY_ACTION_TYPE_DOUBLE_EDIT_CLICK type");
		if (latestShape = my_action_manager_get_latest_graph_target (actionManager)) {
			if (MY_SHAPE_GET_CLASS (latestShape)->double_click_callback) {
				MY_SHAPE_GET_CLASS (latestShape)->double_click_callback (latestShape, x, y, mouseButton, event->state, NULL);
			}
		}
		// 重画
		redraw();
		break;
	case MY_ACTION_TYPE_STRAIGHT_LINE:
		my_debug ("release button on MY_ACTION_TYPE_STRAIGHT_LINE type");
		if (latestShape = my_action_manager_get_latest_graph_target (actionManager)) {
			my_debug ("latestShape %#x", latestShape);
			my_shape_calculate_just_size (latestShape);
			my_shape_shadow_mark_dirty (latestShape);
			my_shape_mark_dirty (latestShape);
			latestShape->isFocusing = TRUE;
			latestShape->isEditing = TRUE;
			switchToSelectFunc ();
			my_shape_init_control_shapes (latestShape, layer);
			g_queue_push_tail (appState->focusedShapes, latestShape);
		}
		// 重画
		redraw();
		// 显示属性
		freshPropertiesWidget (latestShape);
		break;
	case MY_ACTION_TYPE_SPLINE:
		my_debug ("release button on MY_ACTION_TYPE_SPLINE type");
		action = my_action_manager_get_latest_action (actionManager);
		action2 = my_action_manager_get_latest_2th_action (actionManager);
		latest2thShape = my_action_manager_get_latest_2th_graph_target (actionManager);
		if (action && action->phase == MY_ACTION_PHASE_ACTING) {
			if (action2 == NULL || action2->phase != MY_ACTION_PHASE_ACTING || action2->actType != MY_ACTION_TYPE_SPLINE) { // 创建新SPLINE
				
				latestNewShape = spline = g_object_new (MY_TYPE_SPLINE, NULL);
				latestNewShape->appState = appState;
				my_debug ("created new spline %#x", spline);
				latestNewShape->red = 0.;
				latestNewShape->green = 0.;
				latestNewShape->blue = 0.;
				latestNewShape->alpha = 1.;
				//latestNewShape->strokeWidth = 5.;
				my_shape_from_prototype_line (latestNewShape, 
						my_prototype_manager_get_current_prototype (prototypeManager));

				latestNewShape->id = layer->latestId++;
				//MY_SHAPE(curve)->isFocusing = TRUE;
				// 修改action关联的shape
				action->graphTarget = latestNewShape;
				my_layer_add_shape (layer, latestNewShape);
				point = (MyPoint *) g_malloc (sizeof(MyPoint));
				point->x = x;
				point->y = y;
				my_curve_append_point (MY_CURVE (spline), point);
				
				latestNewShape->isFocusing = TRUE;
				latestNewShape->isEditing = TRUE;
				
				g_queue_push_tail (appState->focusedShapes, latestNewShape);
				
			} else {	// 更新spline
				spline = action2->graphTarget;
				action->graphTarget = action2->graphTarget;
				point = (MyPoint *) g_malloc (sizeof(MyPoint));
				point->x = x;
				point->y = y;
				my_curve_append_point (MY_CURVE (spline), point);	

				my_shape_fresh_dirty (MY_SHAPE (spline));
				my_shape_calculate_just_size (MY_SHAPE (spline));
				my_shape_shadow_mark_dirty (MY_SHAPE (spline));
			}
		} else { // 结束spline
			switchToSelectFunc ();
			if (latest2thShape) {
				my_shape_init_control_shapes (latest2thShape, layer);	
				g_queue_push_tail (appState->focusedShapes, latest2thShape);
				isRightClickPopMenu = FALSE;
				// 显示属性
				freshPropertiesWidget (latest2thShape);
			}
		}
		// 重画
		redraw();
		break;		
	case MY_ACTION_TYPE_SPLINE_OVERLAP:
		action = my_action_manager_get_latest_action (actionManager);
		action2 = my_action_manager_get_latest_2th_action (actionManager);
		latest2thShape = my_action_manager_get_latest_2th_graph_target (actionManager);
		if (action && action->phase == MY_ACTION_PHASE_ACTING) {
			if (action2 == NULL || action2->phase != MY_ACTION_PHASE_ACTING || action2->actType != MY_ACTION_TYPE_SPLINE_OVERLAP) { // 创建新SPLINE
				
				latestNewShape = spline = g_object_new (MY_TYPE_SPLINE_OVERLAP, NULL);
				latestNewShape->appState = appState;
				my_debug ("created new spline %#x", spline);
				latestNewShape->red = 0.;
				latestNewShape->green = 0.;
				latestNewShape->blue = 0.;
				latestNewShape->alpha = 1.;
				//latestNewShape->strokeWidth = 5.;
				my_shape_from_prototype_line (latestNewShape, 
						my_prototype_manager_get_current_prototype (prototypeManager));

				latestNewShape->id = layer->latestId++;
				//MY_SHAPE(curve)->isFocusing = TRUE;
				// 修改action关联的shape
				action->graphTarget = latestNewShape;
				my_layer_add_shape (layer, latestNewShape);
				point = (MyPoint *) g_malloc (sizeof(MyPoint));
				point->x = x;
				point->y = y;
				my_curve_append_point (MY_CURVE (spline), point);
				
				latestNewShape->isFocusing = TRUE;
				latestNewShape->isEditing = TRUE;
				
				g_queue_push_tail (appState->focusedShapes, latestNewShape);
				
			} else {	// 更新spline
				spline = action2->graphTarget;
				action->graphTarget = action2->graphTarget;
				point = (MyPoint *) g_malloc (sizeof(MyPoint));
				point->x = x;
				point->y = y;
				my_curve_append_point (MY_CURVE (spline), point);	

				my_shape_fresh_dirty (MY_SHAPE (spline));
				my_shape_calculate_just_size (MY_SHAPE (spline));
				my_shape_shadow_mark_dirty (MY_SHAPE (spline));
			}
		} else { // 结束spline
			switchToSelectFunc ();
			if (latest2thShape) {
				my_shape_init_control_shapes (latest2thShape, layer);	
				g_queue_push_tail (appState->focusedShapes, latest2thShape);
				isRightClickPopMenu = FALSE;
				// 显示属性
				freshPropertiesWidget (latest2thShape);
			}
		}
		// 重画
		redraw();
		break;		
	case MY_ACTION_TYPE_SPLINE_CYC:
		my_debug ("release button on MY_ACTION_TYPE_SPLINE_CYC type");
		action = my_action_manager_get_latest_action (actionManager);
		action2 = my_action_manager_get_latest_2th_action (actionManager);
		latest2thShape = my_action_manager_get_latest_2th_graph_target (actionManager);
		if (action && action->phase == MY_ACTION_PHASE_ACTING) {
			if (action2 == NULL || action2->phase != MY_ACTION_PHASE_ACTING || action2->actType != MY_ACTION_TYPE_SPLINE_CYC ) { // 创建新SPLINE_CYC
				
				latestNewShape = splineCyc = g_object_new (MY_TYPE_SPLINE_CYC, NULL);
				latestNewShape->appState = appState;
				my_debug ("created new splineCyc %#x", splineCyc);
				latestNewShape->red = 0.;
				latestNewShape->green = 0.;
				latestNewShape->blue = 0.;
				latestNewShape->alpha = 1.;
				//latestNewShape->strokeWidth = 5.;
				my_shape_from_prototype_line (latestNewShape, 
						my_prototype_manager_get_current_prototype (prototypeManager));

				latestNewShape->id = layer->latestId++;
				//MY_SHAPE(curve)->isFocusing = TRUE;
				// 修改action关联的shape
				action->graphTarget = latestNewShape;
				my_layer_add_shape (layer, latestNewShape);
				point = (MyPoint *) g_malloc (sizeof(MyPoint));
				point->x = x;
				point->y = y;
				my_curve_append_point (MY_CURVE (splineCyc), point);
				
				latestNewShape->isFocusing = TRUE;
				latestNewShape->isEditing = TRUE;
				
				g_queue_push_tail (appState->focusedShapes, latestNewShape);
				
			} else {	// 更新spline
				splineCyc = action2->graphTarget;
				action->graphTarget = action2->graphTarget;
				point = (MyPoint *) g_malloc (sizeof(MyPoint));
				point->x = x;
				point->y = y;
				my_curve_append_point (MY_CURVE (splineCyc), point);	

				
				my_shape_fresh_dirty (MY_SHAPE (splineCyc));
				my_shape_calculate_just_size (MY_SHAPE (splineCyc));
				my_shape_shadow_mark_dirty (MY_SHAPE (splineCyc));
			}
		} else { // 结束spline cyc
			switchToSelectFunc ();
			if (latest2thShape) {
				my_shape_init_control_shapes (latest2thShape, layer);	
				g_queue_push_tail (appState->focusedShapes, latest2thShape);
				isRightClickPopMenu = FALSE;
				// 显示属性
				freshPropertiesWidget (latest2thShape);
			}
		}
		// 重画
		redraw();
		break;
	case MY_ACTION_TYPE_BEZIER:
		my_debug ("=============================== release button MY_ACTION_TYPE_BEZIER =======================================");
		if (latestShape = my_action_manager_get_latest_graph_target (actionManager)) {
			my_debug ("close bezier %#x isEditable ? %d", latestShape, latestShape->isEditable);
			my_shape_calculate_just_size (latestShape);
			my_shape_shadow_mark_dirty (latestShape);
			my_shape_mark_dirty (latestShape);
			latestShape->isFocusing = TRUE;
			latestShape->isEditing = TRUE;
			switchToSelectFunc ();
			my_shape_init_control_shapes (latestShape, layer);
			g_queue_push_tail (appState->focusedShapes, latestShape);
		}
		// 重画
		redraw();
		// 显示属性
		freshPropertiesWidget (latestShape);
		break;
	case MY_ACTION_TYPE_BEZIER_JM:
	case MY_ACTION_TYPE_BEZIER_SIN:
		my_debug ("release button on MY_ACTION_TYPE_BEZIER type");
		if (latestShape = my_action_manager_get_latest_graph_target (actionManager)) {
			my_debug ("close bezierJM %#x isEditable ? %d", latestShape, latestShape->isEditable);
			my_shape_calculate_just_size (latestShape);
			my_shape_shadow_mark_dirty (latestShape);
			my_shape_mark_dirty (latestShape);
			latestShape->isFocusing = TRUE;
			latestShape->isEditing = TRUE;
			switchToSelectFunc ();
			my_shape_init_control_shapes (latestShape, layer);
			g_queue_push_tail (appState->focusedShapes, latestShape);			
		}
		// 重画
		redraw();
		// 显示属性
		freshPropertiesWidget (latestShape);
		break;
	case MY_ACTION_TYPE_DND:
		my_debug ("release button on MY_ACTION_TYPE_DND type");
		if (latestShape = my_action_manager_get_latest_graph_target (actionManager)) {
			my_debug ("latestShape %#x", latestShape);
			if (MY_IS_CONTROL_SHAPE_RECT (latestShape)) {
				if (MY_CONTROL_SHAPE_RECT (latestShape)->isCanvasControl) {	// 画布控制点
					my_debug ("resizeDrawingArea() in MY_ACTION_TYPE_DND ...");
					resizeDrawingArea ();
					// 刷新所有图形
					for (i = 0, shapeCount = my_layer_shape_count (layer); i < shapeCount; i++) {
						shape = my_layer_peek_nth (layer, i);
						if (MY_SHAPE_GET_CLASS (shape)->fresh_dirty) {
							MY_SHAPE_GET_CLASS (shape)->fresh_dirty (shape);
						}
					}
				}
			} else {
				// 显示属性
				freshPropertiesWidget (latestShape);
			}
		}
		// 重画
		redraw ();
		break;
	case MY_ACTION_TYPE_PICTURE:
		/*
		gdk_draw_pixbuf (appState->pixmap,
					appState->drawingArea->style->white_gc,
					currentPicturePixbuf,
					0,
					0,
					x + appState->orignX,
					y + appState->orignY,
					-1,
					-1,
					GDK_RGB_DITHER_NORMAL, 0, 0);
		*/
		if (currentPrototypePicture.absPath) {
			picture = insertPicture_by_file_path (currentPrototypePicture.absPath, NULL);
			my_layer_add_shape (layer, MY_SHAPE (picture));
			picture->x = x;
			picture->y = y;
			my_shape_calculate_just_size (MY_SHAPE (picture));
			my_shape_shadow_mark_dirty (MY_SHAPE (picture));
			my_shape_mark_dirty (MY_SHAPE (picture));
			MY_SHAPE (picture)->isFocusing = TRUE;
			MY_SHAPE (picture)->isEditing = TRUE;
			switchToSelectFunc ();
			my_shape_init_control_shapes (MY_SHAPE (picture), layer);
			g_queue_push_tail (appState->focusedShapes, picture);
			// 修改action关联的shape
			action = my_action_manager_get_latest_action (actionManager);
			action->graphTarget = MY_SHAPE (picture);
			// 重画
			redraw ();
			// 显示属性
			freshPropertiesWidget (MY_SHAPE (picture));
		}
		break;
	default:
		my_debug ("default type when button released");
	}
	
	if (event->button == 3) { // mouse right button click
		if (isRightClickPopMenu) {
			gtk_menu_popup (GTK_MENU (popupMenu),
						NULL, NULL, NULL, NULL,
						0,
						event->time);
		}

		rightClickX = event->x;
		rightClickY = event->y;
  }
  
  latestNewShape = NULL; // 结束画曲线
  currentDndShape = NULL;
  return TRUE;
}

/* performance tune try
int fps = 50;
clock_t lastClock = 0; // 实现最大fps
*/

static gboolean		motion_notify_event (GtkWidget *widget,
										GdkEventMotion *event) {

  int ix, iy;
  gdouble x, y;
  MyPoint *point, jmPoint;
  GdkModifierType state;
  MyShape *targetShape, *realTargetShape, *hitShape;
  MyAction *activeAction;
  GdkGC *shapeGc;
  MyCurve *curve;
  MyBezier *bez;
  MyAction *action;
	gchar text[50];
  guint contextId;
  
	MyMouse mouseButton;


	my_debug ("motion_notify_event ...");

	/* performance tune try
	if (lastClock == 0) {
		lastClock = clock();
	} else {
		if ( 10 * (clock() - lastClock) < CLOCKS_PER_SEC ) {
		//if ( (clock() - *lastClock) / CLOCKS_PER_SEC < 1 / fps ) {
			my_debug ("beyond fps");
			return TRUE;
		}
	}
	*/
	
  if (event->is_hint)
    gdk_window_get_pointer (event->window, &ix, &iy, &state);
  else
    {
      ix = event->x;
      iy = event->y;
      state = (GdkModifierType) event->state;
    }
	x = ix - appState->orignX;
	y = iy - appState->orignY;
	
	if (state & GDK_BUTTON1_MASK) {
		mouseButton = MY_MOUSE_LEFT;
	} else if (state & GDK_BUTTON2_MASK) {
		mouseButton = MY_MOUSE_MIDDLE;
	} else if (state & GDK_BUTTON3_MASK) {
		mouseButton = MY_MOUSE_RIGHT;
	}

  if (!(state & GDK_BUTTON1_MASK)) { // 不按鼠标移动时,才更新状态栏

	// 显示坐标,鼠标图形改变

	if ((hitShape = my_layer_hit_shape (layer, x, y, appState->hitWidthRadius, appState))) {
		sprintf (text, "%.2f,%.2f hit %#x", x, y, hitShape);
		if (hitShape->isControlShape) {
			gdk_window_set_cursor (event->window, appState->cursorMoveControlShape);
		} else {
			if (actChosen == MY_ACTION_FUNC_SELECT) {
				gdk_window_set_cursor (event->window, appState->cursorMoveNormalShape);
			}
		}
	} else {
		sprintf (text, "%.2f,%.2f", x, y);
		gdk_window_set_cursor (event->window, NULL);
	}
	contextId = gtk_statusbar_get_context_id (GTK_STATUSBAR (appState->statusBar), "mouse_position");
	//gtk_statusbar_pop  (appState->statusBar, contextId);
	if (statusBarMsgStackCount ++ > 1000) {
		gtk_statusbar_remove_all (GTK_STATUSBAR (appState->statusBar), contextId);
	}
	gtk_statusbar_push (GTK_STATUSBAR (appState->statusBar), contextId, text);
  }
  
  if (state & GDK_BUTTON1_MASK && appState->pixmap != NULL) { // 按下鼠标同时移动

	// 更新操作状态
	my_action_manager_update (actionManager, x, y, TRUE, actChosen, NULL, MY_MOUSE_LEFT);

	switch (my_action_manager_get_act_type (actionManager)) {
	case MY_ACTION_TYPE_STRAIGHT_LINE:
		if (latestNewShape == NULL) { // 创建新的直线
			latestNewShape = curve = g_object_new (MY_TYPE_CURVE, NULL);
			MY_SHAPE(curve)->appState = appState;
			my_debug ("created new straight line %#x", curve);
			latestNewShape->red = 0.;
			latestNewShape->green = 0.;
			latestNewShape->blue = 0.;
			latestNewShape->alpha = 1.;
			//latestNewShape->strokeWidth = 5.;
//			latestNewShape->shadowPadding = latestNewShape->boxRadius + 0.707 * latestNewShape->strokeWidth;
			my_shape_from_prototype_line (latestNewShape, 
					my_prototype_manager_get_current_prototype (prototypeManager));

			MY_SHAPE(curve)->id = layer->latestId++;
			//MY_SHAPE(curve)->isFocusing = TRUE;
			// 修改action关联的shape
			action = my_action_manager_get_active_action (actionManager);
			/*
			if (action->graphTarget) {
				action->graphTarget->isFocusing = FALSE;
			}
			*/
			action->graphTarget = MY_SHAPE (curve);
			my_layer_add_shape (layer, MY_SHAPE(curve));
			point = (MyPoint *) g_malloc (sizeof(MyPoint));	//	start point of segment
			point->x = action->startPoint->x;
			point->y = action->startPoint->y;
			my_curve_append_point (curve, point);
			point = (MyPoint *) g_malloc (sizeof(MyPoint));				//	end point of segment
			point->x = x;
			point->y = y;
			my_curve_append_point (curve, point);
			my_shape_calculate_just_size (latestNewShape);
		} else { // 改变直线的端点
			my_curve_update_last_point ( MY_CURVE (latestNewShape), x, y);
			my_shape_calculate_just_size (latestNewShape);
		}
		my_shape_shadow_mark_dirty (latestNewShape);
		my_shape_mark_dirty (latestNewShape);
		// 重画
		redrawOnOdd ();
		break;
	case MY_ACTION_TYPE_BEZIER:
		my_debug ("=============================== motion on MY_ACTION_TYPE_BEZIER =======================================");
		if (latestNewShape == NULL) { // 创建新的贝塞尔
			action = my_action_manager_get_active_action (actionManager);
			bez = (MyBezier *)g_object_new (MY_TYPE_BEZIER, NULL);
			latestNewShape = MY_SHAPE (bez);
			my_bezier_new_with_coordinary (bez,
											layer->latestId++,
											appState,
											action->startPoint->x,
											action->startPoint->y,
											x,
											y,
											appState->scale);
			my_shape_from_prototype_line (latestNewShape, 
					my_prototype_manager_get_current_prototype (prototypeManager));

			// 修改action关联的shape
			action->graphTarget = MY_SHAPE (bez);
			my_layer_add_shape (layer, MY_SHAPE(bez));

		} else { // 改变端点
			bez = MY_BEZIER (latestNewShape);
			my_bezier_update_point (bez, 3, x, y);
			my_bezier_update_point (bez, 1, 0.75 * bez->p0.x + 0.25 * bez->p3.x, 
											0.75 * bez->p0.y + 0.25 * bez->p3.y);
			my_bezier_update_point (bez, 2, 0.25 * bez->p0.x + 0.75 * bez->p3.x, 
											0.25 * bez->p0.y + 0.75 * bez->p3.y);
		}
		my_shape_calculate_just_size (latestNewShape);
		my_shape_shadow_mark_dirty (latestNewShape);
		my_shape_mark_dirty (latestNewShape);
		// 重画
		redrawOnOdd ();
		break;
	case MY_ACTION_TYPE_BEZIER_JM:
		if (latestNewShape == NULL) { // 创建新的贝塞尔尖灭线
			action = my_action_manager_get_active_action (actionManager);
			latestNewShape = bez = g_object_new (MY_TYPE_BEZIER_JM, NULL);
			my_bezier_jm_new_with_coordinary (MY_BEZIER_JM(bez),
											layer->latestId++,
											appState,
											action->startPoint->x,
											action->startPoint->y,
											x,
											y,
											appState->scale);
			my_shape_from_prototype_line (latestNewShape, 
					my_prototype_manager_get_current_prototype (prototypeManager));

			// 修改action关联的shape
			action->graphTarget = MY_SHAPE (bez);
			my_layer_add_shape (layer, MY_SHAPE(bez));
		} else { // 改变端点
			bez = MY_BEZIER (latestNewShape);
			my_bezier_update_point (bez, 3, x, y);
			my_bezier_update_point (bez, 1, 0.75 * bez->p0.x + 0.25 * bez->p3.x, 
											0.75 * bez->p0.y + 0.25 * bez->p3.y);
			my_bezier_update_point (bez, 2, 0.25 * bez->p0.x + 0.75 * bez->p3.x, 
											0.25 * bez->p0.y + 0.75 * bez->p3.y);
		}
		my_shape_calculate_just_size (latestNewShape);
		my_shape_shadow_mark_dirty (latestNewShape);
		my_shape_mark_dirty (latestNewShape);
		// 重画
		redrawOnOdd ();
		break;
	case MY_ACTION_TYPE_BEZIER_SIN:
		my_debug ("=============================== motion on MY_ACTION_TYPE_BEZIER_SIN =======================================");
		if (latestNewShape == NULL) { // 创建新的贝塞尔正弦线
			action = my_action_manager_get_active_action (actionManager);
			bez = (MyBezier *) g_object_new (MY_TYPE_BEZIER_SIN, NULL);
			latestNewShape = MY_SHAPE (bez);
			my_bezier_sin_new_with_coordinary (MY_BEZIER_SIN(bez),
											layer->latestId++,
											appState,
											action->startPoint->x,
											action->startPoint->y,
											x,
											y,
											appState->scale);
			my_shape_from_prototype_line (latestNewShape, 
					my_prototype_manager_get_current_prototype (prototypeManager));

			// 修改action关联的shape
			action->graphTarget = MY_SHAPE (bez);
			my_layer_add_shape (layer, MY_SHAPE(bez));
		} else { // 改变端点
			bez = MY_BEZIER (latestNewShape);
			my_bezier_update_point (bez, 3, x, y);
			my_bezier_update_point (bez, 1, 0.75 * bez->p0.x + 0.25 * bez->p3.x, 
											0.75 * bez->p0.y + 0.25 * bez->p3.y);
			my_bezier_update_point (bez, 2, 0.25 * bez->p0.x + 0.75 * bez->p3.x, 
											0.25 * bez->p0.y + 0.75 * bez->p3.y);
		}
		my_shape_calculate_just_size (latestNewShape);
		my_shape_shadow_mark_dirty (latestNewShape);
		my_shape_mark_dirty (latestNewShape);
		// 重画
		redrawOnOdd ();
		break;
	case MY_ACTION_TYPE_DND:
		my_debug ("=============================== motion on MY_ACTION_TYPE_DND =======================================");
		// 移动图形
		targetShape = my_action_manager_get_active_graph_target (actionManager);
		my_debug ("dnd shift shape %#x delta (%f, %f)", targetShape, x - last_x, y - last_y);

		// 画图fps优化 performance tune try
		/*
		gboolean shouldDraw;
		gdouble minIntervalDraw = 0.1; // second,两次draw间最短间隔时间
//		minIntervalDraw = 1. / 30;
		assert (targetShape);
		if (!currentDndShape) {	// 标志着开始拖动图形
			currentDndShape = targetShape;
			if (timer)
				g_timer_destroy (timer);
			timer = g_timer_new ();
			shouldDraw = TRUE;
			
		} else {	// 继续拖动图形
			gdouble elapsed = g_timer_elapsed (timer, NULL);
			if (elapsed >= minIntervalDraw) {
				g_timer_start(timer);
				shouldDraw = TRUE;
//				printf ("shouldDraw true\n");
			} else {
				shouldDraw = FALSE;
//				printf ("shouldDraw false\n");
				break;
			}
		}
		*/		
		/*
		gboolean shouldDraw;
		guint32 minIntervalDraw = 100; // second,两次draw间最短间隔时间
		assert (targetShape);
		if (!currentDndShape) {	// 标志着开始拖动图形
			currentDndShape = targetShape;
			drawTimestamp = event->time;
			shouldDraw = TRUE;
			
		} else {	// 继续拖动图形			
			if (event->time - drawTimestamp >= minIntervalDraw) {
				drawTimestamp = event->time;
				shouldDraw = TRUE;
			} else {
				shouldDraw = FALSE;
			}
		}
		if (shouldDraw) {
//			printf ("event->time: %u, type:%d, x,y:%f,%f, drwa!\n", event->time, event->type, event->x, event->y);
		} else {
//			printf ("event->time: %u, type:%d, x,y:%f,%f\n", event->time, event->type, event->x, event->y);
			break;
		}
		 */
		

		if (targetShape->isUseShiftTo)
			my_shape_shift_to (targetShape, targetShape, x, y, mouseButton, event->state, NULL);
		else
			my_shape_shift (targetShape, targetShape, x - last_x, y - last_y, mouseButton, event->state, NULL);

		if (MY_IS_CONTROL_SHAPE_RECT (targetShape)) { // 拖动控制图形
			if (MY_CONTROL_SHAPE_RECT (targetShape)->isCanvasControl) {// 拖动画布control rect
				my_debug ("dnd shift canvas control");
				// 重画
				redrawOnOdd();
//				if (shouldDraw)
//					redraw ();
			} else { // 拖动普通图形的control rect
				realTargetShape = MY_CONTROL_SHAPE_RECT (targetShape)->target;
				if (realTargetShape && realTargetShape->isFocusing == FALSE) { //下面focus 控制图形的目标图形
					realTargetShape->isEditing = TRUE;
					realTargetShape->isFocusing = TRUE;

					g_queue_push_tail (appState->focusedShapes, MY_CONTROL_SHAPE_RECT (targetShape)->target);
				}
				my_shape_calculate_just_size (realTargetShape);
				my_shape_shadow_mark_dirty (realTargetShape);
				// 重画
				redrawOnOdd();
//				if (shouldDraw)
//					redraw ();			
			}
		} else {	// 拖动普通图形
			if (targetShape->needMarkDirtyWhenShift) {
				my_shape_mark_dirty (targetShape);
			}
			if (targetShape->isFocusing == FALSE) {	// 下面focus 普通图形
				targetShape->isFocusing = TRUE;
				if (targetShape->isEditable)
					targetShape->isEditing = TRUE;

				g_queue_push_tail (appState->focusedShapes, targetShape);
			}
			// 重画
			redrawOnOdd();
//			if (shouldDraw)
//				redraw ();
		}
		last_x = x;
		last_y = y;
		break;
		/*
	case MY_ACTION_TYPE_EDIT_DND:
		my_debug ("press moving mouse on MY_ACTION_TYPE_EDIT_DND");
		// 移动图形
		targetShape = my_action_manager_get_active_graph_target (actionManager);
		my_debug ("dnd shift %f, %f", x - last_x, y - last_y);
		my_shape_shift (targetShape, x - last_x, y - last_y);
		// 重画
		redraw();
		last_x = x;
		last_y = y;

		break;
		*/
	default:
		my_debug ("default type");
		break;
	}
  }
  return TRUE;
}

void quit (){
  exit (0);
}

static void		save_layer	(gpointer data,
							gpointer user_data) {

	guint i, shapeCount;
	char idBuf[50] = {0};
	MyLayer *layer = (MyLayer *) data;
	MyShape *shape;
	GIOChannel *outChannel = (GIOChannel *) user_data;
	gchar *finalStr;
	gsize stringLen;
	GString * gstring;
	gsize writtenSize;

	sprintf (idBuf, "%d", layer->id);
	g_io_channel_write_chars (outChannel, "<layer id=\"", -1, &writtenSize, NULL);
	g_io_channel_write_chars (outChannel, idBuf, -1, &writtenSize, NULL);
	g_io_channel_write_chars (outChannel, "\">\n", -1, &writtenSize, NULL);

	for (i = 0, shapeCount = my_layer_shape_count (layer); i < shapeCount; i++) {
		shape = my_layer_peek_nth (layer, i);
		my_debug ("shape %#x", shape);
		gstring = my_shape_to_string (shape);
		if (gstring) {
			// do flush
			stringLen = gstring->len;
			finalStr = g_string_free (gstring, FALSE);
			g_io_channel_write_chars (outChannel, finalStr, stringLen, &writtenSize, NULL);
			g_io_channel_write_chars (outChannel, "\n", -1, &writtenSize, NULL);
			my_debug("flushed curve to disk length: %d\n", stringLen);
			if (finalStr)
				g_free(finalStr);
		}
	}

	g_io_channel_write_chars (outChannel, "\n</layer>", -1, &writtenSize, NULL);
}
/**
 * 保存
 */
gint save_callback( GtkWidget *widget,
                    GdkEvent  *event,
                    gpointer   callback_data ){

	GIOStatus status; // 1 is success
	GIOChannel *outChannel = g_io_channel_new_file ("pptmodel.xml",
                       "w",
                       NULL);
	gsize writtenSize;

	g_io_channel_write_chars (outChannel, "<canvas>\n", -1, &writtenSize, NULL);
	//my_canvas_foreach_layer (appState->canvas, save_layer, outChannel);
	g_io_channel_write_chars (outChannel, "\n</canvas>", -1, &writtenSize, NULL);

	if (outChannel)
		g_io_channel_unref (outChannel);
	return TRUE;
}
/**
 * 导出图片
 * @Deprecated
 **/
gint export_callback( GtkWidget *widget,
                    GdkEvent  *event,
                    gpointer   callback_data ){

	GdkPixbuf * pixbuf;
	GError *error;
	gboolean saved;

	pixbuf = gdk_pixbuf_get_from_drawable (NULL,
                              appState->pixmap,
                              gdk_colormap_get_system (),
                              appState->canvasX + 1, // 不包含边框 //0,
                              appState->canvasY + 1,//0,
                              0,
                              0,
                              appState->canvasWidth - 1,//-1,
                              appState->canvasHeight - 1//-1
							  );
	if (pixbuf == NULL){
	  quit();
	}
	//gdk_pixbuf_save (pixbuf, "screenshot.jpg", "jpeg", NULL, "quality", "100",NULL);
	gdk_pixbuf_save (pixbuf, "screenshot.png", "png", NULL, NULL);
	return TRUE;
}

static void		turn_off_all_shape_focus_edit	(void) {

	guint shapeCount, j;
	MyShape *shape;
	my_debug ("turn off all shape focus and edit start...");
	for (j = 0, shapeCount = my_layer_shape_count (layer); j < shapeCount; j++) {
		shape = my_layer_peek_nth (layer, j);
		my_debug ("shape %#x isEditable ? %d", shape, shape->isEditable);
		if (shape->isFocusable)
			shape->isFocusing = FALSE;
		if (shape->isEditable)
			shape->isEditing = FALSE;
		if (MY_IS_CONTROL_SHAPE_RECT (shape) && MY_CONTROL_SHAPE_RECT (shape)->isCanvasControl == FALSE)
			shape->isShowing = FALSE;
		my_debug ("shape %#x isEditable ? %d", shape, shape->isEditable);
	}
	my_debug ("turn off all shape focus and edit done.");
}
/**
 * 切换到线条
 */
static void 	line_shapes_callback	(GtkButton *widget,
										gpointer   user_data){

	actChosen = actChosenLine;
	
	gtk_widget_hide (toolbar_prototype_picture);
	gtk_widget_show (toolbar_prototype_line);
}
/**
 * 切换到图片
 */
static void 	picture_callback	(GtkButton *widget,
									gpointer   user_data){

	actChosen = MY_ACTION_FUNC_PICTURE;
	
	gtk_widget_hide (toolbar_prototype_line);
	gtk_widget_show (toolbar_prototype_picture);
}
/**
 * 准备画曲线
 */
gint ready_curve_callback( GtkWidget *widget,
                    GdkEvent  *event,
                    gpointer   callback_data ){

	MyShape *latestShape;
	if ( gtk_toggle_tool_button_get_active (GTK_TOGGLE_TOOL_BUTTON(widget))) {
		my_debug ("curve callback");
		actChosen = MY_ACTION_FUNC_CURVE;
		my_debug ("actChosen: %d", MY_ACTION_FUNC_CURVE);
		turn_off_all_shape_focus_edit ();
		//my_layer_delete_all_control_shapes	(my_canvas_get_selected_layer (appState->canvas));
		if (latestShape = my_action_manager_get_latest_graph_target (actionManager)) {
			latestShape->isFocusing = FALSE;
		}
		redraw();
	}
	return TRUE;
}
/**
 * 准备画尖灭线
 */
gint ready_bezier_jm_callback( GtkWidget *widget,
                    GdkEvent  *event,
                    gpointer   callback_data ){

	MyShape *latestShape;
	if ( gtk_toggle_tool_button_get_active (GTK_TOGGLE_TOOL_BUTTON(widget))) {
		my_debug ("ready_bezier_jm_callback");
		actChosen = MY_ACTION_FUNC_BEZIER_JM;
		my_debug ("actChosen: %d", actChosen);
		turn_off_all_shape_focus_edit ();
		//my_layer_delete_all_control_shapes	(my_canvas_get_selected_layer (appState->canvas));
		if (latestShape = my_action_manager_get_latest_graph_target (actionManager)) {
			latestShape->isFocusing = FALSE;
		}
		redraw();
	}
	return TRUE;
}
/**
 * 准备画不整合线
 */
gint ready_bezier_sin_callback( GtkWidget *widget,
                    GdkEvent  *event,
                    gpointer   callback_data ){

	MyShape *latestShape;
	if ( gtk_toggle_tool_button_get_active (GTK_TOGGLE_TOOL_BUTTON(widget))) {
		my_debug ("ready_bezier_sin_callback");
		actChosen = MY_ACTION_FUNC_BEZIER_SIN;
		my_debug ("actChosen: %d", actChosen);
		turn_off_all_shape_focus_edit ();
		//my_layer_delete_all_control_shapes	(my_canvas_get_selected_layer (appState->canvas));
		if (latestShape = my_action_manager_get_latest_graph_target (actionManager)) {
			latestShape->isFocusing = FALSE;
		}
		redraw();
	}
	return TRUE;
}
/**
 * 准备画贝塞尔曲线
 */
gint ready_bezier_callback	( GtkWidget *widget,
                    GdkEvent  *event,
                    gpointer   callback_data ){

	MyShape *latestShape;
	if ( gtk_toggle_tool_button_get_active (GTK_TOGGLE_TOOL_BUTTON(widget))) {
		my_debug ("ready_bezier_callback");
		actChosen = MY_ACTION_FUNC_BEZIER;
		my_debug ("actChosen: %d", actChosen);
		turn_off_all_shape_focus_edit ();
		//my_layer_delete_all_control_shapes	(my_canvas_get_selected_layer (appState->canvas));
		if (latestShape = my_action_manager_get_latest_graph_target (actionManager)) {
			latestShape->isFocusing = FALSE;
		}
		redraw();
	}
	return TRUE;
}



static void 	tool_item_picture_clicked_callback	(GtkButton *widget,
													gpointer   user_data){
													
	actChosen = MY_ACTION_FUNC_PICTURE;
	currentPrototypePicture = *((PrototypePicture *) user_data);
	
	gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON(btnPicture), TRUE);
}

													
static void		icon_view_picture_item_activated_callback	(GtkIconView *icon_view,
															GtkTreePath *tree_path,
															gpointer     user_data) {
  GtkListStore *store;
  GtkTreeIter iter;
  GdkPixbuf *pixbuf;
  GdkPixbuf *scaledPixbuf;
  GtkWidget *image;

	store = GTK_LIST_STORE (user_data);
	gtk_tree_model_get_iter (GTK_TREE_MODEL (store), &iter, tree_path);
	gtk_tree_model_get (GTK_TREE_MODEL (store), &iter,
							COL_PICTURE_PATH, &prototypePicture0.absPath,
					  COL_PICTURE_PIXBUF, &pixbuf,
					  -1);
//	add_custom_icon_with_name_from_pixbuf ("custom_auto_picture_0", -1, pixbuf);
//	prototypePicture0.iconName = "custom_auto_picture_0";
	prototypePicture0.pixbuf = pixbuf;
	scaledPixbuf = gdk_pixbuf_scale_simple (pixbuf,
											80,
											80,
											GDK_INTERP_BILINEAR);
	prototypePicture0.thumbnailWidget = gtk_image_new_from_pixbuf (scaledPixbuf);
//	gtk_button_set_image (theButton, gtk_image_new_from_pixbuf (scaledPixbuf));
	
	if (gtk_tree_model_iter_next (GTK_TREE_MODEL (store), &iter)) {
		gtk_tree_model_get (GTK_TREE_MODEL (store), &iter,
							COL_PICTURE_PATH, &prototypePicture1.absPath,
						  COL_PICTURE_PIXBUF, &pixbuf,
						  -1);
		prototypePicture1.pixbuf = pixbuf;
		scaledPixbuf = gdk_pixbuf_scale_simple (pixbuf,
												80,
												80,
												GDK_INTERP_BILINEAR);
		prototypePicture1.thumbnailWidget = gtk_image_new_from_pixbuf (scaledPixbuf);
	}	
	if (gtk_tree_model_iter_next (GTK_TREE_MODEL (store), &iter)) {
		gtk_tree_model_get (GTK_TREE_MODEL (store), &iter,
							COL_PICTURE_PATH, &prototypePicture2.absPath,
						  COL_PICTURE_PIXBUF, &pixbuf,
						  -1);
		prototypePicture2.pixbuf = pixbuf;
		scaledPixbuf = gdk_pixbuf_scale_simple (pixbuf,
												80,
												80,
												GDK_INTERP_BILINEAR);
		prototypePicture2.thumbnailWidget = gtk_image_new_from_pixbuf (scaledPixbuf);
	}
	if (gtk_tree_model_iter_next (GTK_TREE_MODEL (store), &iter)) {
		gtk_tree_model_get (GTK_TREE_MODEL (store), &iter,
							COL_PICTURE_PATH, &prototypePicture3.absPath,
						  COL_PICTURE_PIXBUF, &pixbuf,
						  -1);
		prototypePicture3.pixbuf = pixbuf;
		scaledPixbuf = gdk_pixbuf_scale_simple (pixbuf,
												80,
												80,
												GDK_INTERP_BILINEAR);
		prototypePicture3.thumbnailWidget = gtk_image_new_from_pixbuf (scaledPixbuf);
	}
	gtk_dialog_response (GTK_DIALOG (dialogMorePicture), GTK_RESPONSE_OK);
}


static void		icon_view_line_item_activated_callback	(GtkIconView *icon_view,
															GtkTreePath *tree_path,
															gpointer     user_data) {
  GtkListStore *store;
  GtkTreeIter iter;
  GdkPixbuf *pixbuf;

	store = GTK_LIST_STORE (user_data);
	gtk_tree_model_get_iter (GTK_TREE_MODEL (store), &iter, tree_path);
	gtk_tree_model_get (GTK_TREE_MODEL (store), &iter,
						COL_LINE_CATEGORY, &chosenPrototypeCategory,
						COL_LINE_INDEX, &chosenPrototypeNth,
						COL_LINE_PIXBUF, &pixbuf,
						-1);
	gtk_dialog_response (GTK_DIALOG (dialogMoreLine), GTK_RESPONSE_OK);
}


/**
 * 更多线条
 */
static void 	btnMoreLine_clicked_callback	(GtkButton *widget,
												gpointer   user_data){
													
	gint contentWidth, contentHeight;
	gint cellWidth, cellHeight;
	GtkWidget *swin;
	GtkWidget *vboxInScroll;
	GtkWidget *labelGeneral;
	GtkWidget *dialog = gtk_dialog_new_with_buttons ("My dialog xx",
                                                 NULL,
                                                 GTK_DIALOG_MODAL,
                                                 NULL);
	
	GtkWidget *icon_view; 
	GtkListStore *store;
	GtkTreeIter iter;
	GdkPixbuf *pixbuf;
	GdkPixmap *imagePixmap;
	MyPrototypeLine* prototype;
	guint len;
	guint i;
	GtkWidget *labelFast;
	gint result;
	GdkPixmap *imagePixmap0;
	GdkPixmap *imagePixmap1;
	GdkPixmap *imagePixmap2;
	GdkPixmap *imagePixmap3;


	dialogMoreLine = dialog;
	cellWidth = 80;
	cellHeight = 60;
	gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
	gtk_window_set_decorated(GTK_WINDOW(dialog),FALSE);	
	gtk_window_move (GTK_WINDOW(dialog), 665, 80);
	
	swin = gtk_scrolled_window_new (NULL, NULL);
	gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (dialog)->vbox), swin);
	gtk_container_set_border_width (GTK_CONTAINER (swin), 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swin),	GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	contentWidth = 440;
	contentHeight = 400;
	gtk_widget_set_size_request (GTK_WIDGET (swin), contentWidth, contentHeight);
	
	vboxInScroll = gtk_vbox_new (FALSE, 0);
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (swin), vboxInScroll);
	
	labelGeneral = gtk_label_new (chinese ("通用箭头"));	
	gtk_box_pack_start(GTK_BOX(vboxInScroll), labelGeneral, FALSE, FALSE, 0);
	

	store = gtk_list_store_new (NUM_LINE_COLS,
								G_TYPE_UINT,
								G_TYPE_UINT,
								GDK_TYPE_PIXBUF);
	len = my_prototype_manager_get_length (prototypeManager, MY_PROTOTYPE_CATEGORY_STANDARD);
	i = 0;
	for (; i < len; i++) {
		prototype = my_prototype_manager_peek_nth (prototypeManager, 
													MY_PROTOTYPE_CATEGORY_STANDARD,
													i);
		imagePixmap = gdk_pixmap_new (GTK_WIDGET (widget)->window,//ULL,
						   80,
						   60,
						   -1);//24);
		my_prototype_view_draw_pixmap (linePrototypeView, 
										prototype,
										imagePixmap,
										80,
										60);
		pixbuf = gdk_pixbuf_get_from_drawable (NULL,
												imagePixmap,
												NULL,//gdk_colormap_get_system (),
												0,
												0,
												0,
												0,
												-1,
												-1);
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter,
							COL_LINE_CATEGORY, MY_PROTOTYPE_CATEGORY_STANDARD,
							COL_LINE_INDEX, i,
							COL_LINE_PIXBUF, pixbuf,
							-1);
		g_object_unref (imagePixmap);
	}	
	icon_view = gtk_icon_view_new_with_model (GTK_TREE_MODEL (store));	
	gtk_box_pack_start(GTK_BOX(vboxInScroll), icon_view, TRUE, TRUE, 0);
	gtk_icon_view_set_selection_mode (GTK_ICON_VIEW (icon_view), GTK_SELECTION_MULTIPLE);
//	gtk_icon_view_set_text_column (GTK_ICON_VIEW (icon_view), COL_PICTURE_DISPLAY_NAME);
	gtk_icon_view_set_pixbuf_column (GTK_ICON_VIEW (icon_view), COL_LINE_PIXBUF);
//	gtk_widget_set_size_request (GTK_WIDGET (icon_view), 400, 200);
	g_signal_connect (icon_view, "item-activated", 
					G_CALLBACK (icon_view_line_item_activated_callback), 
					store);
	g_object_unref (store);
	
	
	labelFast = gtk_label_new (chinese ("快速样式"));	
	gtk_box_pack_start(GTK_BOX(vboxInScroll), labelFast, FALSE, FALSE, 0);
	
	
	store = gtk_list_store_new (NUM_LINE_COLS,
								G_TYPE_UINT,
								G_TYPE_UINT,
								GDK_TYPE_PIXBUF);
	len = my_prototype_manager_get_length (prototypeManager, MY_PROTOTYPE_CATEGORY_FAST);
	i = 0;
	for (; i < len; i++) {
		prototype = my_prototype_manager_peek_nth (prototypeManager, 
													MY_PROTOTYPE_CATEGORY_FAST,
													i);
		imagePixmap = gdk_pixmap_new (GTK_WIDGET (widget)->window,//ULL,
						   cellWidth,
						   cellHeight,
						   -1);//24);
		my_prototype_view_draw_pixmap (linePrototypeView, 
										prototype,
										imagePixmap,
										cellWidth,
										cellHeight);
		pixbuf = gdk_pixbuf_get_from_drawable (NULL,
												imagePixmap,
												NULL,//gdk_colormap_get_system (),
												0,
												0,
												0,
												0,
												-1,
												-1);
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter,
							COL_LINE_CATEGORY, MY_PROTOTYPE_CATEGORY_FAST,
							COL_LINE_INDEX, i,
							COL_LINE_PIXBUF, pixbuf,
							-1);
		g_object_unref (imagePixmap);
	}
	icon_view = gtk_icon_view_new_with_model (GTK_TREE_MODEL (store));
	gtk_box_pack_start(GTK_BOX(vboxInScroll), icon_view, TRUE, TRUE, 0);
	gtk_icon_view_set_selection_mode (GTK_ICON_VIEW (icon_view), GTK_SELECTION_MULTIPLE);
	gtk_icon_view_set_pixbuf_column (GTK_ICON_VIEW (icon_view), COL_LINE_PIXBUF);
	g_signal_connect (icon_view, "item-activated", 
					G_CALLBACK (icon_view_line_item_activated_callback), 
					store);
	g_object_unref (store);	
	
	gtk_widget_show_all (dialog);
	
	result = gtk_dialog_run (GTK_DIALOG (dialog));
	if (result == GTK_RESPONSE_OK) {
		
		gtk_container_remove (GTK_CONTAINER (toolbar_prototype_line), GTK_WIDGET (toolitem_line_0));
		gtk_container_remove (GTK_CONTAINER (toolbar_prototype_line), GTK_WIDGET (toolitem_line_1));
		gtk_container_remove (GTK_CONTAINER (toolbar_prototype_line), GTK_WIDGET (toolitem_line_2));
		gtk_container_remove (GTK_CONTAINER (toolbar_prototype_line), GTK_WIDGET (toolitem_line_3));
	
		if (toolitem_line) {
			g_object_unref (toolitem_line);
		}
		toolitem_line = (GtkWidget *)gtk_radio_tool_button_new (NULL);
		
		toolitem_line_0 = gtk_radio_tool_button_new_from_widget (GTK_RADIO_TOOL_BUTTON(toolitem_line));
//		toolitem_line_0 = gtk_radio_tool_button_new (NULL);
		gtk_toolbar_insert (GTK_TOOLBAR(toolbar_prototype_line), toolitem_line_0, 0);
		gtk_widget_set_size_request (GTK_WIDGET (toolitem_line_0), 100, 100);
		imagePixmap0 = gdk_pixmap_new (NULL,
												   cellWidth,
												   cellHeight,
												   24);
		linePrototype0 = my_prototype_manager_peek_nth (prototypeManager, 
														chosenPrototypeCategory,//MY_PROTOTYPE_CATEGORY_STANDARD,
														chosenPrototypeNth / 4 * 4);
		if (linePrototype0) {
			my_prototype_view_draw_pixmap (linePrototypeView, 
											linePrototype0,
											imagePixmap0,
											cellWidth,
											cellHeight);
			gtk_tool_button_set_label_widget (GTK_TOOL_BUTTON (toolitem_line_0), 
												gtk_image_new_from_pixmap (imagePixmap0, NULL));
			gtk_widget_show_all (GTK_WIDGET (toolitem_line_0));
		}
		g_signal_connect (toolitem_line_0, "clicked", 
						G_CALLBACK(toolitem_line_clicked_callback), 
						linePrototype0);


		toolitem_line_1 = gtk_radio_tool_button_new_from_widget (GTK_RADIO_TOOL_BUTTON(toolitem_line));
		gtk_toolbar_insert (GTK_TOOLBAR(toolbar_prototype_line), toolitem_line_1, 1);
		gtk_widget_set_size_request (GTK_WIDGET (toolitem_line_1), 100, 100);
		imagePixmap1 = gdk_pixmap_new (NULL,
												   cellWidth,
												   cellHeight,
												   24);
		linePrototype1 = my_prototype_manager_peek_nth (prototypeManager, 
														chosenPrototypeCategory,//MY_PROTOTYPE_CATEGORY_STANDARD,
														chosenPrototypeNth / 4 * 4+ 1);
		if (linePrototype1) {
			my_prototype_view_draw_pixmap (linePrototypeView, 
											linePrototype1,
											imagePixmap1,
											cellWidth,
											cellHeight);
			gtk_tool_button_set_label_widget (GTK_TOOL_BUTTON (toolitem_line_1), 
												gtk_image_new_from_pixmap (imagePixmap1, NULL));
			gtk_widget_show_all (GTK_WIDGET (toolitem_line_1));
		}
		g_signal_connect (toolitem_line_1, "clicked", 
						G_CALLBACK(toolitem_line_clicked_callback), 
						linePrototype1);


		toolitem_line_2 = gtk_radio_tool_button_new_from_widget (GTK_RADIO_TOOL_BUTTON(toolitem_line));
		gtk_toolbar_insert (GTK_TOOLBAR(toolbar_prototype_line), toolitem_line_2, 2);
		gtk_widget_set_size_request (GTK_WIDGET (toolitem_line_2), 100, 100);
		imagePixmap2 = gdk_pixmap_new (NULL,
												   cellWidth,
												   cellHeight,
												   24);
		linePrototype2 = my_prototype_manager_peek_nth (prototypeManager, 
														chosenPrototypeCategory,//MY_PROTOTYPE_CATEGORY_STANDARD,
														chosenPrototypeNth / 4 * 4 + 2);
		if (linePrototype2) {
			my_prototype_view_draw_pixmap (linePrototypeView, 
											linePrototype2,
											imagePixmap2,
											cellWidth,
											cellHeight);
			gtk_tool_button_set_label_widget (GTK_TOOL_BUTTON (toolitem_line_2), 
												gtk_image_new_from_pixmap (imagePixmap2, NULL));
			gtk_widget_show_all (GTK_WIDGET (toolitem_line_2));		
		}
		g_signal_connect (toolitem_line_2, "clicked", 
						G_CALLBACK(toolitem_line_clicked_callback), 
						linePrototype2);
							
						
		toolitem_line_3 = gtk_radio_tool_button_new_from_widget (GTK_RADIO_TOOL_BUTTON(toolitem_line));
		gtk_toolbar_insert (GTK_TOOLBAR(toolbar_prototype_line), toolitem_line_3, 3);
		gtk_widget_set_size_request (GTK_WIDGET (toolitem_line_3), 100, 100);
		imagePixmap3 = gdk_pixmap_new (NULL,
												   cellWidth,
												   cellHeight,
												   24);
		linePrototype3 = my_prototype_manager_peek_nth (prototypeManager, 
														chosenPrototypeCategory,//MY_PROTOTYPE_CATEGORY_STANDARD,
														chosenPrototypeNth / 4 * 4 + 3);
		if (linePrototype3) {
			my_prototype_view_draw_pixmap (linePrototypeView, 
											linePrototype3,
											imagePixmap3,
											cellWidth,
											cellHeight);
			gtk_tool_button_set_label_widget (GTK_TOOL_BUTTON (toolitem_line_3), 
												gtk_image_new_from_pixmap (imagePixmap3, NULL));
			gtk_widget_show_all (GTK_WIDGET (toolitem_line_3));		
		}
		g_signal_connect (toolitem_line_3, "clicked", 
						G_CALLBACK(toolitem_line_clicked_callback), 
						linePrototype3);

	}
	
	// 设置按下哪个按钮
	switch (chosenPrototypeNth % 4) {
	case 0:
		gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON (toolitem_line_0), TRUE);
		break;
	case 1:
		gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON (toolitem_line_1), TRUE);
		break;
	case 2:
		gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON (toolitem_line_2), TRUE);
		break;
	case 3:
		gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON (toolitem_line_3), TRUE);
		break;
	default:
		assert (0);
	}
	
	gtk_widget_destroy (dialog);
}
													
													
/**
 * 更多图形
 */
static void 	btnMorePicture_clicked_callback	(GtkButton *widget,
												gpointer   user_data){
												
	GtkWindow *window = (GtkWindow *) user_data;
	gchar *dir_name;
	GtkWidget *icon_view;
	GtkListStore *store;
	GDir *dir;
	const gchar *name;
	GtkTreeIter iter;
	GdkPixbuf *file_pixbuf;
	GtkWidget *dialog;
	GtkWidget *swin;
	gint result;

	dir_name = "../Stamps/Oil/";
	
	store = gtk_list_store_new (NUM_PICTURE_COLS,
			      G_TYPE_STRING,
			      G_TYPE_STRING,
			      GDK_TYPE_PIXBUF);

	dir = g_dir_open (dir_name, 0, NULL);
	if (!dir) {
		return;
	}
	name = g_dir_read_name (dir);
	while (name) {
		gchar *path, *display_name;
		gboolean is_dir;
		path = g_build_filename (dir_name, name, NULL);
		is_dir = g_file_test (path, G_FILE_TEST_IS_DIR);

		if (!is_dir && name[0] != '.') {
			display_name = g_filename_to_utf8 (name, -1, NULL, NULL, NULL);
			file_pixbuf = gdk_pixbuf_new_from_file (path, NULL);
			if (!file_pixbuf) {
				GtkWidget *dialogMsg = gtk_message_dialog_new (window,
											GTK_DIALOG_MODAL,
											GTK_MESSAGE_INFO,
											GTK_BUTTONS_OK,
											"不支持此格式!");
				gtk_window_set_title (GTK_WINDOW (dialogMsg), "");
				gtk_dialog_run (GTK_DIALOG (dialogMsg));
				gtk_widget_destroy (dialogMsg);
			}
			
			gtk_list_store_append (store, &iter);
			gtk_list_store_set (store, &iter,
					  COL_PICTURE_PATH, path,
					  COL_PICTURE_DISPLAY_NAME, display_name,
					  COL_PICTURE_PIXBUF, file_pixbuf,
					  -1);
			g_free (path);
			g_free (display_name);
		}
		name = g_dir_read_name (dir);
	}
	g_dir_close (dir);
	  
	  
	dialog = gtk_dialog_new_with_buttons ("",
                                                 window,
                                                 GTK_DIALOG_DESTROY_WITH_PARENT,// | GTK_DIALOG_MODAL,
//                                                 "确定",//GTK_STOCK_OK,
//                                                 GTK_RESPONSE_OK,
//                                                 "取消",//GTK_STOCK_CANCEL,
//                                                 GTK_RESPONSE_REJECT,
                                                 NULL);
	gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
	gtk_window_set_decorated(GTK_WINDOW(dialog),FALSE);
//	gtk_window_set_modal (GTK_WINDOW(dialog),FALSE);
//	gtk_container_set_border_width (GTK_CONTAINER (dialog), 0);
//	gtk_container_set_border_width (GTK_DIALOG (dialog)->vbox, 50);
//	gint x,y;
//	gtk_window_get_position (toolitem_line_0, &x, &y);
//	GtkStyle *style = gtk_widget_get_default_style();
//	GdkBitmap *mask;
//    gdk_pixmap_create_from_xpm_d( GTK_WIDGET(window)->window, &mask,
//                                             &style->bg[GTK_STATE_NORMAL],
//                                             WheelbarrowFull_xpm );
//	GdkBitmap *mask = gdk_bitmap_create_from_data (NULL,
//                             WheelbarrowFull_xpm,
//								1000,
//								1000);
//	gdk_pixmap_create_from_xpm_d( window->window, &mask,
//                                             &style->bg[GTK_STATE_NORMAL],
//                                             WheelbarrowFull_xpm);
//	gtk_widget_shape_combine_mask (dialog,
//                               mask,
//                               0,
//							   0);
	gtk_window_move (GTK_WINDOW(dialog), 665, 80);
	dialogMorePicture = dialog;

	
	swin = gtk_scrolled_window_new (NULL, NULL);
	gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (dialog)->vbox), swin);
//	gtk_box_pack_start (GTK_BOX (vbox), swin, TRUE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (swin), 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swin),	GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request (GTK_WIDGET (swin), 600, 400);
	
	
	icon_view = gtk_icon_view_new_with_model (GTK_TREE_MODEL (store));
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (swin), icon_view);
	gtk_icon_view_set_selection_mode (GTK_ICON_VIEW (icon_view), GTK_SELECTION_MULTIPLE);
	gtk_icon_view_set_text_column (GTK_ICON_VIEW (icon_view), COL_PICTURE_DISPLAY_NAME);
	gtk_icon_view_set_pixbuf_column (GTK_ICON_VIEW (icon_view), COL_PICTURE_PIXBUF);
//	gtk_icon_view_set_activate_on_single_click (icon_view, TRUE); since gtk 3.8
	g_signal_connect (icon_view, "item-activated", 
						G_CALLBACK (icon_view_picture_item_activated_callback), 
						store);
	g_object_unref (store);
	
	gtk_widget_show_all (dialog);

	result = gtk_dialog_run (GTK_DIALOG (dialog));
	if (result == GTK_RESPONSE_OK) {
		gtk_container_remove (GTK_CONTAINER (toolbar_prototype_picture), GTK_WIDGET (toolitem_0));
		gtk_container_remove (GTK_CONTAINER (toolbar_prototype_picture), GTK_WIDGET (toolitem_1));
		gtk_container_remove (GTK_CONTAINER (toolbar_prototype_picture), GTK_WIDGET (toolitem_2));
		gtk_container_remove (GTK_CONTAINER (toolbar_prototype_picture), GTK_WIDGET (toolitem_3));

		toolitem_0 = gtk_radio_tool_button_new_from_widget (GTK_RADIO_TOOL_BUTTON(toolitem_picture));
		gtk_toolbar_insert(GTK_TOOLBAR(toolbar_prototype_picture), toolitem_0, 0);
//		gtk_tool_item_set_tooltip_text (GTK_TOOL_ITEM (toolitem_0), "");
//		gtk_tool_button_set_icon_name (GTK_TOOL_BUTTON(toolitem_0), "custom_auto_picture_0");
		gtk_tool_button_set_label_widget (GTK_TOOL_BUTTON (toolitem_0), prototypePicture0.thumbnailWidget);
		gtk_widget_set_size_request (GTK_WIDGET (toolitem_0), 100, 100);
		g_signal_connect(G_OBJECT(toolitem_0), "clicked", G_CALLBACK(tool_item_picture_clicked_callback), &prototypePicture0);
		gtk_widget_show_all (GTK_WIDGET (toolitem_0));
	  
		toolitem_1 = gtk_radio_tool_button_new_from_widget (GTK_RADIO_TOOL_BUTTON(toolitem_0));
		gtk_toolbar_insert(GTK_TOOLBAR(toolbar_prototype_picture), toolitem_1, 1);
//		gtk_tool_item_set_tooltip_text (GTK_TOOL_ITEM (toolitem_1), "");
		gtk_tool_button_set_label_widget (GTK_TOOL_BUTTON (toolitem_1), prototypePicture1.thumbnailWidget);
		gtk_widget_set_size_request (GTK_WIDGET (toolitem_1), 100, 100);
		g_signal_connect(G_OBJECT(toolitem_1), "clicked", G_CALLBACK(tool_item_picture_clicked_callback), &prototypePicture1);
		gtk_widget_show_all (GTK_WIDGET (toolitem_1));	 

		toolitem_2 = gtk_radio_tool_button_new_from_widget (GTK_RADIO_TOOL_BUTTON(toolitem_0));
		gtk_toolbar_insert(GTK_TOOLBAR(toolbar_prototype_picture), toolitem_2, 1);
//		gtk_tool_item_set_tooltip_text (GTK_TOOL_ITEM (toolitem_1), "");
		gtk_tool_button_set_label_widget (GTK_TOOL_BUTTON (toolitem_2), prototypePicture2.thumbnailWidget);
		gtk_widget_set_size_request (GTK_WIDGET (toolitem_2), 100, 100);
		g_signal_connect(G_OBJECT(toolitem_2), "clicked", G_CALLBACK(tool_item_picture_clicked_callback), &prototypePicture2);
		gtk_widget_show_all (GTK_WIDGET (toolitem_2));	 

		toolitem_3 = gtk_radio_tool_button_new_from_widget (GTK_RADIO_TOOL_BUTTON(toolitem_0));
		gtk_toolbar_insert(GTK_TOOLBAR(toolbar_prototype_picture), toolitem_3, 1);
//		gtk_tool_item_set_tooltip_text (GTK_TOOL_ITEM (toolitem_1), "");
		gtk_tool_button_set_label_widget (GTK_TOOL_BUTTON (toolitem_3), prototypePicture3.thumbnailWidget);
		gtk_widget_set_size_request (GTK_WIDGET (toolitem_3), 100, 100);
		g_signal_connect(G_OBJECT(toolitem_3), "clicked", G_CALLBACK(tool_item_picture_clicked_callback), &prototypePicture3);
		gtk_widget_show_all (GTK_WIDGET (toolitem_3));	  
	}
	
	gtk_widget_destroy (dialog);
}


/**
 * 准备选中图形
 */
gint ready_select_callback( GtkWidget *widget,
                    GdkEvent  *event,
                    gpointer   callback_data ){
	MyShape *latestShape;

//	gtk_widget_hide (toolbar_prototype_line);
//	gtk_widget_hide (toolbar_prototype_picture);
	
	if ( gtk_toggle_tool_button_get_active (GTK_TOGGLE_TOOL_BUTTON(widget))) {
		my_debug ("select callback");
		actChosen = MY_ACTION_FUNC_SELECT;
		my_debug ("actChosen: %d", actChosen);
		turn_off_all_shape_focus_edit ();
		//my_layer_delete_all_control_shapes	(my_canvas_get_selected_layer (appState->canvas));
		if (latestShape = my_action_manager_get_latest_graph_target (actionManager)) {
			latestShape->isFocusing = FALSE;
		}
		redraw();
	}
	return TRUE;
}

/**
 * 准备画直线
 */
gint ready_straight_line_callback( GtkWidget *widget,
                    GdkEvent  *event,
                    gpointer   callback_data ){

	MyShape *latestTarget;

	if ( gtk_toggle_tool_button_get_active (GTK_TOGGLE_TOOL_BUTTON(widget))) {

		actChosen = MY_ACTION_FUNC_STRAIGHT_LINE;
		my_debug ("actChosen: %d", actChosen);
		turn_off_all_shape_focus_edit ();
		/*
		if (latestTarget = my_action_manager_get_latest_graph_target (actionManager)) {
			latestTarget->isFocusing = TRUE;
			latestTarget->isEditing = TRUE;
			//my_curve_controller_init_with_curve	(latestTarget->curveController, latestTarget);
			my_debug ("ready to add shape %#x 's control rect into layer %#x", latestTarget, layer);
			my_shape_init_control_shapes (latestTarget, layer);
		}
		*/
		redraw();
	}
	return TRUE;
}

/**
 * 删除图形
 */
 static void 		deleteFocusedShapeRedraw	(void) {
	 
	 
	MyShape *focusTarget;
	MyControlShapeRect *controlShape;

	focusTarget = (MyShape *) g_queue_peek_nth (appState->focusedShapes, g_queue_get_length (appState->focusedShapes) - 1);
	if (focusTarget) {
		if (MY_IS_CONTROL_SHAPE_RECT (focusTarget)) {
			controlShape = MY_CONTROL_SHAPE_RECT (focusTarget);
			focusTarget = controlShape->target;
		}
		my_debug ("deleting shape %#x...", focusTarget);
		// 删除图形模型
		my_layer_delete_shape (layer, focusTarget);
		g_queue_remove (appState->focusedShapes, focusTarget);
		// 重画
		redraw();
	}
 }
 
 static gboolean	key_release_event_callback (GtkWidget *area,
											GdkEventKey *event,
											gpointer user_data) {
		
	my_debug ("%s\n", gdk_keyval_name (event->keyval));							
	
	switch (event->keyval) {
	case GDK_KEY_Delete:
		printf("key pressed: %s\n", "delete");
		deleteFocusedShapeRedraw ();
		break;
    case GDK_p:
      printf("key pressed: %s\n", "p");
      break;
    case GDK_s:
      if (event->state & GDK_SHIFT_MASK)
      {
        printf("key pressed: %s\n", "shift + s");
      }
      else if (event->state & GDK_CONTROL_MASK)
      {
        printf("key pressed: %s\n", "ctrl + s");
      }
      else
      {
        printf("key pressed: %s\n", "s");
      }
      break;
    case GDK_m:
      if (event->state & GDK_SHIFT_MASK)
      {
        printf("key pressed: %s\n", "shift + m");
      }
      else if (event->state & GDK_CONTROL_MASK)
      {
        printf("key pressed: %s\n", "ctrl + m");
      }
      else
      {
        printf("key pressed: %s\n", "m");
      }
      break;

    default:
      return FALSE; 
  }
	return FALSE; // FALSE to propergation event
}


static void		menu_item_delete_callback		(GtkMenuItem *menuitem,
												gpointer user_data) {
													
		deleteFocusedShapeRedraw ();
}

/**
 * 添加快速样式callback
 */
static void		popup_menu_item_add_prototype_line_activate_callback
												(GtkMenuItem *menuitem,
												gpointer user_data) {
													
	MyShape *focusTarget;
	MyControlShapeRect *controlShape;
	MyPrototypeLine *prototype;
	
	focusTarget = (MyShape *) g_queue_peek_nth (appState->focusedShapes, g_queue_get_length (appState->focusedShapes) - 1);
	if (focusTarget && !focusTarget->isPicture) {
		if (MY_IS_CONTROL_SHAPE_RECT (focusTarget)) {
			controlShape = MY_CONTROL_SHAPE_RECT (focusTarget);
			focusTarget = controlShape->target;
		}
		my_debug ("deleting shape %#x...", focusTarget);
		prototype = (MyPrototypeLine *) g_object_new (MY_TYPE_PROTOTYPE_LINE, NULL);
		my_shape_to_prototype_line (focusTarget, prototype);
		my_prototype_manager_add_prototype (prototypeManager, MY_PROTOTYPE_CATEGORY_FAST, prototype);
	}
}


/**
 * 粘贴截图callback
 */
static void		popup_menu_item_paste_activate_callback		(GtkMenuItem *menuitem,
																gpointer user_data) {

	MyPicture *picture;
	GdkPixbuf *pixbuf;
	GtkClipboard *clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
//	gtk_clipboard_set_can_store (clipboard, NULL, 0);
//	gtk_clipboard_set_text (clipboard, "1234567890", 10);
	pixbuf = gtk_clipboard_wait_for_image (clipboard);
	if (pixbuf) {
		picture = g_object_new (MY_TYPE_PICTURE, NULL);
		my_picture_init_by_pixbuf (picture, pixbuf);
		g_object_unref (pixbuf);
		picture->x = rightClickX - appState->orignX;
		picture->y = rightClickY - appState->orignY;
		MY_SHAPE (picture)->appState = appState;
		my_layer_add_shape (layer, MY_SHAPE (picture));

			my_shape_calculate_just_size (MY_SHAPE (picture));
			my_shape_shadow_mark_dirty (MY_SHAPE (picture));
			my_shape_mark_dirty (MY_SHAPE (picture));
			MY_SHAPE (picture)->isFocusing = TRUE;
			MY_SHAPE (picture)->isEditing = TRUE;
			switchToSelectFunc ();
			my_shape_init_control_shapes (MY_SHAPE (picture), layer);
			g_queue_push_tail (appState->focusedShapes, picture);
			// 重画
			redraw ();
			// 显示属性
			freshPropertiesWidget (MY_SHAPE (picture));
	} else { // failed retrieve
	}
}


static gint delete_callback( GtkWidget *widget,
                    GdkEvent  *event,
                    gpointer   callback_data ){

	MyShape *focusTarget;
	MyControlShapeRect *controlShape;
	
	deleteFocusedShapeRedraw ();

	if (focusTarget = my_action_manager_get_latest_graph_target (actionManager)) {
		if (MY_IS_CONTROL_SHAPE_RECT (focusTarget)) {
			controlShape = MY_CONTROL_SHAPE_RECT (focusTarget);
			focusTarget = controlShape->target;
		}
		my_debug ("deleting shape %#x...", focusTarget);
		// 删除图形模型
		my_layer_delete_shape (layer, focusTarget);
		g_queue_remove (appState->focusedShapes, focusTarget);
		// 重画
		redraw();
	}

	return TRUE;
}

/* Handle the response identifier from the assistant. Either tell the user to
* read the manual, retrieve the new color value or destroy the dialog. */
/**
 * 获取选择的颜色值
 */
static void dialog_response (GtkDialog *dialog,
							gint result,
							gpointer data) {
	GtkWidget *colorsel;
	GdkColor color = { 0, };
	guint16 alpha = 0;
	switch (result) {
	case GTK_RESPONSE_HELP:
		//g_print("Read the GTK+ API documentation.\n");
		break;
	case GTK_RESPONSE_OK:
		colorsel = GTK_COLOR_SELECTION_DIALOG (dialog)->colorsel;
		gtk_color_selection_get_current_color (GTK_COLOR_SELECTION (colorsel), &color);
		alpha = gtk_color_selection_get_current_alpha (GTK_COLOR_SELECTION (colorsel));
		my_debug ("rgba %u,%u,%u,%u", color.red, color.green, color.blue, alpha);
		if (data) { // 更新选中图形颜色
			MyShape *focusTarget = MY_SHAPE (data);
			//updateShapeGCWithFgColor (focusTarget->gc, color);

			focusTarget->red = color.red / (double)65535;
			focusTarget->green = color.green / (double)65535;
			focusTarget->blue = color.blue / (double)65535;
			focusTarget->alpha = alpha / (double)65535;
			redraw ();
		}
	default:
		gtk_widget_destroy (GTK_WIDGET(dialog));
	}
}


/**
 * 上移至最上层
 */
gint 		shift_top_callback		( GtkWidget *widget,
									GdkEvent  *event,
									gpointer   callback_data ){

	MyShape *focusTarget;

	focusTarget = (MyShape *) g_queue_peek_nth (appState->focusedShapes, g_queue_get_length (appState->focusedShapes) - 1);
	if (focusTarget) {
		if (MY_IS_CONTROL_SHAPE_RECT (focusTarget)) {
			focusTarget = MY_CONTROL_SHAPE_RECT (focusTarget)->target;
		}
		my_layer_shift_top_shape (layer, focusTarget);
		// 重画
		redraw  ();
	}
}


/**
 * 上移
 */
gint 		shift_up_callback		( GtkWidget *widget,
									GdkEvent  *event,
									gpointer   callback_data ){

	MyShape *focusTarget;

	focusTarget = (MyShape *) g_queue_peek_nth (appState->focusedShapes, g_queue_get_length (appState->focusedShapes) - 1);
	if (focusTarget) {
		if (MY_IS_CONTROL_SHAPE_RECT (focusTarget)) {
			focusTarget = MY_CONTROL_SHAPE_RECT (focusTarget)->target;
		}
		my_layer_shift_up_shape (layer, focusTarget);
		// 重画
		redraw  ();
	}
}


/**
 * 下移
 */
gint 		shift_down_callback		( GtkWidget *widget,
									GdkEvent  *event,
									gpointer   callback_data ){

	MyShape *focusTarget;
	focusTarget = (MyShape *) g_queue_peek_nth (appState->focusedShapes, g_queue_get_length (appState->focusedShapes) - 1);
	if (focusTarget) {
		if (MY_IS_CONTROL_SHAPE_RECT (focusTarget)) {
			focusTarget = MY_CONTROL_SHAPE_RECT (focusTarget)->target;
		}
		my_layer_shift_down_shape (layer, focusTarget);
		// 重画
		redraw  ();
	}
}


/**
 * 下移至最底层
 */
gint 		shift_bottom_callback	( GtkWidget *widget,
									GdkEvent  *event,
									gpointer   callback_data ){

	MyShape *focusTarget;
	focusTarget = (MyShape *) g_queue_peek_nth (appState->focusedShapes, g_queue_get_length (appState->focusedShapes) - 1);
	if (focusTarget) {
		if (MY_IS_CONTROL_SHAPE_RECT (focusTarget)) {
			focusTarget = MY_CONTROL_SHAPE_RECT (focusTarget)->target;
		}
		my_layer_shift_bottom_shape (layer, focusTarget);
		// 重画
		redraw  ();
	}
}


static void		color_button_property_color_set_callback	(GtkColorButton *widget, gpointer user_data) {

	
	GHashTable *ht = (GHashTable *) user_data;
	MyPropertyInfo *propertyInfo = (MyPropertyInfo *) g_hash_table_lookup (ht, "propertyInfo");
	MyPrototypeLine *prototype = (MyPrototypeLine *) g_hash_table_lookup (ht, "prototype");
	GdkColor color;
	guint16 alpha = gtk_color_button_get_alpha (widget);
	gtk_color_button_get_color (widget, &color);	
	prototype->red = color.red / (double)65535;
	prototype->green = color.green / (double)65535;
	prototype->blue = color.blue / (double)65535;
	prototype->alpha = alpha / (double)65535;
	if (currentPropertyShape) {
		my_shape_from_prototype_line (currentPropertyShape, prototype);
		my_shape_shadow_mark_dirty (currentPropertyShape);
		redraw ();
		freshPropertiesWidget (currentPropertyShape);
	}
}


/**
 * 选择颜色
 */
gint 		color_dialog_callback	( GtkWidget *widget,
											GdkEvent  *event,
											gpointer   callback_data ){

	GtkWidget *dialog, *colorsel;
	MyShape *focusTarget;
	GdkColor color;
	guint alpha;

	focusTarget = (MyShape *) g_queue_peek_nth (appState->focusedShapes, g_queue_get_length (appState->focusedShapes) - 1);
	if (focusTarget) {
		if (MY_IS_CONTROL_SHAPE_RECT (focusTarget)) {
			focusTarget = MY_CONTROL_SHAPE_RECT (focusTarget)->target;
		}
		color.red = focusTarget->red * 65535;
		color.green = focusTarget->green * 65535;
		color.blue = focusTarget->blue * 65535;
		alpha = focusTarget->alpha * 65535;
	} else {
		color.red = 0;
		color.green = 0;
		color.blue = 0;
		alpha = 65535;
	}

	// open dialog
	dialog = gtk_color_selection_dialog_new ("选择颜色");
	gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
	colorsel = GTK_COLOR_SELECTION_DIALOG (dialog)->colorsel;
	gtk_color_selection_set_has_opacity_control (GTK_COLOR_SELECTION (colorsel), TRUE);
	//gtk_color_selection_set_has_palette (GTK_COLOR_SELECTION (colorsel), FALSE);
	// 设置窗口初始颜色
	gtk_color_selection_set_current_color (GTK_COLOR_SELECTION (colorsel), &color);
	gtk_color_selection_set_current_alpha (GTK_COLOR_SELECTION (colorsel), alpha);
	g_signal_connect (G_OBJECT (dialog), "response", G_CALLBACK (dialog_response), focusTarget);
	gtk_widget_show_all (dialog);
}

/**
 * 设置虚线
 */
gint 		dash_callback	( GtkWidget *widget,
											GdkEvent  *event,
											gpointer   callback_data ){

	GtkWidget *dialog, *colorsel;
	MyShape *focusTarget;
	GdkColor color;
	guint alpha;
	
		double dashes[] = {
			   20.0,
			   3.,
			   2.,
			   3.
			   };
		
	focusTarget = (MyShape *) g_queue_peek_nth (appState->focusedShapes, g_queue_get_length (appState->focusedShapes) - 1);
	if (focusTarget) {
		if (MY_IS_CONTROL_SHAPE_RECT (focusTarget)) {
			focusTarget = MY_CONTROL_SHAPE_RECT (focusTarget)->target;
		}
		/*
		double dashes[] = {
				50.0,  // ink
			   10.0,  // skip
			   10.0,  // ink
			   10.0   // skip
                  };
		*/
		my_shape_set_dash (focusTarget, dashes, sizeof(dashes)/sizeof(dashes[0]), 0);
		my_debug ("focus target %#x dashCount %u", focusTarget, focusTarget->dashCount);
		my_shape_shadow_mark_dirty (focusTarget);
		my_shape_mark_dirty (focusTarget);
		redraw ();
	}
	my_debug ("dash callback on focus target %#x", focusTarget);
	return TRUE;
}

/**
 * 设置实线
 */
gint 		solid_callback	( GtkWidget *widget,
											GdkEvent  *event,
											gpointer   callback_data ){

	MyShape *focusTarget;
	
	focusTarget = (MyShape *) g_queue_peek_nth (appState->focusedShapes, g_queue_get_length (appState->focusedShapes) - 1);
	if (focusTarget) {
		if (MY_IS_CONTROL_SHAPE_RECT (focusTarget)) {
			focusTarget = MY_CONTROL_SHAPE_RECT (focusTarget)->target;
		}
		my_shape_set_dash (focusTarget, NULL, 0, 0);
		my_shape_shadow_mark_dirty (focusTarget);
		my_shape_mark_dirty (focusTarget);
		redraw ();
	}
	return TRUE;
}

/**
 * 设置阴影
 */
gint 		shadow_callback	( GtkWidget *widget,
											GdkEvent  *event,
											gpointer   callback_data ){

	MyShape *focusTarget;
	
	focusTarget = (MyShape *) g_queue_peek_nth (appState->focusedShapes, g_queue_get_length (appState->focusedShapes) - 1);
	if (focusTarget) {
		if (MY_IS_CONTROL_SHAPE_RECT (focusTarget)) {
			focusTarget = MY_CONTROL_SHAPE_RECT (focusTarget)->target;
		}
		if (focusTarget->isShadowing)
			focusTarget->isShadowing = FALSE;
		else
			focusTarget->isShadowing = TRUE;

		redraw ();
	}
	return TRUE;
}


#define SHADOW_DELTA 3.
/**
 * 设置阴影位置
 */
static gint 		shadow_position_left_top_callback	( GtkWidget *widget,
											GdkEvent  *event,
											gpointer   callback_data ){

		
	MyShape *focusTarget;
	
	focusTarget = (MyShape *) g_queue_peek_nth (appState->focusedShapes, g_queue_get_length (appState->focusedShapes) - 1);
	if (focusTarget) {
		if (MY_IS_CONTROL_SHAPE_RECT (focusTarget)) {
			focusTarget = MY_CONTROL_SHAPE_RECT (focusTarget)->target;
		}
		if (focusTarget->isShadowing) {
			focusTarget->shadowDeltaX = -SHADOW_DELTA;
			focusTarget->shadowDeltaY = -SHADOW_DELTA;
			redraw ();
		}
	}
	return TRUE;										
}
static gint 		shadow_position_top_callback	( GtkWidget *widget,
											GdkEvent  *event,
											gpointer   callback_data ){

		
	MyShape *focusTarget;
	
	focusTarget = (MyShape *) g_queue_peek_nth (appState->focusedShapes, g_queue_get_length (appState->focusedShapes) - 1);
	if (focusTarget) {
		if (MY_IS_CONTROL_SHAPE_RECT (focusTarget)) {
			focusTarget = MY_CONTROL_SHAPE_RECT (focusTarget)->target;
		}
		if (focusTarget->isShadowing) {
			focusTarget->shadowDeltaX = 0;
			focusTarget->shadowDeltaY = -SHADOW_DELTA;
			redraw ();
		}
	}
	return TRUE;										
}
static gint 		shadow_position_right_top_callback	( GtkWidget *widget,
											GdkEvent  *event,
											gpointer   callback_data ){

		
	MyShape *focusTarget;
	
	focusTarget = (MyShape *) g_queue_peek_nth (appState->focusedShapes, g_queue_get_length (appState->focusedShapes) - 1);
	if (focusTarget) {
		if (MY_IS_CONTROL_SHAPE_RECT (focusTarget)) {
			focusTarget = MY_CONTROL_SHAPE_RECT (focusTarget)->target;
		}
		if (focusTarget->isShadowing) {
			focusTarget->shadowDeltaX = SHADOW_DELTA;
			focusTarget->shadowDeltaY = -SHADOW_DELTA;
			redraw ();
		}
	}
	return TRUE;										
}
static gint 		shadow_position_right_callback	( GtkWidget *widget,
											GdkEvent  *event,
											gpointer   callback_data ){

		
	MyShape *focusTarget;
	
	focusTarget = (MyShape *) g_queue_peek_nth (appState->focusedShapes, g_queue_get_length (appState->focusedShapes) - 1);
	if (focusTarget) {
		if (MY_IS_CONTROL_SHAPE_RECT (focusTarget)) {
			focusTarget = MY_CONTROL_SHAPE_RECT (focusTarget)->target;
		}
		if (focusTarget->isShadowing) {
			focusTarget->shadowDeltaX = SHADOW_DELTA;
			focusTarget->shadowDeltaY = 0.;
			redraw ();
		}
	}
	return TRUE;										
}
static gint 		shadow_position_right_bottom_callback	( GtkWidget *widget,
											GdkEvent  *event,
											gpointer   callback_data ){

		
	MyShape *focusTarget;
	
	focusTarget = (MyShape *) g_queue_peek_nth (appState->focusedShapes, g_queue_get_length (appState->focusedShapes) - 1);
	if (focusTarget) {
		if (MY_IS_CONTROL_SHAPE_RECT (focusTarget)) {
			focusTarget = MY_CONTROL_SHAPE_RECT (focusTarget)->target;
		}
		if (focusTarget->isShadowing) {
			focusTarget->shadowDeltaX = SHADOW_DELTA;
			focusTarget->shadowDeltaY = SHADOW_DELTA;
			redraw ();
		}
	}
	return TRUE;										
}
static gint 		shadow_position_bottom_callback	( GtkWidget *widget,
											GdkEvent  *event,
											gpointer   callback_data ){

		
	MyShape *focusTarget;
	
	focusTarget = (MyShape *) g_queue_peek_nth (appState->focusedShapes, g_queue_get_length (appState->focusedShapes) - 1);
	if (focusTarget) {
		if (MY_IS_CONTROL_SHAPE_RECT (focusTarget)) {
			focusTarget = MY_CONTROL_SHAPE_RECT (focusTarget)->target;
		}
		if (focusTarget->isShadowing) {
			focusTarget->shadowDeltaX = 0.;
			focusTarget->shadowDeltaY = SHADOW_DELTA;
			redraw ();
		}
	}
	return TRUE;										
}
static gint 		shadow_position_left_bottom_callback	( GtkWidget *widget,
											GdkEvent  *event,
											gpointer   callback_data ){

		
	MyShape *focusTarget;
	
	focusTarget = (MyShape *) g_queue_peek_nth (appState->focusedShapes, g_queue_get_length (appState->focusedShapes) - 1);
	if (focusTarget) {
		if (MY_IS_CONTROL_SHAPE_RECT (focusTarget)) {
			focusTarget = MY_CONTROL_SHAPE_RECT (focusTarget)->target;
		}
		if (focusTarget->isShadowing) {
			focusTarget->shadowDeltaX = -SHADOW_DELTA;
			focusTarget->shadowDeltaY = SHADOW_DELTA;
			redraw ();
		}
	}
	return TRUE;										
}
static gint 		shadow_position_left_callback	( GtkWidget *widget,
											GdkEvent  *event,
											gpointer   callback_data ){

		
	MyShape *focusTarget;
	
	focusTarget = (MyShape *) g_queue_peek_nth (appState->focusedShapes, g_queue_get_length (appState->focusedShapes) - 1);
	if (focusTarget) {
		if (MY_IS_CONTROL_SHAPE_RECT (focusTarget)) {
			focusTarget = MY_CONTROL_SHAPE_RECT (focusTarget)->target;
		}
		if (focusTarget->isShadowing) {
			focusTarget->shadowDeltaX = -SHADOW_DELTA;
			focusTarget->shadowDeltaY = 0.;
			redraw ();
		}
	}
	return TRUE;										
}
static gint 		shadow_position_center_callback	( GtkWidget *widget,
											GdkEvent  *event,
											gpointer   callback_data ){

		
	MyShape *focusTarget;
	
	focusTarget = (MyShape *) g_queue_peek_nth (appState->focusedShapes, g_queue_get_length (appState->focusedShapes) - 1);
	if (focusTarget) {
		if (MY_IS_CONTROL_SHAPE_RECT (focusTarget)) {
			focusTarget = MY_CONTROL_SHAPE_RECT (focusTarget)->target;
		}
		if (focusTarget->isShadowing) {
			focusTarget->shadowDeltaX = 0.;
			focusTarget->shadowDeltaY = 0.;
			redraw ();
		}
	}
	return TRUE;										
}


// load custom icon
static void		add_custom_icon_with_name_from_pixbuf	(gchar *iconName, gint size, GdkPixbuf *iconPixbuf) {

	int iconWidth, iconHeight;
	iconWidth = gdk_pixbuf_get_width (iconPixbuf);
	iconHeight = gdk_pixbuf_get_height (iconPixbuf);
	if (size == -1) {
		gtk_icon_theme_add_builtin_icon (iconName, iconWidth, iconPixbuf);
	} else {
		gtk_icon_theme_add_builtin_icon (iconName, size, iconPixbuf);
	}
	g_object_unref (G_OBJECT (iconPixbuf));
}

static void		add_custom_icon_with_name	(gchar *iconName, gint size, char *filePath) {

	int iconWidth, iconHeight;
	GdkPixbuf *iconPixbuf = gdk_pixbuf_new_from_file(filePath, NULL);
	gdk_pixbuf_get_file_info (filePath, &iconWidth, &iconHeight);
	if (size == -1) {
		gtk_icon_theme_add_builtin_icon (iconName, iconWidth, iconPixbuf);
	} else {
		gtk_icon_theme_add_builtin_icon (iconName, size, iconPixbuf);
	}
	g_object_unref (G_OBJECT (iconPixbuf));
}

/* When a file is selected, display the full path in the GtkLabel widget. */
static void		file_changed	(GtkFileChooser *chooser2, gpointer data) {

	gchar *file = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser2));
	my_debug ("open file %s", file);
}

// TODO: parentWindow 参数处理,是否NULL
static MyPicture *		insertPicture_by_file_path		(gchar *filename, GtkWidget *parentWindow) {
	
	
	GtkWidget *dialogMsg;
	char *errorMsg;
	MyStatus myStatus;
	MyPicture *picture;
	
	my_debug ("open file %s", filename);
	/*
	if (my_util_index_of (filename, ".png") >= 0) {
		picture = g_object_new (MY_TYPE_PICTURE_PNG, NULL);
		MY_SHAPE (picture)->appState = appState;
		picture->x = rightClickX - appState->orignX;
		picture->y = rightClickY - appState->orignY;
		myStatus = my_picture_png_init_by_file (picture, filename);
	} else {*/
		picture = g_object_new (MY_TYPE_PICTURE, NULL);
		MY_SHAPE (picture)->appState = appState;
		picture->x = rightClickX - appState->orignX;
		picture->y = rightClickY - appState->orignY;
		myStatus = my_picture_init_by_file (picture, filename);
	//}
	switch (myStatus) {
		
		case MY_STATUS_SUCCESS:
			break;
		
		case MY_STATUS_FILE_NOT_FOUND:
			dialogMsg = gtk_message_dialog_new (parentWindow,
											GTK_DIALOG_MODAL,
											GTK_MESSAGE_INFO,
											GTK_BUTTONS_OK,
											"没找到文件!");
			gtk_window_set_title (GTK_WINDOW (dialogMsg), "");
			gtk_dialog_run (GTK_DIALOG (dialogMsg));
			gtk_widget_destroy (dialogMsg);
			break;
		
		case MY_STATUS_READ_ERROR:
			dialogMsg = gtk_message_dialog_new (parentWindow,
											GTK_DIALOG_MODAL,
											GTK_MESSAGE_INFO,
											GTK_BUTTONS_OK,
											"不支持此格式!");
			gtk_window_set_title (GTK_WINDOW (dialogMsg), "");
			gtk_dialog_run (GTK_DIALOG (dialogMsg));
			gtk_widget_destroy (dialogMsg);
			break;
		assert (0);
	}
	return picture;
}

// 插入图片
static void		insertPicture_dialog_callback	(GtkButton *button, GtkWindow *window) {

	
	GtkWidget *dialogChooser;
	GtkWidget *dialogMsg;
	gchar *filename;
	char *errorMsg;
	MyStatus myStatus;
	MyPicture *picture;
	int fileWidth, fileHeight;
	gint result;

	dialogChooser = gtk_file_chooser_dialog_new ("打开文件",
										window,
										GTK_FILE_CHOOSER_ACTION_OPEN,
										GTK_STOCK_CANCEL,
										GTK_RESPONSE_CANCEL,
										GTK_STOCK_SAVE,
										GTK_RESPONSE_ACCEPT,
										NULL);
	//gtk_dialog_add_filter (dialog, filter);
	result = gtk_dialog_run (GTK_DIALOG (dialogChooser));
	if (result == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialogChooser));
		picture = insertPicture_by_file_path (filename, GTK_WIDGET (window));
		picture->x = rightClickX - appState->orignX;
		picture->y = rightClickY - appState->orignY;
		my_layer_add_shape (layer, MY_SHAPE (picture));
		redraw ();
	}
	gtk_widget_destroy (dialogChooser);
}


static void		open_item_callback		(GtkMenuItem *menuitem,
										gpointer user_data) {

	GtkWidget *window = (GtkWidget *)user_data;
	
											/*
	GtkWidget *dialogChooser;
	GtkWidget *dialogMsg;
	gchar *filename;
	int fileWidth, fileHeight;
	gint result;

	dialogChooser = gtk_file_chooser_dialog_new ("打开文件",
										GTK_WINDOW (window),
										GTK_FILE_CHOOSER_ACTION_OPEN,
										GTK_STOCK_CANCEL,
										GTK_RESPONSE_CANCEL,
										GTK_STOCK_SAVE,
										GTK_RESPONSE_ACCEPT,
										NULL);
	//gtk_dialog_add_filter (dialog, filter);
	result = gtk_dialog_run (GTK_DIALOG (dialogChooser));
	if (result == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialogChooser));
		//filename = "a.png";
		my_debug ("open file %s", filename);
		//gtk_button_set_label (button, filename);

		appState->canvasBgFileName = filename;

		if (appState->canvasBgPixbuf) {
			g_object_unref ((gpointer) appState->canvasBgPixbuf);
			appState->canvasBgPixbuf = NULL;
		}
		appState->canvasBgPixbuf = gdk_pixbuf_new_from_file (filename, NULL);
		if (appState->canvasBgPixbuf) {
			appState->canvasBgX = 0;
			appState->canvasBgY = 0;
			fileWidth = gdk_pixbuf_get_width (appState->canvasBgPixbuf);
			fileHeight = gdk_pixbuf_get_height (appState->canvasBgPixbuf);
			appState->canvasBgWidth = fileWidth;
			appState->canvasBgHeight = fileHeight;
			updateCanvasSize (my_max (fileWidth, appState->canvasWidth),
								my_max (fileHeight, appState->canvasHeight));
			redraw ();
		} else {
			my_debug ("Cannot load %s, image format not supported!", filename);
			dialogMsg = gtk_message_dialog_new (window,
											GTK_DIALOG_MODAL,
											GTK_MESSAGE_INFO,
											GTK_BUTTONS_OK,
											"不支持此格式!");
			gtk_window_set_title (GTK_WINDOW (dialogMsg), "");
			gtk_dialog_run (GTK_DIALOG (dialogMsg));
			gtk_widget_destroy (dialogMsg);
		}
	}
	gtk_widget_destroy (dialogChooser);
	
	*/
					
	//if(FALSE){
	
	GdkPixbuf *pixbuf;
	GtkWidget *dialogChooser;
	GtkWidget *dialogMsg;
	gchar *filename;
	char *errorMsg;
	MyStatus myStatus;
	MyPicture *picture;
	int fileWidth, fileHeight;
	gint result;

	dialogChooser = gtk_file_chooser_dialog_new ("打开文件",
										window,
										GTK_FILE_CHOOSER_ACTION_OPEN,
										GTK_STOCK_CANCEL,
										GTK_RESPONSE_CANCEL,
										GTK_STOCK_SAVE,
										GTK_RESPONSE_ACCEPT,
										NULL);
	//gtk_dialog_add_filter (dialog, filter);
	result = gtk_dialog_run (GTK_DIALOG (dialogChooser));
	if (result == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialogChooser));
		picture = insertPicture_by_file_path (filename, GTK_WIDGET (window));
		picture->x = 0.;
		picture->y = 0.;
		my_layer_add_shape (layer, MY_SHAPE (picture));
		my_shape_init_control_shapes (MY_SHAPE (picture), layer);
		g_queue_push_tail (appState->focusedShapes, picture);
		redraw ();
	}
	gtk_widget_destroy (dialogChooser);
	//}
}


static void		open_callback	(GtkButton *button, GtkWindow *window) {

	
	GtkWidget *dialogChooser;
	GtkWidget *dialogMsg;
	gchar *filename;
	int fileWidth, fileHeight;
	gint result;
	dialogChooser = gtk_file_chooser_dialog_new ("打开文件",
										window,
										GTK_FILE_CHOOSER_ACTION_OPEN,
										GTK_STOCK_CANCEL,
										GTK_RESPONSE_CANCEL,
										GTK_STOCK_SAVE,
										GTK_RESPONSE_ACCEPT,
										NULL);
	//gtk_dialog_add_filter (dialog, filter);
	result = gtk_dialog_run (GTK_DIALOG (dialogChooser));
	if (result == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialogChooser));
		//filename = "a.png";
		my_debug ("open file %s", filename);
		//gtk_button_set_label (button, filename);

		appState->canvasBgFileName = filename;

		if (appState->canvasBgPixbuf) {
			g_object_unref ((gpointer) appState->canvasBgPixbuf);
			appState->canvasBgPixbuf = NULL;
		}
		appState->canvasBgPixbuf = gdk_pixbuf_new_from_file (filename, NULL);
		if (appState->canvasBgPixbuf) {
			appState->canvasBgX = 0;
			appState->canvasBgY = 0;
			fileWidth = gdk_pixbuf_get_width (appState->canvasBgPixbuf);
			fileHeight = gdk_pixbuf_get_height (appState->canvasBgPixbuf);
			appState->canvasBgWidth = fileWidth;
			appState->canvasBgHeight = fileHeight;
			updateCanvasSize (my_max (fileWidth, appState->canvasWidth),
								my_max (fileHeight, appState->canvasHeight));
			redraw ();
		} else {
			my_debug ("Cannot load %s, image format not supported!", filename);
			dialogMsg = gtk_message_dialog_new (window,
											GTK_DIALOG_MODAL,
											GTK_MESSAGE_INFO,
											GTK_BUTTONS_OK,
											"不支持此格式!");
			gtk_window_set_title (GTK_WINDOW (dialogMsg), "");
			gtk_dialog_run (GTK_DIALOG (dialogMsg));
			gtk_widget_destroy (dialogMsg);
		}

		/*
		if (appState->canvasBgSurface) {
			cairo_surface_destroy (appState->canvasBgSurface);
			appState->canvasBgSurface = NULL;
		}
		appState->canvasBgSurface = cairo_image_surface_create_from_png (filename);
		cairo_status_t crStatus = cairo_surface_status (appState->canvasBgSurface);
		if (crStatus == CAIRO_STATUS_SUCCESS) {
			appState->canvasBgX = 0;
			appState->canvasBgY = 0;
			fileWidth = cairo_image_surface_get_width (appState->canvasBgSurface);
			fileHeight = cairo_image_surface_get_height (appState->canvasBgSurface);

			appState->canvasBgWidth = fileWidth;
			appState->canvasBgHeight = fileHeight;
			updateCanvasSize (my_max (fileWidth, appState->canvasWidth),
								my_max (fileHeight, appState->canvasHeight));
			redraw ();
		} else {
			my_debug ("Cannot load %s!", filename);
			if (crStatus == CAIRO_STATUS_NO_MEMORY) {
				dialogMsg = gtk_message_dialog_new (window,
											GTK_DIALOG_MODAL,
											GTK_MESSAGE_INFO,
											GTK_BUTTONS_OK,
											"没有内存!");
			} else if (crStatus == CAIRO_STATUS_FILE_NOT_FOUND) {
				dialogMsg = gtk_message_dialog_new (window,
											GTK_DIALOG_MODAL,
											GTK_MESSAGE_INFO,
											GTK_BUTTONS_OK,
											"没找到文件!");
			} else if (crStatus == CAIRO_STATUS_READ_ERROR) {
				dialogMsg = gtk_message_dialog_new (window,
											GTK_DIALOG_MODAL,
											GTK_MESSAGE_INFO,
											GTK_BUTTONS_OK,
											"读文件发生错误!");
			}
			gtk_window_set_title (GTK_WINDOW (dialogMsg), "");
			gtk_dialog_run (GTK_DIALOG (dialogMsg));
			gtk_widget_destroy (dialogMsg);
		}
		*/
	}
	gtk_widget_destroy (dialogChooser);
}


static void		save_item_callback		(GtkMenuItem *menuitem,
										gpointer user_data) {

	GIOStatus status; // 1 is success
	GIOChannel *outChannel = g_io_channel_new_file ("pptmodel.xml",
                       "w",
                       NULL);
	gsize writtenSize;

	g_io_channel_write_chars (outChannel, "<canvas>\n", -1, &writtenSize, NULL);
	//my_canvas_foreach_layer (appState->canvas, save_layer, outChannel);
	g_io_channel_write_chars (outChannel, "\n</canvas>", -1, &writtenSize, NULL);

	if (outChannel)
		g_io_channel_unref (outChannel);
}

static void		draw_content_on_surface		(cairo_surface_t *destSurface) {

						
	// 画背景色
	cairo_t *cr = cairo_create (destSurface);
	cairo_set_source_rgba (cr,
							appState->canvasBgR,
							appState->canvasBgG,
							appState->canvasBgB,
							appState->canvasBgA);
	cairo_rectangle (cr,
					0,
					0,
					appState->canvasWidth,
					appState->canvasHeight);
	cairo_fill (cr);
	cairo_destroy (cr);
	// 画内容
	draw_content (FALSE, destSurface);
}


static void		export_picture_item_callback		(GtkMenuItem *menuitem,
													gpointer user_data) {
/*
	GdkPixbuf * pixbuf;
	GError *error;
	gboolean saved;

	pixbuf = gdk_pixbuf_get_from_drawable (NULL,
                              appState->pixmap,
                              gdk_colormap_get_system (),
                              appState->canvasX + 1, // 不包含边框 //0,
                              appState->canvasY + 1,//0,
                              0,
                              0,
                              appState->canvasWidth - 1,//-1,
                              appState->canvasHeight - 1//-1
							  );
	if (pixbuf == NULL){
	  quit();
	}
	//gdk_pixbuf_save (pixbuf, "screenshot.jpg", "jpeg", NULL, "quality", "100",NULL);
	gdk_pixbuf_save (pixbuf, "screenshot.png", "png", NULL, NULL);
	*/
														

	cairo_surface_t *destSurface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, appState->canvasWidth, appState->canvasHeight);
	draw_content_on_surface (destSurface);
	cairo_surface_write_to_png (destSurface, "export.png");
	cairo_surface_destroy (destSurface);
}


static void		send_to_ppt_item_callback		(GtkMenuItem *menuitem,
													gpointer user_data) {

	MyPicture *picture;
	GdkPixbuf *pixbuf;
	GtkClipboard *clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);

	/*
	pixbuf = gdk_pixbuf_get_from_drawable (NULL,
                              appState->pixmap,
                              gdk_colormap_get_system (),
                              appState->canvasX + 1, // 不包含边框 //0,
                              appState->canvasY + 1,//0,
                              0,
                              0,
                              appState->canvasWidth - 1,//-1,
                              appState->canvasHeight - 1//-1
							  );
							  */
	
	cairo_surface_t *destSurface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, appState->canvasWidth, appState->canvasHeight);
	draw_content_on_surface (destSurface);
	pixbuf = f_pixbuf_from_cairo_surface (destSurface);
	cairo_surface_destroy (destSurface);

	gtk_clipboard_set_image (clipboard, pixbuf);


	MY_BLOCK_BEGIN

	char cmd[50] = "..\\tool\\wavy_ppt_client\\wavy_ppt_client.exe ";
	STARTUPINFO si;
    PROCESS_INFORMATION pi;
	LPTSTR szCmdline;
    //szCmdline= _tcsdup(TEXT("..\\tool\\wavy_ppt_client\\wavy_ppt_client.exe 9999"));
	strcat (cmd, my_util_int_to_str (config->pptServicePort));
	//szCmdline = _tcsdup ( cmd);

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
 
    // Start the child process.
    if( !CreateProcessA( NULL,   // No module name (use command line)
					   cmd,      // Command line
					   NULL,           // Process handle not inheritable
					   NULL,           // Thread handle not inheritable
					   FALSE,          // Set handle inheritance to FALSE
					   0,              // No creation flags
					   NULL,           // Use parent's environment block
					   NULL,           // Use parent's starting directory
					   &si,            // Pointer to STARTUPINFO structure
					   &pi )           // Pointer to PROCESS_INFORMATION structure
       )
    {
       printf( "CreateProcess failed (%d)./n", GetLastError() );
       return;
    }
 
    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );
 
    // Close process and thread handles.
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

	MY_BLOCK_END
}

// @Deprecated
static void		send_to_ppt_item_callback2		(GtkMenuItem *menuitem,
													gpointer user_data) {

	MyPicture *picture;
	GdkPixbuf *pixbuf;
	GtkClipboard *clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);

	pixbuf = gdk_pixbuf_get_from_drawable (NULL,
                              appState->pixmap,
                              gdk_colormap_get_system (),
                              appState->canvasX + 1, // 不包含边框 //0,
                              appState->canvasY + 1,//0,
                              0,
                              0,
                              appState->canvasWidth - 1,//-1,
                              appState->canvasHeight - 1//-1
							  );
	gtk_clipboard_set_image (clipboard, pixbuf);

	MY_BLOCK_BEGIN // 调python脚本,将clipboard中的image copy到ppt中
	int status; 
	char *err;
    lua_State *L = luaL_newstate();
    luaL_openlibs(L); 
    status = luaL_dostring (L, "os.execute('..\\\\tool\\\\ppt_client\\\\client.exe 9999')"); // 会弹出cmd窗口一闪
    if (status) {
        // If something went wrong, error message is at the top of
        // the stack
		err = lua_tostring(L, -1);
    }
	lua_close(L);
	MY_BLOCK_END
}

			   
static void 	toolitem_line_clicked_callback	(GtkButton *widget,
													gpointer   user_data){
														

	MyPrototypeLine *linePrototype = (MyPrototypeLine *) user_data;

	if ( gtk_toggle_tool_button_get_active (GTK_TOGGLE_TOOL_BUTTON(widget))) {
		
		gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON(btnLineShape), TRUE);		
		my_prototype_manager_set_current_prototype (prototypeManager, linePrototype);	
	
		switch (linePrototype->style) {
		case MY_PROTOTYPE_STYLE_STRAIGHT_LINE:
			actChosenLine = MY_ACTION_FUNC_STRAIGHT_LINE;
			break;
		case MY_PROTOTYPE_STYLE_SPLINE:
			actChosenLine = MY_ACTION_FUNC_SPLINE;
			break;
		case MY_PROTOTYPE_STYLE_SPLINE_OVERLAP:
			actChosenLine = MY_ACTION_FUNC_SPLINE_OVERLAP;
			break;
		case MY_PROTOTYPE_STYLE_SPLINE_CYC:
			actChosenLine = MY_ACTION_FUNC_SPLINE_CYC;
			break;
		case MY_PROTOTYPE_STYLE_BEZIER:
			actChosenLine = MY_ACTION_FUNC_BEZIER;
			break;
		case MY_PROTOTYPE_STYLE_BEZIER_JM:
			actChosenLine = MY_ACTION_FUNC_BEZIER_JM;
			break;
		case MY_PROTOTYPE_STYLE_BEZIER_SIN:
			actChosenLine = MY_ACTION_FUNC_BEZIER_SIN;
			break;
		}
		actChosen = actChosenLine;
		
		my_debug ("actChosen: %d", actChosen);
		turn_off_all_shape_focus_edit ();
		redraw();
		my_action_manager_append_finish_action (actionManager);
	}
}		
	
									
#define GT0() 	app->canvasPreviewWidth = my_max (appState->canvasMinWidth, app->canvasPreviewWidth);\
				app->canvasPreviewHeight = my_max (appState->canvasMinHeight, app->canvasPreviewHeight);

gboolean	onShiftP0 (MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {


	ApplicationState *app = self->appState;
	my_debug ("control shape %#x onShiftP0 ...", self);
	app->canvasPreviewWidth -= dx;
	app->canvasPreviewHeight -= dy;
	GT0()

	if (canvasControlDashBox->x + dx + appState->canvasMinWidth <= app->canvasWidth) {
		canvasControlDashBox->x += dx;
		canvasControlDashBox->width = app->canvasPreviewWidth;
	}
	if (canvasControlDashBox->y + dy + appState->canvasMinHeight <= app->canvasHeight) {
		canvasControlDashBox->y += dy;
		canvasControlDashBox->height = app->canvasPreviewHeight;
	}

	MY_SHAPE (canvasControlDashBox)->isShowing = TRUE;

	needUpdateShapeCoodinaryX = needUpdateShapeCoodinaryY = TRUE;
	return TRUE;
}

gboolean	onShiftP1 (MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {
	
	ApplicationState *app = self->appState;
	app->canvasPreviewHeight -= dy;
	GT0()

	if (canvasControlDashBox->y + dy + appState->canvasMinHeight <= app->canvasHeight) {
		canvasControlDashBox->y += dy;
		canvasControlDashBox->height = app->canvasPreviewHeight;
	}
	MY_SHAPE (canvasControlDashBox)->isShowing = TRUE;

	needUpdateShapeCoodinaryY = TRUE;
	return TRUE;
}

gboolean	onShiftP2 (MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {
	
	ApplicationState *app = self->appState;
	app->canvasPreviewWidth += dx;
	app->canvasPreviewHeight -= dy;
	GT0()

	canvasControlDashBox->width = app->canvasPreviewWidth;
	if (canvasControlDashBox->y + dy + appState->canvasMinHeight <= app->canvasHeight) {
		canvasControlDashBox->y += dy;
		canvasControlDashBox->height = app->canvasPreviewHeight;
	}
	MY_SHAPE (canvasControlDashBox)->isShowing = TRUE;

	needUpdateShapeCoodinaryY = TRUE;
	return TRUE;
}

gboolean	onShiftP3 (MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {
	
	ApplicationState *app = self->appState;
	app->canvasPreviewWidth += dx;
	GT0()

	canvasControlDashBox->width = app->canvasPreviewWidth;
	MY_SHAPE (canvasControlDashBox)->isShowing = TRUE;

	return TRUE;
}

gboolean	onShiftP4 (MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {
	
	ApplicationState *app = self->appState;
	app->canvasPreviewWidth += dx;
	app->canvasPreviewHeight += dy;
	GT0()

	canvasControlDashBox->width = app->canvasPreviewWidth;
	canvasControlDashBox->height = app->canvasPreviewHeight;
	MY_SHAPE (canvasControlDashBox)->isShowing = TRUE;

	return TRUE;
}

gboolean	onShiftP5 (MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {
	
	ApplicationState *app = self->appState;
	app->canvasPreviewHeight += dy;
	GT0()

	canvasControlDashBox->height = app->canvasPreviewHeight;
	MY_SHAPE (canvasControlDashBox)->isShowing = TRUE;

	return TRUE;
}

gboolean	onShiftP6 (MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {
	
	ApplicationState *app = self->appState;
	app->canvasPreviewWidth -= dx;
	app->canvasPreviewHeight += dy;
	GT0()

	if (canvasControlDashBox->x + dx + appState->canvasMinWidth <= app->canvasWidth) {
		canvasControlDashBox->x += dx;
		canvasControlDashBox->width = app->canvasPreviewWidth;
	}
	canvasControlDashBox->height = app->canvasPreviewHeight;
	MY_SHAPE (canvasControlDashBox)->isShowing = TRUE;

	needUpdateShapeCoodinaryX = TRUE;
	return TRUE;
}

gboolean	onShiftP7 (MyShape *self, MyShape *source, gdouble dx, gdouble dy, MyMouse mouse, guint keyMask, gpointer userData) {
	
	ApplicationState *app = self->appState;
	app->canvasPreviewWidth -= dx;
	GT0()

	if (canvasControlDashBox->x + dx + appState->canvasMinWidth <= app->canvasWidth) {
		canvasControlDashBox->x += dx;
		canvasControlDashBox->width = app->canvasPreviewWidth;
	}
	MY_SHAPE (canvasControlDashBox)->isShowing = TRUE;

	needUpdateShapeCoodinaryX = TRUE;
	return TRUE;
}

#define NEW_CANVAS_CONTROL(n) \
	canvasControlP##n = my_control_shape_rect_new (appState, -1, -1, 4, 4, NULL, onShiftP##n, appState);\
	canvasControlP##n->isCanvasControl = TRUE;\
	canvasControlP##n->isAutoShiftSelf = FALSE;\
	my_layer_add_shape (layer, MY_SHAPE (canvasControlP##n));\
	my_debug ("added canvas control rect P%d shape %#x", n, canvasControlP##n);
	/*
	canvasControlP0 = my_control_shape_rect_new (appState, -1, -1, 4, 4, NULL, onShiftP0, appState);
	canvasControlP0->isCanvasControl = TRUE;
	my_layer_add_shape (layer, canvasControlP0);
	my_debug ("added canvas control rect P0 shape %#x", canvasControlP0);
	*/

/*
 * 改变缩放时回调
 */
static void		scale_changed		(GtkWidget * widget, gpointer data) {

	GdkRegion *region;

	GtkRange *range = (GtkRange *) widget;
	guint shapeCount, j;
	MyShape *shape;

	appState->oldScale = appState->scale;
	appState->scale = gtk_range_get_value(range) / 100.;
	my_debug ("scale changed is %f, 1/scale is %f", appState->scale, 1/appState->scale);
	// update shapes with scale
	my_debug ("before update scale ...");
	for (j = 0, shapeCount = my_layer_shape_count (layer); j < shapeCount; j++) {
		shape = my_layer_peek_nth (layer, j);
		my_shape_update_with_scale (shape, appState->scale);
		my_shape_calculate_just_size (shape);
	}
	my_debug ("after update scale");
	/*
	appState->canvasPreviewWidth = appState->canvasStandardWidth * appState->scale;
	appState->canvasPreviewHeight = appState->canvasStandardHeight * appState->scale;
	*/
	appState->canvasPreviewWidth = appState->canvasPreviewWidth / appState->oldScale * appState->scale;
	appState->canvasPreviewHeight = appState->canvasPreviewHeight / appState->oldScale * appState->scale;
	resizeDrawingArea ();
	redraw ();
}
 

static void		buildPopupMenu		(GtkWidget *popupMenu) {

	
	GtkWidget *shiftTop 				= gtk_menu_item_new_with_label (chinese ("上移到顶"));
	GtkWidget *shiftUp 					= gtk_menu_item_new_with_label (chinese ("上移"));
	GtkWidget *shiftDown				= gtk_menu_item_new_with_label (chinese ("下移"));
	GtkWidget *shiftBottom 				= gtk_menu_item_new_with_label (chinese ("下移到底"));
	GtkWidget *popup_menu_item_sep0		= gtk_separator_menu_item_new();
	GtkWidget *popup_menu_item_delete	= gtk_menu_item_new_with_label (chinese ("删除"));
	GtkWidget *popup_menu_item_sep1		= gtk_separator_menu_item_new();
	GtkWidget *popup_menu_item_sep2		= gtk_separator_menu_item_new();
	GtkWidget *popup_menu_item_add_prototype_line = gtk_menu_item_new_with_label (chinese ("添加到快速样式"));
	GtkWidget *popup_menu_item_paste = gtk_menu_item_new_with_label (chinese ("粘贴截图"));
//	GtkWidget *insertPicture 			= gtk_menu_item_new_with_label (chinese ("插入图标"));
	/*
	GtkWidget *color 					= gtk_menu_item_new_with_label (chinese ("颜色"));
	GtkWidget *dash						= gtk_menu_item_new_with_label (chinese ("虚线"));
	GtkWidget *solid 					= gtk_menu_item_new_with_label (chinese ("实线"));
	GtkWidget *popup_menu_item_shadow	= gtk_menu_item_new_with_label (chinese ("阴影位置"));
	GtkWidget *popup_menu_shadow		= gtk_menu_new ();
	GtkWidget *shadowPositionLeftTop	= gtk_menu_item_new_with_label (chinese ("居左上"));
	GtkWidget *shadowPositionTop		= gtk_menu_item_new_with_label (chinese ("居上"));
	GtkWidget *shadowPositionRightTop	= gtk_menu_item_new_with_label (chinese ("居右上"));
	GtkWidget *shadowPositionRight		= gtk_menu_item_new_with_label (chinese ("居右"));
	GtkWidget *shadowPositionRightBottom= gtk_menu_item_new_with_label (chinese ("居右下"));
	GtkWidget *shadowPositionBottom		= gtk_menu_item_new_with_label (chinese ("居下"));
	GtkWidget *shadowPositionLeftBottom	= gtk_menu_item_new_with_label (chinese ("居左下"));
	GtkWidget *shadowPositionLeft		= gtk_menu_item_new_with_label (chinese ("居左"));
	GtkWidget *shadowPositionCenter		= gtk_menu_item_new_with_label (chinese ("居中"));
	*/

	gtk_menu_shell_append (GTK_MENU_SHELL (popupMenu), shiftTop);
	gtk_menu_shell_append (GTK_MENU_SHELL (popupMenu), shiftUp);	
	gtk_menu_shell_append (GTK_MENU_SHELL (popupMenu), shiftDown);
	gtk_menu_shell_append (GTK_MENU_SHELL (popupMenu), shiftBottom);
	gtk_menu_shell_append (GTK_MENU_SHELL (popupMenu), popup_menu_item_sep0);
	gtk_menu_shell_append (GTK_MENU_SHELL (popupMenu), popup_menu_item_delete);	
	gtk_menu_shell_append (GTK_MENU_SHELL (popupMenu), popup_menu_item_add_prototype_line);
	gtk_menu_shell_append (GTK_MENU_SHELL (popupMenu), popup_menu_item_paste);
//	gtk_menu_shell_append (GTK_MENU_SHELL (popupMenu), insertPicture);
	/*
	gtk_menu_shell_append (GTK_MENU_SHELL (popupMenu), color);
	gtk_menu_shell_append (GTK_MENU_SHELL (popupMenu), dash);
	gtk_menu_shell_append (GTK_MENU_SHELL (popupMenu), solid);
	gtk_menu_shell_append (GTK_MENU_SHELL (popupMenu), popup_menu_item_shadow);
	gtk_menu_shell_append (GTK_MENU_SHELL (popupMenu), popup_menu_item_strok_width);
	gtk_menu_shell_append (GTK_MENU_SHELL (popupMenu), popup_menu_item_sep1);
	gtk_menu_shell_append (GTK_MENU_SHELL (popupMenu), popup_menu_item_sep2);
	gtk_menu_shell_append (GTK_MENU_SHELL (popup_menu_shadow), shadowPositionLeftTop);
	gtk_menu_shell_append (GTK_MENU_SHELL (popup_menu_shadow), shadowPositionTop);
	gtk_menu_shell_append (GTK_MENU_SHELL (popup_menu_shadow), shadowPositionRightTop);
	gtk_menu_shell_append (GTK_MENU_SHELL (popup_menu_shadow), shadowPositionRight);
	gtk_menu_shell_append (GTK_MENU_SHELL (popup_menu_shadow), shadowPositionRightBottom);
	gtk_menu_shell_append (GTK_MENU_SHELL (popup_menu_shadow), shadowPositionBottom);
	gtk_menu_shell_append (GTK_MENU_SHELL (popup_menu_shadow), shadowPositionLeftBottom);
	gtk_menu_shell_append (GTK_MENU_SHELL (popup_menu_shadow), shadowPositionLeft);
	gtk_menu_shell_append (GTK_MENU_SHELL (popup_menu_shadow), shadowPositionCenter);
	*/
	g_signal_connect (G_OBJECT (shiftTop), "activate", G_CALLBACK (shift_top_callback), NULL);
	g_signal_connect (G_OBJECT (shiftUp), "activate", G_CALLBACK (shift_up_callback), NULL);
	g_signal_connect (G_OBJECT (shiftDown), "activate", G_CALLBACK (shift_down_callback), NULL);
	g_signal_connect (G_OBJECT (shiftBottom), "activate", G_CALLBACK (shift_bottom_callback), NULL);
	g_signal_connect (G_OBJECT (popup_menu_item_delete), "activate", G_CALLBACK (menu_item_delete_callback), NULL);
	g_signal_connect (G_OBJECT (popup_menu_item_add_prototype_line), "activate", G_CALLBACK (popup_menu_item_add_prototype_line_activate_callback), NULL);
	g_signal_connect (G_OBJECT (popup_menu_item_paste), "activate", G_CALLBACK (popup_menu_item_paste_activate_callback), NULL);
//	g_signal_connect (G_OBJECT (insertPicture), "activate", G_CALLBACK (insertPicture_dialog_callback), NULL);
	/*
	g_signal_connect (G_OBJECT (color), "activate", G_CALLBACK (color_dialog_callback), NULL);
	g_signal_connect (G_OBJECT (dash), "activate", G_CALLBACK (dash_callback), NULL);
	g_signal_connect (G_OBJECT (solid), "activate", G_CALLBACK (solid_callback), NULL);
	g_signal_connect (G_OBJECT (shadowPositionLeftTop), 	"activate", G_CALLBACK (shadow_position_left_top_callback), NULL);
	g_signal_connect (G_OBJECT (shadowPositionTop), 		"activate", G_CALLBACK (shadow_position_top_callback), NULL);
	g_signal_connect (G_OBJECT (shadowPositionRightTop),	"activate", G_CALLBACK (shadow_position_right_top_callback), NULL);
	g_signal_connect (G_OBJECT (shadowPositionRight), 		"activate", G_CALLBACK (shadow_position_right_callback), NULL);
	g_signal_connect (G_OBJECT (shadowPositionRightBottom), "activate", G_CALLBACK (shadow_position_right_bottom_callback), NULL);
	g_signal_connect (G_OBJECT (shadowPositionBottom), 		"activate", G_CALLBACK (shadow_position_bottom_callback), NULL);
	g_signal_connect (G_OBJECT (shadowPositionLeftBottom), 	"activate", G_CALLBACK (shadow_position_left_bottom_callback), NULL);
	g_signal_connect (G_OBJECT (shadowPositionLeft), 		"activate", G_CALLBACK (shadow_position_left_callback), NULL);
	g_signal_connect (G_OBJECT (shadowPositionCenter), 		"activate", G_CALLBACK (shadow_position_center_callback), NULL);
	*/
}

static void		buildMenubar (GtkWidget *menubar_top, GtkWindow *window) {


	GtkWidget *file_item = gtk_menu_item_new_with_label( chinese ("文件") );
	GtkWidget *view_item = gtk_menu_item_new_with_label( chinese ("视图") );
	GtkWidget *tools_item = gtk_menu_item_new_with_label( chinese ("工具") );
	GtkWidget *help_item = gtk_menu_item_new_with_label( chinese ("帮助") );
	GtkWidget *file_menu = gtk_menu_new();
	GtkWidget *view_menu = gtk_menu_new();
	GtkWidget *tools_menu = gtk_menu_new();
	GtkWidget *help_menu = gtk_menu_new();
	GtkWidget *open_item = gtk_menu_item_new_with_label( chinese ("打开"));
	GtkWidget *save_item = gtk_menu_item_new_with_label( chinese ("保存"));
	GtkWidget *close_item = gtk_menu_item_new_with_label( chinese ("关闭"));
	GtkWidget *file_item_separator1 = gtk_separator_menu_item_new();
	GtkWidget *export_picture_item = gtk_menu_item_new_with_label( chinese ("导出图片"));
	GtkWidget *send_to_ppt_item = gtk_menu_item_new_with_label( chinese ("分享到PPT"));
	GtkWidget *file_item_separator2 = gtk_separator_menu_item_new();
	GtkWidget *exit_item = gtk_menu_item_new_with_label( chinese ("退出"));
	GtkWidget *query_item = gtk_menu_item_new_with_label( chinese ("关于 Wavy 1.0"));
	GtkWidget *separator2 = gtk_separator_menu_item_new();
	GtkWidget *about_help_item = gtk_menu_item_new_with_label( chinese ("帮助") );
	GtkWidget *about_help_menu = gtk_menu_new();
	GtkWidget *about_tool_item = gtk_menu_item_new_with_label( "" );
	GtkWidget *about_stuff_item = gtk_menu_item_new_with_label( "" );


	gtk_menu_shell_append( GTK_MENU_SHELL( menubar_top ), file_item );
	gtk_menu_shell_append( GTK_MENU_SHELL( menubar_top ), view_item );
	gtk_menu_shell_append( GTK_MENU_SHELL( menubar_top ), tools_item );
	gtk_menu_shell_append( GTK_MENU_SHELL( menubar_top ), help_item );
	
	gtk_menu_item_set_submenu( GTK_MENU_ITEM( file_item ), file_menu );
	gtk_menu_item_set_submenu( GTK_MENU_ITEM( view_item ), view_menu );
	gtk_menu_item_set_submenu( GTK_MENU_ITEM( tools_item ), tools_menu );
	gtk_menu_item_set_submenu( GTK_MENU_ITEM( help_item ), help_menu );
	
	gtk_menu_shell_append( GTK_MENU_SHELL(file_menu), open_item );
	gtk_menu_shell_append( GTK_MENU_SHELL(file_menu), save_item );
	gtk_menu_shell_append( GTK_MENU_SHELL(file_menu), close_item );
	gtk_menu_shell_append( GTK_MENU_SHELL(file_menu), file_item_separator1 );
	gtk_menu_shell_append( GTK_MENU_SHELL(file_menu), export_picture_item );
	gtk_menu_shell_append( GTK_MENU_SHELL(file_menu), send_to_ppt_item );
	gtk_menu_shell_append( GTK_MENU_SHELL(file_menu), file_item_separator2 );
	gtk_menu_shell_append( GTK_MENU_SHELL(file_menu), exit_item );
	g_signal_connect (G_OBJECT (open_item), "activate", G_CALLBACK (open_item_callback), (gpointer) window);
	g_signal_connect (G_OBJECT (save_item), "activate", G_CALLBACK (save_item_callback), (gpointer) NULL);
	g_signal_connect (G_OBJECT (export_picture_item), "activate", G_CALLBACK (export_picture_item_callback), (gpointer) NULL);
	g_signal_connect (G_OBJECT (send_to_ppt_item), "activate", G_CALLBACK (send_to_ppt_item_callback), (gpointer) NULL);
	g_signal_connect (G_OBJECT (exit_item), "activate", G_CALLBACK (gtk_main_quit), (gpointer) NULL);

	gtk_menu_shell_append( GTK_MENU_SHELL(help_menu), query_item );
	gtk_menu_shell_append( GTK_MENU_SHELL(help_menu), separator2 );
	gtk_menu_shell_append( GTK_MENU_SHELL(help_menu), about_help_item );

	gtk_menu_item_set_submenu ( GTK_MENU_ITEM( about_help_item ), about_help_menu );
	
	gtk_menu_shell_append( GTK_MENU_SHELL(about_help_menu), about_tool_item );
	gtk_menu_shell_append( GTK_MENU_SHELL(about_help_menu), about_stuff_item );
	
}


/**
 * 初始化下拉框可选址
 */
static void		initComboboxOptionsManager		(MyComboboxOptionsManager *comboboxOptionsManager) {


	MyComboboxOption *option;
	
	option = g_malloc0 (sizeof (MyComboboxOption));
	option->value = 1;
	option->text = "是";
	my_combobox_options_manager_append_option (comboboxOptionsManager, "isShadowing", option);
	
	option = g_malloc0 (sizeof (MyComboboxOption));
	option->value = 0;
	option->text = "否";
	my_combobox_options_manager_append_option (comboboxOptionsManager, "isShadowing", option);
	
	option = g_malloc0 (sizeof (MyComboboxOption));
	option->value = 1;
	option->text = "左";
	my_combobox_options_manager_append_option (comboboxOptionsManager, "jmSide", option);
		
	option = g_malloc0 (sizeof (MyComboboxOption));
	option->value = -1;
	option->text = "右";
	my_combobox_options_manager_append_option (comboboxOptionsManager, "jmSide", option);

	option = g_malloc0 (sizeof (MyComboboxOption));
	option->value = 1;
	option->text = "是";
	my_combobox_options_manager_append_option (comboboxOptionsManager, "isDashing", option);

	option = g_malloc0 (sizeof (MyComboboxOption));
	option->value = 0;
	option->text = "否";
	my_combobox_options_manager_append_option (comboboxOptionsManager, "isDashing", option);
	
	option = g_malloc0 (sizeof (MyComboboxOption));
	option->value = 1;
	option->text = "正";
	my_combobox_options_manager_append_option (comboboxOptionsManager, "overlapSide", option);
		
	option = g_malloc0 (sizeof (MyComboboxOption));
	option->value = 0;
	option->text = "反";
	my_combobox_options_manager_append_option (comboboxOptionsManager, "overlapSide", option);
}


static void		initConfig		(MyConfig *config) {

	
    lua_State *L = luaL_newstate();
	const char *e;
    luaL_openlibs(L); 
	if(luaL_loadfile(L, "..\\etc\\wavy-config.lua") || lua_pcall(L,0,0,0)){
		e = lua_tostring(L,-1);
        luaL_error(L,"loadfile error! %s \n",lua_tostring(L,-1));    
    }

	lua_getglobal(L, "config");
	lua_pushstring(L, "ppt_service_port");//首先把元素的名字压入栈顶    
	lua_gettable(L, -2);//值会放在栈顶，同时刚才压入的元素名字被弹出 
	config->pptServicePort = (int)lua_tonumber(L, -1);
	lua_pop(L, 2);

	lua_close(L);
}


/**
 * MAIN
 */
int		main	( int   argc, char *argv[] ) {

  GtkWidget *window;
  GtkWidget *vbox;
  GtkWidget *menubar_top;
  GtkWidget *toolbar;
  GtkToolItem *export;
  GtkToolItem *open;
  GtkWidget *recent;
  GtkWidget	*chooser;
  GtkToolItem *save;
  GtkToolItem *exit;
  GtkToolItem *sep;
  GtkToolItem *btnCurveJM;
  GtkToolItem *btnBezier;
  GtkToolItem *btnBezierJm;
  GtkToolItem *btnBezierSin;
//  GtkToolItem *btnStraightLine;
  GtkToolItem *btnDelete;
  GtkToolItem *btnColor;
  GtkToolItem *txtLineWidth;
  GtkEntry	  *inputLineWidth;
  GtkWidget *hbox;
  GtkWidget *swin_property;
  GtkWidget	*swin;
  GtkWidget *drawing_area;
  GtkWidget *bottomBar;
  GtkWidget *statusBar;
  GtkWidget *scale;
  ApplicationState *standardLinePrototypeAppState;
  GtkWidget *btnMoreLine;
  GtkWidget *btnMorePicture;
  GtkWidget *tabLabel2;
	GtkWidget *tabContent2;
	double dashes[] = { 2.0 };
	GtkWidget *noteBook;
	GtkWidget *tabLabel1;
	GtkWidget *hboxInTab0;
	GdkPixmap *imagePixmap;
	GdkPixmap *imagePixmap1;
	GdkPixmap *imagePixmap2;
	GdkPixmap *imagePixmap3;
	

  gtk_init (&argc, &argv);

	my_debug ("GUI starting up ...");

	// 应用状态初始化
	config = (MyConfig *) g_malloc0 (sizeof (MyConfig));
	initConfig (config);
	my_debug ("appState init ...");
	appState = (ApplicationState *) g_malloc0 (sizeof(ApplicationState));
//	appState->backend = my_blur_create (1024, 768);
	appState->scale = 1.;
	appState->oldScale = appState->scale;
	appState->cursorMoveNormalShape = gdk_cursor_new_for_display (gdk_display_get_default (), GDK_FLEUR);
	appState->cursorMoveControlShape = gdk_cursor_new_for_display (gdk_display_get_default (), GDK_DIAMOND_CROSS);
	appState->shadowBufWidth	= 1000;
	appState->shadowBufHeight	= 1000;
	appState->shadowBufStride = appState->shadowBufWidth * 4;
	appState->shadowBufSize =  sizeof(unsigned char) * appState->shadowBufStride * appState->shadowBufHeight;
	appState->shadowBuf = (unsigned char *) g_malloc0(appState->shadowBufSize);
	appState->shadowBuf2 = (unsigned char *)  g_malloc0(appState->shadowBufSize);
	// 窗口灰色
	appState->windowWidth = 1000; // maxmium window will override this setting
	appState->windowHeight = 600; // maxmium window will override this setting
	appState->drawingAreaPadding = 10.5;
	appState->drawingAreaBgR = 192.0 / 255;
	appState->drawingAreaBgG = 192.0 / 255;
	appState->drawingAreaBgB = 192.0 / 255;
	appState->drawingAreaBgA = 1.;
	// 画布白色
	/*
	appState->canvasWidth = appState->canvasPreviewWidth = appState->canvasStandardWidth = 800;
	appState->canvasHeight = appState->canvasPreviewHeight = appState->canvasStandardHeight = 600;
	*/
	appState->canvasWidth = appState->canvasPreviewWidth = 800;
	appState->canvasHeight = appState->canvasPreviewHeight = 600;
	appState->canvasMinWidth = 150;
	appState->canvasMinHeight = 100;
#ifdef MY_GUI_DEBUG
	appState->canvasBgR = .8;
	appState->canvasBgG = .8;
	appState->canvasBgB = .8;
	appState->canvasBgA = 1.;
#else
	appState->canvasBgR = 1.;
	appState->canvasBgG = 1.;
	appState->canvasBgB = 1.;
	appState->canvasBgA = 1.;
#endif
	appState->focusedShapes = g_queue_new();
	appState->hitWidthRadius = 6;
	//appState->orignX = 0;
	//appState->orignY = 0;

	appState->canvasBgPixbuf = NULL;
	appState->canvasBgSurface = NULL;

	layer = g_object_new (MY_TYPE_LAYER, NULL);
	appState->currentLayer = layer;
	my_debug ("init layer %#x", layer);


	// 构造标准线型快速样式所需的数据结构
	prototypeManager = (MyPrototypeManager *) g_object_new (MY_TYPE_PROTOTYPE_MANAGER, NULL);
	my_prototype_manager_init_prototypes_of_standard_category (prototypeManager);	
	
	linePrototypeView = (MyPrototypeView *) g_object_new (MY_TYPE_PROTOTYPE_VIEW, NULL);
	
	standardLinePrototypeAppState = (ApplicationState *) g_malloc0 (sizeof (ApplicationState));
	my_prototype_view_set_application_state (linePrototypeView, standardLinePrototypeAppState);
	standardLinePrototypeAppState->scale = 1.;
	standardLinePrototypeAppState->oldScale = standardLinePrototypeAppState->scale;
	standardLinePrototypeAppState->canvasX = 0.;
	standardLinePrototypeAppState->canvasY = 0.;
	standardLinePrototypeAppState->canvasWidth = 100;
	standardLinePrototypeAppState->canvasHeight = 100;
	standardLinePrototypeAppState->canvasBgR = 1.;
	standardLinePrototypeAppState->canvasBgG = 1.;
	standardLinePrototypeAppState->canvasBgB = 1.;
	standardLinePrototypeAppState->canvasBgA = 1.;
	standardLinePrototypeAppState->shadowBufWidth	= 1000;
	standardLinePrototypeAppState->shadowBufHeight	= 1000;
	standardLinePrototypeAppState->shadowBufStride = standardLinePrototypeAppState->shadowBufWidth * 4;
	standardLinePrototypeAppState->shadowBufSize =  sizeof(unsigned char) * standardLinePrototypeAppState->shadowBufStride * standardLinePrototypeAppState->shadowBufHeight;
	standardLinePrototypeAppState->shadowBuf = g_malloc0(standardLinePrototypeAppState->shadowBufSize);
	standardLinePrototypeAppState->shadowBuf2 = g_malloc0(standardLinePrototypeAppState->shadowBufSize);



	// 添加画布的控制点
	NEW_CANVAS_CONTROL(0)
	NEW_CANVAS_CONTROL(1)
	NEW_CANVAS_CONTROL(2)
	NEW_CANVAS_CONTROL(3)
	NEW_CANVAS_CONTROL(4)
	NEW_CANVAS_CONTROL(5)
	NEW_CANVAS_CONTROL(6)
	NEW_CANVAS_CONTROL(7)
	canvasControlDashBox = (MyControlShapeRect *) g_object_new (MY_TYPE_CONTROL_SHAPE_RECT, NULL);
	canvasControlDashBox->isCanvasControl = TRUE;
	MY_SHAPE (canvasControlDashBox)->appState = appState;
	MY_SHAPE (canvasControlDashBox)->isShiftable = FALSE;
	MY_SHAPE (canvasControlDashBox)->red = 0;
	MY_SHAPE (canvasControlDashBox)->green = 0;
	MY_SHAPE (canvasControlDashBox)->blue = 0;
	MY_SHAPE (canvasControlDashBox)->alpha = 1;
	
	my_shape_set_dash (MY_SHAPE (canvasControlDashBox), dashes, sizeof(dashes)/sizeof(dashes[0]), 0);
	canvasControlDashBox->x = 0;
	canvasControlDashBox->y = 0;
	canvasControlDashBox->isFilling = FALSE;
	canvasControlDashBox->bgR = 0;
	canvasControlDashBox->bgG = 0;
	canvasControlDashBox->bgB = 0;
	canvasControlDashBox->bgA = 0;
	my_layer_add_shape (layer, MY_SHAPE (canvasControlDashBox));
	my_debug ("canvasControlDashBox %#x");


  // 顶层window
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_icon_from_file (GTK_WINDOW (window), "../icon/logo.png", NULL);
  gtk_window_set_resizable( GTK_WINDOW (window), TRUE);
  gtk_window_set_default_size ( GTK_WINDOW (window), appState->windowWidth, appState->windowHeight);
  gtk_container_set_border_width (GTK_CONTAINER (window), 0);
  
  
												 
  
//	GtkWidget *layout = gtk_layout_new  (NULL, NULL);
//	gtk_container_add (GTK_CONTAINER (window), layout);
//	gtk_layout_set_size (layout, 900, 900);
//	gtk_widget_set_size_request (layout, 300, 300);
	
//	theButton = gtk_button_new ();
//	gtk_button_set_image (theButton, gtk_image_new_from_file ("../Stamps/Oil/001.jpg"));
//	gtk_layout_put  (layout, theButton, 20, 20);
  
  // vbox
  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), vbox);
//	gtk_layout_put  (layout, vbox, 0, 0);
//	gtk_box_pack_start (GTK_BOX (vbox), layout, FALSE, FALSE, 0);

//	GtkWidget *fixed = gtk_fixed_new ();
//	gtk_box_pack_start (GTK_BOX (vbox), fixed, FALSE, FALSE, 0);
//	gtk_toolbar_insert(GTK_TOOLBAR(vbox), fixed, -1);
//	GtkWidget *btnArrow0 = gtk_tool_button_new  (gtk_image_new_from_file ("../icon/toolbar_arrow.png"), NULL);
//	gtk_fixed_put (GTK_FIXED (fixed), btnArrow0, 100, 100);
	
//	GtkWidget *btnArrow1 = gtk_tool_button_new  (gtk_image_new_from_file ("../icon/toolbar_arrow.png"), NULL);
//	gtk_layout_put  (layout, btnArrow1, 20, 100);
	
	
	// top menu bar
	menubar_top = gtk_menu_bar_new ();
//	gtk_layout_put  (layout, menubar_top, 10, 20);
	gtk_box_pack_start (GTK_BOX (vbox), menubar_top, FALSE, FALSE, 0);
	
	buildMenubar (menubar_top, GTK_WINDOW (window));
	
	// tab
	noteBook = gtk_notebook_new ();
	gtk_box_pack_start(GTK_BOX(vbox), noteBook, FALSE, FALSE, 0);
	
	tabLabel1 = gtk_label_new (chinese (" 工具 "));
	hboxInTab0 = gtk_hbox_new (FALSE, 0);
	gtk_notebook_append_page (GTK_NOTEBOOK (noteBook), hboxInTab0, tabLabel1);
  
  // toolbar
  toolbar = gtk_toolbar_new();
  gtk_box_pack_start(GTK_BOX(hboxInTab0), toolbar, TRUE, TRUE, 0);
  gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
  gtk_container_set_border_width(GTK_CONTAINER(toolbar), 0);
  gtk_widget_set_size_request (toolbar, 300, 100);
  
	//	custom icons
	add_custom_icon_with_name	("custom_select_arrow", -1,			"../icon/select_arrow.png");
	add_custom_icon_with_name	("custom_line",  -1, 				"../icon/line.png");
	add_custom_icon_with_name	("custom_picture",  -1, 			"../icon/picture.png");
	add_custom_icon_with_name	("custom_bezier", -1,				"../icon/bezier.png");
	add_custom_icon_with_name	("custom_prototype_line", 50,		"../icon/prototype_line.png");
	add_custom_icon_with_name	("custom_prototype_bezier", 50,		"../icon/prototype_bezier.png");
	add_custom_icon_with_name	("custom_prototype_bezier_jm", 50,	"../icon/prototype_bezier_jm.png");
	add_custom_icon_with_name	("custom_prototype_bezier_sin", 50,	"../icon/prototype_bezier_sin.png");
    
  //   select button
  btnSelect = gtk_radio_tool_button_new (NULL);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), btnSelect, -1);
  gtk_tool_button_set_label (GTK_TOOL_BUTTON(btnSelect), chinese ("选择"));
//  gtk_tool_item_set_is_important (btnSelect, TRUE);
  gtk_tool_item_set_tooltip_text (GTK_TOOL_ITEM (btnSelect), chinese ("选择"));
  gtk_tool_button_set_icon_name (GTK_TOOL_BUTTON(btnSelect), "custom_select_arrow");
  gtk_widget_set_size_request (GTK_WIDGET (btnSelect), 100, 100);
  g_signal_connect(G_OBJECT(btnSelect), "clicked", G_CALLBACK(ready_select_callback), NULL);

  //   line shapes button
  btnLineShape = gtk_radio_tool_button_new_from_widget (GTK_RADIO_TOOL_BUTTON(btnSelect));
  gtk_tool_item_set_tooltip_text (GTK_TOOL_ITEM (btnLineShape), chinese ("线型"));
  gtk_tool_button_set_icon_name (GTK_TOOL_BUTTON(btnLineShape), "custom_bezier");
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), btnLineShape, -1);
  gtk_widget_set_size_request (GTK_WIDGET (btnLineShape), 100, 100);
  g_signal_connect(G_OBJECT(btnLineShape), "clicked", G_CALLBACK(line_shapes_callback), NULL);
  
  //   picture button
  btnPicture = (GtkWidget *) gtk_radio_tool_button_new_from_widget (GTK_RADIO_TOOL_BUTTON(btnSelect));
  gtk_tool_item_set_tooltip_text (GTK_TOOL_ITEM (btnPicture), chinese ("图片"));
  gtk_tool_button_set_icon_name (GTK_TOOL_BUTTON(btnPicture), "custom_picture");
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM (btnPicture), -1);
  gtk_widget_set_size_request (btnPicture, 100, 100);
  g_signal_connect(G_OBJECT(btnPicture), "clicked", G_CALLBACK(picture_callback), toolbar_prototype_line);
  
 
  // tool bar of prototype line
  toolbar_prototype_line = gtk_toolbar_new();
  gtk_box_pack_start(GTK_BOX(hboxInTab0), toolbar_prototype_line, TRUE, TRUE, 0);
  gtk_toolbar_set_style(GTK_TOOLBAR(toolbar_prototype_line), GTK_TOOLBAR_ICONS);
  gtk_container_set_border_width(GTK_CONTAINER(toolbar_prototype_line), 0);
  gtk_widget_set_size_request (toolbar_prototype_line, 400, 100);
  
  // tool bar of prototype picture
  toolbar_prototype_picture = gtk_toolbar_new();
  gtk_box_pack_start(GTK_BOX(hboxInTab0), toolbar_prototype_picture, TRUE, TRUE, 0);
  gtk_toolbar_set_style(GTK_TOOLBAR(toolbar_prototype_picture), GTK_TOOLBAR_ICONS);
  gtk_container_set_border_width(GTK_CONTAINER(toolbar_prototype_picture), 0);
  gtk_widget_set_size_request (toolbar_prototype_picture, 400, 100);

/*
  //   straight line button
  btnStraightLine = gtk_radio_tool_button_new (NULL);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar_prototype_line), btnStraightLine, -1);
  //gtk_tool_button_set_label (GTK_TOOL_BUTTON(btnStraightLine), "直线");
  gtk_tool_item_set_tooltip_text (GTK_TOOL_ITEM (btnStraightLine), "直线");
  gtk_tool_button_set_icon_name (GTK_TOOL_BUTTON(btnStraightLine), "custom_prototype_line");
  //gtk_tool_button_set_stock_id (GTK_TOOL_BUTTON(btnStraightLine), GTK_STOCK_CONNECT);
  gtk_widget_set_size_request (btnStraightLine, 100, 100);
  g_signal_connect(G_OBJECT(btnStraightLine), "clicked", G_CALLBACK(ready_straight_line_callback), NULL);

  //   button 贝塞尔曲线
  btnBezier = gtk_radio_tool_button_new_from_widget (GTK_RADIO_TOOL_BUTTON(btnStraightLine));
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar_prototype_line), btnBezier, -1);
  //gtk_tool_button_set_label (GTK_TOOL_BUTTON(btnBezier), "贝塞尔曲线");
  gtk_tool_item_set_tooltip_text (GTK_TOOL_ITEM (btnBezier), "曲线");
  gtk_tool_button_set_icon_name (GTK_TOOL_BUTTON(btnBezier), "custom_prototype_bezier");
  gtk_widget_set_size_request (btnBezier, 100, 100);
  g_signal_connect(G_OBJECT(btnBezier), "clicked", G_CALLBACK(ready_bezier_callback), NULL);

  //   button 新尖灭线
  btnBezierJm = gtk_radio_tool_button_new_from_widget (GTK_RADIO_TOOL_BUTTON(btnStraightLine));
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar_prototype_line), btnBezierJm, -1);
  gtk_tool_item_set_tooltip_text (GTK_TOOL_ITEM (btnBezierJm), "尖灭线");
  gtk_tool_button_set_icon_name (GTK_TOOL_BUTTON(btnBezierJm), "custom_prototype_bezier_jm");
  gtk_widget_set_size_request (btnBezierJm, 100, 100);
  g_signal_connect(G_OBJECT(btnBezierJm), "clicked", G_CALLBACK(ready_bezier_jm_callback), NULL);

  //   button 不整合线（正弦线）
  btnBezierSin = gtk_radio_tool_button_new_from_widget (GTK_RADIO_TOOL_BUTTON(btnStraightLine));
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar_prototype_line), btnBezierSin, -1);
  gtk_tool_item_set_tooltip_text (GTK_TOOL_ITEM (btnBezierSin), "不整合线");
  gtk_tool_button_set_icon_name (GTK_TOOL_BUTTON(btnBezierSin), "custom_prototype_bezier_sin");
  gtk_widget_set_size_request (btnBezierSin, 100, 100);
  g_signal_connect(G_OBJECT(btnBezierSin), "clicked", G_CALLBACK(ready_bezier_sin_callback), NULL);
*/

	toolitem_line = gtk_radio_tool_button_new (NULL);
	
	toolitem_line_0 = gtk_radio_tool_button_new_from_widget (GTK_RADIO_TOOL_BUTTON(toolitem_line));
//	toolitem_line_0 = gtk_radio_tool_button_new (NULL);
	gtk_toolbar_insert (GTK_TOOLBAR(toolbar_prototype_line), toolitem_line_0, -1);
	gtk_widget_set_size_request (GTK_WIDGET (toolitem_line_0), 100, 100);
//	gtk_widget_show (toolitem_line_0);
	imagePixmap = gdk_pixmap_new (NULL,
						   80,
						   60,
						   24);
	linePrototype0 = my_prototype_manager_peek_nth (prototypeManager, 
											MY_PROTOTYPE_CATEGORY_STANDARD,
											0);
	my_prototype_view_draw_pixmap (linePrototypeView, 
						linePrototype0,
						imagePixmap,
						80,
						60);
	gtk_tool_button_set_label_widget (GTK_TOOL_BUTTON (toolitem_line_0), 
							gtk_image_new_from_pixmap (imagePixmap, NULL));
	gtk_widget_show_all (GTK_WIDGET (toolitem_line_0));
	g_signal_connect (toolitem_line_0, "clicked", G_CALLBACK(toolitem_line_clicked_callback), linePrototype0);
	
	
	toolitem_line_1 = gtk_radio_tool_button_new_from_widget (GTK_RADIO_TOOL_BUTTON(toolitem_line));
	gtk_toolbar_insert (GTK_TOOLBAR(toolbar_prototype_line), toolitem_line_1, -1);
	gtk_widget_set_size_request (GTK_WIDGET (toolitem_line_1), 100, 100);
	imagePixmap1 = gdk_pixmap_new (NULL,
						   80,
						   60,
						   24);
	linePrototype1 = my_prototype_manager_peek_nth (prototypeManager, 
											MY_PROTOTYPE_CATEGORY_STANDARD,
											1);
	if (linePrototype1) {
		my_prototype_view_draw_pixmap (linePrototypeView, 
							linePrototype1,
							imagePixmap1,
							80,
							60);
		gtk_tool_button_set_label_widget (GTK_TOOL_BUTTON (toolitem_line_1), gtk_image_new_from_pixmap (imagePixmap1, NULL));
		gtk_widget_show_all (GTK_WIDGET (toolitem_line_1));		
	}
	g_signal_connect (toolitem_line_1, "clicked", G_CALLBACK(toolitem_line_clicked_callback), linePrototype1);

	
	toolitem_line_2 = gtk_radio_tool_button_new_from_widget (GTK_RADIO_TOOL_BUTTON(toolitem_line));
	gtk_toolbar_insert (GTK_TOOLBAR(toolbar_prototype_line), toolitem_line_2, -1);
	gtk_widget_set_size_request (GTK_WIDGET (toolitem_line_2), 100, 100);
	imagePixmap2 = gdk_pixmap_new (NULL,
						   80,
						   60,
						   24);
	linePrototype2 = my_prototype_manager_peek_nth (prototypeManager, 
											MY_PROTOTYPE_CATEGORY_STANDARD,
											2);
	if (linePrototype2) {
		my_prototype_view_draw_pixmap (linePrototypeView, 
							linePrototype2,
							imagePixmap2,
							80,
							60);
		gtk_tool_button_set_label_widget (GTK_TOOL_BUTTON (toolitem_line_2), gtk_image_new_from_pixmap (imagePixmap2, NULL));
		gtk_widget_show_all (GTK_WIDGET (toolitem_line_2));		
	}
	g_signal_connect (toolitem_line_2, "clicked", G_CALLBACK(toolitem_line_clicked_callback), linePrototype2);


	toolitem_line_3 = gtk_radio_tool_button_new_from_widget (GTK_RADIO_TOOL_BUTTON(toolitem_line));
	gtk_toolbar_insert (GTK_TOOLBAR(toolbar_prototype_line), toolitem_line_3, -1);
	gtk_widget_set_size_request (GTK_WIDGET (toolitem_line_3), 100, 100);
	imagePixmap3 = gdk_pixmap_new (NULL,
						   80,
						   60,
						   24);
	linePrototype3 = my_prototype_manager_peek_nth (prototypeManager, 
											MY_PROTOTYPE_CATEGORY_STANDARD,
											3);
	if (linePrototype3) {
		my_prototype_view_draw_pixmap (linePrototypeView, 
							linePrototype3,
							imagePixmap3,
							80,
							60);
		gtk_tool_button_set_label_widget (GTK_TOOL_BUTTON (toolitem_line_3), gtk_image_new_from_pixmap (imagePixmap3, NULL));
		gtk_widget_show_all (GTK_WIDGET (toolitem_line_3));		
	}
	g_signal_connect (toolitem_line_3, "clicked", G_CALLBACK(toolitem_line_clicked_callback), linePrototype3);


  // more line button
  btnMoreLine = (GtkWidget *) gtk_tool_button_new  (gtk_image_new_from_file ("../icon/toolbar_arrow.png"), NULL);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar_prototype_line), GTK_TOOL_ITEM (btnMoreLine), -1);
  g_signal_connect(G_OBJECT(btnMoreLine), "clicked", G_CALLBACK(btnMoreLine_clicked_callback), window);


	toolitem_picture = (GtkWidget *) gtk_radio_tool_button_new (NULL);
	
  // more picture button
  btnMorePicture = (GtkWidget *) gtk_tool_button_new  (gtk_image_new_from_file ("../icon/toolbar_arrow.png"), NULL);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar_prototype_picture), GTK_TOOL_ITEM (btnMorePicture), -1);
  g_signal_connect(G_OBJECT(btnMorePicture), "clicked", G_CALLBACK(btnMorePicture_clicked_callback), window);

	
	
/* maybe do extensible design like this:
#include "my_extend_config.h"
#define my_extend_tool_bar_from_stock(var,stock,tooltipText,event,callback,userData) \
			GtkWidget *var;\
			var = gtk_tool_button_new_from_stock (stock);\
			gtk_tool_item_set_tooltip_text (GTK_TOOL_ITEM (var), tooltipText);\
			gtk_toolbar_insert(GTK_TOOLBAR(toolbar), var, -1);\
			g_signal_connect(G_OBJECT(var), event, G_CALLBACK(callback), userData);

	my_extend_tool_bar_from_stock (btnDelete2, GTK_STOCK_DELETE, "删除", "clicked", delete_callback, NULL);
*/
/*
  //   color button
  btnColor = gtk_tool_button_new (NULL, "颜色");
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), btnColor, -1);
  g_signal_connect(G_OBJECT(btnColor), "clicked", G_CALLBACK(color_dialog_callback), NULL);
*/

		/*
  //   text line width
  txtLineWidth = gtk_tool_item_new ();
  //gtk_container_set_border_width (GTK_CONTAINER (txtLineWidth), 3);
  //gtk_entry_set_max_length (txtLineWidth, 3);
  gtk_entry_set_width_chars (txtLineWidth, 5);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), txtLineWidth, -1);
  inputLineWidth = gtk_entry_new ();
  g_object_set (inputLineWidth, "editable", TRUE, NULL);
  gtk_container_add (GTK_CONTAINER (txtLineWidth), inputLineWidth);
*/

//  gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);

  my_debug ("pack toolbar done.");

	tabLabel2 = gtk_label_new (chinese ("  图像  "));
	tabContent2 = gtk_label_new ("tabcontent2");
	gtk_notebook_append_page (GTK_NOTEBOOK (noteBook), tabContent2, tabLabel2);
	
	

	hbox = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);


	// scroll window of property
	swin_property = gtk_scrolled_window_new (NULL, NULL);
	gtk_box_pack_start (GTK_BOX (hbox), swin_property, TRUE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (swin_property), 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swin_property),	GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	
	
	vbox_properties = gtk_vbox_new (FALSE, 0);
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (swin_property), vbox_properties);
	gtk_widget_set_size_request (GTK_WIDGET (vbox_properties), 50, 500);
	/*
	table_property = gtk_table_new (20, 2, TRUE);
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (swin_property), table_property);
	gtk_widget_set_size_request (GTK_WIDGET (table_property), 50, 500);
	*/
	comboboxOptionsManager = g_object_new (MY_TYPE_COMBOBOX_OPTIONS_MANAGER, NULL);
	initComboboxOptionsManager (comboboxOptionsManager);
	propertiesView = (MyPropertiesView *) g_object_new (MY_TYPE_PROPERTIES_VIEW, NULL);
	my_properties_view_set_combobox_options_manager (propertiesView, comboboxOptionsManager);
	propertiesView->combobox_property_changed_callback = combobox_property_changed_callback;
//	propertiesView->entry_property_preedit_changed_callback = entry_property_preedit_changed_callback;
	propertiesView->entry_property_activate_callback = entry_property_activate_callback;
	propertiesView->color_button_property_color_set_callback = color_button_property_color_set_callback;
	
	
	// scroll window of drawing area
	swin = gtk_scrolled_window_new (NULL, NULL);
	gtk_box_pack_start (GTK_BOX (hbox), swin, TRUE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (swin), 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swin),	GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request (GTK_WIDGET (swin), 850, 600);

  my_debug ("pack scroll window done.");

  // drawing area
  appState->drawingArea = drawing_area = gtk_drawing_area_new ();
  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (swin), drawing_area);
  gtk_widget_set_size_request (GTK_WIDGET (drawing_area),
								-1,
								-1);
  //gtk_box_pack_start (GTK_BOX (vbox), drawing_area, TRUE, TRUE, 0);
  //gtk_widget_set_double_buffered (drawing_area, FALSE); // performance tune try

GTK_WIDGET_SET_FLAGS (drawing_area, GTK_CAN_FOCUS );
//  gtk_widget_set_can_focus (drawing_area, TRUE);
  gtk_widget_add_events (drawing_area, GDK_EXPOSURE_MASK
			 | GDK_LEAVE_NOTIFY_MASK
			 | GDK_BUTTON_PRESS_MASK
			 | GDK_BUTTON_RELEASE_MASK
			 | GDK_POINTER_MOTION_MASK
			 | GDK_POINTER_MOTION_HINT_MASK
			 | GDK_KEY_RELEASE_MASK
			 | GDK_FOCUS_CHANGE_MASK 
			 );

  /* Event signals */
  g_signal_connect (drawing_area, "expose_event", G_CALLBACK (expose_event), NULL);
  g_signal_connect (drawing_area, "configure_event", G_CALLBACK (configure_event), NULL);
  g_signal_connect (drawing_area, "motion_notify_event", G_CALLBACK (motion_notify_event), NULL);
  g_signal_connect (drawing_area, "button_release_event", G_CALLBACK (button_release_event), NULL);
  g_signal_connect (drawing_area, "key_release_event", G_CALLBACK (key_release_event_callback), NULL);


  g_signal_connect (window, "destroy", G_CALLBACK (quit), NULL);


  my_debug ("add drawing_area done.");

  // 初始功能
  actChosen = MY_ACTION_FUNC_SELECT;
  actionManager = (MyActionManager *) g_object_new (MY_TYPE_ACTION_MANAGER, NULL);

  //  add popup menu
	popupMenu = gtk_menu_new ();
	buildPopupMenu (popupMenu);


	gtk_menu_attach_to_widget (GTK_MENU (popupMenu), drawing_area, NULL);
	gtk_widget_show_all (popupMenu); // must call this

  my_debug ("popupMenu done.");

  g_signal_connect (drawing_area, "button_press_event", G_CALLBACK (button_press_event), NULL);

  // bottom bar
  bottomBar = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX(vbox), bottomBar, FALSE, FALSE, 0);

  // status bar
  statusBar = appState->statusBar = gtk_statusbar_new ();
  gtk_box_pack_start (GTK_BOX(bottomBar), statusBar, FALSE, FALSE, 0);
  gtk_widget_set_size_request (statusBar, 400, 20);

  // scale
  scale = gtk_hscale_new_with_range (10., 400., 1.);
  gtk_box_pack_end (GTK_BOX(bottomBar), scale, FALSE, FALSE, 0);
  gtk_scale_set_value_pos (GTK_SCALE(scale), GTK_POS_LEFT);
//  gtk_scale_add_mark (GTK_SCALE(scale), 100., GTK_POS_RIGHT, "xxx");
  gtk_range_set_value (GTK_RANGE(scale), 100.);
  gtk_widget_set_size_request (scale, 200, 10);

  g_signal_connect(G_OBJECT(scale), "value_changed", G_CALLBACK(scale_changed), NULL);

	// main loop
	my_debug ("gtk_widget_show_all (window) ...");
  gtk_widget_show_all (window);
	my_debug ("gtk_widget_show_all (window)");
	gtk_widget_hide (toolbar_prototype_line);
	gtk_widget_hide (toolbar_prototype_picture);
	gtk_window_maximize (GTK_WINDOW (window));
	
	
  gtk_main ();

  return 0;
}
