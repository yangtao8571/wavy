#define MY_LOG_LEVEL 3

#include <assert.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <glib-object.h>
#include "my_constant.h"
#include "my_rectangle.h"
#include "my_point.h"
#include "my_vector.h"
#include "my_util.h"
#include "my_application_state.h"
#include "my_shape.h"
#include "my_curve.h"
#include "my_bezier.h"
#include "my_bezier_jm.h"
#include "my_bezier_sin.h"
#include "my_spline.h"
#include "my_spline_cyc.h"
#include "my_prototype_line.h"
#include "my_combobox_options_manager.h"
#include "my_properties_view.h"


#define MY_PROPERTIES_VIEW_GET_PRIVATE(o)		(G_TYPE_INSTANCE_GET_PRIVATE ((o), MY_TYPE_PROPERTIES_VIEW, MyPropertiesViewPrivate))

G_DEFINE_TYPE (MyPropertiesView, my_properties_view, G_TYPE_OBJECT);
//G_DEFINE_TYPE_WITH_PRIVATE (MyPropertiesView, my_properties_view, G_TYPE_OBJECT)
 
typedef struct _MyPropertiesViewPrivate {
	GtkWidget *table_property;
	GQueue *propertiesQ;
	MyComboboxOptionsManager *	comboboxOptionsManager;
} MyPropertiesViewPrivate;

/******************************************************************************
 * static private variable 
 ******************************************************************************/
 
static void			removeChildWidget		(GtkWidget *widget, gpointer data);
static void			freePropertyInfo		 (gpointer data, gpointer user_data);
/******************************************************************************
 * declare static function 
 ******************************************************************************/
static void my_properties_view_init (MyPropertiesView* self);
static void my_properties_view_finalize (GObject* object);
static void my_properties_view_dispose (GObject* object);
/******************************************************************************
 * implement function 
 ******************************************************************************/
static void my_properties_view_class_init (MyPropertiesViewClass* klass)
{
	GObjectClass	*g_object_class;
	// Add private structure
	g_type_class_add_private (klass, sizeof (MyPropertiesViewPrivate));
	// Get the parent gobject class
	g_object_class = G_OBJECT_CLASS(klass);
	
    /* Hook overridable methods */
	
	/* Hook finalization functions */
	g_object_class->dispose = my_properties_view_dispose; /* instance destructor, reverse of init */
	g_object_class->finalize = my_properties_view_finalize; /* class finalization, reverse of class init */
}

// constructor
static void		my_properties_view_init (MyPropertiesView* self)
{
	/* Initialize public fields */
	
	/* Initialize private fields */
	MyPropertiesViewPrivate* priv = MY_PROPERTIES_VIEW_GET_PRIVATE (self);
	priv->propertiesQ = g_queue_new ();

	g_object_new (MY_TYPE_COMBOBOX_OPTIONS_MANAGER, NULL);
}

static void my_properties_view_finalize (GObject* object)
{  
  /* Always chain up to the parent class; as with dispose(), finalize()
   * is guaranteed to exist on the parent's class virtual function table
   */
	G_OBJECT_CLASS (my_properties_view_parent_class)->finalize (object);
}

static void my_properties_view_dispose (GObject* object)
{
	/* Reverse what was allocated by instance init */
	MyPropertiesView *self = MY_PROPERTIES_VIEW (object);
	MyPropertiesViewPrivate* priv = MY_PROPERTIES_VIEW_GET_PRIVATE (self);
 

 	if (priv->table_property) {
		gtk_container_foreach (GTK_CONTAINER (priv->table_property),
								removeChildWidget,
								priv->table_property);
		priv->table_property = NULL;
	}

	if (priv->propertiesQ) {
		g_queue_foreach (priv->propertiesQ, (GFunc) freePropertyInfo, NULL);
		g_queue_free (priv->propertiesQ);
	}

  /* Always chain up to the parent class; there is no need to check if
   * the parent class implements the dispose() virtual function: it is
   * always guaranteed to do so
   */
	G_OBJECT_CLASS (my_properties_view_parent_class)->dispose (object);
}


/**
 * 显示原型的属性
 */
