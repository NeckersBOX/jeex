/*
 * "notebook-manage.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <glib/gi18n.h>
#include <libintl.h>
#include <locale.h>
#include "header.h"

static void *
update_box (GtkWidget * widget, void *data)
{
  _update (NULL, NULL);
  gtk_widget_hide ((GtkWidget *) data);
  return NULL;
}

static void *
cancel_box (GtkWidget * widget, void *data)
{
  gtk_widget_hide ((GtkWidget *) data);
  return NULL;
}

/* Make update field */
static void
notebook_make_update_field (GtkWidget * table, GtkWidget ** update)
{
  GtkWidget *label, *button, *icon, *vbox, *hbox;
  static GdkColor color;

  /* Make event box */
  *update = gtk_event_box_new ();
  gtk_table_attach (GTK_TABLE (table), *update, 0, 1, 0, 1, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 1);
  gdk_color_parse ("#bfd7de", &color);
  gtk_widget_modify_bg (*update, GTK_STATE_NORMAL, &color);

  /* Make horizontal box */
  hbox = gtk_hbox_new (FALSE, 2);
  gtk_container_add (GTK_CONTAINER (*update), hbox);
  gtk_widget_show (hbox);

  /* Icon */
  vbox = gtk_vbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (hbox), vbox, FALSE, TRUE, 0);
  gtk_widget_show (vbox);

  icon = gtk_image_new_from_stock (GTK_STOCK_DIALOG_WARNING, GTK_ICON_SIZE_DIALOG);
  gtk_box_pack_start (GTK_BOX (vbox), icon, FALSE, TRUE, 2);
  gtk_widget_show (icon);

  /* Text */
  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label),
                        _("<big><b>File has been modified</b></big>\n\n"
                          "This file has been modified by another program.\n"
                          "Press <b>Cancel</b> not to show this warning or\n"
                          "<b>Update</b> to update the text."));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 4);
  gtk_widget_show (label);

  /* Button */
  vbox = gtk_vbutton_box_new ();
  gtk_button_box_set_layout (GTK_BUTTON_BOX (vbox), GTK_BUTTONBOX_SPREAD);
  gtk_box_pack_start (GTK_BOX (hbox), vbox, TRUE, TRUE, 0);
  gtk_widget_show (vbox);

  button = gtk_button_new_from_stock (GTK_STOCK_REFRESH);
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, TRUE, 0);
  gtk_widget_show (button);
  g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (update_box), (void *) *update);

  button = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, TRUE, 0);
  gtk_widget_show (button);
  g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (cancel_box), (void *) *update);
}

