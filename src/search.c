/*
 * "search.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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
#include <math.h>
#include <string.h>
#include <strings.h>
#ifndef regex_time_limit_off
#include <time.h>
#endif
#include <libintl.h>
#include <locale.h>
#include <glib/gi18n.h>
#include <stdlib.h>
#include "header.h"

#ifdef regex_time_limit
# if regex_time_limit < 0
#    warning (jeex): regex_time_limit macro: bad initialization value.
#    undef  regex_time_limit
#    define regex_time_limit 300
# endif
#else
#    define regex_time_limit 300
#endif

struct search_and_replace
{
  GtkWidget *search;
  GtkWidget *replace;
};

/* Redirect functions to strcasecmp, created especially to
 * work as GCompareDataFunc paramater.
 */
int
ac_strcasecmp (const void *a, const void *b)
{
  g_return_val_if_fail (a || b, -1);

  if (!strcasecmp ((char *) a ? a : "(null)", (char *) b ? b : "(null)"))
    {
      return 0;
    }

  return 1;
}

GtkTextInsert *gti_find, *gti_replace;

/* Function to memorized the search format */
static void *
find_format (GtkWidget * widget, int format)
{
  gti_find->format = format;
  return NULL;
}

/* Function to memorized the replace format */
static void *
replace_format (GtkWidget * widget, int format)
{
  gti_replace->format = format;
  return NULL;
}


/* Tests that the string passed as parameter is in the 
 * list of the strings in the automatic completion list.
 */
gboolean
ac_string_exist (char *str)
{
  GSList *tmp;

  g_return_val_if_fail (str, TRUE);

  tmp = g_slist_find_custom (ac->string_list, (gconstpointer) str, ac->strcasecmp);

  return tmp ? TRUE : FALSE;
}

/* Add the passed string as parameter to the list of the
 * automatic completion strings.
 */
void
ac_add_string (char *str)
{
  g_return_if_fail (str);

  ac->string_list = g_slist_prepend (ac->string_list, (void *) str);
}