void			my_properties_view_render		(MyPropertiesView *self, GtkWidget *container, MyPrototypeLine *proto) {

	
	MyPropertiesViewPrivate* priv = MY_PROPERTIES_VIEW_GET_PRIVATE (self);
	GQueue *propertiesQ = priv->propertiesQ;
	MyPropertyInfo *propertyInfo;
	guint i, j, propertyCount, count;
	GtkWidget *labelWidget;
	GtkWidget *valueWidget;
	GHashTable *ht;
	GdkColor color;
	guint alpha;
	gint comboboxIndex;
	GQueue *options;
	MyComboboxOption *option;
	
	if (proto->style != MY_PROTOTYPE_STYLE_PICTURE) {
	// 通用属性
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "color";
		propertyInfo->label = "颜色";
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_COLOR_BUTTON;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;

		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "red";
		propertyInfo->label = "红";
		propertyInfo->value = my_util_double_to_str (proto->red);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;
		
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "green";
		propertyInfo->label = "绿";
		propertyInfo->value = my_util_double_to_str (proto->green);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;
		
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "blue";
		propertyInfo->label = "蓝";
		propertyInfo->value = my_util_double_to_str (proto->blue);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;
		
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "alpha";
		propertyInfo->label = "透明度";
		propertyInfo->value = my_util_double_to_str (proto->alpha);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;
		
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "strokeWidth";
		propertyInfo->label = "线型宽度";
		propertyInfo->value = my_util_double_to_str (proto->strokeWidth);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;
		
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "isDashing";
		propertyInfo->label = "虚线";
		propertyInfo->valueInt = proto->isDashing;
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_COMBOBOX;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_INT;

		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "orignDashesStr";
		propertyInfo->label = "虚线样式";
		propertyInfo->value = g_strdup (proto->orignDashesStr);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_CHAR;
		
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "dashOffset";
		propertyInfo->label = "虚线偏移";
		propertyInfo->value = my_util_double_to_str (proto->dashOffset);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;
	}
		
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "isShadowing";
		propertyInfo->label = "阴影";
		propertyInfo->valueInt = proto->isShadowing;
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_COMBOBOX;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_INT;
		
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "shadowDeltaX";
		propertyInfo->label = "阴影偏移x";
		propertyInfo->value = my_util_double_to_str (proto->shadowDeltaX);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;

		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "shadowDeltaY";
		propertyInfo->label = "阴影偏移y";
		propertyInfo->value = my_util_double_to_str (proto->shadowDeltaY);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;
	

	switch (proto->style) {
	case MY_PROTOTYPE_STYLE_PICTURE:
		break;
	case MY_PROTOTYPE_STYLE_STRAIGHT_LINE:
		break;
	case MY_PROTOTYPE_STYLE_SPLINE:
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "segmentCountPerHemite";
		propertyInfo->label = "点间线段数";
		propertyInfo->value = my_util_int_to_str (proto->segmentCountPerHemite);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_INT;
		
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "dPt1_x";
		propertyInfo->label = "起点导数x";
		propertyInfo->value = my_util_double_to_str (proto->dPt1_x);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;
		
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "dPt1_y";
		propertyInfo->label = "起点导数y";
		propertyInfo->value = my_util_double_to_str (proto->dPt1_y);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;
		
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "dPtn_x";
		propertyInfo->label = "终点导数x";
		propertyInfo->value = my_util_double_to_str (proto->dPtn_x);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;
		
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "dPtn_y";
		propertyInfo->label = "终点导数y";
		propertyInfo->value = my_util_double_to_str (proto->dPtn_y);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;
		break;
	case MY_PROTOTYPE_STYLE_SPLINE_OVERLAP:
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "segmentCountPerHemite";
		propertyInfo->label = "点间线段数";
		propertyInfo->value = my_util_int_to_str (proto->segmentCountPerHemite);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_INT;
		
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "dPt1_x";
		propertyInfo->label = "起点导数x";
		propertyInfo->value = my_util_double_to_str (proto->dPt1_x);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;
		
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "dPt1_y";
		propertyInfo->label = "起点导数y";
		propertyInfo->value = my_util_double_to_str (proto->dPt1_y);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;
		
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "dPtn_x";
		propertyInfo->label = "终点导数x";
		propertyInfo->value = my_util_double_to_str (proto->dPtn_x);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;
		
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "dPtn_y";
		propertyInfo->label = "终点导数y";
		propertyInfo->value = my_util_double_to_str (proto->dPtn_y);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;
		
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "overlapOffset";
		propertyInfo->label = "超覆起始偏移";
		propertyInfo->value = my_util_double_to_str (proto->overlapOffset);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;

		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "overlapRadius";
		propertyInfo->label = "超覆半径";
		propertyInfo->value = my_util_double_to_str (proto->overlapRadius);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;

		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "overlapInterval";
		propertyInfo->label = "超覆间距";
		propertyInfo->value = my_util_double_to_str (proto->overlapInterval);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;
		
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "overlapSide";
		propertyInfo->label = "超覆方向";
		propertyInfo->valueInt = proto->overlapSide;
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_COMBOBOX;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_INT;

		break;
	case MY_PROTOTYPE_STYLE_SPLINE_CYC:
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "segmentCountPerHemite";
		propertyInfo->label = "点间线段数";
		propertyInfo->value = my_util_int_to_str (proto->segmentCountPerHemite);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_INT;
		break;
	case MY_PROTOTYPE_STYLE_BEZIER:
		break;
	case MY_PROTOTYPE_STYLE_BEZIER_JM:
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "jmOffset";
		propertyInfo->label = "尖起始位置";
		propertyInfo->value = my_util_double_to_str (proto->jmOffset);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;
		
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "jmInterval";
		propertyInfo->label = "尖间距";
		propertyInfo->value = my_util_double_to_str (proto->jmInterval);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;
		
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "jmEdgeLen";
		propertyInfo->label = "尖长度";
		propertyInfo->value = my_util_double_to_str (proto->jmEdgeLen);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;
		
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "jmSide";
		propertyInfo->label = "尖方向";
		propertyInfo->valueInt = proto->jmSide;
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_COMBOBOX;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;
		break;
	case MY_PROTOTYPE_STYLE_BEZIER_SIN:
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "offset";
		propertyInfo->label = "起点";
		propertyInfo->value = my_util_double_to_str (proto->offset);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;
		
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "interval";
		propertyInfo->label = "正弦点间距";
		propertyInfo->value = my_util_double_to_str (proto->interval);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;
		
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "segmentCount";
		propertyInfo->label = "线段数";
		propertyInfo->value = my_util_double_to_str (proto->segmentCount);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_INT;
		
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "phase";
		propertyInfo->label = "相位";
		propertyInfo->value = my_util_double_to_str (proto->phase);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;
		
		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "amplitude";
		propertyInfo->label = "振幅";
		propertyInfo->value = my_util_double_to_str (proto->amplitude);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;

		propertyInfo = (MyPropertyInfo *) g_malloc0 (sizeof (MyPropertyInfo));
		g_queue_push_tail (propertiesQ, propertyInfo);
		propertyInfo->id = "zhouqi";
		propertyInfo->label = "周期";
		propertyInfo->value = my_util_double_to_str (proto->zhouqi);
		propertyInfo->widgetType = MY_PROPERTY_WIDGET_TYPE_ENTRY;
		propertyInfo->valueType = MY_PROPERTY_VALUE_TYPE_DOUBLE;
		break;
	default:
		assert (0);
		break;
	}
	
	priv->table_property = gtk_table_new (25, 2, TRUE);
	gtk_container_add (GTK_CONTAINER (container), priv->table_property);

	// 构造widgets
	for (i = 0, propertyCount = g_queue_get_length (propertiesQ); i < propertyCount; i++) {
		propertyInfo = (MyPropertyInfo *) g_queue_peek_nth (propertiesQ, i);
		labelWidget = gtk_label_new (chinese (propertyInfo->label));
		gtk_table_attach_defaults (GTK_TABLE (priv->table_property), labelWidget, 0, 1, i, i+1);

		ht = g_hash_table_new (g_str_hash, g_str_equal);
		g_hash_table_insert (ht, "prototype", proto);
		g_hash_table_insert (ht, "propertyInfo", propertyInfo);
		switch (propertyInfo->widgetType) {
		case MY_PROPERTY_WIDGET_TYPE_COLOR_BUTTON:
			color.red = proto->red * 65535;
			color.green = proto->green * 65535;
			color.blue = proto->blue * 65535;
			alpha = proto->alpha * 65535;
			valueWidget = gtk_color_button_new_with_color (&color);
			gtk_color_button_set_alpha (GTK_COLOR_BUTTON (valueWidget), alpha);
			gtk_color_button_set_use_alpha (GTK_COLOR_BUTTON (valueWidget), TRUE);
			if (self->color_button_property_color_set_callback) {
				g_signal_connect (valueWidget, "color-set", G_CALLBACK (self->color_button_property_color_set_callback), ht);
			}
			break;
		case MY_PROPERTY_WIDGET_TYPE_ENTRY:
			valueWidget = gtk_entry_new ();
			gtk_entry_set_text (GTK_ENTRY (valueWidget), propertyInfo->value);
			if (self->entry_property_activate_callback) {
				g_signal_connect (valueWidget, "activate", G_CALLBACK (self->entry_property_activate_callback), ht);
			}
			break;
		case MY_PROPERTY_WIDGET_TYPE_COMBOBOX:
			valueWidget = gtk_combo_box_text_new ();
			options = my_combobox_options_manager_get_options (priv->comboboxOptionsManager, propertyInfo->id);
			if (options) {
				for (j = 0, count = g_queue_get_length (options); j < count; j++) {
					option = g_queue_peek_nth (options, j);
					gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (valueWidget), chinese (option->text));
				}
			}
			comboboxIndex = my_combobox_options_manager_get_option_index (priv->comboboxOptionsManager,
																		propertyInfo->id,
																		propertyInfo->valueInt);
			gtk_combo_box_set_active (GTK_COMBO_BOX (valueWidget), comboboxIndex);
			if (self->combobox_property_changed_callback) {
				g_signal_connect (valueWidget, "changed", G_CALLBACK (self->combobox_property_changed_callback), ht);
			}
			break;
		default:
			assert (0);
		}
		gtk_table_attach_defaults (GTK_TABLE (priv->table_property), valueWidget, 1, 2, i, i+1);
	}
	gtk_widget_show_all (container);
}