/* Make insert field */
static void
notebook_make_insert_field (GtkTextInsert * insert)
{
  /* Insert Field */
  insert->l_insert = gtk_label_new (_("Text: "));
  gtk_table_attach (GTK_TABLE (insert->table), insert->l_insert, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (insert->l_insert);

  insert->e_insert = gtk_entry_new ();
  gtk_widget_set_usize (insert->e_insert, 80, 20);
  gtk_table_attach (GTK_TABLE (insert->table), insert->e_insert, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (insert->e_insert);

  insert->i_insert = gtk_image_new_from_stock (GTK_STOCK_JUMP_TO, GTK_ICON_SIZE_MENU);
  gtk_widget_show (insert->i_insert);
  insert->b_insert = gtk_button_new ();
  gtk_button_set_image (GTK_BUTTON (insert->b_insert), insert->i_insert);
  gtk_button_set_relief (GTK_BUTTON (insert->b_insert), GTK_RELIEF_NONE);
  gtk_widget_show (insert->b_insert);
  gtk_table_attach (GTK_TABLE (insert->table), insert->b_insert, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 0, 0);

  /* Options */
  insert->o_hex = gtk_radio_button_new_with_label (NULL, "Hex");
  gtk_widget_show (insert->o_hex);
  gtk_table_attach (GTK_TABLE (insert->table), insert->o_hex, 4, 5, 0, 1,
                    GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

  insert->o_oct = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (insert->o_hex), "Oct");
  gtk_widget_show (insert->o_oct);
  gtk_table_attach (GTK_TABLE (insert->table), insert->o_oct, 5, 6, 0, 1,
                    GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

  insert->o_dec = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (insert->o_hex), "Dec");
  gtk_widget_show (insert->o_dec);
  gtk_table_attach (GTK_TABLE (insert->table), insert->o_dec, 6, 7, 0, 1,
                    GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

  insert->o_bin = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (insert->o_hex), "Bin");
  gtk_widget_show (insert->o_bin);
  gtk_table_attach (GTK_TABLE (insert->table), insert->o_bin, 7, 8, 0, 1,
                    GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

  insert->o_str =
    gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (insert->o_hex), "Ascii");
  gtk_widget_show (insert->o_str);
  gtk_table_attach (GTK_TABLE (insert->table), insert->o_str, 8, 9, 0, 1,
                    GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

  insert->format = HEX;

  /* Signals */
  g_signal_connect (G_OBJECT (insert->b_close), "clicked", G_CALLBACK (insert_d), NULL);
  g_signal_connect (G_OBJECT (insert->b_insert), "clicked", G_CALLBACK (insert_text), insert);

  g_signal_connect (G_OBJECT (insert->o_hex), "clicked", G_CALLBACK (insert_format), (void *) HEX);
  g_signal_connect (G_OBJECT (insert->o_oct), "clicked", G_CALLBACK (insert_format), (void *) OCT);
  g_signal_connect (G_OBJECT (insert->o_dec), "clicked", G_CALLBACK (insert_format), (void *) DEC);
  g_signal_connect (G_OBJECT (insert->o_bin), "clicked", G_CALLBACK (insert_format), (void *) BIN);
  g_signal_connect (G_OBJECT (insert->o_str), "clicked", G_CALLBACK (insert_format), (void *) ASCII);
}

/* Make regex field */
static void
notebook_make_regex_field (GtkTextRegex * wregex)
{
  /* Regex Field */
  wregex->l_regex = gtk_label_new (_("Regex: "));
  gtk_table_attach (GTK_TABLE (wregex->table), wregex->l_regex, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (wregex->l_regex);

  wregex->e_regex = gtk_entry_new ();
  gtk_widget_set_usize (wregex->e_regex, 160, 20);
  gtk_table_attach (GTK_TABLE (wregex->table), wregex->e_regex, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (wregex->e_regex);

  /* Find with regex */
  wregex->i_find = gtk_image_new_from_stock (GTK_STOCK_FIND, GTK_ICON_SIZE_MENU);
  gtk_widget_show (wregex->i_find);
  wregex->b_find = gtk_button_new ();
  gtk_button_set_image (GTK_BUTTON (wregex->b_find), wregex->i_find);
  gtk_button_set_relief (GTK_BUTTON (wregex->b_find), GTK_RELIEF_NONE);
  gtk_widget_show (wregex->b_find);
  gtk_table_attach (GTK_TABLE (wregex->table), wregex->b_find, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 0, 0);

  /* Clean Regex Entry */
  wregex->i_clear = gtk_image_new_from_stock (GTK_STOCK_CLEAR, GTK_ICON_SIZE_MENU);
  gtk_widget_show (wregex->i_clear);
  wregex->b_clear = gtk_button_new ();
  gtk_button_set_image (GTK_BUTTON (wregex->b_clear), wregex->i_clear);
  gtk_button_set_relief (GTK_BUTTON (wregex->b_clear), GTK_RELIEF_NONE);
  gtk_widget_show (wregex->b_clear);
  gtk_table_attach (GTK_TABLE (wregex->table), wregex->b_clear, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 0, 0);

  /* Signals */
  g_signal_connect (G_OBJECT (wregex->b_close), "clicked", G_CALLBACK (regex_close_bar), NULL);
  g_signal_connect (G_OBJECT (wregex->b_find), "clicked", G_CALLBACK (regex_search_text), wregex);
  g_signal_connect (G_OBJECT (wregex->b_clear), "clicked", G_CALLBACK (regex_entry_clear), wregex);
}

/* Generates textview field */
static void
notebook_make_textview_field (GtkWidget * table_notebook, int current)
{
  GtkTextIter start, end;
  GtkWidget *scroll;
  GtkTextView *ptr;

  /* Viewport */
  viewport = gtk_viewport_new (NULL, NULL);
  gtk_widget_set_size_request (viewport, 440, -1);
  gtk_table_attach_defaults (GTK_TABLE (table_notebook), viewport, 0, 1, 2, 3);
  gtk_widget_show (viewport);

  /* Scrollbar */
  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add (GTK_CONTAINER (viewport), scroll);
  gtk_widget_show (scroll);

  /* Textview */
  notebook->textview[current] = gtk_text_view_new ();
  gtk_widget_set_double_buffered (notebook->textview[current], TRUE);
  ptr = GTK_TEXT_VIEW (notebook->textview[current]);
  notebook->buffer[current] = gtk_text_view_get_buffer (ptr);
  gtk_widget_show (notebook->textview[current]);
  gtk_widget_set_sensitive (notebook->textview[current], FALSE);
  gtk_text_view_set_accepts_tab (ptr, FALSE);
  gtk_container_add (GTK_CONTAINER (scroll), notebook->textview[current]);
  gtk_text_view_set_justification (ptr, GTK_JUSTIFY_LEFT);
  gtk_text_view_set_editable (ptr, FALSE);
  gtk_text_view_set_wrap_mode (ptr, GTK_WRAP_NONE);
  gtk_text_view_set_cursor_visible (ptr, TRUE);
  gtk_text_view_set_left_margin (ptr, 5);
  gtk_text_buffer_get_bounds (GTK_TEXT_BUFFER (notebook->buffer[current]), &start, &end);
  gtk_text_buffer_delete (GTK_TEXT_BUFFER (notebook->buffer[current]), &start, &end);
  textview->find_type = FALSE;

  /* Creating tag for the text highlight when one tries */
  textview->tag =
    gtk_text_buffer_create_tag (notebook->buffer[current], "find_text",
                                "background", "#000000", "foreground",
                                "#FFFFFF", "weight", PANGO_WEIGHT_BOLD, NULL);
}

static void
tab_modified (void)
{
  char *name, *tmp;
  extern JeexPreferences *preferences;
  extern JeexFileInfo *file[64];
  int i;

  /* Checks that the variable _ncurrent(file)->name is set
   * usually, when you are open a file, this is always true condition.
   */
  if (_ncurrent (file)->name && !(_ncurrent (file)->new))
    {
      _ncurrent (file)->modified = TRUE;

      /* Gets file name */
      name = g_path_get_basename (_ncurrent (file)->name);

      /* Resize string if length > preferences->max_tabchars */
      if ((i = strlen (name)) > preferences->max_tabchars)
        {
          tmp = g_strndup (name, preferences->max_tabchars);
          g_free (name);
          name = g_strdup_printf ("*%s...", tmp);
          g_free (tmp);
        }
      else
        {
          _ncurrent (file)->modified = TRUE;

          tmp = g_strdup_printf ("*%s", name);
          g_free (name);
          name = g_strdup (tmp);
          g_free (tmp);
        }

      gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook->notebook),
                                  _ncurrent (notebook->table), gtk_label_new (name));
      g_free (name);
    }
}

/* Makes a new notebook page */
void *
make_new_notebook_page (char *_label)
{
  extern gboolean check_p;
  extern JeexPreferences *preferences;
  extern JeexFileInfo *file[64];
  register int i;
  char *label = g_strdup (_label);

  /* Checks which has not been exceeded the 64 open tabs */
  if (notebook->n_page > 64)
    {
      _error (_
              ("<big><b>Too many tabs open!!</b></big>\n"
               "\nJeex cannot handle more than 64 tabs at once!"));
      return NULL;
    }

  if (check_p)
    {
      delete_bookmarks ();

      notebook->current = gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook->notebook));
    }

  /* Table contains the text field and info on current byte */
  _ncurrent (notebook->table) = gtk_table_new (4, 1, FALSE);
  gtk_widget_show (_ncurrent (notebook->table));

  /* Insertion bar */
  _ncurrent (notebook->insert) = (GtkTextInsert *) g_malloc (sizeof (GtkTextInsert));

  _ncurrent (notebook->insert)->table = gtk_table_new (1, 9, FALSE);
  gtk_table_attach (GTK_TABLE (_ncurrent (notebook->table)),
                    _ncurrent (notebook->insert)->table, 0, 1, 1, 2,
                    GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

  /*  Button to close the bar */
  _ncurrent (notebook->insert)->i_close = gtk_image_new_from_stock (GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU);
  gtk_widget_show (_ncurrent (notebook->insert)->i_close);
  _ncurrent (notebook->insert)->b_close = gtk_button_new ();
  gtk_button_set_image (GTK_BUTTON (_ncurrent (notebook->insert)->b_close),
                        _ncurrent (notebook->insert)->i_close);
  gtk_widget_show (_ncurrent (notebook->insert)->b_close);
  gtk_button_set_relief (GTK_BUTTON (_ncurrent (notebook->insert)->b_close), GTK_RELIEF_NONE);
  gtk_table_attach (GTK_TABLE (_ncurrent (notebook->insert)->table),
                    _ncurrent (notebook->insert)->b_close, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);

  /* Making various fields */
  notebook_make_insert_field (_ncurrent (notebook->insert));
  notebook_make_textview_field (_ncurrent (notebook->table), notebook->current);
  notebook_make_update_field (_ncurrent (notebook->table), &(_ncurrent (notebook->update)));

  /* Regex bar */
  _ncurrent (notebook->regex) = (GtkTextRegex *) g_malloc (sizeof (GtkTextRegex));

  _ncurrent (notebook->regex)->table = gtk_table_new (1, 5, FALSE);
  gtk_table_attach (GTK_TABLE (_ncurrent (notebook->table)),
                    _ncurrent (notebook->regex)->table, 0, 1, 3, 4,
                    GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

  /*  Button to close the bar */
  _ncurrent (notebook->regex)->i_close = gtk_image_new_from_stock (GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU);
  gtk_widget_show (_ncurrent (notebook->regex)->i_close);
  _ncurrent (notebook->regex)->b_close = gtk_button_new ();
  gtk_button_set_image (GTK_BUTTON (_ncurrent (notebook->regex)->b_close),
                        _ncurrent (notebook->regex)->i_close);
  gtk_widget_show (_ncurrent (notebook->regex)->b_close);
  gtk_button_set_relief (GTK_BUTTON (_ncurrent (notebook->regex)->b_close), GTK_RELIEF_NONE);
  gtk_table_attach (GTK_TABLE (_ncurrent (notebook->regex)->table),
                    _ncurrent (notebook->regex)->b_close, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);

  notebook_make_regex_field (_ncurrent (notebook->regex));

  /* Making page title */
  if ((i = strlen (label)) > preferences->max_tabchars)
    {
      label = g_strdup_printf ("%s...", g_strndup (label, preferences->max_tabchars));
    }
  _ncurrent (notebook->page) = gtk_label_new (label);
  gtk_widget_show (_ncurrent (notebook->page));

  /* Append page to notebook */
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook->notebook),
                            _ncurrent (notebook->table), _ncurrent (notebook->page));

  /* Font to use in text field */
  gtk_widget_modify_font (_ncurrent (notebook->textview), textview->font);

  /* Making file structure */
  _ncurrent (file) = (JeexFileInfo *) g_malloc (sizeof (JeexFileInfo));
  _ncurrent (file)->bookmark.n = 0;

  notebook->n_page++;

  /* Signals */
  g_signal_connect_object (G_OBJECT (_ncurrent (notebook->buffer)), "changed",
                           G_CALLBACK (update_status_bar), NULL, G_CONNECT_SWAPPED);
  g_signal_connect_object (G_OBJECT (_ncurrent (notebook->buffer)), "changed",
                           G_CALLBACK (tab_modified), NULL, G_CONNECT_SWAPPED);
  g_signal_connect_object (G_OBJECT (_ncurrent (notebook->buffer)),
                           "mark_set", G_CALLBACK (update_status_bar), NULL, G_CONNECT_SWAPPED);

  return NULL;
}

/* Manages the change of notebook page. */
void *
notebook_change (GtkNotebook * note, GtkNotebookPage * page, int current)
{
  char **info, *tmp;
  extern GtkWidget *jeex_main_window;
  extern JeexFileInfo *file[64];

  delete_bookmarks ();

  notebook->current = (current > 0) ? current : 0;

  /* Sets the various buffers and insert overall to
   * those of current tab
   */
  textview->insert = _ncurrent (notebook->insert);
  textview->regex = _ncurrent (notebook->regex);
  textview->buffer = _ncurrent (notebook->buffer);
  textview->textview = _ncurrent (notebook->textview);


  if (!(_ncurrent (file)->new))
    {
      info = inap (_ncurrent (file)->name);

      /*  Update window title */
      tmp = g_strdup_printf ("%s ( %s ) - %s", info[0], info[1], Jeex);
      gtk_window_set_title (GTK_WINDOW (jeex_main_window), tmp);
      g_free (tmp);
      g_free (info[0]);
      g_free (info[1]);
      g_free (info);

      able_file_widget (NULL, NULL);
    }
  else
    {
      gtk_window_set_title (GTK_WINDOW (jeex_main_window), Jeex);
      disable_file_widget (NULL, NULL);
    }

  load_bookmarks ();

  update_status_bar (NULL, NULL);
  plugin_change_tab_emit_signal ();

  return NULL;
}
