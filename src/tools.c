/*
 * "tools.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <libintl.h>
#include <time.h>
#include <locale.h>
#include "header.h"

extern JeexFileInfo *file[64];
struct show_file_struct show_file_struct;
static GString **file_struct = NULL;
static int current_struct = 0;
static int total_struct = 0;
static int alloc_file_struct = 0;
static int line_type = 0;

static void make_file_struct (GString ***, char *, int, JeexTypeCode *);
static void highlight_syntax (void);

void *
show_file_structures (GtkWidget * widget, void *data)
{
  gtk_widget_show (show_file_struct.main_widget);

  return NULL;
}

/* Function to show a window to split a file */
void *
split_window_file (GtkWidget * widget, void *data)
{
  GtkSplit *_swf;
  GtkWidget *image;
  extern GtkWidget *jeex_main_window;

  /* Allocating memory for the structure */
  _swf = (GtkSplit *) g_malloc (sizeof (GtkSplit));
  _swf->opt = (struct jeex_split *) g_malloc0 (sizeof (struct jeex_split));

  /* Window */
  _swf->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (_swf->window), Jeex);
  gtk_window_set_default_size (GTK_WINDOW (_swf->window), 350, 170);
  gtk_container_set_border_width (GTK_CONTAINER (_swf->window), 5);
  gtk_window_set_modal (GTK_WINDOW (_swf->window), TRUE);
  gtk_window_set_transient_for (GTK_WINDOW (_swf->window), GTK_WINDOW (jeex_main_window));
  gtk_window_set_position (GTK_WINDOW (_swf->window), GTK_WIN_POS_CENTER_ON_PARENT);

  /* Table */
  _swf->table = gtk_table_new (5, 2, FALSE);
  gtk_container_add (GTK_CONTAINER (_swf->window), _swf->table);

  /* Box */
  _swf->hbox = gtk_hbox_new (FALSE, 0);
  gtk_table_attach_defaults (GTK_TABLE (_swf->table), _swf->hbox, 0, 2, 4, 5);

  _swf->cbox = gtk_hbutton_box_new ();
  gtk_box_pack_start (GTK_BOX (_swf->hbox), _swf->cbox, TRUE, TRUE, 5);

  /* Close Button */
  _swf->cancel = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  GTK_WIDGET_SET_FLAGS (_swf->cancel, GTK_CAN_DEFAULT);
  gtk_container_add (GTK_CONTAINER (_swf->cbox), _swf->cancel);

  /* Split Button */
  image = gtk_image_new_from_stock (GTK_STOCK_DND_MULTIPLE, GTK_ICON_SIZE_BUTTON);
  _swf->split = gtk_button_new_with_label (_("Split"));
  gtk_button_set_image (GTK_BUTTON (_swf->split), image);
  GTK_WIDGET_SET_FLAGS (_swf->split, GTK_CAN_DEFAULT);
  gtk_widget_grab_default (_swf->split);
  gtk_container_add (GTK_CONTAINER (_swf->cbox), _swf->split);

  /* Output */
  _swf->l_output = gtk_label_new (_("Destination Folder: "));
  gtk_table_attach (GTK_TABLE (_swf->table), _swf->l_output, 0, 1, 0, 1,
                    GTK_FILL, GTK_FILL | GTK_EXPAND, 0, 0);

  _swf->m_output =
    gtk_file_chooser_button_new (_("Choose folder.."), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
  gtk_table_attach (GTK_TABLE (_swf->table), _swf->m_output, 1, 2, 0, 1,
                    GTK_FILL, GTK_FILL | GTK_EXPAND, 0, 0);

  _swf->t_output = gtk_label_new (_("Destination file name: "));
  gtk_table_attach (GTK_TABLE (_swf->table), _swf->t_output, 0, 1, 1, 2,
                    GTK_FILL, GTK_FILL | GTK_EXPAND, 0, 0);

  _swf->e_output = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE (_swf->table), _swf->e_output, 1, 2, 1, 2,
                    GTK_FILL, GTK_FILL | GTK_EXPAND, 0, 0);

  _swf->check_default_output = gtk_check_button_new_with_label (_("Use default destination"));
  _swf->output = TRUE;
  gtk_table_attach (GTK_TABLE (_swf->table), _swf->check_default_output, 0, 2,
                    2, 3, GTK_EXPAND, GTK_FILL | GTK_EXPAND, 0, 0);

  /* Bytes */
  _swf->l_bytes = gtk_label_new (_("Bytes in each file: "));
  gtk_table_attach (GTK_TABLE (_swf->table), _swf->l_bytes, 0, 1, 3, 4,
                    GTK_FILL, GTK_FILL | GTK_EXPAND, 0, 0);

  _swf->s_bytes = gtk_spin_button_new_with_range (1.0, G_MAXINT, 1.0);
  gtk_table_attach (GTK_TABLE (_swf->table), _swf->s_bytes, 1, 2, 3, 4,
                    GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

  /* Signals */
  g_signal_connect (G_OBJECT (_swf->cancel), "clicked", G_CALLBACK (destroy_this), _swf->window);
  g_signal_connect (G_OBJECT (_swf->check_default_output), "toggled",
                    G_CALLBACK (split_disable_output), _swf);
  g_signal_connect (G_OBJECT (_swf->split), "clicked", G_CALLBACK (split_file), _swf);

  gtk_widget_show_all (_swf->window);

  return NULL;
}

