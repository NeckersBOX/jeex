/*
 * "view.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#include <errno.h>
#include <gtk/gtk.h>
#include <ctype.h>
#include <libintl.h>
#include <locale.h>
#include <glib/gi18n.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include "header.h"


/* Function for alternating cell color  */
static void
cell_color_string (GtkTreeViewColumn * column, GtkCellRenderer * renderer,
                   GtkTreeModel * model, GtkTreeIter * iter, void *data)
{
  char *t;
  PangoFontDescription *font;

  gtk_tree_model_get (model, iter, 1, &t, -1);

  font = pango_font_description_from_string ("Monospace 8");

  if (!(atoi (t) % 2))
    {
      g_object_set (renderer, "foreground", "#000000", "foreground-set", TRUE,
                    "background", "#EAEBFF", "background-set", TRUE, "font_desc", font, NULL);
    }
  else
    {
      g_object_set (renderer, "foreground", "#000000", "foreground-set", TRUE,
                    "background", "#FFFFFF", "background-set", TRUE, "font_desc", font, NULL);
    }
}

/* Shows characters table */
void *
chars_table (GtkWidget * widget, void *data)
{
  GtkWidget *win, *table, *label, *treelist, *scroll;
  GtkWidget *close;
  extern GtkWidget *jeex_main_window;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  GtkListStore *store;
  GtkTreeIter iter;
  int i;
  struct
  {
    char *ascii;
    char *dec;
    char *hex;
    char *oct;
    char *bin;
  } CharValue;

  /* Window */
  win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (win), _("Characters Table"));
  gtk_window_set_default_size (GTK_WINDOW (win), -1, 270);
  gtk_container_set_border_width (GTK_CONTAINER (win), 10);
  gtk_window_set_transient_for (GTK_WINDOW (win), GTK_WINDOW (jeex_main_window));
  gtk_window_set_position (GTK_WINDOW (win), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_widget_show (win);

  /* Table */
  table = gtk_table_new (3, 1, FALSE);
  gtk_container_add (GTK_CONTAINER (win), table);
  gtk_widget_show (table);

  /* Text */
  label =
    gtk_label_new (_
                   ("Here is the list of characters converted in ascii,\n"
                    "binary, hexadecimal and octal.\n"));
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  /* Characters list */
  treelist = gtk_tree_view_new ();
  renderer = gtk_cell_renderer_text_new ();
  store =
    gtk_list_store_new (5, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
  /* Ascii */
  column = gtk_tree_view_column_new_with_attributes ("Ascii", renderer, "text", 0, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treelist), column);
  gtk_tree_view_column_set_resizable (column, TRUE);
  /* Decimal */
  column = gtk_tree_view_column_new_with_attributes ("Dec", renderer, "text", 1, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treelist), column);
  gtk_tree_view_column_set_resizable (column, TRUE);
  /* Hexadecimal */
  column = gtk_tree_view_column_new_with_attributes ("Hex", renderer, "text", 2, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treelist), column);
  gtk_tree_view_column_set_resizable (column, TRUE);
  /* Octal */
  column = gtk_tree_view_column_new_with_attributes ("Oct", renderer, "text", 3, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treelist), column);
  gtk_tree_view_column_set_resizable (column, TRUE);
  /* Binary */
  column = gtk_tree_view_column_new_with_attributes ("Bin", renderer, "text", 4, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treelist), column);
  gtk_tree_view_column_set_resizable (column, TRUE);

  gtk_tree_view_column_set_cell_data_func (column, renderer, cell_color_string, treelist, NULL);

  /* Inserting elements */
  for (i = 0; i < 256; ++i)
    {
      if (i < 127)
        {
          CharValue.ascii = g_strdup_printf ("%c", (!g_ascii_isgraph (i)) ? 0x20 : (char) i);
        }
      else if (i > 160)
        {
          CharValue.ascii = g_strdup_printf ("%lc", i);
        }
      else
        {
          CharValue.ascii = g_strdup_printf (" ");
        }
      CharValue.dec = g_strdup_printf ("%d", i);
      CharValue.hex = g_strdup_printf ("%2.2X", i);
      CharValue.oct = g_strdup_printf ("%o", i);
      CharValue.bin = int_to_binary (i);
      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter, 0, CharValue.ascii, 1, CharValue.dec,
                          2, CharValue.hex, 3, CharValue.oct, 4, CharValue.bin, -1);
      g_free (CharValue.ascii);
      g_free (CharValue.dec);
      g_free (CharValue.hex);
      g_free (CharValue.oct);
      free (CharValue.bin);
    }

  gtk_tree_view_set_model (GTK_TREE_VIEW (treelist), GTK_TREE_MODEL (store));
  g_object_unref (store);
  gtk_widget_show (treelist);

  /* List scrollbar */
  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scroll), treelist);
  gtk_table_attach (GTK_TABLE (table), scroll, 0, 1, 1, 2, GTK_FILL, GTK_FILL | GTK_EXPAND, 0, 0);
  gtk_widget_show (scroll);

  /* Close Button */
  close = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  gtk_table_attach (GTK_TABLE (table), close, 0, 1, 2, 3, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
  gtk_widget_show (close);

  g_signal_connect (G_OBJECT (close), "clicked", G_CALLBACK (destroy_this), (void *) win);

  return NULL;
}