/* Show the dialog where search and replace text. */
void *
replace_text (GtkWidget * widget, void *data)
{
  GtkWidget *swindow, *table, *radio[2];
  GtkWidget *label_s, *label_r;
  GtkWidget *ok, *cancel, *replace, *replace_all;
  GtkWidget *bbox;
  extern GtkWidget *jeex_main_window;
  extern JeexFileInfo *file[64];
  GSList *rgroup;
  GtkEntryCompletion *completion;
  struct search_and_replace *sar;

  sar = (struct search_and_replace *) g_malloc (sizeof (struct search_and_replace));

  /* Control that there is an open file */
  if (!_ncurrent (file)->name && !_ncurrent (file)->new)
    {
      _error (_
              ("<big><b>No files open</b></big>\n"
               "\nCannot start a search in the text.\n" "You must open a file first!"));
    }
  else
    {
      /* Main Window */
      swindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
      gtk_window_set_transient_for (GTK_WINDOW (swindow), GTK_WINDOW (jeex_main_window));
      gtk_window_set_position (GTK_WINDOW (swindow), GTK_WIN_POS_CENTER_ON_PARENT);
      gtk_window_set_modal (GTK_WINDOW (swindow), TRUE);
      gtk_window_set_title (GTK_WINDOW (swindow), _("Find"));
      gtk_container_set_border_width (GTK_CONTAINER (swindow), 8);
      gtk_widget_set_size_request (swindow, 500, 200);
      gtk_widget_show (swindow);

      /* Table */
      table = gtk_table_new (7, 6, FALSE);
      gtk_container_add (GTK_CONTAINER (swindow), table);
      gtk_widget_show (table);

      /* Search Label */
      label_s = gtk_label_new (g_strdup_printf ("                %s", _("Find: ")));
      gtk_table_attach (GTK_TABLE (table), label_s, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
      gtk_widget_show (label_s);

      /* Search Entry */
      sar->search = gtk_entry_new ();
      gtk_table_attach (GTK_TABLE (table), sar->search, 1, 6, 0, 1,
                        GTK_FILL | GTK_EXPAND, GTK_FILL, 10, 0);
      gtk_widget_show (sar->search);

      /* Autocompletion list relative at Search Entry */
      completion = gtk_entry_completion_new ();
      gtk_entry_set_completion (GTK_ENTRY (sar->search), completion);
      g_object_unref (completion);
      gtk_entry_completion_set_model (completion, GTK_TREE_MODEL (ac->list));
      gtk_entry_completion_set_text_column (completion, 0);

      /* Choice of format */
      /* Label */
      gti_find->l_insert = gtk_label_new (g_strdup_printf ("            %s", _("Format: ")));
      gtk_table_attach (GTK_TABLE (table), gti_find->l_insert, 0, 1, 1, 2,
                        GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
      gtk_widget_show (gti_find->l_insert);

      /* Options */
      gti_find->o_hex = gtk_radio_button_new_with_label (NULL, "Hex");
      gtk_widget_show (gti_find->o_hex);
      gtk_table_attach (GTK_TABLE (table), gti_find->o_hex, 1, 2, 1, 2,
                        GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

      gti_find->o_oct =
        gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (gti_find->o_hex), "Oct");
      gtk_widget_show (gti_find->o_oct);
      gtk_table_attach (GTK_TABLE (table), gti_find->o_oct, 2, 3, 1, 2,
                        GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

      gti_find->o_dec =
        gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (gti_find->o_hex), "Dec");
      gtk_widget_show (gti_find->o_dec);
      gtk_table_attach (GTK_TABLE (table), gti_find->o_dec, 3, 4, 1, 2,
                        GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

      gti_find->o_bin =
        gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (gti_find->o_hex), "Bin");
      gtk_widget_show (gti_find->o_bin);
      gtk_table_attach (GTK_TABLE (table), gti_find->o_bin, 4, 5, 1, 2,
                        GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

      gti_find->o_str =
        gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (gti_find->o_hex), "Ascii");
      gtk_widget_show (gti_find->o_str);
      gtk_table_attach (GTK_TABLE (table), gti_find->o_str, 5, 6, 1, 2,
                        GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

      gti_find->format = HEX;

      /* Replace Label */
      label_r = gtk_label_new (_(" Replace with: "));
      gtk_table_attach (GTK_TABLE (table), label_r, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
      gtk_widget_show (label_r);

      /* Replace Entry */
      sar->replace = gtk_entry_new ();
      gtk_table_attach (GTK_TABLE (table), sar->replace, 1, 6, 2, 3,
                        GTK_FILL | GTK_EXPAND, GTK_FILL, 10, 0);
      gtk_widget_show (sar->replace);

      /* Autocompletion list relative at Replace Entry */
      completion = gtk_entry_completion_new ();
      gtk_entry_set_completion (GTK_ENTRY (sar->replace), completion);
      g_object_unref (completion);
      gtk_entry_completion_set_model (completion, GTK_TREE_MODEL (ac->list));
      gtk_entry_completion_set_text_column (completion, 0);

      /* Choice of format */
      /* Label */
      gti_replace->l_insert = gtk_label_new (g_strdup_printf ("            %s", _("Format: ")));
      gtk_table_attach (GTK_TABLE (table), gti_replace->l_insert, 0, 1, 3, 4,
                        GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
      gtk_widget_show (gti_replace->l_insert);

      /* Options */
      gti_replace->o_hex = gtk_radio_button_new_with_label (NULL, "Hex");
      gtk_widget_show (gti_replace->o_hex);
      gtk_table_attach (GTK_TABLE (table), gti_replace->o_hex, 1, 2, 3, 4,
                        GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

      gti_replace->o_oct =
        gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (gti_replace->o_hex), "Oct");
      gtk_widget_show (gti_replace->o_oct);
      gtk_table_attach (GTK_TABLE (table), gti_replace->o_oct, 2, 3, 3, 4,
                        GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

      gti_replace->o_dec =
        gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (gti_replace->o_hex), "Dec");
      gtk_widget_show (gti_replace->o_dec);
      gtk_table_attach (GTK_TABLE (table), gti_replace->o_dec, 3, 4, 3, 4,
                        GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

      gti_replace->o_bin =
        gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (gti_replace->o_hex), "Bin");
      gtk_widget_show (gti_replace->o_bin);
      gtk_table_attach (GTK_TABLE (table), gti_replace->o_bin, 4, 5, 3, 4,
                        GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

      gti_replace->o_str =
        gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (gti_replace->o_hex), "Ascii");
      gtk_widget_show (gti_replace->o_str);
      gtk_table_attach (GTK_TABLE (table), gti_replace->o_str, 5, 6, 3, 4,
                        GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

      gti_replace->format = HEX;

      /* Radio Button to choose research modality */
      radio[0] = gtk_radio_button_new_with_label (NULL, _("Start search from the beginning."));
      rgroup = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radio[0]));
      gtk_widget_show (radio[0]);
      gtk_table_attach (GTK_TABLE (table), radio[0], 0, 6, 4, 5, GTK_EXPAND, GTK_FILL, 0, 0);

      radio[1] = gtk_radio_button_new_with_label (rgroup, _("Start search from the end."));
      gtk_widget_show (radio[1]);
      gtk_table_attach (GTK_TABLE (table), radio[1], 0, 6, 5, 6, GTK_EXPAND, GTK_FILL, 0, 0);

      /* Horizontal Button Box */
      bbox = gtk_hbutton_box_new ();
      gtk_button_box_set_layout (GTK_BUTTON_BOX (bbox), GTK_BUTTONBOX_SPREAD);
      gtk_widget_show (bbox);
      gtk_table_attach_defaults (GTK_TABLE (table), bbox, 0, 6, 6, 7);

      /* Cancel Button */
      cancel = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
      gtk_box_pack_start (GTK_BOX (bbox), cancel, FALSE, FALSE, 0);
      GTK_WIDGET_SET_FLAGS (cancel, GTK_CAN_DEFAULT);
      gtk_widget_grab_default (cancel);
      gtk_widget_show (cancel);

      /* Replace Button */
      replace = gtk_button_new_with_label (_("Replace"));
      gtk_widget_show (replace);
      gtk_box_pack_start (GTK_BOX (bbox), replace, FALSE, FALSE, 0);

      /* Find Button */
      ok = gtk_button_new_from_stock (GTK_STOCK_FIND);
      gtk_widget_show (ok);
      gtk_box_pack_start (GTK_BOX (bbox), ok, FALSE, FALSE, 0);

      /* Replace All Button */
      replace_all = gtk_button_new_with_label (_("Replace All"));
      GTK_WIDGET_SET_FLAGS (replace_all, GTK_CAN_DEFAULT);
      gtk_widget_show (replace_all);
      gtk_box_pack_start (GTK_BOX (bbox), replace_all, FALSE, FALSE, 0);

      /* Signals */
      g_signal_connect (G_OBJECT (cancel), "clicked", G_CALLBACK (destroy_this), (void *) swindow);
      g_signal_connect (G_OBJECT (ok), "clicked", G_CALLBACK (find_this_text), (void *) sar->search);
      g_signal_connect (G_OBJECT (radio[0]), "clicked", G_CALLBACK (find_method), (void *) FALSE);
      g_signal_connect (G_OBJECT (replace), "clicked", G_CALLBACK (single_replace), (void *) sar);
      g_signal_connect (G_OBJECT (replace_all), "clicked",
                        G_CALLBACK (search_and_replace), (void *) sar);
      g_signal_connect (G_OBJECT (radio[1]), "clicked", G_CALLBACK (find_method), (void *) TRUE);

      g_signal_connect (G_OBJECT (gti_find->o_hex), "clicked", G_CALLBACK (find_format), (void *) HEX);
      g_signal_connect (G_OBJECT (gti_find->o_oct), "clicked", G_CALLBACK (find_format), (void *) OCT);
      g_signal_connect (G_OBJECT (gti_find->o_dec), "clicked", G_CALLBACK (find_format), (void *) DEC);
      g_signal_connect (G_OBJECT (gti_find->o_bin), "clicked", G_CALLBACK (find_format), (void *) BIN);
      g_signal_connect (G_OBJECT (gti_find->o_str), "clicked", G_CALLBACK (find_format), (void *) ASCII);

      g_signal_connect (G_OBJECT (gti_replace->o_hex), "clicked",
                        G_CALLBACK (replace_format), (void *) HEX);
      g_signal_connect (G_OBJECT (gti_replace->o_oct), "clicked",
                        G_CALLBACK (replace_format), (void *) OCT);
      g_signal_connect (G_OBJECT (gti_replace->o_dec), "clicked",
                        G_CALLBACK (replace_format), (void *) DEC);
      g_signal_connect (G_OBJECT (gti_replace->o_bin), "clicked",
                        G_CALLBACK (replace_format), (void *) BIN);
      g_signal_connect (G_OBJECT (gti_replace->o_str), "clicked",
                        G_CALLBACK (replace_format), (void *) ASCII);
    }

  return NULL;
}

/* Function to insert a blank space every 4 byte, to start from choose
 * position.
 */
static char *
insert_space_string (const char *o_str, int pos)
{
  GString *str;
  int i, len = strlen (o_str ? o_str : "\0");

  if (len < 1)
    return NULL;

  switch (pos)
    {
    case 0:
      i = 0;
      break;
    case 1:
      i = 3;
      break;
    case 2:
      i = 6;
      break;
    case 3:
      i = 9;
      break;
    default:
      if (pos > 3)
        {
          i = 12;
        }
      else if (pos < 0)
        {
          i = 0;
        }
      break;
    }

  str = g_string_new (o_str);
  for (; i < len; i += 13)
    {
      g_string_insert_c (str, i, 0x20);
    }

  return str->str;
}

/* Function to replace a single occurrence */
void *
single_replace (GtkWidget * widget, void *data)
{
  struct search_and_replace *sar = (struct search_and_replace *) data;
  GtkTextIter start, begin, end;
  gboolean success = FALSE, check[2] = { FALSE, FALSE };
  GtkWidget *dialog;
  char *find, *o_find, *replace, *output, *entry;
  int i;

  /* Gets search text and the one with which to replace it */
  entry = (char *) gtk_entry_get_text (GTK_ENTRY (sar->search));
  find = obtain_value (gti_find->format, entry, &check[0]);
  entry = (char *) gtk_entry_get_text (GTK_ENTRY (sar->replace));
  replace = obtain_value (gti_replace->format, entry, &check[1]);
  entry = NULL;

  /* Adds research key in the autocompletion list */
  if (!ac->string_exist (find))
    {
      gtk_list_store_append (ac->list, &(ac->iter));
      ac->add_string (find);
      gtk_list_store_set (ac->list, &(ac->iter), 0, find, -1);
    }

  o_find = g_strdup (find);
  for (i = 0; i < 5; ++i)
    {
      if (success)
        {
          break;
        }
      else
        {
          find = insert_space_string (o_find, i);

          if (textview->find_type == FALSE)
            {
              gtk_text_buffer_get_start_iter (textview->buffer, &start);
            }
          else
            {
              gtk_text_buffer_get_end_iter (textview->buffer, &start);
            }
        }

      /* Research from start */
      if (textview->find_type == FALSE)
        {
          gtk_text_buffer_get_start_iter (textview->buffer, &start);
          success = gtk_text_iter_forward_search (&start, (char *) find, 0, &begin, &end, NULL);
          gtk_text_iter_forward_char (&start);
          success =
            gtk_text_iter_forward_search (&start, (char *) find,
                                          GTK_TEXT_SEARCH_VISIBLE_ONLY, &begin, &end, NULL);
        }
      /* Research from end */
      else
        {
          gtk_text_buffer_get_end_iter (textview->buffer, &start);
          success = gtk_text_iter_backward_search (&start, (char *) find, 0, &begin, &end, NULL);
          gtk_text_iter_backward_char (&start);
          success =
            gtk_text_iter_backward_search (&start, (char *) find,
                                           GTK_TEXT_SEARCH_VISIBLE_ONLY, &begin, &end, NULL);
        }
    }
  g_free (o_find);

  if (success)
    {
      /* Replace */
      gtk_text_buffer_delete (textview->buffer, &begin, &end);
      gtk_text_buffer_insert (textview->buffer, &begin, replace, -1);
    }
  else
    {
      output = g_strdup_printf (_("String '%s' not found."), find);
      dialog =
        gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, output, NULL);
      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);
      g_free (output);
    }

  if (find && !(*check))
    {
      g_free (find);
    }

  if (replace && !(check[1]))
    {
      g_free (replace);
    }

  return NULL;
}

/* Function to replace of all occurrance */
void *
search_and_replace (GtkWidget * widget, void *data)
{
  struct search_and_replace *sar = (struct search_and_replace *) data;
  GtkTextIter start, begin, end;
  gboolean success, check[2] = { FALSE, FALSE }, cc = FALSE;
  int i = 0, c = i;
  GtkWidget *dialog;
  char *find, *o_find, *replace, *entry;
  char *output;

  /* Gets search text and the one with which to replace it */
  entry = (char *) gtk_entry_get_text (GTK_ENTRY (sar->search));
  find = obtain_value (gti_find->format, entry, &check[0]);
  entry = (char *) gtk_entry_get_text (GTK_ENTRY (sar->replace));
  replace = obtain_value (gti_replace->format, entry, &check[1]);
  entry = NULL;

  /* Adds research key in the autocompletion list */
  if (!ac->string_exist (find))
    {
      gtk_list_store_append (ac->list, &(ac->iter));
      ac->add_string (find);
      gtk_list_store_set (ac->list, &(ac->iter), 0, find, -1);
    }

  /* Research from start */
  if (textview->find_type == FALSE)
    {
      gtk_text_buffer_get_start_iter (textview->buffer, &start);
      success = gtk_text_iter_forward_search (&start, (char *) find, 0, &begin, &end, NULL);
    }
  /* Research from end */
  else
    {
      gtk_text_buffer_get_end_iter (textview->buffer, &start);
      success = gtk_text_iter_backward_search (&start, (char *) find, 0, &begin, &end, NULL);
    }

  o_find = g_strdup (find);
  for (i = 0; i < 5; ++i)
    {
      find = insert_space_string (o_find, i);

      while (success)
        {
          /* Research from start */
          if (textview->find_type == FALSE)
            {
              gtk_text_iter_forward_char (&start);
              success =
                gtk_text_iter_forward_search (&start, (char *) find,
                                              GTK_TEXT_SEARCH_VISIBLE_ONLY, &begin, &end, NULL);
            }
          /* Research from end */
          else
            {
              gtk_text_iter_backward_char (&start);
              success =
                gtk_text_iter_backward_search (&start, (char *) find,
                                               GTK_TEXT_SEARCH_VISIBLE_ONLY, &begin, &end, NULL);
            }

          /* Replace */
          if (success)
            {
              c++;
              gtk_text_buffer_delete (textview->buffer, &begin, &end);
              gtk_text_buffer_insert (textview->buffer, &begin, replace, -1);
              start = begin;
              cc = TRUE;
            }
        }

      if (textview->find_type == FALSE)
        {
          gtk_text_buffer_get_start_iter (textview->buffer, &start);
        }
      else
        {
          gtk_text_buffer_get_end_iter (textview->buffer, &start);
        }
    }

  /* If wasn't found no string.. */
  if (!cc)
    {
      output = g_strdup_printf (_("String '%s' not found"), find);
    }
  /* ..otherwise returns a message with the number of 
   * occurrences found in the text.
   */
  else
    {
      output = g_strdup_printf (_("Found and replaced %i occurrences."), c);
    }
  dialog =
    gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, output, NULL);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
  g_free (output);

  if (find && !(*check))
    {
      g_free (find);
    }

  if (replace && !(check[1]))
    {
      g_free (replace);
    }

  return NULL;
}

/* Function to take the input text to search */
void *
find_text (GtkWidget * widget, void *data)
{
  GtkWidget *swindow, *table, *label, *entry, *ok, *cancel, *bbox, *radio[2];
  extern GtkWidget *jeex_main_window;
  extern JeexFileInfo *file[64];
  GSList *rgroup;
  GtkEntryCompletion *completion;

  if (!_ncurrent (file)->name && !_ncurrent (file)->new)
    {
      _error (_
              ("<big><b>No files open</b></big>\n"
               "\nCannot start a search in the text.\n" "You must open a file first!"));
    }
  else
    {
      /* Window */
      swindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
      gtk_window_set_modal (GTK_WINDOW (swindow), TRUE);
      gtk_window_set_transient_for (GTK_WINDOW (swindow), GTK_WINDOW (jeex_main_window));
      gtk_window_set_position (GTK_WINDOW (swindow), GTK_WIN_POS_CENTER_ON_PARENT);
      gtk_window_set_title (GTK_WINDOW (swindow), _("Find"));
      gtk_container_set_border_width (GTK_CONTAINER (swindow), 15);
      gtk_widget_set_size_request (swindow, 350, 150);
      gtk_widget_show (swindow);

      /* Table */
      table = gtk_table_new (5, 6, FALSE);
      gtk_container_add (GTK_CONTAINER (swindow), table);
      gtk_widget_show (table);

      /* Search Label */
      label = gtk_label_new (_("Find: "));
      gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
      gtk_widget_show (label);

      /* Search Entry */
      entry = gtk_entry_new ();
      gtk_table_attach (GTK_TABLE (table), entry, 1, 6, 0, 1, GTK_FILL | GTK_EXPAND, GTK_FILL, 10, 0);
      gtk_widget_show (entry);

      /* Autocompletion list relative at Replace Entry */
      completion = gtk_entry_completion_new ();
      gtk_entry_set_completion (GTK_ENTRY (entry), completion);
      g_object_unref (completion);
      gtk_entry_completion_set_model (completion, GTK_TREE_MODEL (ac->list));
      gtk_entry_completion_set_text_column (completion, 0);

      /* Choice of format */
      /* Label */
      gti_find->l_insert = gtk_label_new (_(" Format: "));
      gtk_table_attach (GTK_TABLE (table), gti_find->l_insert, 0, 1, 1, 2,
                        GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
      gtk_widget_show (gti_find->l_insert);

      /* Options */
      gti_find->o_hex = gtk_radio_button_new_with_label (NULL, "Hex");
      gtk_widget_show (gti_find->o_hex);
      gtk_table_attach (GTK_TABLE (table), gti_find->o_hex, 1, 2, 1, 2,
                        GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

      gti_find->o_oct =
        gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (gti_find->o_hex), "Oct");
      gtk_widget_show (gti_find->o_oct);
      gtk_table_attach (GTK_TABLE (table), gti_find->o_oct, 2, 3, 1, 2,
                        GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

      gti_find->o_dec =
        gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (gti_find->o_hex), "Dec");
      gtk_widget_show (gti_find->o_dec);
      gtk_table_attach (GTK_TABLE (table), gti_find->o_dec, 3, 4, 1, 2,
                        GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

      gti_find->o_bin =
        gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (gti_find->o_hex), "Bin");
      gtk_widget_show (gti_find->o_bin);
      gtk_table_attach (GTK_TABLE (table), gti_find->o_bin, 4, 5, 1, 2,
                        GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

      gti_find->o_str =
        gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (gti_find->o_hex), "Ascii");
      gtk_widget_show (gti_find->o_str);
      gtk_table_attach (GTK_TABLE (table), gti_find->o_str, 5, 6, 1, 2,
                        GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

      gti_find->format = HEX;

      /* Radio Button to choose the research modality */
      radio[0] = gtk_radio_button_new_with_label (NULL, _("Start search from the beginning."));
      rgroup = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radio[0]));
      gtk_widget_show (radio[0]);
      gtk_table_attach (GTK_TABLE (table), radio[0], 0, 6, 2, 3, GTK_EXPAND, GTK_FILL, 0, 0);

      radio[1] = gtk_radio_button_new_with_label (rgroup, _("Start search from the end."));
      gtk_widget_show (radio[1]);
      gtk_table_attach (GTK_TABLE (table), radio[1], 0, 6, 3, 4, GTK_EXPAND, GTK_FILL, 0, 0);

      /* Horizontal Button Box */
      bbox = gtk_hbutton_box_new ();
      gtk_table_attach_defaults (GTK_TABLE (table), bbox, 0, 6, 4, 5);
      gtk_widget_show (bbox);

      /* Close Button */
      cancel = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
      gtk_box_pack_start (GTK_BOX (bbox), cancel, FALSE, FALSE, 0);
      GTK_WIDGET_SET_FLAGS (cancel, GTK_CAN_DEFAULT);
      gtk_widget_grab_default (cancel);
      gtk_widget_show (cancel);

      /* Find Button */
      ok = gtk_button_new_from_stock (GTK_STOCK_FIND);
      gtk_widget_show (ok);
      gtk_box_pack_start (GTK_BOX (bbox), ok, FALSE, FALSE, 0);


      /* Signals */
      g_signal_connect (G_OBJECT (cancel), "clicked", G_CALLBACK (destroy_this), (void *) swindow);
      g_signal_connect (G_OBJECT (ok), "clicked", G_CALLBACK (find_this_text), (void *) entry);
      g_signal_connect (G_OBJECT (radio[0]), "clicked", G_CALLBACK (find_method), (void *) FALSE);
      g_signal_connect (G_OBJECT (radio[1]), "clicked", G_CALLBACK (find_method), (void *) TRUE);

      g_signal_connect (G_OBJECT (gti_find->o_hex), "clicked", G_CALLBACK (find_format), (void *) HEX);
      g_signal_connect (G_OBJECT (gti_find->o_oct), "clicked", G_CALLBACK (find_format), (void *) OCT);
      g_signal_connect (G_OBJECT (gti_find->o_dec), "clicked", G_CALLBACK (find_format), (void *) DEC);
      g_signal_connect (G_OBJECT (gti_find->o_bin), "clicked", G_CALLBACK (find_format), (void *) BIN);
      g_signal_connect (G_OBJECT (gti_find->o_str), "clicked", G_CALLBACK (find_format), (void *) ASCII);
    }
  return NULL;
}

/* Function for searching the text starting from the text beginning,
 * receives as parameters the GtkTextBuffer, as second the search text,
 * for third the string length or, if string is NULL-terminated, -1 and
 * as fourth and last the boolean variable returned from previous 
 * research. 
 * Returns TRUE if string was found or if boolean variable is TRUE, 
 * FALSE if wasn't found it or if some error occurred.
 */
static gboolean
jeex_text_forward_search (GtkTextBuffer * buff, char *str, int l_str, gboolean p)
{
  GtkTextMark *mark = NULL;
  GtkTextIter start, begin, end;
  gboolean chk = FALSE, s;
  int len = l_str, i;

  g_return_val_if_fail (str || buff, FALSE);
  g_return_val_if_fail (l_str >= -1, FALSE);

  if (len == -1)
    {
      len = strlen (str);
    }

  /* Initialize Iter */
  gtk_text_buffer_get_start_iter (buff, &start);
  s = gtk_text_iter_forward_search (&start, str, 0, &begin, &end, NULL);

  while (s)
    {
      /* Wanted in the visible text the word choice and the 
       * recovery ites of beginning and end.
       */
      s = gtk_text_iter_forward_search (&start, str, GTK_TEXT_SEARCH_VISIBLE_ONLY, &begin, &end, NULL);

      /* If was found an occurrence, i apply the highlighting's tag */
      if (s)
        {
          gtk_text_buffer_apply_tag (buff, textview->tag, &begin, &end);
          mark = gtk_text_buffer_create_mark (buff, "mark_word", &end, FALSE);
          start = begin;
          chk = TRUE;
        }
      else
        {
          break;
        }

      /* Surplus up to exceed the word just found, this "for" was 
       * inserted to correct a bug present until version 8.6 of jeex.
       */
      for (i = 0; i < len; ++i)
        {
          gtk_text_iter_forward_char (&start);
        }
    }

  /* If were found occurrences move the scrollbar at 
   * point where it was found the last.
   */
  if (chk && GTK_IS_TEXT_MARK (mark))
    {
      gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (textview->textview), mark);
    }

  return (p ? TRUE : chk);
}

