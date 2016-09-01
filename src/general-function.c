/*
 * "general-function.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#define _GNU_SOURCE
#include <ctype.h>
#include <fcntl.h>
#include <gdk/gdkkeysyms.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <glib/gi18n.h>
#include <strings.h>
#include <math.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libintl.h>
#include <locale.h>
#include "header.h"

#define jeex_get_text(start, end) \
        (char *) gtk_text_buffer_get_text (_ncurrent(notebook->buffer), \
                                            start, end, FALSE);
#define jeex_statusbar_set(statusbar, message) \
        if ( GTK_IS_STATUSBAR(statusbar) ) { \
             gtk_statusbar_pop (GTK_STATUSBAR(statusbar), 1); \
             gtk_statusbar_push (GTK_STATUSBAR(statusbar), 1, message); \
        }

extern JeexPreferences *preferences;
extern JeexMenu *jeex_menu;
extern JeexFileInfo *file[64];

/* Function to dis\able more widget */
void *
jeex_widget_set_sensitive (gboolean status, ...)
{
  register GtkWidget *widget;
  va_list arg;

  va_start (arg, status);

  while ((widget = va_arg (arg, GtkWidget *)))
    {
      gtk_widget_set_sensitive (widget, status);
    }

  va_end (arg);

  return NULL;
}

/* Function to disable widgets usable only on the original file */
void *
disable_file_widget (GtkWidget * widget, void *data)
{
  char *name, *tmp;
  register int i;
  extern JeexPreferences *preferences;

  /* Disable widgets */
  jeex_widget_set_sensitive (FALSE, jeex_menu->tools_menu.split,
                             jeex_menu->file_menu.remove, jeex_menu->edit_menu.update, NULL);

  /* Checks that the variable _ncurrent(file)->name is set
   * usually this is always true condition.
   */
  if (_ncurrent (file)->name && !(_ncurrent (file)->new))
    {
      /* Gets file name */
      name = g_path_get_basename (_ncurrent (file)->name);

      if ((i = strlen (name)) > preferences->max_tabchars)
        {
          tmp = g_strndup (name, preferences->max_tabchars);
          g_free (name);
          if (_ncurrent (file)->modified)
            name = g_strdup_printf ("*%s...", tmp);
          else
            name = g_strdup_printf ("%s...", tmp);
          g_free (tmp);
        }
      else if (_ncurrent (file)->modified)
        {
          tmp = g_strdup (name);
          g_free (name);
          name = g_strdup_printf ("*%s", tmp);
          g_free (tmp);
        }

      /* Sets new tab title */
      gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook->notebook),
                                  _ncurrent (notebook->table), gtk_label_new (name));
      g_free (name);
    }

  return NULL;
}

/* Function to able widgets usable only on the original file */
void *
able_file_widget (GtkWidget * widget, void *data)
{
  char *name, *tmp;
  register int i;
  extern JeexPreferences *preferences;

  /* Able widgets */
  jeex_widget_set_sensitive (TRUE, jeex_menu->tools_menu.split,
                             jeex_menu->file_menu.remove, jeex_menu->edit_menu.update, NULL);

  /* Checks that the variable _ncurrent(file)->name is set
   * usually this is always true condition.
   */
  if (_ncurrent (file)->name && !(_ncurrent (file)->new))
    {
      name = g_path_get_basename (_ncurrent (file)->name);

      if ((i = strlen (name)) > preferences->max_tabchars)
        {
          tmp = g_strndup (name, preferences->max_tabchars);
          g_free (name);
          if (_ncurrent (file)->modified)
            name = g_strdup_printf ("*%s...", tmp);
          else
            name = g_strdup_printf ("%s...", tmp);
          g_free (tmp);
        }
      else if (_ncurrent (file)->modified)
        {
          tmp = g_strdup (name);
          g_free (name);
          name = g_strdup_printf ("*%s", tmp);
          g_free (tmp);
        }

      /* Sets new tab title */
      gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook->notebook),
                                  _ncurrent (notebook->table), gtk_label_new (name));
      g_free (name);
    }

  return NULL;
}

