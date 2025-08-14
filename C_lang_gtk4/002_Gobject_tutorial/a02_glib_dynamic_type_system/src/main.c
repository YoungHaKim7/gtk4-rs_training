#include <glib-object.h>

typedef struct _GTypeInfo GTypeInfo;
struct _GTypeInfo {
    /* interface types, classed types, instantiated types */
    guint16 class_size;

    GBaseInitFunc base_init;
    GBaseFinalizeFunc base_finalize;

    /* classed types, instantiated types */
    GClassInitFunc class_init;
    GClassFinalizeFunc class_finalize;
    gconstpointer class_data;

    /* instantiated types */
    guint16 instance_size;
    guint16 n_preallocs;
    GInstanceInitFunc instance_init;

    /* value handling */
    const GTypeValueTable* value_table;
};

GType g_type_register_static(GType parent_type,
    const gchar* type_name,
    const GTypeInfo* info,
    GTypeFlags flags);

GType g_type_register_fundamental(GType type_id,
    const gchar* type_name,
    const GTypeInfo* info,
    const GTypeFundamentalInfo* finfo,
    GTypeFlags flags);

int main(int argc, char** argv)
{
    GObject *instance1, *instance2;
    GObjectClass *class1, *class2;

    // Create two GObject instances
    instance1 = g_object_new(G_TYPE_OBJECT, NULL);
    instance2 = g_object_new(G_TYPE_OBJECT, NULL);

    g_print("The address of instance1 is %p\n", (void*)instance1);
    g_print("The address of instance2 is %p\n", (void*)instance2);

    // Get their class pointers
    class1 = G_OBJECT_GET_CLASS(instance1);
    class2 = G_OBJECT_GET_CLASS(instance2);

    g_print("The address of the class of instance1 is %p\n", (void*)class1);
    g_print("The address of the class of instance2 is %p\n", (void*)class2);

    // Free memory
    g_object_unref(instance1);
    g_object_unref(instance2);

    return 0;
}