/* Function for searching the text starting from the text ending,
 * receives as parameters the GtkTextBuffer, as second the search text,
 * for third the string length or, if string is NULL-terminated, -1 and
 * as fourth and last the boolean variable returned from previous 
 * research. 
 * Returns TRUE if string was found or if boolean variable is TRUE, 
 * FALSE if wasn't found it or if some error occurred.
 */
static gboolean
jeex_text_backward_search (GtkTextBuffer * buff, char *str, int l_str, gboolean p)
{
  GtkTextMark *mark = NULL;
  GtkTextIter start, begin, end;
  gboolean chk = FALSE, s;
  int len = l_str, i;

  g_return_val_if_fail (str || buff, FALSE);
  g_return_val_if_fail (l_str >= -1, FALSE);

  if (len == -1)
    {
      len = strlen (str);
    }

  /* Initialize iter */
  gtk_text_buffer_get_end_iter (buff, &start);
  s = gtk_text_iter_backward_search (&start, str, 0, &begin, &end, NULL);

  while (s)
    {
      /* Wanted in the visible text the word choice and the 
       * recovery ites of beginning and end.
       */
      s = gtk_text_iter_backward_search (&start, str, GTK_TEXT_SEARCH_VISIBLE_ONLY, &begin, &end, NULL);

      /* If was found a occurrence, apply highlighting tag */
      if (s)
        {
          gtk_text_buffer_apply_tag (buff, textview->tag, &begin, &end);
          mark = gtk_text_buffer_create_mark (buff, "mark_word", &end, FALSE);
          start = begin;
          chk = TRUE;
        }
      else
        {
          break;
        }

      /* Going back up to exceed the word just found, this "for" was 
       * inserted to correct a bug present until version 8.6 of jeex.
       */
      for (i = 0; i < len; ++i)
        {
          gtk_text_iter_backward_char (&start);
        }
    }

  /* If were found occurrences move the scrollbar at 
   * point where it was found the last.
   */
  if (chk && GTK_IS_TEXT_MARK (mark))
    {
      gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (textview->textview), mark);
    }

  return (p ? TRUE : chk);
}