/* Gets document stats */
void *
stats (GtkWidget * widget, void *data)
{
  GtkWidget *win, *table, *label, *button, *treelist, *scroll;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  extern GtkWidget *jeex_main_window;
  GtkListStore *store;
  GtkTreeIter iter;
  const char *type[7] = {
    N_("Hexadecimal"),
    N_("Decimal"),
    N_("Octal"),
    N_("Binary"),
    N_("Text"),
    N_("Occurrence"),
    N_("Percent")
  };
  static int stat[0xFF + 2] = { 0 };
  enum ENUM_TYPE
  { HEX = 0, DEC, OCT, BIN, TXT, OCC, PER };
  unsigned char *bin;
  char *s[7];
  float percentual = 0;
  int i, c = 0, len;

  /* Main Window */
  win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (win), _("Document Statistics"));
  gtk_container_set_border_width (GTK_CONTAINER (win), 10);
  gtk_window_set_transient_for (GTK_WINDOW (win), GTK_WINDOW (jeex_main_window));
  gtk_window_set_default_size (GTK_WINDOW (win), 485, 375);
  gtk_window_set_position (GTK_WINDOW (win), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_widget_show (win);

  /* Table */
  table = gtk_table_new (4, 1, FALSE);
  gtk_container_add (GTK_CONTAINER (win), table);
  gtk_widget_show (table);

  /* Title */
  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<big><b>Document Statistics</b></big>"));
  gtk_table_set_row_spacing (GTK_TABLE (table), 0, 10);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1,
                    GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_FILL, 0, 0);
  gtk_widget_show (label);

  /* Recovery Data */
  bin = (unsigned char *) ohfa (&len);
  update_status_bar (NULL, NULL);

  /* Processing Statistics.. */
  for (i = 0; i < len; ++i)
    {
      if (g_ascii_isgraph (i))
        {
          c++;
        }
      stat[bin[i]]++;
    }

  free (bin);

  for (i = 0; i <= 0xFF; ++i)
    {
      stat[0xFF + 1] += stat[i];
    }

  /* General Document Info */
  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label),
                        g_strdup_printf (_
                                         ("<b>Document Rows:</b>\t\t%d\n"
                                          "<b>Document Space:</b>\t\t%d\n"
                                          "<b>Document Characters:</b>\t%d\n"),
                                         stat[0x0A], stat[0x20], c));
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_label_set_selectable (GTK_LABEL (label), TRUE);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  /* Making Stats List */
  treelist = gtk_tree_view_new ();
  renderer = gtk_cell_renderer_text_new ();
  store =
    gtk_list_store_new (7, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                        G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

  /* Making column */
  for (i = 0; i < 7; ++i)
    {
      column = gtk_tree_view_column_new_with_attributes (_(type[i]), renderer, "text", i, NULL);
      gtk_tree_view_append_column (GTK_TREE_VIEW (treelist), column);
      gtk_tree_view_column_set_resizable (column, TRUE);
    }
  gtk_tree_view_column_set_cell_data_func (column, renderer, cell_color_string, treelist, NULL);

  for (i = 0; i <= 0xFF; i++)
    {
      gtk_list_store_append (store, &iter);

      s[0] = g_strdup_printf ("0x%2.2X", i);
      s[1] = g_strdup_printf ("%.3d", i);
      s[2] = g_strdup_printf ("%.3o", i);
      s[3] = int_to_binary (i);
      s[4] = (i > 160 || g_ascii_isgraph (i)) ? g_strdup_printf ("%lc", i) : g_strdup (" ");
      s[5] = g_strdup_printf ("%d", stat[i]);
      percentual = (((float) stat[i]) * 100.00) / ((float) stat[0xFF + 1]);
      s[6] = g_strdup_printf ("%.2f%%", percentual);

      gtk_list_store_set (store, &iter, HEX, s[0], DEC, s[1], OCT, s[2], BIN,
                          s[3], TXT, s[4], OCC, s[5], PER, s[6], -1);

      g_free (s[0]);
      g_free (s[1]);
      g_free (s[2]);
      free (s[3]);
      g_free (s[4]);
      g_free (s[5]);
      g_free (s[6]);
    }

  gtk_tree_view_set_model (GTK_TREE_VIEW (treelist), GTK_TREE_MODEL (store));
  g_object_unref (store);
  gtk_widget_show (treelist);

  /* Adds scrollbar to list */
  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scroll), treelist);
  gtk_table_attach_defaults (GTK_TABLE (table), scroll, 0, 1, 2, 3);
  gtk_widget_show (scroll);

  /* Button CLOSE */
  button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  gtk_table_attach (GTK_TABLE (table), button, 0, 1, 3, 4,
                    GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_FILL, 0, 2);
  gtk_widget_show (button);

  /* Signals */
  g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (destroy_this), (void *) win);

  return NULL;
}

