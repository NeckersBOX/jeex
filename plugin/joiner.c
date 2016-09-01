/*
 * "joiner.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
 *
 * Jeex is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Jeex is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
*/

#define _BSD_SOURCE
#include <dirent.h>
#ifndef _DIRENT_HAVE_D_TYPE
# error (jeex): You do not have the d_type variable in your dirent structure.
#endif
#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <stdlib.h>
#include <libintl.h>
#include <errno.h>
#include <locale.h>
#include "header.h"

static JeexTypes *plugin_jeex_types;
static GtkWidget *join_menu;

struct jeex_joiner
{
  char *output;
};

typedef struct
{
  GtkWidget *window;
  GtkWidget *table;
  GtkWidget *hbox;
  GtkWidget *cbox;
  GtkWidget *join;
  GtkWidget *cancel;
  GtkWidget *l_input, *m_input;
  GtkWidget *l_output, *m_output;
  GtkWidget *t_output, *e_output;
  GtkWidget *check_default_output;
  gboolean output;
  struct jeex_joiner *opt;
} GtkJoiner;

/* Disable widgets regarding the output in the window
 * to join the files
 */
static void *
join_disable_output (GtkWidget * widget, GtkJoiner * o)
{
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (o->check_default_output)))
    {
      gtk_widget_set_sensitive (o->l_output, FALSE);
      gtk_widget_set_sensitive (o->m_output, FALSE);
      gtk_widget_set_sensitive (o->e_output, FALSE);
      gtk_widget_set_sensitive (o->t_output, FALSE);
      o->output = FALSE;
    }
  else
    {
      gtk_widget_set_sensitive (o->l_output, TRUE);
      gtk_widget_set_sensitive (o->m_output, TRUE);
      gtk_widget_set_sensitive (o->e_output, TRUE);
      gtk_widget_set_sensitive (o->t_output, TRUE);
      o->output = TRUE;
    }

  return NULL;
}

/* Function to join the files */
static void *
join_file (GtkWidget * widget, GtkJoiner * w)
{
  FILE *fi, *fo;
  DIR *input;
  int len;
  char *s;
  unsigned char *tmp, *inp;
  struct dirent *dirent;
  GFileError error_type;

  inp = (unsigned char *) gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER (w->m_input));

  /* Gets output file name */
  if (w->output)
    {
      w->opt->output =
        g_strdup_printf ("%s/%s",
                         gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER
                                                              (w->m_output)),
                         (*(gtk_entry_get_text (GTK_ENTRY (w->e_output))) ==
                          '\0') ? "output" : gtk_entry_get_text (GTK_ENTRY (w->e_output)));
    }
  else
    {
      w->opt->output = g_strdup_printf ("%s/output", inp);
    }

  /* Store in an array of all the input file */
  if (!(input = opendir ((char *) inp)))
    {
      _error (_("Cannot open input folder."));
      return NULL;
    }

  if (!(fo = fopen (w->opt->output, "wb")))
    {
      error_type = g_file_error_from_errno (errno);
      file_error_control (error_type, WB_MODE);
      closedir (input);
      return NULL;
    }
  g_free (w->opt->output);

  /* Joins of files */
  while ((dirent = readdir (input)))
    {
      if (dirent->d_type == 0x08)
        {
          s = g_strdup_printf ("%s/%s", (char *) inp, dirent->d_name);
          if (!(fi = fopen (s, "rb")))
            {
              error_type = g_file_error_from_errno (errno);
              file_error_control (error_type, RB_MODE);
              continue;
            }
          g_free (s);

          fseek (fi, 0, SEEK_END);
          len = ftell (fi);
          rewind (fi);

          tmp = (unsigned char *) g_malloc (len * sizeof (unsigned char));
          fread (tmp, sizeof (unsigned char), len, fi);
          fclose (fi);

          fwrite (tmp, sizeof (unsigned char), len, fo);
          g_free (tmp);
        }
    }
  closedir (input);
  fclose (fo);

  _info (_("File has been joined!"), FALSE);

  return NULL;
}