/* Function to search text */
void *
find_this_text (GtkWidget * widget, void *data)
{
  gboolean check = FALSE, cc = FALSE;
  int i = 0;
  GtkWidget *dialog;
  char *find, *o_find, *entry;
  char *output;
  extern JeexMenu *jeex_menu;

  /* Gets text entered in research entry. */
  entry = (char *) gtk_entry_get_text (GTK_ENTRY ((GtkWidget *) data));
  find = obtain_value (gti_find->format, entry, &check);
  entry = NULL;

  g_return_val_if_fail (find, NULL);

  /* Adds research key in the autocompletion list */
  if (!ac->string_exist (find))
    {
      gtk_list_store_append (ac->list, &(ac->iter));
      ac->add_string (find);
      gtk_list_store_set (ac->list, &(ac->iter), 0, find, -1);
    }

  o_find = g_strdup (find);

  /* Research from start */
  if (!textview->find_type)
    {
      for (i = 0; i < 5; ++i)
        {
          /* Making search string */
          find = insert_space_string (o_find, i);
          cc = jeex_text_forward_search (textview->buffer, find, -1, cc);
          g_free (find);
        }
    }
  /* Research from end */
  else
    {
      for (i = 0; i < 5; ++i)
        {
          /* Making search string */
          find = insert_space_string (o_find, i);
          cc = jeex_text_backward_search (textview->buffer, find, -1, cc);
          g_free (find);
        }
    }


  /* If wasn't found any occurrence, see the information message. */
  if (!cc)
    {
      output = g_strdup_printf (_("String '%s' not found."), o_find);
      dialog =
        gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, output, NULL);
      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);
      g_free (output);

    }
  /* If was found instead abling the button to reset the 
   * highlight of the text.
   */
  else
    {
      gtk_widget_set_sensitive (jeex_menu->search_menu.reset_high, TRUE);
    }

  g_free (o_find);

  return NULL;
}