/* Gets file path and name, returns a array with
 * in first element the name, and last element the
 * path.
 */
char **
inap (char *str)
{
  char **info = (char **) g_malloc (2 * sizeof (char *));
  char *tmp, *home;

  info[0] = g_path_get_basename (str);
  info[1] = g_path_get_dirname (str);

  tmp = getenv ("HOME");
  home = g_strdup (tmp ? tmp : "");

  if (fthx ((unsigned char *) info[1], home) && strcmp (info[1], home))
    {
      tmp = g_strdup_printf ("~%s", info[1] + strlen (home));
      g_free (info[1]);
      info[1] = g_strdup (tmp);
      g_free (tmp);
    }

  g_free (home);

  return info;
}

static char *
jeex_get_current_byte_iter (GtkTextIter * start, GtkTextIter * end)
{
  char *str;

  end = gtk_text_iter_copy (start);

  if (!gtk_text_iter_is_end (start))
    {
      gtk_text_iter_forward_chars (end, 2);

      str = jeex_get_text (start, end);

      if (str[0] == 0x20 && str[1] == 0x20)
        {
          gtk_text_iter_backward_chars (end, 2);
          gtk_text_iter_backward_chars (start, 2);
        }

      while (str[0] == 0x20 || str[0] == 0x0A)
        {
          if (gtk_text_iter_is_end (end))
            {
              break;
            }
          gtk_text_iter_forward_char (start);
          gtk_text_iter_forward_char (end);
          g_free (str);
          str = jeex_get_text (start, end);
        }

      while (str[1] == 0x20 || str[1] == 0x0A)
        {
          if (gtk_text_iter_is_start (start))
            {
              break;
            }
          gtk_text_iter_backward_char (start);
          gtk_text_iter_backward_char (end);
          g_free (str);
          str = jeex_get_text (start, end);
        }

      if (*str != 0x20 && *str != 0x0A)
        {
          return str;
        }
    }

  gtk_text_iter_backward_chars (start, 2);
  str = jeex_get_text (start, end);
  while (*str == 0x20 || *str == 0x0A)
    {
      if (gtk_text_iter_is_start (start))
        {
          break;
        }
      gtk_text_iter_backward_char (start);
      gtk_text_iter_backward_char (end);
      g_free (str);
      str = jeex_get_text (start, end);
    }

  return str;
}

static void
jeex_setting_field (unsigned int xb)
{
  char *t;

  if (GTK_IS_ENTRY (byte_info.e_gint8))
    {
      t = g_strdup_printf ("%d", (signed char) xb);
      gtk_entry_set_text (GTK_ENTRY (byte_info.e_gint8), t);
      g_free (t);
    }

  if (GTK_IS_ENTRY (byte_info.e_guint8))
    {
      t = g_strdup_printf ("%d", (unsigned char) xb);
      gtk_entry_set_text (GTK_ENTRY (byte_info.e_guint8), t);
      g_free (t);
    }

  if (GTK_IS_ENTRY (byte_info.e_ascii))
    {
      switch (xb)
        {
        case ' ':
          t = g_strdup (_("Space"));
          break;
        case '\n':
          t = g_strdup ("\\n");
          break;
        case '\t':
          t = g_strdup ("\\t");
          break;
        case '\v':
          t = g_strdup ("\\v");
          break;
        case '\r':
          t = g_strdup ("\\r");
          break;
        case '\a':
          t = g_strdup ("\\a");
          break;
        default:
          if (!g_ascii_isgraph (xb))
            {
              t = g_strdup (_("Unprintable"));
            }
          else
            {
              t = g_strdup_printf ("%c", (char) xb);
              break;
            }
          break;
        }
      gtk_entry_set_text (GTK_ENTRY (byte_info.e_ascii), t);
      g_free (t);
    }

  if (GTK_IS_ENTRY (byte_info.e_binary))
    {
      t = int_to_binary (xb);
      gtk_entry_set_text (GTK_ENTRY (byte_info.e_binary), t);
      free (t);
    }

  if (GTK_IS_ENTRY (byte_info.e_hex))
    {
      if (preferences->zero_prefix)
        {
          t = g_strdup_printf ("0x%2.2X", xb);
        }
      else
        {
          t = g_strdup_printf ("%2.2X", xb);
        }

      gtk_entry_set_text (GTK_ENTRY (byte_info.e_hex), t);
      g_free (t);
    }

  if (GTK_IS_ENTRY (byte_info.e_octal))
    {
      t = g_strdup_printf ("%o", xb);
      gtk_entry_set_text (GTK_ENTRY (byte_info.e_octal), t);
      g_free (t);
    }
}

