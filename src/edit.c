/*
 * "edit.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#include <ctype.h>
#include <errno.h>
#include <gtk/gtk.h>
#include <glib/gprintf.h>
#include <glib/gi18n.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <math.h>
#include <libintl.h>
#include <locale.h>
#include <stdlib.h>
#include "header.h"

/* Function to delete all text entered */
void *
reset (GtkWidget * widget, void *data)
{
  GtkTextIter start, end;

  gtk_text_buffer_get_bounds (textview->buffer, &start, &end);
  gtk_text_buffer_delete (textview->buffer, &start, &end);

  return NULL;
}

/* Function to update the text */
void *
_update (GtkWidget * widget, void *data)
{
  unsigned char *ddata;
  unsigned long dlen;
  char *str, i;
  char **info;
  FILE *of;
  GtkTextIter iter, start, end;
  GtkTextMark *mark;
  GFileError error_type;
  extern JeexPreferences *preferences;
  extern JeexFileInfo *file[64];

  logging_action (_("Updating file `%s'\n"), _ncurrent (file)->name);

  /* Checks if file exist and can be reads */
  if (!(of = fopen (_ncurrent (file)->name, "rb")))
    {
      /* Checks error type. */
      error_type = g_file_error_from_errno (errno);
      file_error_control (error_type, RB_MODE);
      return NULL;
    }

  /* Checks if file is a folder */
  if (g_file_test (_ncurrent (file)->name, G_FILE_TEST_IS_DIR))
    {
      _error (_("<big><b>Unable to open file</b></big>\n"
                "\nThe file you are trying to open is a directory."));
      fclose (of);
      return NULL;
    }

  /* Checks size */
  if (!control_size (of))
    {
      _error (g_strdup_printf
              (_("<big><b>File too large!</b></big>\n"
                 "\nFile size higher than <i>%s</i>."),
               g_format_size_for_display ((goffset) preferences->max_filesize)));
      fclose (of);
      return NULL;
    }

  /* Reset text */
  gtk_text_buffer_get_bounds (_ncurrent (notebook->buffer), &start, &end);
  gtk_text_buffer_delete (_ncurrent (notebook->buffer), &start, &end);

  /* Recovery various data to insert the new text */
  _ncurrent (notebook->buffer) =
    gtk_text_view_get_buffer (GTK_TEXT_VIEW (_ncurrent (notebook->textview)));
  mark = gtk_text_buffer_get_insert (_ncurrent (notebook->buffer));
  gtk_text_buffer_get_iter_at_mark (_ncurrent (notebook->buffer), &iter, mark);

  /* Sets color */
  gdk_color_parse (preferences->color, &(textview->color));
  gtk_widget_modify_text (_ncurrent (notebook->textview), GTK_STATE_NORMAL, &(textview->color));

  /* Loads of text */
  dlen = load_exe (of, &ddata);
  info_type (FALSE, ddata, dlen);
  str = make_string (&ddata, dlen);
  fclose (of);

  gtk_text_buffer_set_text (_ncurrent (notebook->buffer), str, -1);
  g_free (str);
  free (ddata);

  info = inap (_ncurrent (file)->name);
  _ncurrent (file)->new = FALSE;
  _ncurrent (file)->modified = FALSE;

  /* Resize string if length > preferences->max_tabchars */
  if ((i = strlen (info[0])) > preferences->max_tabchars)
    {
      str = g_strndup (info[0], preferences->max_tabchars);
      g_free (info[0]);
      info[0] = g_strdup_printf ("%s...", str);
      g_free (str);
    }
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook->notebook),
                              _ncurrent (notebook->table), gtk_label_new (info[0]));

  g_free (info[0]);
  g_free (info[1]);
  g_free (info);

  return NULL;
}

struct dialog_bits_data
{
  GtkWidget *bit1_e, *bit2_e, *result_e;
  int operation;
  GtkWidget *check_bit1[8], *check_bit2[8];
} dialog_bits_widget;