/* Function to reset text highlighting */
void *
reset_tag (GtkWidget * widget, void *data)
{
  GtkTextIter start, end;
  extern JeexMenu *jeex_menu;

  /* Gets start and end inter of text */
  gtk_text_buffer_get_start_iter (textview->buffer, &start);
  gtk_text_buffer_get_end_iter (textview->buffer, &end);

  /* Erasing all tags applied in the text */
  gtk_text_buffer_remove_all_tags (textview->buffer, &start, &end);

  gtk_widget_set_sensitive (jeex_menu->search_menu.reset_high, FALSE);

  return NULL;
}

/* Function to determine the method of research */
void *
find_method (GtkWidget * widget, gboolean type)
{
  textview->find_type = type;

  return NULL;
}

/* Go to offset line choosed */
void *
jump_to_offset_gui (GtkWidget * widget, void **data)
{
  GtkWidget *win, *table, *label, *max_value, *entry, *go, *close, *hbox, *box;
  extern GtkWidget *jeex_main_window;
  char *markup;

  /* Window */
  win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_modal (GTK_WINDOW (win), FALSE);
  gtk_window_set_transient_for (GTK_WINDOW (win), GTK_WINDOW (jeex_main_window));
  gtk_window_set_position (GTK_WINDOW (win), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_title (GTK_WINDOW (win), _("Go to offset"));
  gtk_widget_set_size_request (win, 330, 82);
  gtk_container_set_border_width (GTK_CONTAINER (win), 5);
  gtk_widget_show (win);

  /* Table */
  table = gtk_table_new (2, 3, FALSE);
  gtk_container_add (GTK_CONTAINER (win), table);
  gtk_widget_show (table);

  /* Label */
  label = gtk_label_new (_("Offset: "));
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  /* Entry */
  entry = gtk_entry_new ();
  gtk_entry_set_max_length (GTK_ENTRY (entry), 8);
  gtk_widget_set_size_request (entry, -1, 20);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
  gtk_widget_show (entry);

  /* Max Offset Value */
  markup =
    g_markup_printf_escaped (" <b>&lt;=</b> <i>%8.8X</i> ",
                             gtk_text_buffer_get_line_count (textview->buffer) * 0x10);
  max_value = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (max_value), markup);
  g_free (markup);
  gtk_table_attach (GTK_TABLE (table), max_value, 2, 3, 0, 1, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
  gtk_widget_show (max_value);

  /* Horizontal Box */
  hbox = gtk_hbox_new (TRUE, 0);
  gtk_table_attach_defaults (GTK_TABLE (table), hbox, 0, 3, 1, 2);
  gtk_widget_show (hbox);

  /* Horizontal Button Box */
  box = gtk_hbutton_box_new ();
  gtk_button_box_set_layout (GTK_BUTTON_BOX (box), GTK_BUTTONBOX_EDGE);
  gtk_container_add (GTK_CONTAINER (hbox), box);
  gtk_widget_show (box);

  /* Close Button */
  close = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  GTK_WIDGET_SET_FLAGS (close, GTK_CAN_DEFAULT);
  gtk_widget_grab_default (close);
  gtk_container_add (GTK_CONTAINER (box), close);
  gtk_widget_show (close);

  /* Go Button */
  go = gtk_button_new_from_stock (GTK_STOCK_JUMP_TO);
  gtk_container_add (GTK_CONTAINER (box), go);
  gtk_widget_show (go);

  g_signal_connect (G_OBJECT (close), "clicked", G_CALLBACK (destroy_this), (void *) win);

  g_signal_connect (G_OBJECT (go), "clicked", G_CALLBACK (jump_to_offset), (void *) entry);

  return NULL;
}

/* Go to offset choosed */
void *
jump_to_offset (GtkWidget * widget, GtkEntry * entry)
{
  char *offset, c;
  int i, len, offset_v;
  GtkTextIter iter;
  GtkTextMark *mark;

  offset = (char *) gtk_entry_get_text (entry);
  offset = g_utf8_normalize (offset, -1, G_NORMALIZE_DEFAULT);
  len = strlen (offset);

  /* Checks that string isn't empty */
  if (strisnull (offset))
    {
      _error (_("<big><b>String is empty.</b></big>"));
      return NULL;
    }

  offset = g_ascii_strup (offset, -1);

  for (i = 0; i < len; ++i)
    {
      if (!g_ascii_isxdigit (offset[i]))
        {
          _error (_("<big><b>Entered offset not valid.</b></big>"));
          return NULL;
        }
    }

  /* Conversion from hexadecimal to decimal */
  offset_v = 0;
  for (i = 0; i < len; ++i)
    {
      if (offset[i] >= 0x30 && offset[i] <= 0x39)
        {
          c = offset[i] - 0x30;
        }
      else
        {
          c = offset[i] - 0x37;
        }
      offset_v += c * pow (16, (len - 1) - i);
    }
  offset_v /= 16;

  /* Positioning on offset chosen */
  gtk_text_buffer_get_iter_at_mark (textview->buffer, &iter,
                                    gtk_text_buffer_get_insert (textview->buffer));
  gtk_text_iter_set_line (&iter, offset_v);
  mark = gtk_text_buffer_create_mark (textview->buffer, "mark_offset", &iter, FALSE);
  if (GTK_IS_TEXT_MARK (mark))
    {
      gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (textview->textview), mark);
    }

  g_free (offset);
  return NULL;
}