void *
change_show_format (GtkWidget * widget, void *data)
{
  static char *format[4] = {
    N_("Ascii"),
    N_("Binary"),
    N_("Octal"),
    N_("Decimal")
  }, *cformat;
  unsigned char *tmp;
  int current, len, i;
  GtkTextBuffer *buff;
  GString *s;

  cformat = gtk_combo_box_get_active_text (GTK_COMBO_BOX ((GtkWidget *) widget));
  for (current = 0; current < 4; ++current)
    {
      if (!strcmp (cformat, _(format[current])))
        break;
    }

  buff = gtk_text_view_get_buffer (GTK_TEXT_VIEW ((GtkWidget *) data));

  switch (current)
    {
    case 0:                    /* Get Ascii Text */
      tmp = (unsigned char *) ohfa (&len);
      s = g_string_new ("");
      for (i = 0; i < len; ++i)
        {
          if (g_ascii_isprint (tmp[i]) || tmp[i] == 0x20)
            g_string_append_c (s, tmp[i]);
          else
            g_string_append_c (s, '.');

          /* Insert one space every 4 bytes */
          if (!((i + 1) % 4) && i && ((i + 1) % 16) && i != len - 1)
            g_string_append_c (s, 0x20);

          /* Case in who has taken 16 bytes */
          if (!((i + 1) % 16) && i && i != len - 1)
            g_string_append_c (s, 0x0A);
          else if (i != len - 1)
            g_string_append_c (s, 0x20);
        }
      free (tmp);
      gtk_text_buffer_set_text (buff, s->str, -1);
      g_string_free (s, TRUE);
      break;
    case 1:                    /* Get Binary Text */
      tmp = (unsigned char *) ohfa (&len);
      s = g_string_new ("");
      for (i = 0; i < len; ++i)
        {
          g_string_append_printf (s, "%s", int_to_binary (tmp[i]));

          /* Insert one space every 4 bytes */
          if (!((i + 1) % 4) && i && ((i + 1) % 16) && i != len - 1)
            g_string_append_c (s, 0x20);

          /* Case in who has taken 16 bytes */
          if (!((i + 1) % 16) && i && i != len - 1)
            g_string_append_c (s, 0x0A);
          else if (i != len - 1)
            g_string_append_c (s, 0x20);
        }
      free (tmp);
      gtk_text_buffer_set_text (buff, s->str, -1);
      g_string_free (s, TRUE);
      break;
    case 2:                    /* Get Octal Text */
      tmp = (unsigned char *) ohfa (&len);
      s = g_string_new ("");
      for (i = 0; i < len; ++i)
        {
          g_string_append_printf (s, "%3.3o", tmp[i]);

          /* Insert one space every 4 bytes */
          if (!((i + 1) % 4) && i && ((i + 1) % 16) && i != len - 1)
            g_string_append_c (s, 0x20);

          /* Case in who has taken 16 bytes */
          if (!((i + 1) % 16) && i && i != len - 1)
            g_string_append_c (s, 0x0A);
          else if (i != len - 1)
            g_string_append_c (s, 0x20);
        }
      free (tmp);
      gtk_text_buffer_set_text (buff, s->str, -1);
      g_string_free (s, TRUE);
      break;
    case 3:                    /* Get Octal Text */
      tmp = (unsigned char *) ohfa (&len);
      s = g_string_new ("");
      for (i = 0; i < len; ++i)
        {
          g_string_append_printf (s, "%3.3d", tmp[i]);

          /* Insert one space every 4 bytes */
          if (!((i + 1) % 4) && i && ((i + 1) % 16) && i != len - 1)
            g_string_append_c (s, 0x20);

          /* Case in who has taken 16 bytes */
          if (!((i + 1) % 16) && i && i != len - 1)
            g_string_append_c (s, 0x0A);
          else if (i != len - 1)
            g_string_append_c (s, 0x20);
        }
      free (tmp);
      gtk_text_buffer_set_text (buff, s->str, -1);
      g_string_free (s, TRUE);
      break;
    }


  return NULL;
}

