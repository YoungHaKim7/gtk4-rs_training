#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#if defined(__has_include)
#  if __has_include(<gphoto2/gphoto2-camera.h>) && __has_include(<gphoto2/gphoto2-context.h>)
#    include <gphoto2/gphoto2-camera.h>
#    include <gphoto2/gphoto2-context.h>
#    define HAVE_GPHOTO2 1
#  else
#    define HAVE_GPHOTO2 0
     typedef struct Camera Camera;
     typedef struct CameraList CameraList;
     typedef struct _GPContext GPContext;
     typedef struct _CameraFile CameraFile;
     typedef struct _CameraFilePath { char folder[256]; char name[256]; } CameraFilePath;
#  endif
#else
#  include <gphoto2/gphoto2-camera.h>
#  include <gphoto2/gphoto2-context.h>
#  define HAVE_GPHOTO2 1
#endif

#if defined(__has_include)
#  if __has_include(<wiringPi.h>)
#    include <wiringPi.h>
#    define HAVE_WIRINGPI 1
#  else
#    define HAVE_WIRINGPI 0
     static inline int wiringPiSetupGpio(void) { return 0; }
     static inline int digitalRead(int pin) { (void)pin; return 1; }
#  endif
#else
#  include <wiringPi.h>
#  define HAVE_WIRINGPI 1
#endif

#if defined(__has_include)
#  if __has_include(<gtk/gtk.h>)
#    include <gtk/gtk.h>
#    define HAVE_GTK 1
#  else
#    define HAVE_GTK 0
     typedef void GtkWidget;
     typedef void GdkPixbuf;
     typedef void GError;
     typedef void* gpointer;
#  endif
#else
#  include <gtk/gtk.h>
#  define HAVE_GTK 1
#endif
#include <pthread.h>

// GTK compatibility helpers (allow older code to build on newer GTK)
#ifndef GTK_ORIENTATION_HORIZONTAL
#define GTK_ORIENTATION_HORIZONTAL 0
#define GTK_ORIENTATION_VERTICAL 1
#endif
#ifndef gtk_vbox_new
#define gtk_vbox_new(homogeneous, spacing) gtk_box_new(GTK_ORIENTATION_VERTICAL, spacing)
#endif

// Forward declarations
static void* main_gtk(void* arg);
static void* cam_main(void* arg);
static void set_image_scaled(GtkWidget* img, const char* path);
static void detect_cams(void);
static int open_cam(Camera ** camera, const char *model, const char *port, GPContext *context);

    //program
    static int running = 1;
    //libgphoto2
static CameraList* list;
static Camera** cams;
static GPContext* context;
static const char *name, *value;
static int ret, count;
    static int pic = 0;
    static int front = 1;
    static const char* workingDir;
    //GTK
    static GtkWidget *window, *vbox,*hboxDir, *hboxCamLabels, *hboxFrontPics, *hboxBackPics, *hboxStatus, *lblDir, *btnConfigDir, *lblCams, *lblFront, *lblBack, *lblCurrentStatus;
    static GtkWidget *front_cams[3];
    static GtkWidget *back_cams[3];
    static GdkPixbuf *pxbscaled = NULL;
    static GdkPixbuf *pixbuf = NULL;
static GError* err = NULL;

