/*
 * MAIN.C File
 * Main file that contains declarations of GTK wrapper functions
 * Tested 23/11/2023 - on Ubuntu with GTK 3.0 (will upgrade to 4.0 soon)
 */
#include <gtk/gtk.h>
#define OL_NULLGTKWIN (OlGtkWindow){.handle = NULL, .app = NULL}

typedef struct
{
    GtkWidget* handle;
    GtkApplication* app; // handle to the app it belongs
} OlGtkWindow;

typedef struct
{
    GtkWidget* handle;
    int width, height;
} OlGtkCanvas;

GtkApplication* ol_new_gtkapp(char* name)
{
    return gtk_application_new(name, G_APPLICATION_FLAGS_NONE);
}

OlGtkWindow ol_new_gtkwin(GtkApplication* app, char* name, int w, int h)
{
    if(!app) return OL_NULLGTKWIN;
    OlGtkWindow win;
    win.app = app;
    win.handle = gtk_application_window_new(app);
    gtk_window_set_default_size(GTK_WINDOW(win.handle), w, h);
    gtk_window_set_title(GTK_WINDOW(win.handle), name);
    gtk_widget_show_all(win.handle);
    return win;
}

OlGtkCanvas ol_new_gtkcanvas(OlGtkWindow win, int width, int height)
{
    OlGtkCanvas canvas;
    canvas.handle = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(win.handle), canvas.handle);
    if(height && width) {
        gtk_widget_set_size_request(canvas.handle, width, height);
        canvas.width = width, canvas.height = height;
    }
    return canvas;
}