
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <glib.h>
#include <glib-object.h>

#include <f2c.h>
#include <clapack.h>


static void calculate2Dimension(double *b){
	integer n = 2;
	integer nrhs = 1;
	double dl[1] = {1};
	double d[2] = {4,4};
	double du[1] = {1};
	int i;
	integer lda = n;
	integer ipiv = n;
	integer ldb = 2;
	integer info;
	dgtsv_(&n, &nrhs, dl, d, du, b, &ldb, &info);

	printf("info: %d\n", info);
	for (i= 0; i< n; i++ ) {
        printf("s[ %d ] = %f\n", i, b[ i ] );
    }
}

int main2(int argc, char* argv[])
{
    GtkWidget* window;
	double b[2] = {2,-2};
	GtkLabel *label;

    gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_usize(window, 300, 200);

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);   
    gtk_window_set_title(GTK_WINDOW(window), "GTK+ with VS2010");

    gtk_widget_show(window);     

	
	calculate2Dimension(b);
	b[0] = 4;
	b[1] = 4;
	calculate2Dimension(b);

    gtk_main();   
    return 0;
}