static void
jeex_check_hash (void)
{
  static struct stat info;

  if (!_ncurrent (file)->new)
    {
      if (g_file_test (_ncurrent (file)->name, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR))
        {
          stat (_ncurrent (file)->name, &info);
          if (info.st_mtime > _ncurrent (file)->mtime)
            {
              _ncurrent (file)->mtime = info.st_mtime;
              jeex_widget_show (_ncurrent (notebook->update));
            }
        }
    }
}

static char *
jeex_make_statusbar_message (GtkTextMark * mark)
{
  GtkTextIter iter;
  int len;
  unsigned char *tmp;
  char *str, *r;

  gtk_text_buffer_get_iter_at_mark (_ncurrent (notebook->buffer), &iter, mark);

  if (_ncurrent (file)->new)
    {
      tmp = (unsigned char *) ohfa (&len);
      free (tmp);

      str = g_format_size_for_display ((goffset) len);
      str = g_utf8_normalize (str, -1, G_NORMALIZE_DEFAULT);
      r =
        g_strdup_printf (_
                         ("Offset: %8.8X - Column: %d - Size: %s ( %d byte )"),
                         gtk_text_iter_get_line (&iter) * 0x10,
                         gtk_text_iter_get_line_offset (&iter) + 1, str, len);
      g_free (str);
      return r;
    }

  r =
    g_strdup_printf (_("Offset: %8.8X - Column: %d"),
                     gtk_text_iter_get_line (&iter) * 0x10,
                     gtk_text_iter_get_line_offset (&iter) + 1);

  return r;
}

void *
update_status_bar (GtkWidget * widget, void *data)
{
  GtkTextMark *mark;
  GtkTextIter start, end;
  char *cb, *str;
  unsigned int xb;

  notebook->current = (notebook->current < 0) ? 0 : notebook->current;

  return_val_if_fail (GTK_IS_TEXT_BUFFER (_ncurrent (notebook->buffer)), NULL);

  if (!_ncurrent (file)->name && !_ncurrent (file)->new)
    {
      jeex_widget_hide (table_info);
      str = g_strdup (_("No File Open"));
    }
  else
    {
      jeex_widget_show (table_info);
      mark = gtk_text_buffer_get_insert (_ncurrent (notebook->buffer));

      gtk_text_buffer_get_iter_at_mark (_ncurrent (notebook->buffer), &start, mark);
      gtk_text_buffer_get_iter_at_mark (_ncurrent (notebook->buffer), &end, mark);
      if (!gtk_text_iter_is_end (&end))
        gtk_text_iter_forward_char (&end);
      else if (gtk_text_iter_is_end (&end) && !gtk_text_iter_is_start (&start))
        gtk_text_iter_backward_char (&start);

      cb = jeex_get_text (&start, &end);
      if (!g_ascii_isxdigit (*cb) && *cb != 0x20 && *cb != 0x0A)
        {
          gtk_text_buffer_delete (_ncurrent (notebook->buffer), &start, &end);
        }
      g_free (cb);

      gtk_text_buffer_get_iter_at_mark (_ncurrent (notebook->buffer), &start, mark);
      gtk_text_buffer_get_iter_at_mark (_ncurrent (notebook->buffer), &end, mark);
      if (!gtk_text_iter_is_end (&end) && !gtk_text_iter_is_start (&start))
        gtk_text_iter_backward_char (&start);
      cb = jeex_get_text (&start, &end);
      if (!g_ascii_isxdigit (*cb) && *cb != 0x20 && *cb != 0x0A)
        {
          gtk_text_buffer_delete (_ncurrent (notebook->buffer), &start, &end);
        }
      g_free (cb);
      cb = jeex_get_current_byte_iter (&start, &end);
      xb = (unsigned int) strtol (cb, NULL, 16);
      g_free (cb);

      jeex_setting_field (xb);
      jeex_check_hash ();
      str = jeex_make_statusbar_message (mark);
    }

  jeex_statusbar_set (bar, str);
  g_free (str);

  return NULL;
}