/* Disable widgets regarding the output where split file */
void *
split_disable_output (GtkWidget * widget, GtkSplit * o)
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

/* Function to split the file */
void *
split_file (GtkWidget * widget, GtkSplit * w)
{
  FILE *fi, *fo;
  int bytes, n_file = 1;
  unsigned char *tmp;
  char *output;
  GFileError error_type;

  /* Gets output file name */
  if (w->output)
    {
      if (strisnull ((char *) gtk_entry_get_text (GTK_ENTRY (w->e_output))))
        {
          _error (_("<big><b>No Output File</b></big>\n" "\nYou must specify an output file name!"));
          return NULL;
        }
      else
        {
          w->opt->output =
            g_strdup_printf ("%s/%s",
                             gtk_file_chooser_get_current_folder
                             (GTK_FILE_CHOOSER (w->m_output)),
                             gtk_entry_get_text (GTK_ENTRY (w->e_output)));
        }
    }
  else
    {
      w->opt->output = g_strdup (_ncurrent (file)->name);
    }

  /* Gets input file name */
  if (!(fi = fopen (_ncurrent (file)->name, "r")))
    {
      error_type = g_file_error_from_errno (errno);
      file_error_control (error_type, RB_MODE);
      return NULL;
    }

  /* Gets file size */
  fseek (fi, 0, SEEK_END);
  bytes = ftell (fi);
  rewind (fi);

  /* Checking whether the file size is less than that choice, if it
   * were the most from reading would be the total bytes,
   * otherwise those chosen by the user.
   */
  if (bytes < gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (w->s_bytes)))
    {
      w->opt->bytes = bytes;
    }
  else
    {
      w->opt->bytes = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (w->s_bytes));
    }

  /* Split of file */
  while (1)
    {
      output = g_strdup_printf ("%s.%d", w->opt->output, n_file);

      if (!(fo = fopen (output, "w")))
        {
          error_type = g_file_error_from_errno (errno);
          file_error_control (error_type, WB_MODE);
          fclose (fi);
          return NULL;
        }

      /* Case in which there are still more files to be written (> 1) */
      if (bytes > w->opt->bytes)
        {
          tmp = (unsigned char *) g_try_malloc (w->opt->bytes * sizeof (unsigned char));
          fread (tmp, sizeof (unsigned char), w->opt->bytes, fi);
          fwrite (tmp, sizeof (unsigned char), w->opt->bytes, fo);
          g_free (tmp);
          bytes -= w->opt->bytes;;
        }
      else
        {
          tmp = (unsigned char *) g_try_malloc (bytes * sizeof (unsigned char));
          fread (tmp, sizeof (unsigned char), bytes, fi);
          fwrite (tmp, sizeof (unsigned char), bytes, fo);
          fclose (fo);
          g_free (output);
          break;
        }

      fclose (fo);
      g_free (output);
      n_file++;
    }

  fclose (fi);

  _info (_("File has been split\n"), FALSE);

  return NULL;
}