/* Function to show a window to join more file */
static void *
join_window_file (GtkWidget * widget, void *data)
{
  GtkJoiner *_uwf;
  GtkWidget *image;
  extern GtkWidget *jeex_main_window;

  _uwf = (GtkJoiner *) g_try_malloc (sizeof (GtkJoiner));
  _uwf->opt = (struct jeex_joiner *) g_try_malloc0 (sizeof (struct jeex_joiner));

  /* Window */
  _uwf->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (_uwf->window), Jeex);
  gtk_window_set_default_size (GTK_WINDOW (_uwf->window), 350, 170);
  gtk_container_set_border_width (GTK_CONTAINER (_uwf->window), 5);
  gtk_window_set_modal (GTK_WINDOW (_uwf->window), TRUE);
  gtk_window_set_transient_for (GTK_WINDOW (_uwf->window), GTK_WINDOW (jeex_main_window));
  gtk_window_set_position (GTK_WINDOW (_uwf->window), GTK_WIN_POS_CENTER_ON_PARENT);

  /* Table */
  _uwf->table = gtk_table_new (5, 2, FALSE);
  gtk_container_add (GTK_CONTAINER (_uwf->window), _uwf->table);

  /* Box */
  _uwf->hbox = gtk_hbox_new (FALSE, 0);
  gtk_table_attach_defaults (GTK_TABLE (_uwf->table), _uwf->hbox, 0, 2, 4, 5);

  _uwf->cbox = gtk_hbutton_box_new ();
  gtk_box_pack_start (GTK_BOX (_uwf->hbox), _uwf->cbox, TRUE, TRUE, 5);

  /* Close Button */
  _uwf->cancel = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  GTK_WIDGET_SET_FLAGS (_uwf->cancel, GTK_CAN_DEFAULT);
  gtk_container_add (GTK_CONTAINER (_uwf->cbox), _uwf->cancel);

  /* Join Button */
  image = gtk_image_new_from_stock (GTK_STOCK_DND, GTK_ICON_SIZE_BUTTON);
  _uwf->join = gtk_button_new_with_label (_("Join"));
  gtk_button_set_image (GTK_BUTTON (_uwf->join), image);
  GTK_WIDGET_SET_FLAGS (_uwf->join, GTK_CAN_DEFAULT);
  gtk_widget_grab_default (_uwf->join);
  gtk_container_add (GTK_CONTAINER (_uwf->cbox), _uwf->join);

  /* Input */
  _uwf->l_input = gtk_label_new (_("Input Folder: "));
  gtk_table_attach (GTK_TABLE (_uwf->table), _uwf->l_input, 0, 1, 0, 1,
                    GTK_FILL, GTK_FILL | GTK_EXPAND, 0, 0);

  _uwf->m_input =
    gtk_file_chooser_button_new (_("Choose folder..."), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
  gtk_table_attach (GTK_TABLE (_uwf->table), _uwf->m_input, 1, 2, 0, 1,
                    GTK_FILL, GTK_FILL | GTK_EXPAND, 0, 0);

  /* Output */
  _uwf->l_output = gtk_label_new (_("Destination Folder: "));
  gtk_table_attach (GTK_TABLE (_uwf->table), _uwf->l_output, 0, 1, 1, 2,
                    GTK_FILL, GTK_FILL | GTK_EXPAND, 0, 0);

  _uwf->m_output =
    gtk_file_chooser_button_new (_("Choose folder..."), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
  gtk_table_attach (GTK_TABLE (_uwf->table), _uwf->m_output, 1, 2, 1, 2,
                    GTK_FILL, GTK_FILL | GTK_EXPAND, 0, 0);

  _uwf->t_output = gtk_label_new (_("Destination file name: "));
  gtk_table_attach (GTK_TABLE (_uwf->table), _uwf->t_output, 0, 1, 2, 3,
                    GTK_FILL, GTK_FILL | GTK_EXPAND, 0, 0);

  _uwf->e_output = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE (_uwf->table), _uwf->e_output, 1, 2, 2, 3,
                    GTK_FILL, GTK_FILL | GTK_EXPAND, 0, 0);

  _uwf->check_default_output = gtk_check_button_new_with_label (_("Use default Destination"));
  _uwf->output = TRUE;
  gtk_table_attach (GTK_TABLE (_uwf->table), _uwf->check_default_output, 0, 2,
                    3, 4, GTK_EXPAND, GTK_FILL | GTK_EXPAND, 0, 0);

  /* Signals */
  g_signal_connect (G_OBJECT (_uwf->cancel), "clicked", G_CALLBACK (destroy_this), _uwf->window);
  g_signal_connect (G_OBJECT (_uwf->check_default_output), "toggled",
                    G_CALLBACK (join_disable_output), _uwf);
  g_signal_connect (G_OBJECT (_uwf->join), "clicked", G_CALLBACK (join_file), _uwf);

  gtk_widget_show_all (_uwf->window);

  return NULL;
}

gboolean joiner_start (JeexTypes *jeex_types)
{
 GtkWidget *tools_menu;
 GtkAccelGroup *group;

 group = gtk_accel_group_new ();
 gtk_window_add_accel_group (GTK_WINDOW (jeex_types->main_window), group);

 plugin_jeex_types = jeex_types;
 tools_menu = jeex_types->menu->tools_menu.tools;

 join_menu =
    jeex_menu_label_with_stock_append (GTK_MENU (tools_menu), _("Join more files"),
                                       GTK_STOCK_DND, G_CALLBACK (join_window_file),
									   group, GDK_J, GDK_CONTROL_MASK, NULL);

 return TRUE;
}

void joiner_end (gboolean activated)
{
 if ( activated && GTK_IS_WIDGET (join_menu) )
      gtk_widget_destroy (join_menu);
}