static void			removeChildWidget		(GtkWidget *widget, gpointer data) {


	gtk_container_remove (GTK_CONTAINER (data), widget);
}


static void		freePropertyInfo	 (gpointer data, gpointer user_data) {


	MyPropertyInfo *propertyInfo = (MyPropertyInfo *) data;
	if (propertyInfo->value) {
		g_free (propertyInfo->value);
	}
	g_free (propertyInfo);
}


void			my_properties_view_clear		(MyPropertiesView *self, GtkWidget *container) {

	
	MyPropertiesViewPrivate* priv = MY_PROPERTIES_VIEW_GET_PRIVATE (self);
	if (priv->table_property) {
		gtk_container_foreach (GTK_CONTAINER (priv->table_property),
								removeChildWidget,
								priv->table_property);
	}
	if (container) {
		gtk_container_remove (GTK_CONTAINER (container), priv->table_property);
		priv->table_property = NULL;
	}
	
	if (priv->propertiesQ) {
		g_queue_foreach (priv->propertiesQ, (GFunc) freePropertyInfo, NULL);
		g_queue_clear (priv->propertiesQ);
	}
}


void						my_properties_view_set_combobox_options_manager		(MyPropertiesView *self, 
																				MyComboboxOptionsManager *comboboxOptionsManager) {

	
	MyPropertiesViewPrivate* priv = MY_PROPERTIES_VIEW_GET_PRIVATE (self);
	priv->comboboxOptionsManager = comboboxOptionsManager;
}


MyComboboxOptionsManager *	my_properties_view_get_combobox_options_manager		(MyPropertiesView *self) {

	
	MyPropertiesViewPrivate* priv = MY_PROPERTIES_VIEW_GET_PRIVATE (self);
	return priv->comboboxOptionsManager;
}