/* Extracts all present strings in opened file */
void *
strings_extract (GtkWidget * widget, void *data)
{
  GtkWidget *win, *table, *lcs, *label, *treelist, *scroll;
  GtkWidget *close;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  GtkListStore *store;
  extern GtkWidget *jeex_main_window;
  GtkTreeIter iter;
  char *bin, *str = NULL;
  int i, count, totstr = 0, len;

  /* Main window */
  win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (win), _("Strings Extractor"));
  gtk_window_set_modal (GTK_WINDOW (win), TRUE);
  gtk_window_set_default_size (GTK_WINDOW (win), -1, 320);
  gtk_container_set_border_width (GTK_CONTAINER (win), 10);
  gtk_window_set_transient_for (GTK_WINDOW (win), GTK_WINDOW (jeex_main_window));
  gtk_window_set_position (GTK_WINDOW (win), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_widget_show (win);

  /* Table */
  table = gtk_table_new (4, 1, FALSE);
  gtk_container_add (GTK_CONTAINER (win), table);
  gtk_widget_show (table);

  /* Title */
  label = gtk_label_new (_("Here are all the strings found in the current file.\n"));
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  /* List */
  treelist = gtk_tree_view_new ();
  renderer = gtk_cell_renderer_text_new ();
  store = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_STRING);

  /* Making column */
  column = gtk_tree_view_column_new_with_attributes ("Offset", renderer, "text", 0, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treelist), column);
  gtk_tree_view_column_set_resizable (column, TRUE);

  column = gtk_tree_view_column_new_with_attributes ("String", renderer, "text", 1, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treelist), column);
  gtk_tree_view_column_set_resizable (column, TRUE);

  bin = ohfa (&len);
  update_status_bar (NULL, NULL);

  /* Extracting strings */
  for (i = count = 0; i < len; ++i)
    {
      if (g_ascii_isgraph (bin[i]) || bin[i] == 0x20)
        {
          if (!count)
            str = g_strdup_printf ("%c", bin[i]);
          else
            str = g_strdup_printf ("%s%c", str, bin[i]);
          count++;
        }
      else
        {
          if (str)
            {
              if (count > 3)
                {
                  gtk_list_store_append (store, &iter);
                  gtk_list_store_set (store, &iter, 0,
                                      g_strdup_printf ("%8.8X", ((i / 16) * 0x10) + 0x10), 1, str, -1);
                  totstr++;
                }
              g_free (str);
              str = NULL;
            }
          count = 0;
        }
    }
  if (str)
    {
      if (count >= 3)
        {
          gtk_list_store_append (store, &iter);
          gtk_list_store_set (store, &iter, 0,
                              g_strdup_printf ("%8.8X", ((i / 16) * 0x10) + 0x10), 1, str, -1);
          totstr++;
        }
      g_free (str);
      str = NULL;
    }
  free (bin);
  gtk_tree_view_set_model (GTK_TREE_VIEW (treelist), GTK_TREE_MODEL (store));
  g_object_unref (store);
  gtk_widget_show (treelist);

  /* Adds scrollbar to list */
  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scroll), treelist);
  gtk_table_attach (GTK_TABLE (table), scroll, 0, 1, 1, 2, GTK_FILL, GTK_FILL | GTK_EXPAND, 0, 0);
  gtk_widget_show (scroll);

  /* Label to info on strings number */
  lcs = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (lcs),
                        g_strdup_printf (_("\n<b>Strings found</b>: <i>%d</i>\n"), totstr));
  gtk_label_set_selectable (GTK_LABEL (lcs), TRUE);
  gtk_table_attach (GTK_TABLE (table), lcs, 0, 1, 2, 3, GTK_FILL | GTK_FILL, GTK_FILL | GTK_FILL, 0, 0);
  gtk_widget_show (lcs);

  /* Close Button */
  close = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  gtk_table_attach (GTK_TABLE (table), close, 0, 1, 3, 4, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
  gtk_widget_show (close);

  g_signal_connect (G_OBJECT (close), "clicked", G_CALLBACK (destroy_this), (void *) win);

  return NULL;
}