static void
change_bit_value (void)
{
  unsigned char byte[2] = { 0, 0 };
  int i;
  char *free_me;

  for (i = 7; i > -1; i--)
    {
      if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dialog_bits_widget.check_bit1[i])))
        byte[0] = (byte[0] | 1) << (i ? 1 : 0);
      else
        byte[0] = (byte[0] | 0) << (i ? 1 : 0);

      if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dialog_bits_widget.check_bit2[i])))
        byte[1] = (byte[1] | 1) << (i ? 1 : 0);
      else
        byte[1] = (byte[1] | 0) << (i ? 1 : 0);
    }

  gtk_entry_set_text (GTK_ENTRY (dialog_bits_widget.bit1_e), free_me = g_strdup_printf ("%d", byte[0]));
  g_free (free_me);
  gtk_entry_set_text (GTK_ENTRY (dialog_bits_widget.bit2_e), free_me = g_strdup_printf ("%d", byte[1]));
  g_free (free_me);

  switch (dialog_bits_widget.operation)
    {
    case 1:
      byte[0] &= byte[1];
      break;
    case 2:
      byte[0] ^= byte[1];
      break;
    case 3:
      byte[0] |= byte[1];
      break;
    }
  gtk_entry_set_text (GTK_ENTRY (dialog_bits_widget.result_e), free_me =
                      g_strdup_printf ("%d", byte[0]));
  g_free (free_me);
}

void
change_bits_operation (GtkComboBox * widget, void *data)
{
  dialog_bits_widget.operation = gtk_combo_box_get_active (widget) + 1;
  change_bit_value ();
}