#if !HAVE_GPHOTO2
// Provide no-op stubs to allow building without libgphoto2
#define GP_OK 0
#define GP_CAPTURE_IMAGE 0
typedef struct _GPPortInfoList GPPortInfoList;
typedef struct _CameraAbilitiesList CameraAbilitiesList;
typedef struct _CameraAbilities { int dummy; } CameraAbilities;
typedef struct _GPPortInfo { int dummy; } GPPortInfo;
static inline int gp_list_new(CameraList** l){ (void)l; return GP_OK; }
static inline void gp_list_reset(CameraList* l){ (void)l; }
static inline int gp_camera_autodetect(CameraList* l, GPContext* c){ (void)l;(void)c; return 0; }
static inline int gp_list_get_name(CameraList* l, int i, const char** n){ (void)l;(void)i;(void)n; return GP_OK; }
static inline int gp_list_get_value(CameraList* l, int i, const char** v){ (void)l;(void)i;(void)v; return GP_OK; }
static inline int gp_camera_new(Camera** c){ (void)c; return GP_OK; }
static inline int gp_abilities_list_new(CameraAbilitiesList** a){ (void)a; return GP_OK; }
static inline int gp_abilities_list_load(CameraAbilitiesList* a, GPContext* c){ (void)a;(void)c; return GP_OK; }
static inline int gp_abilities_list_lookup_model(CameraAbilitiesList* a, const char* m){ (void)a;(void)m; return GP_OK; }
static inline int gp_abilities_list_get_abilities(CameraAbilitiesList* a, int m, CameraAbilities* aa){ (void)a;(void)m;(void)aa; return GP_OK; }
static inline int gp_camera_set_abilities(Camera* c, CameraAbilities a){ (void)c;(void)a; return GP_OK; }
static inline int gp_port_info_list_new(GPPortInfoList** l){ (void)l; return GP_OK; }
static inline int gp_port_info_list_load(GPPortInfoList* l){ (void)l; return GP_OK; }
static inline int gp_port_info_list_count(GPPortInfoList* l){ (void)l; return GP_OK; }
static inline int gp_port_info_list_lookup_path(GPPortInfoList* l, const char* p){ (void)l;(void)p; return GP_OK; }
static inline int gp_port_info_list_get_info(GPPortInfoList* l, int p, GPPortInfo* i){ (void)l;(void)p;(void)i; return GP_OK; }
static inline int gp_camera_set_port_info(Camera* c, GPPortInfo i){ (void)c;(void)i; return GP_OK; }
static inline int gp_camera_capture(Camera* c, int cap, CameraFilePath* path, GPContext* ctx){ (void)c;(void)cap;(void)path;(void)ctx; return GP_OK; }
static inline int gp_camera_file_get(Camera* c, const char* f, const char* n, int t, CameraFile* file, GPContext* ctx){ (void)c;(void)f;(void)n;(void)t;(void)file;(void)ctx; return GP_OK; }
static inline int gp_camera_file_delete(Camera* c, const char* f, const char* n, GPContext* ctx){ (void)c;(void)f;(void)n;(void)ctx; return GP_OK; }
static inline void gp_file_free(CameraFile* f){ (void)f; }
static inline int gp_file_new_from_fd(CameraFile** file, int fd){ (void)file;(void)fd; return GP_OK; }
static inline int gp_camera_exit(Camera* c, GPContext* ctx){ (void)c;(void)ctx; return GP_OK; }
static inline void gp_camera_free(Camera* c){ (void)c; }
#endif

static void btnConfigDir_ConfigureDirectory(GtkWidget* widget, gpointer data) {
#if HAVE_GTK
    GtkWidget *dialog;
GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
gint res;

                dialog = gtk_file_chooser_dialog_new ("Open File", window,
                                      action,
                                      "_Cancel",
                                      GTK_RESPONSE_CANCEL,
                                      "_Open",
                                      GTK_RESPONSE_ACCEPT,
                                      NULL);

res = gtk_dialog_run (GTK_DIALOG (dialog));
if (res == GTK_RESPONSE_ACCEPT)
  {
    char *filename;
    GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
    workingDir = gtk_file_chooser_get_filename (chooser);
  }

  //gtk_label_set_text((GTK_LABEL)lblDir, workingDir);

gtk_widget_destroy (dialog);
#else
    (void)widget; (void)data;
#endif
}