/* Hides regex bar */
void *
regex_close_bar (GtkWidget * widget, void **data)
{
  gtk_widget_hide (textview->regex->table);

  return NULL;
}

/* Shows regex bar */
void *
regex_show_bar (GtkWidget * widget, void **data)
{
  gtk_widget_show (textview->regex->table);

  return NULL;
}


static gboolean
regex_search_block (char *s_regex, char *data)
{
  int i;
  GRegex *regex;
  GMatchInfo *match_info;
  char *c_word, *find;
  GArray *p_word;
#ifndef regex_time_limit_off
  time_t timer = time (NULL);
#endif

  if (!data)
    return FALSE;

  if (!(regex = g_regex_new (s_regex, 0, 0, NULL)))
    {
      _error (_("<big><b>Error while make a new regex.</b></big>"));
      return FALSE;
    }

  if (!g_regex_match_full (regex, data, -1, 0, 0, &match_info, NULL))
    {
      return FALSE;
    }

  p_word = g_array_new (TRUE, TRUE, sizeof (char *));

  while (g_match_info_matches (match_info))
    {
#ifndef regex_time_limit_off
      if ((time (NULL) - timer) > regex_time_limit)
        {
          _info (_("<big><b>Time available exhausted</b></big>"), FALSE);
          g_match_info_free (match_info);
          g_regex_unref (regex);

          return FALSE;
        }
#endif

      c_word = g_match_info_fetch (match_info, 0);
      if (!c_word)
        {
#if defined(DEBUG_ENABLE) && DEBUG_ENABLE
          g_debug ("%s:%d - regex_search_text:\n"
                   "Error has occurred in function g_match_info_fetch.\n"
                   "Jeex don't know why because the regex are of the glib.\n"
                   "Sorry for the inconvenience.", __FILE__, __LINE__);
#endif
          return FALSE;
        }

      for (i = 0; (find = g_array_index (p_word, char *, i)); i++)
        {
          if (!strcmp (find, c_word))
            {
              g_free (c_word);
              g_match_info_next (match_info, NULL);

              i = -1;
              break;
            }
        }

      if (i < 0)
        continue;

      if (*c_word != 0x00)
        {
          for (i = 0; i < 5; i++)
            {
              find = insert_space_string (c_word, i);
              if (!find)
                break;
              jeex_text_forward_search (textview->buffer, find, -1, FALSE);
              g_free (find);
            }
        }

      find = g_strdup (c_word);
      g_array_prepend_val (p_word, find);
      g_free (c_word);

      g_match_info_next (match_info, NULL);
    }

  for (i = 0; (find = g_array_index (p_word, char *, i)); i++)
    g_free (find);
  g_array_free (p_word, TRUE);
  g_match_info_free (match_info);
  g_regex_unref (regex);

  return TRUE;
}

