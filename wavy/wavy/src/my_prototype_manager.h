#ifndef __MY_PROTOTYPE_MANAGER_H__
#define __MY_PROTOTYPE_MANAGER_H__

#define MY_TYPE_PROTOTYPE_MANAGER                  (my_prototype_manager_get_type ())
#define MY_PROTOTYPE_MANAGER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MY_TYPE_PROTOTYPE_MANAGER, MyPrototypeManager))
#define MY_IS_PROTOTYPE_MANAGER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MY_TYPE_PROTOTYPE_MANAGER))
#define MY_PROTOTYPE_MANAGER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), MY_TYPE_PROTOTYPE_MANAGER, MyPrototypeManagerClass))
#define MY_IS_PROTOTYPE_MANAGER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), MY_TYPE_PROTOTYPE_MANAGER))
#define MY_PROTOTYPE_MANAGER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), MY_TYPE_PROTOTYPE_MANAGER, MyPrototypeManagerClass))


typedef struct _MyShape					MyShape;
//typedef struct _MyPrototypeLine			MyPrototypeLine;
typedef struct _MyPrototypeManager		MyPrototypeManager;
typedef struct _MyPrototypeManagerClass	MyPrototypeManagerClass;

struct _MyPrototypeManager {
	GObject parent_instance;
};

struct _MyPrototypeManagerClass {
	GObjectClass parent_class;
    /* Overridable methods */
};

/* used by MY_TYPE_PROTOTYPE_MANAGER */
GType my_prototype_manager_get_type (void);

/* Overridable methods wrapper */

/* Non-Overridable Methods */
void				my_prototype_manager_init_prototypes_of_standard_category	(MyPrototypeManager *self);
void				my_prototype_manager_add_prototype				(MyPrototypeManager *self, MyPrototypeCategory category, MyPrototypeLine *prototype);
void				my_prototype_manager_remove_prototype			(MyPrototypeManager *self, MyPrototypeCategory category, MyPrototypeLine *prototype);
void				my_prototype_manager_set_current_prototype		(MyPrototypeManager *self, MyPrototypeLine *prototype);
MyPrototypeLine *	my_prototype_manager_get_current_prototype 		(MyPrototypeManager *self);
guint				my_prototype_manager_get_length					(MyPrototypeManager *self, MyPrototypeCategory category);
MyPrototypeLine *	my_prototype_manager_peek_nth					(MyPrototypeManager *self, MyPrototypeCategory category, guint n);

#endif /*__MY_PROTOTYPE_MANAGER_H__*/