void
show_file_struct_search (void)
{
  char *types, **type, *data, **tmp;
  int len, size_types = 0, i;
  JeexTypeCode *type_code;

  current_struct = 0;

  types = (char *) gtk_entry_get_text (GTK_ENTRY (show_file_struct.e_types));
  if (!(*types))
    {
      jeex_widget_show (show_file_struct.error_label);
      return;
    }
  
  tmp = g_strsplit (types, ",", -1);
  type = types_analyze (tmp);
  g_strfreev (tmp);
  
  types = NULL;
  if (!(check_valid_type (type, &type_code)))
    {
      jeex_widget_show (show_file_struct.error_label);
      return;
    }
  g_strfreev (type);

  data = ohfa (&len);
  for (i = 0; type_code[i] != -1; i++)
    {
      switch (type_code[i])
        {
        case TYPE_CHAR:
          size_types += sizeof (char);
          break;
        case TYPE_INT:
          size_types += sizeof (int);
          break;
        case TYPE_DOUBLE:
          size_types += sizeof (double);
          break;
        case TYPE_FLOAT:
          size_types += sizeof (float);
          break;
        case TYPE_LONG:
          size_types += sizeof (long);
          break;
        case TYPE_UNSIGNED:
          size_types += sizeof (unsigned);
          break;
        case TYPE_SHORT:
          size_types += sizeof (short);
          break;
        case TYPE_SIGNED:
          size_types += sizeof (signed);
          break;
        }
    }
  line_type = i;

  if (len % size_types || !len)
    {
      jeex_widget_show (show_file_struct.error_label);
      free (data);
      free (type_code);

      return;
    }
  types = g_strdup_printf (_("Structure 1/%d"), len / size_types);
  gtk_label_set_text (GTK_LABEL (show_file_struct.label), types);
  g_free (types);
  jeex_widget_show (show_file_struct.label);
  jeex_widget_show (show_file_struct.b_prev);
  gtk_widget_set_sensitive (show_file_struct.b_prev, FALSE);
  jeex_widget_show (show_file_struct.b_next);

  if (file_struct)
    {
      for (i = 0; i < alloc_file_struct; i++)
        g_string_free (file_struct[i], TRUE);
      g_free (file_struct);
      file_struct = NULL;
    }

  alloc_file_struct = len / size_types;
  file_struct = (GString **) g_malloc (alloc_file_struct * sizeof (GString *));
  make_file_struct (&file_struct, data, len, type_code);
  free (data);
  free (type_code);

  jeex_widget_show (show_file_struct.scrolled_window);
  gtk_text_buffer_set_text (show_file_struct.buffer, file_struct[current_struct]->str,
                            file_struct[current_struct]->len);
  highlight_syntax ();

  gtk_widget_set_sensitive (show_file_struct.b_next, (gboolean) (total_struct - 1));
  jeex_widget_hide (show_file_struct.error_label);
}

static void
make_file_struct (GString *** file_struct, char *data, int len, JeexTypeCode * type_code)
{
  int j, b = 0, fd[2], i, cnt = 0;
  char c;
  double d;
  float f;
  long l;
  unsigned u;
  signed s;
  short h;

  pid_t pid;

  pipe (fd);
  if ((pid = fork ()) < 0)
    {
      g_error ("Cannot make a child process.");
      return;
    }

  if (!pid)
    {
      close (fd[0]);
      write (fd[1], data, len);
      exit (0);
    }
  else
    {
      close (fd[1]);
      srand (time (NULL));

      do
        {
          (*file_struct)[cnt] = g_string_new ("\n\n");
          g_string_printf ((*file_struct)[cnt], "struct _jeex%s {\n", rand_name ());

          for (j = 0; type_code[j] != -1; j++)
            {
              switch (type_code[j])
                {
                case TYPE_CHAR:
                  read (fd[0], &c, sizeof (char));
                  b += sizeof (char);

                  if (isprint (c))
                    g_string_append_printf ((*file_struct)[cnt], "       char %s = '%c';\n",
                                            rand_name (), c);
                  else
                    g_string_append_printf ((*file_struct)[cnt], "       char %s = '\\x%.2X';\n",
                                            rand_name (), c);

                  break;
                case TYPE_INT:
                  read (fd[0], &i, sizeof (int));
                  b += sizeof (int);

                  g_string_append_printf ((*file_struct)[cnt], "       int %s = %i;\n", rand_name (), i);

                  break;
                case TYPE_DOUBLE:
                  read (fd[0], &d, sizeof (double));
                  b += sizeof (double);

                  g_string_append_printf ((*file_struct)[cnt], "       double %s = %lf;\n", rand_name (),
                                          d);

                  break;
                case TYPE_FLOAT:
                  read (fd[0], &f, sizeof (float));
                  b += sizeof (float);

                  g_string_append_printf ((*file_struct)[cnt], "       float %s = %f;\n", rand_name (),
                                          f);

                  break;
                case TYPE_LONG:
                  read (fd[0], &l, sizeof (long));
                  b += sizeof (long);

                  g_string_append_printf ((*file_struct)[cnt], "       long %s = %ld;\n", rand_name (),
                                          l);

                  break;
                case TYPE_UNSIGNED:
                  read (fd[0], &u, sizeof (unsigned));
                  b += sizeof (unsigned);

                  g_string_append_printf ((*file_struct)[cnt], "       unsigned %s = %u;\n",
                                          rand_name (), u);

                  break;
                case TYPE_SHORT:
                  read (fd[0], &h, sizeof (short));
                  b += sizeof (short);

                  g_string_append_printf ((*file_struct)[cnt], "       short %s = %hd;\n", rand_name (),
                                          h);

                  break;
                case TYPE_SIGNED:
                  read (fd[0], &s, sizeof (signed));
                  b += sizeof (signed);

                  g_string_append_printf ((*file_struct)[cnt], "       signed %s = %d;\n", rand_name (),
                                          s);

                  break;
                }
            }

          g_string_append_printf ((*file_struct)[cnt], "};\n");
          cnt++;
        }
      while (b < len);
    }
  total_struct = cnt;
}