void *
show_in_other_format (GtkWidget * widget, void *data)
{
  GtkWidget *window, *textarea, *combo, *vbox, *label, *hbox, *scroll, *button, *viewport;
  GtkTextBuffer *buff;
  GString *s;
  int len, i;
  unsigned char *tmp;
  extern GtkWidget *jeex_main_window;

  /* Window */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), _("Document in other format"));
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (jeex_main_window));
  gtk_window_set_modal (GTK_WINDOW (window), TRUE);
  gtk_window_set_default_size (GTK_WINDOW (window), 325, 375);
  gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER_ON_PARENT);

  /* Vertical Box */
  vbox = gtk_vbox_new (FALSE, 2);
  gtk_container_add (GTK_CONTAINER (window), vbox);

  /* Choose format area */
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<i>Choose format:</i>"));
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);

  combo = gtk_combo_box_new_text ();
  gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Ascii"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Binary"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Octal"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Decimal"));
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
  gtk_box_pack_start (GTK_BOX (vbox), combo, FALSE, TRUE, 1);

  gtk_box_pack_start (GTK_BOX (vbox), gtk_hseparator_new (), FALSE, TRUE, 1);

  /* Text area */
  viewport = gtk_viewport_new (NULL, NULL);
  gtk_box_pack_start (GTK_BOX (vbox), viewport, TRUE, TRUE, 0);

  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add (GTK_CONTAINER (viewport), scroll);

  textarea = gtk_text_view_new ();
  gtk_text_view_set_editable (GTK_TEXT_VIEW (textarea), FALSE);
  gtk_text_view_set_justification (GTK_TEXT_VIEW (textarea), GTK_JUSTIFY_LEFT);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (textarea), GTK_WRAP_NONE);
  gtk_text_view_set_left_margin (GTK_TEXT_VIEW (textarea), 5);
  gtk_widget_modify_font (textarea, textview->font);
  gtk_widget_modify_text (textarea, GTK_STATE_NORMAL, &(textview->color));
  gtk_container_add (GTK_CONTAINER (scroll), textarea);
  buff = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textarea));

  /* Get Ascii Text */
  tmp = (unsigned char *) ohfa (&len);
  s = g_string_new ("");
  for (i = 0; i < len; ++i)
    {
      if (g_ascii_isprint (tmp[i]) || tmp[i] == 0x20)
        g_string_append_c (s, tmp[i]);
      else
        g_string_append_c (s, '.');

      /* Division of a space every 4 bytes */
      if (!((i + 1) % 4) && i && ((i + 1) % 16) && i != len - 1)
        {
          g_string_append_c (s, 0x20);
        }

      /* Case in who has taken 16 bytes */
      if (!((i + 1) % 16) && i && i != len - 1)
        {
          g_string_append_c (s, 0x0A);
        }
      else if (i != len - 1)
        {
          g_string_append_c (s, 0x20);
        }
    }
  free (tmp);
  gtk_text_buffer_set_text (buff, s->str, -1);
  g_string_free (s, TRUE);

  /* Button CLOSE */
  button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, TRUE, 0);
  g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (destroy_this), window);

  g_signal_connect (G_OBJECT (combo), "changed", G_CALLBACK (change_show_format), textarea);

  gtk_widget_show_all (window);

  return NULL;
}