void* main_gtk(void* arg) {
#if HAVE_GTK
    (void)arg;
    //GTK WIDGET QUEUE REDRAW WOULD REDRAW AT APPROPRIATE TIME


    //INIT GTK
    gtk_init(NULL, NULL);

    //SETUP WINDOW
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    //button1=gtk_button_new_with_label("Click me");
    //gtk_button_set_label(GTK_BUTTON(button1), "click me 1");
    g_signal_connect(window,"delete-event", G_CALLBACK(gtk_main_quit), NULL);

    workingDir = "/media/pi/SD CARD";
    lblDir = gtk_label_new("Save to: dir\t");
    btnConfigDir = gtk_button_new_with_label("Configure Directory");
    g_signal_connect(btnConfigDir, "clicked", G_CALLBACK(btnConfigDir_ConfigureDirectory), NULL);
    lblCams = gtk_label_new("\t\t\t\t\t\t\t\tCAM 1\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tCAM2\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tCAM3");
    lblFront = gtk_label_new("Front");
    lblBack = gtk_label_new("Back ");
    lblCurrentStatus = gtk_label_new("Current Status: Idle");

    //SET WINDOW SIZE AND TITLE
    //gtk_widget_set_size_request(window, 600, 400);
    gtk_window_set_title(GTK_WINDOW(window), "CaptureGui");

    //RESIZE IMAGES
    //image
    front_cams[0] = gtk_image_new();
    front_cams[1] = gtk_image_new();
    front_cams[2] = gtk_image_new();
    back_cams[0] = gtk_image_new();
    back_cams[1] = gtk_image_new();
    back_cams[2] = gtk_image_new();

    GdkPixbuf* pxb;
    pxb = gdk_pixbuf_new(0, 0, 8, 500, 300);
    gtk_image_set_from_pixbuf(GTK_IMAGE(front_cams[0]), pxb);
    gtk_image_set_from_pixbuf(GTK_IMAGE(front_cams[1]), pxb);
    gtk_image_set_from_pixbuf(GTK_IMAGE(front_cams[2]), pxb);
    gtk_image_set_from_pixbuf(GTK_IMAGE(back_cams[0]), pxb);
    gtk_image_set_from_pixbuf(GTK_IMAGE(back_cams[1]), pxb);
    gtk_image_set_from_pixbuf(GTK_IMAGE(back_cams[2]), pxb);

    //g_print("1");

    //front_cams[0] = gtk_image_new_from_file("/media/pi/SD CARD/cam-1_0_a_front.jpg");
    //set_image_scaled(front_cams[0], "/media/pi/SD CARD/cam-1_1_a_front.jpg");
    //set_image_scaled(front_cams[1], "/media/pi/SD CARD/cam-1_1_a_front.jpg");
    //set_image_scaled(front_cams[2], "/media/pi/SD CARD/cam-1_1_b_back.jpg");

    //PACK
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


    //gtk_box_pack_start(GTK_BOX(hbox), front_cams[0], 0, 0, 0);
    //gtk_box_pack_start(GTK_BOX(hbox), front_cams[1], 0, 0, 0);
    //gtk_box_pack_start(GTK_BOX(hbox), front_cams[2], 0, 0, 0);
    //gtk_box_pack_start(GTK_BOX(hbox), back_cams[0], 0, 0, 0);
    //gtk_box_pack_start(GTK_BOX(hbox), back_cams[1], 0, 0, 0);
    //gtk_box_pack_start(GTK_BOX(hbox), back_cams[2], 0, 0, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    //ADD ELEMENTS TO GUI
    //gtk_container_add(GTK_CONTAINER(window), image1);
    //gtk_container_add(GTK_CONTAINER(window), image2);
    //image2 = gtk_image_new_from_file("/media/pi/SD CARD/cam-1_0_b_back.jpg");
    //gtk_container_add(GTK_CONTAINER(window), image2);

    //SHOW GUI
    gtk_widget_show_all(window);
    gtk_main();

    return NULL;
#else
    (void)arg; (void)window; (void)vbox; (void)hboxDir; (void)hboxCamLabels; (void)hboxFrontPics; (void)hboxBackPics; (void)hboxStatus;
    (void)lblDir; (void)btnConfigDir; (void)lblCams; (void)lblFront; (void)lblBack; (void)lblCurrentStatus;
    (void)front_cams; (void)back_cams;
    fprintf(stderr, "GTK not available. Running headless.\n");
    while (running == 1) { usleep(100000); }
    return NULL;
#endif
}

void set_image_scaled(GtkWidget* img, const char* path) {
#if HAVE_GTK
    pixbuf = gdk_pixbuf_new_from_file(path, &err);
    g_assert_no_error(err);
    pxbscaled = gdk_pixbuf_scale_simple(pixbuf, 500, 300, GDK_INTERP_BILINEAR);
    gtk_image_set_from_pixbuf(GTK_IMAGE(img), pxbscaled);
    g_object_unref(pixbuf);
    g_object_unref(pxbscaled);
#else
    (void)img; (void)path;
#endif
}

void* cam_main(void* arg) {
    (void)arg;
    while (running == 1) {
    if (digitalRead(4) == 0)
        {
            printf("taking pics of %s item %i\n", (front == 1) ? "front" : "back", pic);
            for (int i = 0; i < count; i++)
            {
                int fd, retval;
                CameraFile *file;
                CameraFilePath cfPath;
                strcpy(cfPath.folder, "/");
                strcpy(cfPath.name, "foo1.jpg");
                printf("Capturing cam%i...\n", i + 1);
                #if HAVE_GPHOTO2
                int res = gp_camera_capture(cams[i], GP_CAPTURE_IMAGE, &cfPath, context);
                #else
                int res = 0;
                (void)cfPath; (void)context;
                #endif
                //printf(gp_port_result_as_string(res));
                printf("capture result: %i\n", res);
                //Camera won't take pic if busy and will continue to program end
                char buf[256];
                snprintf(buf, sizeof(buf), "%s/cam-%i_%i_%s.jpg", workingDir, i + 1, pic, (front == 1) ? "a_front" : "b_back"); //a_ to make front come before back otherwise systems will order incorrectly
                fd = open(buf, O_CREAT | O_WRONLY, 0644);
                #if HAVE_GPHOTO2
                retval = gp_file_new_from_fd(&file, fd);
                retval = gp_camera_file_get(cams[i], cfPath.folder, cfPath.name, GP_FILE_TYPE_NORMAL, file, context);
                retval = gp_camera_file_delete(cams[i], cfPath.folder, cfPath.name, context);
                gp_file_free(file);
                #else
                (void)retval; (void)file;
                write(fd, "\0", 1);
                #endif
                //debug
                //if (front == 1 && i == 0)
                //  set_image_scaled(front_cams[0], buf);
                if (front == 1)
                    set_image_scaled(front_cams[i], buf);
                else
                    set_image_scaled(back_cams[i], buf);
            }
            if (front == 1)
                front = 0;
            else
            {
                front = 1;
                pic += 1;
            }
            printf("pics taken...\n");
        }
    }
    return NULL;
}

