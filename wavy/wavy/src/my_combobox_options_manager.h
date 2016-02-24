#ifndef __MY_COMBOBOX_OPTIONS_MANAGER_H__
#define __MY_COMBOBOX_OPTIONS_MANAGER_H__

#define MY_TYPE_COMBOBOX_OPTIONS_MANAGER                  (my_combobox_options_manager_get_type ())
#define MY_COMBOBOX_OPTIONS_MANAGER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_COMBOBOX_OPTIONS_MANAGER, MyComboboxOptionsManager))
#define MY_IS_COMBOBOX_OPTIONS_MANAGER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_COMBOBOX_OPTIONS_MANAGER))
#define MY_COMBOBOX_OPTIONS_MANAGER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), MY_TYPE_COMBOBOX_OPTIONS_MANAGER, MyComboboxOptionsManagerClass))
#define MY_IS_COMBOBOX_OPTIONS_MANAGER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), MY_TYPE_COMBOBOX_OPTIONS_MANAGER))
#define MY_COMBOBOX_OPTIONS_MANAGER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), MY_TYPE_COMBOBOX_OPTIONS_MANAGER, MyComboboxOptionsManagerClass))

typedef struct {
	//gint	index;
	gint	value;
	gchar *	text;
} MyComboboxOption;

typedef struct _MyComboboxOptionsManager		MyComboboxOptionsManager;
typedef struct _MyComboboxOptionsManagerClass	MyComboboxOptionsManagerClass;


struct _MyComboboxOptionsManager {
	GObject parent_instance;
};

struct _MyComboboxOptionsManagerClass {
	GObjectClass parent_class;
    /* Overridable methods */
};

/* used by MY_TYPE_COMBOBOX_OPTIONS_MANAGER */
GType my_combobox_options_manager_get_type (void);

/* Overridable methods wrapper */

/* Non-Overridable Methods */
void			my_combobox_options_manager_append_option		(MyComboboxOptionsManager *self, gchar *name, MyComboboxOption *option);
GQueue *		my_combobox_options_manager_get_options			(MyComboboxOptionsManager *self, gchar *name);
gint			my_combobox_options_manager_get_option_index	(MyComboboxOptionsManager *self, gchar *name, gint value);
gint			my_combobox_options_manager_get_option_value	(MyComboboxOptionsManager *self, gchar *name, gint index);

#endif /*__MY_COMBOBOX_OPTIONS_MANAGER_H__*/