/* Checks that string passed as parameter isn't empty. */
gboolean
strisnull (void *vstr)
{
  int len = strlen ((char *) vstr);

  while (len--)
    {
      switch (((char *) vstr)[len])
        {
        case '\0':
        case ' ':
        case '\t':
        case '\v':
        case '\a':
        case '\b':
        case '\n':
        case '\r':
          break;
        default:
          return FALSE;
          break;
        }
    }
  return TRUE;
}

/* Function to error manage */
void *
_error (char *message)
{
  GtkWidget *dialog;
  extern GtkWidget *jeex_main_window;

  dialog =
    gtk_message_dialog_new_with_markup (GTK_WINDOW (jeex_main_window), 0,
                                        GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, NULL);
  gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG (dialog), message);
  gtk_window_set_title (GTK_WINDOW (dialog), _("Error"));
  gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
  gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (jeex_main_window));
  gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);

  return NULL;
}

/* Function to info message */
gboolean
_info (char *message, gboolean choose)
{
  GtkWidget *dialog;
  int result;
  extern GtkWidget *jeex_main_window;

  if (choose)
    {
      dialog =
        gtk_message_dialog_new_with_markup (GTK_WINDOW (jeex_main_window), 0,
                                            GTK_MESSAGE_INFO, GTK_BUTTONS_OK_CANCEL, NULL);
      gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG (dialog), message);
    }
  else
    {
      dialog =
        gtk_message_dialog_new_with_markup (GTK_WINDOW (jeex_main_window), 0,
                                            GTK_MESSAGE_INFO, GTK_BUTTONS_OK, NULL);
      gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG (dialog), message);
    }

  gtk_window_set_title (GTK_WINDOW (dialog), "Info");
  gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
  gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (jeex_main_window));
  gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER_ON_PARENT);
  result = gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);

  if (result == GTK_RESPONSE_OK)
    {
      return TRUE;
    }
  else
    {
      return FALSE;
    }
}