void *
regex_search_text (GtkWidget * widget, void *data)
{
  char *s_regex, *t_bytes, *s_exam;
  long total, bexam;
  GtkTextIter start, end;
  GtkTextRegex *wregex = (GtkTextRegex *) data;
  extern JeexMenu *jeex_menu;
#ifndef regex_time_limit_off
  time_t timer = time (NULL);
#endif

  s_regex = (char *) gtk_entry_get_text (GTK_ENTRY (wregex->e_regex));
  if (*s_regex == '\0')
    {
      _error (_("<big><b>Regex not found.</b></big>"));
      return NULL;
    }

  gtk_text_buffer_get_bounds (_ncurrent (notebook->buffer), &start, &end);
  t_bytes = gtk_text_buffer_get_text (_ncurrent (notebook->buffer), &start, &end, FALSE);
  total = g_utf8_strlen (t_bytes, -1);

  for (bexam = 0xC000; bexam < total; bexam += 0xC000)
    {
      if (bexam > 0xC000)
        s_exam = g_strndup (t_bytes + (bexam - 0xC098), 0xC000);
      else
        s_exam = g_strndup (t_bytes + (bexam - 0xC000), 0xC000);

      if (!regex_search_block (s_regex, s_exam))
        {
          g_free (s_exam);
          bexam = -1;
          break;
        }

      g_free (s_exam);

#ifndef regex_time_limit_off
      if ((time (NULL) - timer) > regex_time_limit)
        {
          _info (_("<big><b>Time available exhausted</b></big>"), FALSE);
          bexam = -1;
          break;
        }
#endif
    }

  if (bexam != -1)
    {
      s_exam = g_strdup (t_bytes + (bexam - 0xC000));
      regex_search_block (s_regex, s_exam);
      g_free (s_exam);
    }

  gtk_widget_set_sensitive (jeex_menu->search_menu.reset_high, TRUE);
  g_free (t_bytes);

  return NULL;
}

void *
regex_entry_clear (GtkWidget * widget, void *data)
{
  GtkTextRegex *wregex = (GtkTextRegex *) data;

  gtk_entry_set_text (GTK_ENTRY (wregex->e_regex), "\0");

  return NULL;
}
