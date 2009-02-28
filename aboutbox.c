#include"aboutbox.h"
#include "FB_logo.xpm"

//static const char about_text[] = "Sound Stretcher Plugin\n\n
//By Florian Berger\n<florian.berger@jk.uni-linz.ac.at> \n\n (c) 2001";
static const char GPL_text[] = "\
This program is free software; you can redistribute it and/or modify \
it under the terms of the GNU General Public License as published by \
the Free Software Foundation; either version 2 of the License, or \
(at your option) any later version.\n\n\
This program is distributed in the hope that it will be useful, \
but WITHOUT ANY WARRANTY; without even the implied warranty of \
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \
GNU General Public License for more details.\n\n\
You should have received a copy of the GNU General Public License \
along with this program; if not, write to the Free Software \
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, \
USA.";
static const char copy_text[] = "Copyright (C) 2001\n\
Florian Berger\n<harpin_floh@yahoo.de>\n\
http://www.geocities.com/harpin_floh/home.html";
static const char title_text[] = APPNAME" - "GUITUNE_VERSION_STRING;


static GtkWidget * about_dialog  = NULL;



static gint about_destroy_cb(GtkWidget * w, GdkEventAny * e, gpointer data)
{
	gtk_widget_destroy(about_dialog);
	about_dialog = NULL;
	return TRUE;
}

static void about_ok_cb(GtkButton * button, gpointer data)
{
	gtk_widget_destroy(GTK_WIDGET(about_dialog));
	about_dialog = NULL;
}

void about_cb(void)
{
        GtkWidget * vbox, * text, * scrolltext, * button;
        GtkWidget * titlelabel, * copylabel;

        GdkPixbuf * logopix;
        GtkWidget * logo;

        GdkPixmap * FBlogopix;
        GdkBitmap * FBlogomask;
        GtkWidget * FBlogo;
        GtkWidget * copyhbox, * copy_rbox, * copy_lbox;

	GtkTextBuffer *text_buf;

	GdkScreen    *screen;
	GtkIconTheme *icon_theme;

	if (about_dialog != NULL)
            return;

	about_dialog = gtk_dialog_new();
        gtk_widget_show(about_dialog);

	screen = gtk_widget_get_screen (about_dialog);
	icon_theme = gtk_icon_theme_get_for_screen (screen);

        /* title logo */
	logopix = gtk_icon_theme_load_icon (icon_theme, "guitune_logo", 48,
					    0, NULL);
					    
        logo = gtk_image_new ();
	if (logopix)
	{
	    gtk_image_set_from_pixbuf (GTK_IMAGE (logo), logopix);
	    g_object_unref (logopix);
	}

        /* FB-logo */
        FBlogopix = gdk_pixmap_create_from_xpm_d(about_dialog->window, &FBlogomask,
                                                 NULL,
                                                 (gchar **) FB_logo_xpm);
        FBlogo = gtk_pixmap_new(FBlogopix,FBlogomask);


        gtk_signal_connect(GTK_OBJECT(about_dialog), "destroy",
			   GTK_SIGNAL_FUNC(about_destroy_cb), NULL);
	gtk_window_set_title(GTK_WINDOW(about_dialog), "About "APPNAME);


        /* labels */
        titlelabel = gtk_label_new(title_text);
        copylabel  = gtk_label_new(copy_text);
        gtk_label_set_justify(GTK_LABEL(copylabel),GTK_JUSTIFY_LEFT);

        copy_lbox = gtk_hbox_new(FALSE,0);
        copy_rbox = gtk_hbox_new(FALSE,0);
        gtk_box_pack_end  (GTK_BOX(copy_lbox), FBlogo,    FALSE, TRUE,  0);
        gtk_box_pack_start(GTK_BOX(copy_rbox), copylabel, FALSE, TRUE,  0);
        copyhbox = gtk_hbox_new(FALSE,0);
        gtk_box_pack_start(GTK_BOX(copyhbox), copy_lbox,    TRUE, TRUE,  5);
        gtk_box_pack_start(GTK_BOX(copyhbox), copy_rbox,    TRUE, TRUE,  5);

        vbox = gtk_vbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(about_dialog)->vbox), vbox,
			   TRUE, TRUE, 5);

        scrolltext = gtk_scrolled_window_new(NULL,NULL);
	text = gtk_text_view_new();
	text_buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));
	gtk_text_view_set_editable(GTK_TEXT_VIEW(text), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text), GTK_WRAP_WORD);
	gtk_text_buffer_set_text(text_buf, GPL_text, -1);
        scrolltext = gtk_scrolled_window_new(NULL,NULL);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolltext),
                                       GTK_POLICY_AUTOMATIC,
                                       GTK_POLICY_AUTOMATIC);
        gtk_container_add(GTK_CONTAINER(scrolltext),text);

        gtk_box_pack_start(GTK_BOX(vbox), logo, FALSE, TRUE, 5);
        gtk_box_pack_start(GTK_BOX(vbox), titlelabel, FALSE, TRUE, 5);
        gtk_box_pack_start(GTK_BOX(vbox), copyhbox, FALSE, TRUE, 5);
        gtk_box_pack_start(GTK_BOX(vbox), scrolltext, TRUE, TRUE, 5);
        gtk_container_set_border_width(GTK_CONTAINER(vbox), 8);
        gtk_widget_set_usize (scrolltext,-1,110);

	button = gtk_button_new_with_label("Close");
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(about_dialog)->action_area),
			   button, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(button), "clicked",
			   GTK_SIGNAL_FUNC(about_ok_cb), NULL);
	GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
	gtk_widget_grab_default(button);
	gtk_widget_show(button);
//        gtk_widget_set_usize (sndstretch_about_dialog,-1,-1);
	gtk_widget_show_all(about_dialog);
}