/* Function for the quit request */
void *
secure_quit (GtkWidget * widget, void *data)
{
  GtkWidget *finestra, *testo, *tabella, *button_ok, *button_no;
  extern GtkWidget *jeex_main_window;

  /* Window */
  finestra = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_transient_for (GTK_WINDOW (finestra), GTK_WINDOW (jeex_main_window));
  gtk_window_set_position (GTK_WINDOW (finestra), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_title (GTK_WINDOW (finestra), _("Quit"));
  gtk_window_set_resizable (GTK_WINDOW (finestra), FALSE);

  /* Table */
  tabella = gtk_table_new (2, 3, TRUE);
  gtk_container_add (GTK_CONTAINER (finestra), tabella);
  gtk_widget_show (tabella);

  /* Text */
  testo = gtk_label_new (_("\n Are you sure to quit Jeex? \n" " ( Unsaved changes will be lost ) \n"));
  gtk_table_attach (GTK_TABLE (tabella), testo, 0, 3, 0, 1, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
  gtk_widget_show (testo);

  /* OK Button */
  button_ok = gtk_button_new_from_stock (GTK_STOCK_OK);
  gtk_table_attach (GTK_TABLE (tabella), button_ok, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 10, 10);
  gtk_widget_show (button_ok);

  /* CANCEL Button */
  button_no = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
  gtk_table_attach (GTK_TABLE (tabella), button_no, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 10, 10);
  GTK_WIDGET_SET_FLAGS (button_no, GTK_CAN_DEFAULT);
  gtk_widget_grab_default (button_no);
  gtk_widget_show (button_no);

  gtk_widget_show (finestra);

  /* Signals */
  g_signal_connect (G_OBJECT (button_no), "clicked", G_CALLBACK (destroy_this), (void *) finestra);
  g_signal_connect (G_OBJECT (button_ok), "clicked", G_CALLBACK (gtk_main_quit), NULL);

  return NULL;
}

/* Function to show or hide main window */
void *
hide_or_view_window (GtkWidget * w, void *d)
{
  extern int check_p;
  extern GtkWidget *jeex_main_window;

  if (!GTK_WIDGET_VISIBLE (jeex_main_window))
    {
      logging_action (_("Show jeex window."));
      gtk_widget_show_all (jeex_main_window);
      jeex_widget_hide (textview->insert->table);
      jeex_widget_hide (_ncurrent (notebook->update));

      if (!check_p)
        {
          jeex_widget_hide (table_info);
        }
    }
  else
    {
      logging_action (_("Hide jeex window."));
      gtk_widget_hide_all (jeex_main_window);
    }

  return NULL;
}

/* Function to convert in binary */
char *
int_to_binary (int x)
{
  char *local_str, *tmp_str;
  register int y = x, mem = 2;

  local_str = (char *) g_malloc (mem);

  while (y)
    {
      if (!(y % 2))
        {
          local_str[mem - 2] = '0';
        }
      else
        {
          local_str[mem - 2] = '1';
        }

      local_str = (char *) realloc (local_str, mem++);

      if (y == 1)
        {
          break;
        }
      else
        {
          y /= 2;
        }
    }

  local_str[mem - 2] = 0x00;

  mem = strlen (local_str);

  tmp_str = g_strdup (local_str);

  for (y = 0; y < mem; ++y)
    {
      local_str[7 - y] = tmp_str[y];
    }

  if (mem < 8)
    {
      for (y = 0; y < (8 - mem); ++y)
        {
          local_str[y] = '0';
        }
    }

  local_str[8] = 0x00;

  g_free (tmp_str);

  return local_str;
}

/* Search the second string in the first */
int
strpos (char *str, char *substr)
{
  int len = strlen (str), i, pos = 0, c, lensub = strlen (substr);

  for (i = 0; i < len; ++i)
    {
      if (str[i] == *substr)
        {
          pos = i + 1;
          for (c = 0; c < lensub; ++c)
            {
              if (str[i + c] != substr[c])
                {
                  pos = 0;
                  break;
                }
            }
          if (pos)
            {
              break;
            }
        }
    }
  return pos - 1;
}

/* Replace a string */
char *
strreplace (char *string, char *search_string, char *replace_string)
{
  GRegex *regex;
  char *str;

  regex = g_regex_new (search_string, 0, 0, NULL);
  str = g_regex_replace (regex, string, -1, 0, replace_string, 0, NULL);
  g_regex_unref (regex);

  return str;
}

/* It solves the .. */
static char *
resolve_dp (char *s)
{
  char *str;
  int i, check = 0;
  struct
  {
    int start;
    int end;
  } p;

  p.end = strpos (s, "/../") + 3;
  for (i = p.end; i > -1; i--)
    {
      if (s[i] == '/')
        {
          ++check;
        }
      if (check == 3)
        {
          break;
        }
    }
  if (check != 3)
    {
      ++i;
    }
  p.start = i;
  str = (char *) g_try_malloc ((p.end - p.start) + 2);
  for (i = p.start; i <= p.end; ++i)
    {
      str[i - p.start] = s[i];
    }
  str = strreplace (s, str, "/");

  return str;
}

/* Function to obtain the absolute path of the file */
char *
abspath (char *str)
{
  char *ap;

  /* Gets path to use */
  if (*str != '/')
    {
      ap = g_strdup_printf ("%s/%s", g_get_current_dir (), str);
    }
  else
    {
      ap = g_strdup (str);
    }

  /* Generating current path released only .. */
  while (strstr (ap, "/./"))
    {
      ap = strreplace (ap, "/./", "/");
    }

  /* Solves the  .. */
  while (strpos (ap, "/../") != -1)
    {
      ap = resolve_dp (ap);
    }

  return ap;
}

/* Destroy widget passed as parameter */
void *
destroy_this (GtkWidget * w, GtkWidget * widget)
{
  if (GTK_IS_WIDGET (widget))
    {
      gtk_widget_destroy (widget);
    }

  return NULL;
}

unsigned int
get_ram_size_from_proc (unsigned int *value)
{
  char buff[64] = { 0 }, *s_val = NULL;
  int fd, begin = 9, end = 0;
  unsigned int val = 0;

  if ((fd = open ("/proc/meminfo", O_RDONLY)) == -1)
    {
      if (value)
        *value = val;
      return val;
    }

  if (read (fd, buff, 64) < 32)
    {
      if (value)
        *value = val;
      return val;
    }

  close (fd);

  while (!isdigit (buff[begin++]));
  for (begin--; isdigit (buff[begin + end]); end++);
  s_val = strndup (buff + begin, end);
  val = (unsigned int) atoi (s_val) * 1024;
  free (s_val);

  if (value)
    *value = val;

  return val;
}

/* Solves the escape of the various special characters
 * like \n, \r, \t, ecc..
 */
static char *
resolve_escape (char *str)
{
  int i, x, mem = 0;
  char *s = (char *) malloc (++mem);

  for (i = strlen (str), x = 0; x < i; ++x)
    {
      if (str[x] == '\\')
        {
          s[mem - 1] = '\\';
          switch (str[x + 1])
            {
            case '\\':
              break;

            case 'n':
              s[mem - 1] = '\n';
              break;
            case 'r':
              s[mem - 1] = '\r';
              break;
            case 't':
              s[mem - 1] = '\t';
              break;
            case 'v':
              s[mem - 1] = '\v';
              break;
            case 'a':
              s[mem - 1] = '\a';
              break;
            case 'b':
              s[mem - 1] = '\b';
              break;
            case '\'':
              s[mem - 1] = '\'';
              break;
            case '"':
              s[mem - 1] = '\"';
              break;
            case '0':
              s[mem - 1] = '\0';
              break;
            default:
              s = (char *) realloc (s, ++mem);
              s[mem - 1] = str[x + 1];
              break;
            }
          x++;
        }
      else
        s[mem - 1] = str[x];

      s = (char *) realloc (s, ++mem);
    }
  s[mem - 1] = 0x00;

  return s;
}

/* Gets hexadecimal value from insert format */
char *
obtain_value (int type, char *str, gboolean * check)
{
  char *text = NULL, *tmp, *ftmp;
  int x, m = 1, c = 0, p;
  *check = FALSE;

  g_return_val_if_fail (str, NULL);

  switch (type)
    {
      /* Conversion decimal value in hexadecimal */
    case DEC:
      x = atoi (str);
      if (x > 255 || x < -255)
        {
          _error (_
                  ("<big><b>Invalid value</b></big>\n"
                   "\nValue must be between 255 and -255.\n"
                   "It is the maximum value supported by a byte."));
        }
      else
        {
          tmp = g_strdup_printf ("%2.2X", x);
          text = g_strdup_printf (" %s ", (strlen (tmp) > 2) ? tmp + (strlen (tmp) - 2) : tmp);
          g_free (tmp);
        }
      break;
      /* Conversion hexadecimal value in 1 byte block */
    case HEX:
      text = (char *) g_try_malloc (m);
      for (p = 0, x = strlen (str); p < x; p++)
        {
          if (str[p] == 0x20)
            {
              continue;
            }

          if (!g_ascii_isxdigit (str[p]))
            {
              _error (_("<big><b>The entered value entered is not valid.</b></big>"));
              if (text)
                {
                  g_free (text);
                  text = NULL;
                }
              break;
            }
          if (c < 2)
            {
              if (str[p] >= 'A' && str[p] <= 'F')
                {
                  text[m - 1] = str[p];
                }
              else
                {
                  text[m - 1] = toupper (str[p]);
                }

              text = (char *) g_realloc (text, ++m);
              c++;
            }
          else
            {
              c = 0;
              text[m - 1] = ' ';
              text = (char *) g_realloc (text, ++m);
              --p;
            }
        }
      if (text)
        text[m - 1] = 0x00;
      break;
      /* Conversion from octal to hexadecimal */
    case OCT:
      c = 0;
      for (x = 0, p = strlen (str); x < p; ++x)
        {
          if (str[x] == 0x20)
            {
              continue;
            }

          if (str[x] < 0x30 || str[x] > 0x37)
            {
              _error (_("<big><b>Invalid Value</b></big>\n\n"
                        "The entered value entered includes values less\n"
                        "than 0 or greater than 7, not admitted\n" "in octal numbers."));
              x = -1;
              break;
            }

          c += (str[x] - 0x30) * pow (8, (p - 1) - x);
        }
      if ((c > 255 || c < -255) && x != -1)
        {
          _error (_
                  ("<big><b>Invalid Value</b></big>\n"
                   "\nValue must be between 255 and -255.\n"
                   "It is the maximum value supported by a byte."));
        }
      else if (x != -1)
        {
          text = g_strdup_printf (" %2.2X ", c);
        }
      break;
      /* Conversion from binary to hexadecimal */
    case BIN:
      c = 0;
      for (x = 0, p = strlen (str); x < p; ++x)
        {
          if (str[x] == 0x20)
            {
              continue;
            }

          if (str[x] != 0x31 && str[x] != 0x30)
            {
              _error (_("<big><b>Invalid Value</b></big>\n"
                        "\nValue entered includes characters other\n"
                        "than 0 and 1, not admitted in binary numbers.\n"));
              x = -1;
              break;
            }
          c += (str[x] - 0x30) * pow (2, (p - 1) - x);
        }
      if ((c > 255 || c < -255) && x != -1)
        {
          _error (_
                  ("<big><b>Invalid Value</b></big>\n"
                   "\nValue must be between 255 and -255.\n"
                   "It is the maximum value supported by a byte."));
        }
      else if (x != -1)
        {
          text = g_strdup_printf (" %2.2X ", c);
        }
      break;
      /* Conversion from ascii to hexadecimal */
    case ASCII:
      tmp = g_utf8_normalize (str, -1, G_NORMALIZE_DEFAULT);
      tmp = resolve_escape (tmp);

      text = (char *) g_try_malloc (1 + ((p = strlen (tmp)) * 3));
      *check = TRUE;
      strcpy (text, "");

      for (x = 0; x < p; ++x)
        {
          ftmp = g_strdup_printf ("%-2.2X ", tmp[x]);
          if (strlen (ftmp) > 2)
            {
              strcat (text, ftmp + (strlen (ftmp) - 3));
            }
          else
            {
              strcat (text, ftmp);
            }
          g_free (ftmp);
        }
      free (tmp);

      break;
    default:
      _error (_("<big><b>Unknown Operation</b></big>\n" "\nReport this bug to: hds619@gmail.com"));
      break;
    }

  return text;
}

void
callback_widget_hide (GtkWidget * object, void *data)
{
  jeex_widget_hide (((GtkWidget *) data));
}