int main(int argc, char **argv)
{
    //Kill any processes using cams
    system("pkill -f gphoto2");

    //Wiring pi init
    wiringPiSetupGpio();

    //Init
    #if HAVE_GPHOTO2
    context = gp_context_new();
    #else
    context = NULL;
    #endif

    detect_cams();


    pthread_t logic_thread_handle, gui_thread_handle;
    pthread_create(&logic_thread_handle, NULL, cam_main, NULL);
    pthread_create(&gui_thread_handle, NULL, main_gtk, NULL);
    pthread_join(gui_thread_handle, 0);
    pthread_join(logic_thread_handle, 0);

    //Deinit
    for (int i = 0; i < count; i++)
    {
        gp_camera_exit(cams[i], context);
        gp_camera_free(cams[i]);
    }
    return 0;
}

void detect_cams(void) {
    //Detecting all cameras and loading them into mem
        //Detecting all cameras
    #if HAVE_GPHOTO2
    ret = gp_list_new(&list);
    if (ret < GP_OK) return;
    gp_list_reset(list);
    count = gp_camera_autodetect(list, context);
    if (count < 1) {
        printf("No cameras detected.\n");
        count = 0;
        return;
    }


    //Open all cameras
    printf("Number of cameras: %d\n", count);
    cams = calloc((size_t)count, sizeof(Camera*));
    for (int i = 0; i < count; i++)
    {
        gp_list_get_name(list, i, &name);
        gp_list_get_value(list, i, &value);
        ret = open_cam(&cams[i], name, value, context);
        if (ret < GP_OK)
            fprintf(stderr, "Camera %s on port %s failed to open\n", name, value);
    }
    #else
    count = 0;
    #endif
}

int open_cam(Camera ** camera, const char *model, const char *port, GPContext *context) {
    #if !HAVE_GPHOTO2
    (void)camera; (void)model; (void)port; (void)context;
    return 0;
    #else
    GPPortInfoList      *portinfolist = NULL;
    CameraAbilitiesList *abilities = NULL;
    int     ret, m, p;
    CameraAbilities a;
    GPPortInfo  pi;

    ret = gp_camera_new (camera);
    if (ret < GP_OK) return ret;

    if (!abilities) {
        /* Load all the camera drivers we have... */
        ret = gp_abilities_list_new (&abilities);
        if (ret < GP_OK) return ret;
        ret = gp_abilities_list_load (abilities, context);
        if (ret < GP_OK) return ret;
    }

    /* First lookup the model / driver */
        m = gp_abilities_list_lookup_model (abilities, model);
    if (m < GP_OK) return m;
        ret = gp_abilities_list_get_abilities (abilities, m, &a);
    if (ret < GP_OK) return ret;
        ret = gp_camera_set_abilities (*camera, a);
    if (ret < GP_OK) return ret;

    if (!portinfolist) {
        /* Load all the port drivers we have... */
        ret = gp_port_info_list_new (&portinfolist);
        if (ret < GP_OK) return ret;
        ret = gp_port_info_list_load (portinfolist);
        if (ret < 0) return ret;
        ret = gp_port_info_list_count (portinfolist);
        if (ret < 0) return ret;
    }

    /* Then associate the camera with the specified port */
        p = gp_port_info_list_lookup_path (portinfolist, port);
        switch (p) {
        case GP_ERROR_UNKNOWN_PORT:
                fprintf (stderr, "The port you specified "
                        "('%s') can not be found. Please "
                        "specify one of the ports found by "
                        "'gphoto2 --list-ports' and make "
                        "sure the spelling is correct "
                        "(i.e. with prefix 'serial:' or 'usb:').",
                                port);
                break;
        default:
                break;
        }
        if (p < GP_OK) return p;

        ret = gp_port_info_list_get_info (portinfolist, p, &pi);
        if (ret < GP_OK) return ret;
        ret = gp_camera_set_port_info (*camera, pi);
        if (ret < GP_OK) return ret;
    return GP_OK;
    #endif
}