/* Function to show dialog to operate with bits */
void *
insert_from_bits_show_dialog (GtkWidget * widget, void *data)
{
  GtkWidget *dialog, *content_area;
  extern GtkWidget *jeex_main_window;
  GtkWidget *hbox, *vbox, *main_vbox, *main_hbox, *m_hbox;
  GtkWidget *label, *op, *frame;
  short int i;
  char *free_me, *byte;

  dialog = gtk_dialog_new_with_buttons (_("Operations with bits"),
                                        GTK_WINDOW (jeex_main_window), 0,
                                        GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
                                        GTK_STOCK_CANCEL, GTK_RESPONSE_ACCEPT, NULL);
  content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
  gtk_window_set_default_size (GTK_WINDOW (dialog), 450, 235);

  main_vbox = gtk_vbox_new (FALSE, 5);
  gtk_container_add (GTK_CONTAINER (content_area), main_vbox);
  gtk_container_set_border_width (GTK_CONTAINER (content_area), 10);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<big><b>Operations with bits</b></big>"));
  gtk_box_pack_start (GTK_BOX (main_vbox), label, FALSE, TRUE, 0);

  gtk_box_pack_start (GTK_BOX (main_vbox), gtk_hseparator_new (), TRUE, TRUE, 0);

  main_hbox = gtk_hbox_new (FALSE, 3);
  gtk_box_pack_start (GTK_BOX (main_vbox), main_hbox, FALSE, TRUE, 0);

  /* First byte value */
  vbox = gtk_vbox_new (FALSE, 3);
  gtk_box_pack_start (GTK_BOX (main_hbox), vbox, FALSE, TRUE, 0);

  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);
  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>First Byte Value</b>"));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);

  dialog_bits_widget.bit1_e = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (dialog_bits_widget.bit1_e), "0");
  gtk_editable_set_editable (GTK_EDITABLE (dialog_bits_widget.bit1_e), FALSE);
  gtk_widget_set_size_request (dialog_bits_widget.bit1_e, 100, -1);
  gtk_box_pack_start (GTK_BOX (vbox), dialog_bits_widget.bit1_e, FALSE, TRUE, 0);

  /* Operation */
  vbox = gtk_vbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (main_hbox), vbox, FALSE, TRUE, 0);

  op = gtk_combo_box_new_text ();
  gtk_combo_box_append_text (GTK_COMBO_BOX (op), "AND ( & )");
  gtk_combo_box_append_text (GTK_COMBO_BOX (op), "XOR ( ^ )");
  gtk_combo_box_append_text (GTK_COMBO_BOX (op), "OR ( | )");
  gtk_combo_box_set_active (GTK_COMBO_BOX (op), 0);
  gtk_widget_set_size_request (hbox, -1, 10);
  gtk_box_pack_start (GTK_BOX (vbox), op, TRUE, TRUE, 0);
  dialog_bits_widget.operation = 1;
  g_signal_connect (G_OBJECT (op), "changed", G_CALLBACK (change_bits_operation), NULL);

  /* Last byte value */
  vbox = gtk_vbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (main_hbox), vbox, FALSE, TRUE, 0);

  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Last Byte Value</b>"));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);

  dialog_bits_widget.bit2_e = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (dialog_bits_widget.bit2_e), "0");
  gtk_editable_set_editable (GTK_EDITABLE (dialog_bits_widget.bit2_e), FALSE);
  gtk_widget_set_size_request (dialog_bits_widget.bit2_e, 100, -1);
  gtk_box_pack_start (GTK_BOX (vbox), dialog_bits_widget.bit2_e, FALSE, TRUE, 0);

  /* Preview Byte Result */
  vbox = gtk_vbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (main_hbox), vbox, FALSE, TRUE, 0);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), "<big><b>=</b></big>");
  gtk_box_pack_start (GTK_BOX (vbox), label, TRUE, TRUE, 0);

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (main_hbox), vbox, FALSE, TRUE, 0);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Result</b>"));
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, TRUE, 0);

  dialog_bits_widget.result_e = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (dialog_bits_widget.result_e), "0");
  gtk_editable_set_editable (GTK_EDITABLE (dialog_bits_widget.result_e), FALSE);
  gtk_widget_set_size_request (dialog_bits_widget.result_e, 100, -1);
  gtk_box_pack_start (GTK_BOX (vbox), dialog_bits_widget.result_e, FALSE, TRUE, 0);

  gtk_box_pack_start (GTK_BOX (main_vbox), gtk_hseparator_new (), TRUE, TRUE, 0);
  /* --------------------------------------- */

  /* Bits of first byte */
  frame = gtk_frame_new (NULL);
  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>First Byte's bits</b>"));
  gtk_frame_set_label_widget (GTK_FRAME (frame), label);
  gtk_box_pack_start (GTK_BOX (main_vbox), frame, TRUE, TRUE, 0);

  vbox = gtk_vbox_new (TRUE, 0);
  gtk_container_add (GTK_CONTAINER (frame), vbox);

  m_hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), m_hbox, TRUE, TRUE, 0);
  for (i = 7; i > -1; i--)
    {
      free_me = g_strdup_printf ("Bit %d", i + 1);
      dialog_bits_widget.check_bit1[i] = gtk_check_button_new_with_label (free_me);
      g_free (free_me);
      gtk_box_pack_start (GTK_BOX (m_hbox), dialog_bits_widget.check_bit1[i], TRUE, TRUE, 0);
      g_signal_connect (G_OBJECT (dialog_bits_widget.check_bit1[i]), "toggled",
                        G_CALLBACK (change_bit_value), NULL);
    }

  /* Bits of last byte */
  frame = gtk_frame_new (NULL);
  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Last Byte's bits</b>"));
  gtk_frame_set_label_widget (GTK_FRAME (frame), label);
  gtk_box_pack_start (GTK_BOX (main_vbox), frame, TRUE, TRUE, 0);

  vbox = gtk_vbox_new (TRUE, 0);
  gtk_container_add (GTK_CONTAINER (frame), vbox);

  m_hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), m_hbox, TRUE, TRUE, 0);
  for (i = 7; i > -1; i--)
    {
      free_me = g_strdup_printf ("Bit %d", i + 1);
      dialog_bits_widget.check_bit2[i] = gtk_check_button_new_with_label (free_me);
      g_free (free_me);
      gtk_box_pack_start (GTK_BOX (m_hbox), dialog_bits_widget.check_bit2[i], TRUE, TRUE, 0);
      g_signal_connect (G_OBJECT (dialog_bits_widget.check_bit2[i]), "toggled",
                        G_CALLBACK (change_bit_value), NULL);
    }

  gtk_widget_show_all (main_vbox);
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
      free_me = g_strdup (gtk_entry_get_text (GTK_ENTRY (dialog_bits_widget.result_e)));
      byte = g_strdup_printf ("%.2X", atoi (free_me));
      g_free (free_me);
      gtk_text_buffer_insert_at_cursor (textview->buffer, byte, -1);
      g_free (byte);
    }
  gtk_widget_destroy (dialog);

  return NULL;
}

/* Function to select all text */
void *
select_all (GtkWidget * widget, void *data)
{
  GtkTextIter start, end;

  gtk_text_buffer_get_bounds (textview->buffer, &start, &end);
  gtk_text_buffer_select_range (textview->buffer, &start, &end);

  return NULL;
}

/* Function to copy the selected text */
void *
copy (GtkWidget * widget, void *data)
{
  GtkClipboard *board;

  /* Gets selected part */
  board = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
  textview->buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview->textview));

  /* Copy selected text */
  gtk_text_buffer_copy_clipboard (textview->buffer, board);

  return NULL;
}