typedef struct
{
  GtkWidget *outfile;
  GtkWidget *expanded;
  GtkWidget *label;
} FileDifferences;

void
elaborate_diff (GtkFileChooserButton * widget, void *user_data)
{
  FileDifferences *diff = (FileDifferences *) user_data;
  char *filename;
  unsigned char *data[2];
  FILE *of;
  GString *string;
  GFileError error_type;
  signed int size[2], len, i, shared_n = 0, shared_t = 0, c;

  filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (widget));
  if (!(of = fopen (filename, "rb")))
    {
      error_type = g_file_error_from_errno (errno);
      file_error_control (error_type, RB_MODE);
      return;
    }

  fseek (of, 0, SEEK_END);
  data[0] = (unsigned char *) malloc ((size[0] = ftell (of)) * sizeof (unsigned char));
  rewind (of);
  fread (data[0], sizeof (unsigned char), size[0], of);
  fclose (of);

  data[1] = (unsigned char *) ohfa (&size[1]);
  len = (size[0] > size[1]) ? size[1] : size[0];

  string = g_string_new ("");
  for (i = 0; i < len; i++)
    {
      for (c = i; i < len && data[0][i] == data[1][i]; i++, shared_t++)
        {
          if (i == c)
            {
              g_string_append_printf (string,
                                      _("<b>%d</b>: <i>start</i> ( %8.8X ) "), ++shared_n, c + 0x10);
              c = -1;
            }
        }
      if (c == -1)
        g_string_append_printf (string, _("- <i>end</i> ( %8.8X )\n"), i + 0x10);

    }

  gtk_widget_set_sensitive (diff->expanded, TRUE);
  gtk_expander_set_label (GTK_EXPANDER (diff->expanded),
                          g_strdup_printf ("%d ( %2.1f%% )", shared_n, (float) (shared_t * 100) / len));
  gtk_label_set_markup (GTK_LABEL (diff->label), string->str);
  g_string_free (string, TRUE);
}