void *
change_structure (GtkWidget * widget, void *data)
{
  gboolean type = (gboolean) GPOINTER_TO_INT (data);
  char *str = NULL;

  if (!type)
    {
      current_struct++;
      if ((current_struct + 1) == total_struct)
        {
          gtk_widget_set_sensitive (show_file_struct.b_next, FALSE);
        }

      if (total_struct > 1)
        {
          gtk_widget_set_sensitive (show_file_struct.b_prev, TRUE);
        }
    }
  else
    {
      current_struct--;
      if (!current_struct)
        {
          gtk_widget_set_sensitive (show_file_struct.b_prev, FALSE);
        }

      if ((current_struct + 1) < total_struct && total_struct > 1)
        {
          gtk_widget_set_sensitive (show_file_struct.b_next, TRUE);
        }
    }

  gtk_text_buffer_set_text (show_file_struct.buffer, file_struct[current_struct]->str,
                            file_struct[current_struct]->len);

  str = g_strdup_printf (_("Structure %d/%d"), current_struct + 1, total_struct);
  gtk_label_set_text (GTK_LABEL (show_file_struct.label), str);
  highlight_syntax ();
  g_free (str);

  return NULL;
}

static void
highlight_syntax (void)
{
  GtkTextIter start, end, m;
  GtkTextBuffer *buffer = show_file_struct.buffer;
  char *str;
  int i, c, k = 20;
  gboolean chk;

  gtk_text_buffer_get_start_iter (buffer, &start);
  gtk_text_buffer_get_iter_at_line_offset (buffer, &end, 0, 6);
  gtk_text_buffer_apply_tag (buffer, show_file_struct.syntax_highlight[0], &start, &end);

  gtk_text_buffer_get_end_iter (buffer, &m);
  str = gtk_text_buffer_get_text (buffer, &start, &m, FALSE);

  for (i = 0; i < line_type; i++)
    {
      gtk_text_buffer_get_iter_at_line_offset (buffer, &start, i + 1, 7);
      for (k += 7, c = 0; str[k] != 0x20; c++, k++);
      gtk_text_buffer_get_iter_at_line_offset (buffer, &end, i + 1, 7 + c);
      gtk_text_buffer_apply_tag (buffer, show_file_struct.syntax_highlight[1], &start, &end);
      c += 0x10;
      chk = (gboolean) str[k + 9] == '\'';
      gtk_text_buffer_get_iter_at_line_offset (buffer, &start, i + 1, c);
      for (k += 9; str[k] != ';'; k++, c++);
      gtk_text_buffer_get_iter_at_line_offset (buffer, &end, i + 1, c);
      if (chk)
        gtk_text_buffer_apply_tag (buffer, show_file_struct.syntax_highlight[2], &start, &end);
      else
        gtk_text_buffer_apply_tag (buffer, show_file_struct.syntax_highlight[3], &start, &end);
      while (str[k++] != '\n');
    }
  g_free (str);
}