/* Function to cut selected text */
void *
cut (GtkWidget * widget, void *data)
{
  GtkClipboard *board;

  /* Gets selected part */
  board = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
  textview->buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview->textview));

  /* Cuts selected text */
  gtk_text_buffer_cut_clipboard (textview->buffer, board, TRUE);

  return NULL;
}

/* Function to delete the selected text */
void *
delete (GtkWidget * widget, void *data)
{
  gtk_text_buffer_delete_selection (textview->buffer, TRUE, TRUE);

  return NULL;
}

/* Function to paste selected text */
void *
paste (GtkWidget * widget, void *data)
{
  GtkClipboard *board;
  char *str;
  GString *string;
  int len, i;

  /* Gets selected part */
  board = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
  textview->buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview->textview));
  str = g_strdup (gtk_clipboard_wait_for_text (board));
  len = strlen (str);
  string = g_string_new ("");

  /* Validate string */
  for (i = 0; i < len; ++i)
    {
      if (g_ascii_isxdigit (str[i]) || str[i] == 0x20)
        g_string_append_c (string, str[i]);
    }

  /* Paste the text and realize editable */
  gtk_text_buffer_insert_at_cursor (textview->buffer, string->str, -1);

  g_string_free (string, TRUE);
  g_free (str);

  return NULL;
}

/* Hides insertion bar */
void *
insert_d (GtkWidget * widget, void **data)
{
  gtk_widget_hide (textview->insert->table);

  return NULL;
}

/* Shows insertion bar */
void *
insert_a (GtkWidget * widget, void **data)
{
  gtk_widget_show (textview->insert->table);

  return NULL;
}

/* Change the type of format with the one chosen by user */
void *
insert_format (GtkWidget * widget, int format)
{
  textview->insert->format = format;
  return NULL;
}

/* Insert the text in any format in the field where the bytes 
 * are displayed in hexadecimal.
 */
void *
insert_text (GtkWidget * widget, GtkTextInsert * i)
{
  char *text, *entry;
  gboolean check = FALSE;

  entry = (char *) gtk_entry_get_text (GTK_ENTRY (i->e_insert));
  text = obtain_value (i->format, entry, &check);
  entry = NULL;

  if (text)
    {
      gtk_text_buffer_insert_at_cursor (textview->buffer, text, -1);
      if (!check)
        {
          g_free (text);
        }
    }

  return NULL;
}

void *
reverse_byte (GtkWidget * widget, void *data)
{
  GtkTextIter start, end;
  char *str, *p, buff[2];
  GString *s;
  int len, c, i;

  /* Gets Str */
  gtk_text_buffer_get_selection_bounds (textview->buffer, &start, &end);
  str = gtk_text_buffer_get_text (textview->buffer, &start, &end, FALSE);

  /* Reversing */
  len = strlen (str);
  p = str;
  s = g_string_new ("");
  for (c = 0; c < len;)
    {
      if (*p == 0x20 || *p == 0x0A)
        {
          p++;
          c++;
          continue;
        }

      buff[0] = p[0];
      buff[1] = p[1];

      g_string_append_c (s, strtol (buff, NULL, 16));

      p += 2;
      c += 2;
    }
  g_string_append_c (s, 0x00);
  g_free (str);
  str = g_strdup (g_strreverse (s->str));
  len = strlen (str);
  g_string_free (s, TRUE);

  s = g_string_new ("");

  for (c = 0; c < len; c++)
    {
      p = g_strdup_printf ("%.2X", str[c]);
      i = strlen (p);
      g_string_append_printf (s, "%c%c", p[i - 2], p[i - 1]);
      g_free (p);

      /* Division of a space every 4 bytes */
      if (!((c + 1) % 4) && c && ((c + 1) % 16) && c != len - 1)
        {
          g_string_append_c (s, 0x20);
        }

      /* Case in who has taken 16 bytes */
      if (!((c + 1) % 16) && c && c != len - 1)
        {
          g_string_append_c (s, 0x0A);
        }
      else if (c != len - 1)
        {
          g_string_append_c (s, 0x20);
        }
    }

  g_free (str);

  /* Delete and insert the new reversed text */
  gtk_text_buffer_delete (textview->buffer, &start, &end);
  gtk_text_buffer_insert (textview->buffer, &start, s->str, -1);

  g_string_free (s, TRUE);

  return NULL;
}
