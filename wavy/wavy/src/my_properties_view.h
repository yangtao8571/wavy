#ifndef __MY_PROPERTIES_VIEW_H__
#define __MY_PROPERTIES_VIEW_H__

#define MY_TYPE_PROPERTIES_VIEW                  (my_properties_view_get_type ())
#define MY_PROPERTIES_VIEW(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_PROPERTIES_VIEW, MyPropertiesView))
#define MY_IS_PROPERTIES_VIEW(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_PROPERTIES_VIEW))
#define MY_PROPERTIES_VIEW_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), MY_TYPE_PROPERTIES_VIEW, MyPropertiesViewClass))
#define MY_IS_PROPERTIES_VIEW_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), MY_TYPE_PROPERTIES_VIEW))
#define MY_PROPERTIES_VIEW_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), MY_TYPE_PROPERTIES_VIEW, MyPropertiesViewClass))



typedef enum {
	MY_PROPERTY_WIDGET_TYPE_COLOR_BUTTON,
	MY_PROPERTY_WIDGET_TYPE_ENTRY,
	MY_PROPERTY_WIDGET_TYPE_COMBOBOX
} MyPropertyWidgetType;

typedef enum {
	MY_PROPERTY_VALUE_TYPE_INT,
	MY_PROPERTY_VALUE_TYPE_DOUBLE,
	MY_PROPERTY_VALUE_TYPE_CHAR
} MyPropertyValueType;

typedef struct {
	gchar	*				id;			// 内部id
	gchar	*				label;
	gchar	*				value;		// 字符串值
	gint					valueInt;	// 整数值
	gboolean				isReadOnly;
	GQueue	*				allOptions;	// 下拉可选值
	MyPropertyWidgetType	widgetType;
	MyPropertyValueType		valueType;
} MyPropertyInfo;


typedef struct _MyPropertiesView		MyPropertiesView;
typedef struct _MyPropertiesViewClass	MyPropertiesViewClass;


struct _MyPropertiesView {
	GObject parent_instance;
	void (*entry_property_activate_callback)			(GtkEntry *entry, gpointer  user_data);
	void (*entry_property_preedit_changed_callback)		(GtkEntry *entry,
														 gchar    *preedit,
														 gpointer  user_data);
	void (*combobox_property_changed_callback)			(GtkComboBox *widget, gpointer user_data);	
	void (*color_button_property_color_set_callback)	(GtkColorButton *widget, gpointer user_data);
};

struct _MyPropertiesViewClass {
	GObjectClass parent_class;
    /* Overridable methods */
};

/* used by MY_TYPE_PROPERTIES_VIEW */
GType my_properties_view_get_type (void);

/* Overridable methods wrapper */

/* Non-Overridable Methods */

void			my_properties_view_render		(MyPropertiesView *self, GtkWidget *container, MyPrototypeLine *proto);
void			my_properties_view_clear		(MyPropertiesView *self, GtkWidget *container);
void			my_properties_view_set_combobox_options_manager	(MyPropertiesView *self, MyComboboxOptionsManager *comboboxOptionsManager);
MyComboboxOptionsManager *			my_properties_view_get_combobox_options_manager	(MyPropertiesView *self);

#endif /*__MY_PROPERTIES_VIEW_H__*/