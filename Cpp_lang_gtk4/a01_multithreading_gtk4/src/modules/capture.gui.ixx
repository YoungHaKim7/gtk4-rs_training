module;
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdio.h>

export module capture.gui;

// GUI state lives inside this module
namespace gui_state {
    static GtkWidget *window = nullptr, *vbox = nullptr, *hboxDir = nullptr, *hboxCamLabels = nullptr,
                     *hboxFrontPics = nullptr, *hboxBackPics = nullptr, *hboxStatus = nullptr,
                     *lblDir = nullptr, *btnConfigDir = nullptr, *lblCams = nullptr, *lblFront = nullptr,
                     *lblBack = nullptr, *lblCurrentStatus = nullptr;

    static GtkWidget* front_cams[3] = {nullptr, nullptr, nullptr};
    static GtkWidget* back_cams[3]  = {nullptr, nullptr, nullptr};

    static GdkPixbuf* pxbscaled = nullptr;
    static GdkPixbuf* pixbuf = nullptr;
    static GError* err = nullptr;

    static char* workingDir = nullptr;
}

using namespace gui_state;

static void set_image_scaled(GtkWidget* img, const char* path)
{
    pixbuf = gdk_pixbuf_new_from_file(path, &err);
    g_assert_no_error(err);
    pxbscaled = gdk_pixbuf_scale_simple(pixbuf, 500, 300, GDK_INTERP_BILINEAR);
    gtk_image_set_from_pixbuf(GTK_IMAGE(img), pxbscaled);
    g_object_unref(pixbuf);
    g_object_unref(pxbscaled);
}

static void btnConfigDir_ConfigureDirectory(GtkWidget* /*widget*/, gpointer /*data*/)
{
    GtkWidget* dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;

    dialog = gtk_file_chooser_dialog_new("Open File", window,
        action,
        "_Cancel",
        GTK_RESPONSE_CANCEL,
        "_Open",
        GTK_RESPONSE_ACCEPT,
        NULL);

    gint res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser* chooser = GTK_FILE_CHOOSER(dialog);
        if (workingDir) {
            g_free(workingDir);
            workingDir = NULL;
        }
        workingDir = gtk_file_chooser_get_filename(chooser);
        gchar* label_text = g_strdup_printf("Save to: %s\t", workingDir);
        gtk_label_set_text(GTK_LABEL(lblDir), label_text);
        g_free(label_text);
    }

    gtk_widget_destroy(dialog);
}

export void gui_update_status(const char* text)
{
    if (lblCurrentStatus) {
        gtk_label_set_text(GTK_LABEL(lblCurrentStatus), text);
    }
}

export void gui_set_image(bool front, int index, const char* path)
{
    if (index < 0 || index >= 3) return;
    if (front) {
        set_image_scaled(front_cams[index], path);
    } else {
        set_image_scaled(back_cams[index], path);
    }
}

export const char* gui_get_working_dir()
{
    return workingDir;
}

static void* main_gtk_impl(void* /*arg*/)
{
    gtk_init(NULL, NULL);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

    if (workingDir) {
        g_free(workingDir);
        workingDir = NULL;
    }
    workingDir = g_strdup("/media/pi/SD CARD");
    lblDir = gtk_label_new("");
    {
        gchar* label_text = g_strdup_printf("Save to: %s\t", workingDir);
        gtk_label_set_text(GTK_LABEL(lblDir), label_text);
        g_free(label_text);
    }
    btnConfigDir = gtk_button_new_with_label("Configure Directory");
    g_signal_connect(btnConfigDir, "clicked", G_CALLBACK(btnConfigDir_ConfigureDirectory), NULL);
    lblCams = gtk_label_new("\t\t\t\t\t\t\t\tCAM 1\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tCAM2\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tCAM3");
    lblFront = gtk_label_new("Front");
    lblBack = gtk_label_new("Back ");
    lblCurrentStatus = gtk_label_new("Current Status: Idle");

    gtk_window_set_title(GTK_WINDOW(window), "CaptureGui");

    front_cams[0] = gtk_image_new();
    front_cams[1] = gtk_image_new();
    front_cams[2] = gtk_image_new();
    back_cams[0] = gtk_image_new();
    back_cams[1] = gtk_image_new();
    back_cams[2] = gtk_image_new();

    GdkPixbuf* pxb = gdk_pixbuf_new(0, 0, 8, 500, 300);
    gtk_image_set_from_pixbuf(GTK_IMAGE(front_cams[0]), pxb);
    gtk_image_set_from_pixbuf(GTK_IMAGE(front_cams[1]), pxb);
    gtk_image_set_from_pixbuf(GTK_IMAGE(front_cams[2]), pxb);
    gtk_image_set_from_pixbuf(GTK_IMAGE(back_cams[0]), pxb);
    gtk_image_set_from_pixbuf(GTK_IMAGE(back_cams[1]), pxb);
    gtk_image_set_from_pixbuf(GTK_IMAGE(back_cams[2]), pxb);

    hboxDir = gtk_box_new(0, 0);
    hboxCamLabels = gtk_box_new(0, 0);
    hboxFrontPics = gtk_box_new(0, 0);
    hboxBackPics = gtk_box_new(0, 0);
    hboxStatus = gtk_box_new(0, 0);
    vbox = gtk_vbox_new(0, 10);
    gtk_box_pack_end(GTK_BOX(hboxDir), btnConfigDir, 0, 0, 0);
    gtk_box_pack_end(GTK_BOX(hboxDir), lblDir, 0, 0, 0);
    gtk_box_pack_start(GTK_BOX(hboxCamLabels), lblCams, 0, 0, 0);
    gtk_box_pack_start(GTK_BOX(hboxFrontPics), lblFront, 0, 0, 0);
    gtk_box_pack_start(GTK_BOX(hboxFrontPics), front_cams[0], 0, 0, 10);
    gtk_box_pack_start(GTK_BOX(hboxFrontPics), front_cams[1], 0, 0, 10);
    gtk_box_pack_start(GTK_BOX(hboxFrontPics), front_cams[2], 0, 0, 10);
    gtk_box_pack_start(GTK_BOX(hboxBackPics), lblBack, 0, 0, 0);
    gtk_box_pack_start(GTK_BOX(hboxBackPics), back_cams[0], 0, 0, 10);
    gtk_box_pack_start(GTK_BOX(hboxBackPics), back_cams[1], 0, 0, 10);
    gtk_box_pack_start(GTK_BOX(hboxBackPics), back_cams[2], 0, 0, 10);
    gtk_box_pack_start(GTK_BOX(hboxStatus), lblCurrentStatus, 0, 0, 10);
    gtk_box_pack_start(GTK_BOX(vbox), hboxDir, 0, 0, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hboxCamLabels, 0, 0, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hboxFrontPics, 0, 0, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hboxBackPics, 0, 0, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hboxStatus, 0, 0, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    gtk_widget_show_all(window);
    gtk_main();
    return nullptr;
}

export void* gui_thread_entry(void* arg)
{
    return main_gtk_impl(arg);
}
