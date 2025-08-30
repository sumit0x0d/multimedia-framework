#include <math.h>

#include <gtk/gtk.h>
#include <GL/gl.h>
#include <gtk/gtkglarea.h>


int init (GtkWidget *widget)
{
	if (gtk_gl_area_make_current (GTK_GL_AREA(widget))) {
		glViewport(0,0, widget->allocation.width, widget->allocation.height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,100, 100,0, -1,1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	return TRUE;
}

int draw (GtkWidget *widget, GdkEventExpose *event)
{
	if (event->count > 0) return TRUE;

	if (gtk_gl_area_make_current (GTK_GL_AREA(widget))) {
		glClearColor(0,0,0,1);
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(1,1,1);
		glBegin(GL_TRIANGLES);
		glVertex2f(10,10);
		glVertex2f(10,90);
		glVertex2f(90,90);
		glEnd();
		gtk_gl_area_swap_buffers (GTK_GL_AREA(widget));
	}

	return TRUE;
}

int reshape (GtkWidget *widget, GdkEventConfigure *event)
{
	if (gtk_gl_area_make_current (GTK_GL_AREA(widget))) {
		glViewport(0,0, widget->allocation.width, widget->allocation.height);
	}

	return TRUE;
}

int main (int argc, char **argv)
{
	GtkWidget *window, *glarea;

	int attrlist[] = {
		GDK_GL_RGBA,
		GDK_GL_RED_SIZE,1,
		GDK_GL_GREEN_SIZE,1,
		GDK_GL_BLUE_SIZE,1,
		GDK_GL_DOUBLEBUFFER,
		GDK_GL_NONE };

	gtk_init (&argc, &argv);
	if (gdk_gl_query () == FALSE) return 0;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW(window), "Simple");
	gtk_container_set_border_width (GTK_CONTAINER(window), 10);
	g_signal_connect (window, "delete_event",
			  G_CALLBACK (gtk_main_quit), NULL);

	/* You should always delete gtk_gl_area widgets before exit or else
	   GLX contexts are left undeleted, this may cause problems (=core dump)
	   in some systems.
	   Destroy method of objects is not automatically called on exit.
	   You need to manually enable this feature. Do gtk_quit_add_destroy()
	   for all your top level windows unless you are certain that they get
	   destroy signal by other means. */

	gtk_quit_add_destroy (1, GTK_OBJECT(window));

	glarea = GTK_WIDGET(gtk_gl_area_new (attrlist));
	gtk_widget_set_size_request (GTK_WIDGET(glarea), 100, 100);

	gtk_widget_set_events (GTK_WIDGET(glarea), GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);

	g_signal_connect (glarea, "expose_event", G_CALLBACK(draw), NULL);
	g_signal_connect (glarea, "configure_event", G_CALLBACK(reshape), NULL);
	g_signal_connect (glarea, "realize", G_CALLBACK(init), NULL);

	gtk_container_add (GTK_CONTAINER(window), GTK_WIDGET(glarea));
	gtk_widget_show (GTK_WIDGET(glarea));
	gtk_widget_show (GTK_WIDGET(window));

	gtk_main ();
	return 0;
}