void *
file_difference (GtkWidget * widget, void *data)
{
  GtkWidget *window, *hbox, *image, *main_vbox, *label;
  GtkWidget *close, *vbox, *scroll;
  FileDifferences *diffwidget = (FileDifferences *) g_malloc (sizeof (FileDifferences));
  extern GtkWidget *jeex_main_window;

  /* Window */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), _("Differences with other files"));
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);
  gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (jeex_main_window));
  gtk_window_set_modal (GTK_WINDOW (window), TRUE);
  gtk_window_set_default_size (GTK_WINDOW (window), 490, 205);
  gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER_ON_PARENT);

  /* The Main VBox */
  main_vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), main_vbox);

  /* Title */
  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<big><b>Differences with other files</b></big>\n"));
  gtk_box_pack_start (GTK_BOX (main_vbox), label, FALSE, TRUE, 0);

  /* Image */
  hbox = gtk_hbox_new (FALSE, 10);
  gtk_box_pack_start (GTK_BOX (main_vbox), hbox, TRUE, TRUE, 0);

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), vbox, FALSE, TRUE, 0);

  image = gtk_image_new_from_file (JEEX_PATH_IMG "/diff.png");
  gtk_box_pack_start (GTK_BOX (vbox), image, FALSE, TRUE, 0);

  /* - Info on the differences - */
  vbox = gtk_vbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (hbox), vbox, TRUE, TRUE, 0);

  /* Out Filename */
  hbox = gtk_hbox_new (FALSE, 3);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>File name to compare:</b>"));
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);

  diffwidget->outfile =
    gtk_file_chooser_button_new (_("Choose a compare file..."), GTK_FILE_CHOOSER_ACTION_OPEN);
  gtk_box_pack_start (GTK_BOX (hbox), diffwidget->outfile, TRUE, TRUE, 0);

  /* Separator */
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 3);
  gtk_box_pack_start (GTK_BOX (hbox), gtk_hseparator_new (), TRUE, TRUE, 0);

  /* Expander */
  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), vbox, FALSE, TRUE, 0);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("Shared parts:"));
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, TRUE, 0);

  diffwidget->expanded = gtk_expander_new (_("No File Open"));
  gtk_widget_set_sensitive (diffwidget->expanded, FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), diffwidget->expanded, TRUE, TRUE, 0);

  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_widget_set_usize (scroll, -1, 256);
  gtk_container_add (GTK_CONTAINER (diffwidget->expanded), scroll);

  diffwidget->label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (diffwidget->label), _("No File Open"));

  hbox = gtk_vbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (hbox), diffwidget->label, FALSE, TRUE, 0);
  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scroll), hbox);

  /* - end Info on the differences - */

  /* Separator */
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (main_vbox), hbox, FALSE, TRUE, 3);
  gtk_box_pack_start (GTK_BOX (hbox), gtk_hseparator_new (), TRUE, TRUE, 0);

  /* Button */
  hbox = gtk_hbutton_box_new ();
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbox), GTK_BUTTONBOX_END);
  gtk_box_pack_start (GTK_BOX (main_vbox), hbox, FALSE, TRUE, 0);

  close = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  gtk_box_pack_start (GTK_BOX (hbox), close, FALSE, TRUE, 0);
  g_signal_connect (G_OBJECT (close), "clicked", G_CALLBACK (destroy_this), window);

  gtk_widget_show_all (window);

  g_signal_connect (G_OBJECT (diffwidget->outfile), "file-set", G_CALLBACK (elaborate_diff), diffwidget);

  return NULL;
}


/* Function to show through dialog window the md5, sha-1 and
 * sha-256 hash of opened file.
 */
void *
md5_file (GtkWidget * widget, void *data)
{
  unsigned char *_file;
  unsigned int len;
  GtkWidget *win, *title, *frame, *close, *table, *hash;
  extern GtkWidget *jeex_main_window;

  _file = (unsigned char *) ohfa ((int *) &len);

  /* Window */
  win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_transient_for (GTK_WINDOW (win), GTK_WINDOW (jeex_main_window));
  gtk_window_set_position (GTK_WINDOW (win), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_title (GTK_WINDOW (win), _("Hash View"));
  gtk_container_set_border_width (GTK_CONTAINER (win), 8);
  gtk_window_set_default_size (GTK_WINDOW (win), 472, -1);
  gtk_widget_show (win);

  /* Table */
  table = gtk_table_new (3, 1, FALSE);
  gtk_container_add (GTK_CONTAINER (win), table);
  gtk_widget_show (table);

  /* Title */
  title = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (title), "<big><b>Hash View</b></big>");
  gtk_table_attach (GTK_TABLE (table), title, 0, 1, 0, 1, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
  gtk_widget_show (title);

  /* Frame */
  frame = gtk_frame_new (_("Hash content"));
  gtk_table_attach (GTK_TABLE (table), frame, 0, 1, 1, 2, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
  gtk_widget_show (frame);

  /* Label Hash */
  hash = gtk_label_new (NULL);
  gtk_label_set_selectable (GTK_LABEL (hash), TRUE);
  gtk_label_set_markup (GTK_LABEL (hash),
                        g_strdup_printf ("\n<b>MD5:</b> <i>%s</i> \n"
                                         "<b>SHA-1:</b> <i>%s</i> \n"
                                         "<b>SHA-256:</b> <i>%s</i> \n\n",
                                         g_compute_checksum_for_data
                                         (G_CHECKSUM_MD5, _file, len),
                                         g_compute_checksum_for_data
                                         (G_CHECKSUM_SHA1, _file, len),
                                         g_compute_checksum_for_data (G_CHECKSUM_SHA256, _file, len)));
  gtk_container_add (GTK_CONTAINER (frame), hash);
  gtk_widget_show (hash);

  /* Close Button */
  close = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  gtk_table_attach (GTK_TABLE (table), close, 0, 1, 2, 3,
                    GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 0, 0);
  gtk_widget_show (close);

  g_signal_connect (G_OBJECT (close), "clicked", G_CALLBACK (destroy_this), (void *) win);

  free (_file);

  return NULL;
}

void *
add_view (GtkWidget * widget, void *data)
{
  extern JeexPreferences *preferences;
  extern JeexFileInfo *file[64];
  extern GtkWidget *jeex_main_window;
  GtkWidget *dialog, *paned[2], *textview, *scroll;
  GtkWidget *button, *container, *hbox;
  GtkTextBuffer *buffer;
  PangoFontDescription *font_desc;
  char *str, **tmp_info;
  int len, i;
  GString *string;

  if ( !_ncurrent (file)->new ) {
       tmp_info = inap (_ncurrent (file)->name);
       str = g_strdup_printf (_("View - %s ( %s )"), tmp_info[0], tmp_info[1]);
       g_free (tmp_info[0]);
       g_free (tmp_info[1]);
       g_free (tmp_info);
  }
  else str = g_strdup (_("View - New File"));

  dialog = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (jeex_main_window));
  gtk_window_set_title (GTK_WINDOW (dialog), str);
  gtk_window_set_default_size (GTK_WINDOW (dialog), 648, 0xFF);
  g_free (str);
  gtk_container_set_border_width (GTK_CONTAINER (dialog), 3);

  container = gtk_vbox_new (FALSE, 3);
  gtk_container_add (GTK_CONTAINER (dialog), container);

  str = ohfa (&len);

  /* Offset Paned */
  paned[0] = gtk_hpaned_new ();
  gtk_widget_show (paned[0]);
  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_box_pack_start (GTK_BOX (container), scroll, TRUE, TRUE, 0);
  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scroll), paned[0]);
  gtk_widget_show (scroll);

  textview = gtk_text_view_new ();
  gtk_text_view_set_editable (GTK_TEXT_VIEW (textview), FALSE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (textview), GTK_WRAP_NONE);
  gtk_text_view_set_left_margin (GTK_TEXT_VIEW (textview), 3);
  gtk_text_view_set_right_margin (GTK_TEXT_VIEW (textview), 3);
  font_desc = pango_font_description_from_string (preferences->font);
  gtk_widget_modify_font (textview, font_desc);
  pango_font_description_free (font_desc);
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
  gtk_paned_add1 (GTK_PANED (paned[0]), textview);
  gtk_widget_show (textview);

  string = g_string_new ("");
  for (i = 0; i < len; i += 0x10)
    if ((i + 0x10) < len)
      g_string_append_printf (string, "%8.8X\n", i);
    else
      g_string_append_printf (string, "%8.8X", i);
  gtk_text_buffer_set_text (buffer, string->str, string->len);
  g_string_free (string, TRUE);

  /* Ascii and Hexadecimal Paned */
  paned[1] = gtk_hpaned_new ();
  gtk_paned_add2 (GTK_PANED (paned[0]), paned[1]);
  gtk_widget_show (paned[1]);

  /* Hexadecimal */
  textview = gtk_text_view_new ();
  gtk_text_view_set_editable (GTK_TEXT_VIEW (textview), FALSE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (textview), GTK_WRAP_NONE);
  gtk_text_view_set_left_margin (GTK_TEXT_VIEW (textview), 3);
  gtk_text_view_set_right_margin (GTK_TEXT_VIEW (textview), 3);
  font_desc = pango_font_description_from_string (preferences->font);
  gtk_widget_modify_font (textview, font_desc);
  pango_font_description_free (font_desc);
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
  gtk_paned_add1 (GTK_PANED (paned[1]), textview);
  gtk_widget_show (textview);

  string = g_string_new ("");
  for (i = 0; i < len; i++)
    {
      g_string_append_printf (string, "%.2X", (unsigned char) str[i]);

      if (!((i + 1) % 4) && i && ((i + 1) % 16) && i != len - 1)
        {
          g_string_append_c (string, 0x20);
        }

      if (!((i + 1) % 16) && i && i != len - 1)
        {
          g_string_append_c (string, 0x0A);
        }
      else if (i != len - 1)
        {
          g_string_append_c (string, 0x20);
        }
    }
  gtk_text_buffer_set_text (buffer, string->str, string->len);
  g_string_free (string, TRUE);

  /* Ascii */
  textview = gtk_text_view_new ();
  gtk_text_view_set_editable (GTK_TEXT_VIEW (textview), FALSE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (textview), GTK_WRAP_NONE);
  gtk_text_view_set_left_margin (GTK_TEXT_VIEW (textview), 3);
  gtk_text_view_set_right_margin (GTK_TEXT_VIEW (textview), 3);
  font_desc = pango_font_description_from_string (preferences->font);
  gtk_widget_modify_font (textview, font_desc);
  pango_font_description_free (font_desc);
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
  gtk_paned_add2 (GTK_PANED (paned[1]), textview);
  gtk_widget_show (textview);

  string = g_string_new ("");
  for (i = 0; i < len; i++)
    {
      g_string_append_c (string, isprint (str[i]) ? str[i] : '.');
      if (!((i + 1) % 16) && i && i != len - 1)
        {
          g_string_append_c (string, 0x0A);
        }
    }
  gtk_text_buffer_set_text (buffer, string->str, string->len);
  g_string_free (string, TRUE);

  free (str);

  /* Button */
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (container), hbox, FALSE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), gtk_hseparator_new (), TRUE, TRUE, 0);

  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (container), hbox, FALSE, TRUE, 0);
  button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (destroy_this), dialog);
  gtk_box_pack_end (GTK_BOX (hbox), button, FALSE, TRUE, 0);

  gtk_widget_show_all (dialog);

  return NULL;
}
