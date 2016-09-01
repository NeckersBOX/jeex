/*
 * "file.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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
#include <errno.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <libintl.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <magic.h>
#include <time.h>
#include <unistd.h>
#include "header.h"

#define chk_free(var) \
        if ( var ) \
             g_free (var);

#define remove_current_page \
        gtk_notebook_remove_page (GTK_NOTEBOOK (notebook->notebook), notebook->current); \
                                                                                         \
        textview->insert = _ncurrent (notebook->insert);                                 \
        textview->insert->table = _ncurrent (notebook->insert)->table;                   \
        textview->regex = _ncurrent (notebook->regex);                                   \
        textview->regex->table = _ncurrent (notebook->regex)->table;                     \
        textview->buffer = _ncurrent (notebook->buffer);                                 \
        textview->textview = _ncurrent (notebook->textview);

typedef enum
{
  GENERIC_FORMAT = 0, JASCII, JOCT, JHEX, JDEC, JBIN
} JFormat;

gboolean check_p = FALSE;
JeexFileInfo *file[64];
JeexFileRecent *recent_file;

static JFormat save_format = GENERIC_FORMAT;

extern JeexPreferences *preferences;
extern JeexMenu *jeex_menu;

/* Function to open the file passed as parameter */
static void *
open_this_file (GtkWidget * widget, char *name)
{
  int i, tot;
  char *t;

  if (check_p)
    {
      /* Checking if file was opened */
      tot = gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook->notebook));

      for (i = 0; i < tot; ++i)
        {
          if (!(file[i]->name))
            {
              continue;
            }

          if (!strcmp (file[i]->name, name))
            {
              /* If was opened in previous time, position current tab
               * on tab where it's and closes dialog.
               */
              gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook->notebook), i);

              return NULL;
            }
        }

      t = g_path_get_basename (name);
      make_new_notebook_page (t);
      g_free (t);
    }
  else
    {
      check_p = TRUE;
    }

  _ncurrent (file)->name = (char *) name;
  file_open (NULL);

  gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook->notebook), notebook->current);

  return NULL;
}

/* Function to manage recente file */
static void *
add_recent_file (char *name, char *path)
{
  char *filename;
  GtkWidget *elem;
  int i, c;
  extern JeexToolbar *jeex_toolbar;

  /* Check if is first recent file that users entering */
  if (!recent_file->check)
    {
      elem = gtk_separator_menu_item_new ();
      gtk_menu_shell_insert (GTK_MENU_SHELL (jeex_menu->file_menu.file), elem, 4);
      gtk_widget_show (elem);
      recent_file->check = TRUE;

      jeex_toolbar->menu.menu = gtk_menu_new ();
      gtk_menu_tool_button_set_menu (GTK_MENU_TOOL_BUTTON
                                     (jeex_toolbar->open_file), jeex_toolbar->menu.menu);
    }


  filename =
    g_strdup_printf ("%d. %s/%s",
                     (recent_file->count_check) ? preferences->max_recentfile :
                     ++(recent_file->count), path, name);

  c = recent_file->count_check ? preferences->max_recentfile : (recent_file->count - 1);

  /* Checking if file wasn't opened */
  for (i = 0; i < c; ++i)
    {
      if (!strcasecmp (filename + 3, recent_file->name[i] + 3))
        {
          --(recent_file->count);

          /* Reorder list */
          for (++i; i < c; ++i)
            {
              *(recent_file->name[i]) = (*(recent_file->name[i])) - 1;
              recent_file->name[i - 1] = recent_file->name[i];
            }

          recent_file->name[c - 1] = g_strdup (filename);
          *(recent_file->name[c - 1]) = c + 0x30;

          /* Destroying old elements */
          for (i = 0; i < c; ++i)
            {
              gtk_widget_destroy (recent_file->elem[0][i]);
              gtk_widget_destroy (recent_file->elem[1][i]);
            }

          /* Inserting new elements */
          for (i = 0; i < c; ++i)
            {
              /* Inserting in the list */
              recent_file->elem[0][i] = gtk_menu_item_new_with_label (recent_file->name[i]);
              gtk_menu_shell_insert (GTK_MENU_SHELL
                                     (jeex_menu->file_menu.file), recent_file->elem[0][i], i + 5);
              gtk_widget_show (recent_file->elem[0][i]);

              recent_file->elem[1][i] = gtk_menu_item_new_with_label (recent_file->name[i]);
              gtk_menu_shell_insert (GTK_MENU_SHELL (jeex_toolbar->menu.menu),
                                     recent_file->elem[1][i], i + 2);
              gtk_widget_show (recent_file->elem[1][i]);

              /* Connecting Signal */
              if (recent_file->name[i][3] == '~')
                filename = g_strdup_printf ("%s/%s", g_get_home_dir (), recent_file->name[i] + 5);

              g_signal_connect (G_OBJECT (recent_file->elem[0][i]),
                                "activate", G_CALLBACK (open_this_file), (void *) filename);
              g_signal_connect (G_OBJECT (recent_file->elem[1][i]),
                                "activate", G_CALLBACK (open_this_file), (void *) filename);
            }
          return NULL;
        }
    }

  if (recent_file->count < (preferences->max_recentfile + 1) && !recent_file->count_check)
    {
      /* Inserting name */
      recent_file->elem[0][recent_file->count - 1] = gtk_menu_item_new_with_label (filename);
      gtk_menu_shell_insert (GTK_MENU_SHELL (jeex_menu->file_menu.file),
                             recent_file->elem[0][recent_file->count - 1], recent_file->count + 4);
      gtk_widget_show (recent_file->elem[0][recent_file->count - 1]);
      g_signal_connect (G_OBJECT
                        (recent_file->elem[0][recent_file->count - 1]),
                        "activate", G_CALLBACK (open_this_file), (void *) _ncurrent (file)->name);

      recent_file->elem[1][recent_file->count - 1] = gtk_menu_item_new_with_label (filename);
      gtk_menu_shell_insert (GTK_MENU_SHELL (jeex_toolbar->menu.menu),
                             recent_file->elem[1][recent_file->count - 1], recent_file->count + 2);
      gtk_widget_show (recent_file->elem[1][recent_file->count - 1]);
      g_signal_connect (G_OBJECT
                        (recent_file->elem[1][recent_file->count - 1]),
                        "activate", G_CALLBACK (open_this_file), (void *) _ncurrent (file)->name);

      recent_file->name[recent_file->count - 1] = g_strdup (filename);

      g_free (filename);

      if (recent_file->count == preferences->max_recentfile)
        {
          recent_file->count = 0;
          recent_file->count_check = TRUE;
        }
    }
  else
    {

      for (i = 1; i < c; ++i)
        {
          *(recent_file->name[i]) = (*(recent_file->name[i])) - 1;

          g_free (recent_file->name[i - 1]);
          recent_file->name[i - 1] = g_strdup (recent_file->name[i]);

          if (!(i - 1))
            {
              gtk_widget_destroy (*(recent_file->elem[0]));
              gtk_widget_destroy (*(recent_file->elem[1]));
            }
          gtk_widget_destroy (recent_file->elem[0][i]);
          gtk_widget_destroy (recent_file->elem[1][i]);
        }

      g_free (recent_file->name[i - 1]);
      recent_file->name[i - 1] = g_strdup (filename);


      for (i = 0; i < c; ++i)
        {
          /* Inserting on list */
          recent_file->elem[0][i] = gtk_menu_item_new_with_label (recent_file->name[i]);
          gtk_menu_shell_insert (GTK_MENU_SHELL (jeex_menu->file_menu.file),
                                 recent_file->elem[0][i], i + 5);
          gtk_widget_show (recent_file->elem[0][i]);

          recent_file->elem[1][i] = gtk_menu_item_new_with_label (recent_file->name[i]);
          gtk_menu_shell_insert (GTK_MENU_SHELL (jeex_toolbar->menu.menu), recent_file->elem[1][i],
                                 i + 2);
          gtk_widget_show (recent_file->elem[1][i]);

          /* Connecting Signal */
          if (recent_file->name[i][3] == '~')
            filename =
              g_strdup_printf ("%s/%s", g_get_home_dir (),
                               recent_file->name[i] + preferences->max_recentfile);

          g_signal_connect (G_OBJECT (recent_file->elem[0][i]), "activate",
                            G_CALLBACK (open_this_file), (void *) filename);

          g_signal_connect (G_OBJECT (recent_file->elem[1][i]), "activate",
                            G_CALLBACK (open_this_file), (void *) filename);
        }
    }

  return NULL;
}

/* Checks the error type receive in opening a file
 * and shows a dialog with problem description-
 */
void *
file_error_control (GFileError type, FileMode mode)
{
  GString *message;

  switch (mode)
    {
    case RB_MODE:
      message = g_string_new (_("<big><b>Error while opening the file</b></big>\n\n"));
      switch (type)
        {
        case G_FILE_ERROR_PERM:
        case G_FILE_ERROR_ACCES:
          g_string_append (message,
                           _("File cannot be read because you don't have\n" "sufficient rights."));
          break;
        case G_FILE_ERROR_NOENT:
        case G_FILE_ERROR_NXIO:
        case G_FILE_ERROR_NODEV:
          g_string_append (message, _("File cannot be open because it wasn't found."));
          break;
        case G_FILE_ERROR_NAMETOOLONG:
          g_string_append (message,
                           _("File cannot be read because the name is too\n"
                             "long. Usually max length is 255."));
          break;
        case G_FILE_ERROR_TXTBSY:
          g_string_append (message,
                           _("File cannot be open because is used by other\n"
                             "operations or processes."));
          break;
        case G_FILE_ERROR_LOOP:
          g_string_append (message,
                           _("The file you're trying to open is a symbolic\n"
                             "link to another symbolic link.\n"
                             "This then creates an endless cycle of\n"
                             "links and makes it impossible to determine\n"
                             "the final file to be opened."));
          break;
        case G_FILE_ERROR_NOMEM:
          g_string_free (message, TRUE);
          message =
            g_string_new (_("<b><big>Space in memory exhausted!</big></b>\n\n"
                            "Space in virtual memory is exhausted, this makes\n"
                            "it impossible to load the file into RAM."));
          break;
        case G_FILE_ERROR_IO:
          g_string_append (message,
                           _
                           ("File cannot be read because a I/O error has occurred.\n"
                            "Check that the device you are trying to read from\n" "isn't damaged."));
          break;
        case G_FILE_ERROR_FAILED:
        default:
          g_string_append (message, _("An unknown error has occurred while opening the file."));
          break;
        }
      break;
    case WB_MODE:
      message = g_string_new (_("<big><b>Error while saving the file</b></big>\n\n"));
      switch (type)
        {
        case G_FILE_ERROR_ACCES:
          g_string_append (message,
                           _("File cannot be saved because you don't have\n" "sufficient rights."));
          break;
        case G_FILE_ERROR_NAMETOOLONG:
          g_string_append (message,
                           _("File cannot be saved because the name is too\n"
                             "long. Usually max length is 255."));
          break;
        case G_FILE_ERROR_NOMEM:
          g_string_free (message, TRUE);
          message =
            g_string_new (_("<b><big>Space in memory exhausted!</big></b>\n\n"
                            "Space in virtual memory is exhausted, this makes\n"
                            "it impossible to load the file into RAM."));
          break;
        case G_FILE_ERROR_IO:
          g_string_append (message,
                           _
                           ("File cannot be saved because a I/O error has occurred.\n"
                            "Checks that the device you are trying to save from\n" "isn't damaged."));
          break;
        case G_FILE_ERROR_FAILED:
        default:
          g_string_append (message, _("An unknown error has occurred while saving the file."));
          break;
        }
      break;
    default:
      message =
        g_string_new (_("<big><b>Unknown Error</b></big>\n\n"
                        "It wasn't possible to identify the error\n"
                        "occurred while opening the file.\n\n"
                        "Report this bug to: <i>hds619@gmail.com</i>"));
      break;
    }

  _error (message->str);
  g_string_free (message, TRUE);

  return NULL;
}

/* Checks if beginning string passed as first parameter
 * is ugual at string passed as second parameter.
 */
gboolean
fthx (unsigned char *str, const char *type)
{
  int i, len = strlen (type ? type : "");

  for (i = 0; i < len; ++i)
    {
      if (str[i] != type[i])
        return FALSE;
    }

  return TRUE;
}

/* Gets info on the current file if called with FALSE ( must
 * specify the file data and length of data ), otherwhise
 * returns info as string, if called with TRUE.
 */
char *
info_type (gboolean mode, ...)
{
  va_list arg;
  unsigned char *data;
  int i, len;
  gboolean test = TRUE;
  struct stat info_p;
  struct tm *now;
  char *p, *prms = g_strdup ("---------"), *mime_type;
  magic_t magic_cookie = NULL;

  if (!mode)
    {
      va_start (arg, mode);
      data = va_arg (arg, unsigned char *);
      len = va_arg (arg, int);

      for (i = 0; i < len; ++i)
        {
          if (data[i] <= 0 || data[i] >= 255)
            {
              test = FALSE;
              break;
            }
        }

      magic_cookie = magic_open(MAGIC_MIME | MAGIC_RAW | MAGIC_ERROR);
      magic_load (magic_cookie, NULL);

      mime_type = (char *) magic_buffer (magic_cookie, data, len);
      if ( *mime_type )
           _ncurrent (file)->type = g_strdup (mime_type);
      else _ncurrent (file)->type = g_strdup ("text/plain");

      stat (_ncurrent (file)->name, &info_p);
      p = g_strdup_printf ("%o", info_p.st_mode);

      for (i = 3, len = 0; i < 6; ++i, len += 3)
        {
          switch (p[i])
            {
            case '1':
              prms[len + 2] = 'x';
              break;
            case '2':
              prms[len + 1] = 'w';
              break;
            case '3':
              prms[len + 1] = 'w';
              prms[len + 2] = 'x';
              break;
            case '4':
              prms[len] = 'r';
              break;
            case '5':
              prms[len] = 'r';
              prms[len + 2] = 'x';
              break;
            case '6':
              prms[len] = 'r';
              prms[len + 1] = 'w';
              break;
            case '7':
              prms[len] = 'r';
              prms[len + 1] = 'w';
              prms[len + 2] = 'x';
              break;
            }
        }

      g_free (p);
      _ncurrent (file)->permission = g_strdup (prms);
      g_free (prms);

      now = localtime (&(info_p.st_atime));
      strftime (_ncurrent (file)->last_access,
                sizeof (_ncurrent (file)->last_access), _("%a %m %b %Y %H:%M:%S"), now);

      now = localtime (&(info_p.st_mtime));
      strftime (_ncurrent (file)->last_modify,
                sizeof (_ncurrent (file)->last_access), _("%a %m %b %Y %H:%M:%S"), now);

      va_end (arg);

      return NULL;
    }
  else
    return
      g_strdup_printf (_(" <span font_weight=\"bold\">Mime Type:</span> %s \n"
                         " <span font_weight=\"bold\">Last access:</span> %s\n"
                         " <span font_weight=\"bold\">Last modify:</span> %s\n"
                         " <span font_weight=\"bold\">Permissions:</span>"
                         " <span style=\"italic\">%s</span>"),
                       _ncurrent (file)->type, _ncurrent (file)->last_access,
                       _ncurrent (file)->last_modify, _ncurrent (file)->permission);
}

/* Get info on the current file ( path, name, size )
 * and make the format string.
 */
char *
obtain_info (void)
{
  char **info, *str, *tmp, *t;
  int len;

  /* No opened file */
  if (!_ncurrent (file)->name || !g_file_test (_ncurrent (file)->name, G_FILE_TEST_EXISTS))
    {
      t = ohfa (&len);
      tmp = g_format_size_for_display ((goffset) len);
      free (t);

      str =
        g_markup_printf_escaped (_
                                 ("  <span font_weight=\"bold\">File Size: </span>"
                                  "  <span style=\"italic\">%s</span>  ( <i>%d</i> byte )\n"
                                  "  <span font_weight=\"bold\">File Path: </span>"
                                  "  <span style=\"italic\">N/D</span>\n"
                                  "  <span font_weight=\"bold\">File Name: </span>"
                                  "  <span style=\"italic\">N/D</span>\n"), tmp, len);
      g_free (tmp);
    }
  /* Opened File */
  else
    {
      info = inap (_ncurrent (file)->name);

      t = strreplace (info[0], "<", "&lt;");
      info[0] = strreplace (t, ">", "&gt;");
      g_free (t);
      t = strreplace (info[0], "%", "%%");
      g_free (info[0]);
      info[0] = g_strdup (t);
      g_free (t);

      t = strreplace (info[1], "<", "&lt;");
      info[1] = strreplace (t, ">", "&gt;");
      g_free (t);
      t = strreplace (info[1], "%", "%%");
      g_free (info[1]);
      info[1] = g_strdup (t);
      g_free (t);

      t = ohfa (&len);
      tmp = g_format_size_for_display ((goffset) len);
      free (t);

      t = g_strdup_printf (_(" <span font_weight=\"bold\">File Size: </span>"
                             " <span style=\"italic\">%s</span> "
                             " ( <span style=\"italic\">%d</span> byte )\n"
                             " <span font_weight=\"bold\">File Path: </span>"
                             "<span style=\"italic\">%s</span>\n"
                             " <span font_weight=\"bold\">File Name: </span>"
                             "<span style=\"italic\">%s</span>\n"
                             "\n%s\n"), tmp, len, info[1], info[0], info_type (TRUE));
      str = g_markup_printf_escaped (t, NULL);
      g_free (t);

      g_free (tmp);
    }

  return str;
}

void
remove_last_page (void)
{
  extern GtkWidget *jeex_main_window;
  extern JeexToolbar *jeex_toolbar;
  extern struct show_file_struct show_file_struct;
  GtkTextIter start, end;

  check_p = FALSE;

  /* NULL-ing elements */
  if ((*file)->name)
    (*file)->name = NULL;

  gtk_window_set_title (GTK_WINDOW (jeex_main_window), Jeex);

  jeex_widget_hide (textview->insert->table);
  jeex_widget_hide (textview->regex->table);
  jeex_widget_hide (_ncurrent (notebook->update));
  jeex_widget_hide (table_info);

  /* Disabling buttons regarding operations with File */
  jeex_widget_set_sensitive (FALSE, jeex_menu->file_menu.close,
                             jeex_menu->file_menu.append,
                             jeex_menu->file_menu.save,
                             jeex_menu->file_menu.save_as,
                             jeex_menu->file_menu.save_html,
                             jeex_menu->tools_menu.file_structures,
                             jeex_menu->tools_menu.split,
                             jeex_menu->view_menu.hash,
                             jeex_menu->file_menu.info,
                             jeex_menu->file_menu.remove,
                             jeex_menu->edit_menu.copy,
                             jeex_menu->edit_menu.cut,
                             jeex_menu->edit_menu.delete,
                             jeex_menu->edit_menu.select_all,
                             jeex_menu->edit_menu.paste,
                             jeex_menu->edit_menu.update,
                             jeex_menu->edit_menu.reset,
                             jeex_menu->search_menu.replace,
                             jeex_menu->search_menu.regex,
                             jeex_menu->edit_menu.insert,
                             jeex_menu->edit_menu.revert,
                             jeex_menu->edit_menu.insert_from_bit,
                             jeex_menu->search_menu.go_offset,
                             jeex_menu->search_menu.search,
                             jeex_menu->tools_menu.strings,
                             jeex_menu->view_menu.stats,
                             jeex_menu->view_menu.add_view,
                             jeex_menu->bookmark_menu.manage,
                             jeex_menu->bookmark_menu.add,
                             *(notebook->textview),
                             jeex_toolbar->save_file,
                             jeex_toolbar->search, jeex_toolbar->replace,
                             jeex_toolbar->copy, jeex_toolbar->cut,
                             jeex_toolbar->paste,
                             jeex_menu->view_menu.file_in_other_format,
                             jeex_menu->view_menu.difference, NULL);
  callback_widget_hide (NULL, show_file_struct.main_widget);

  /* Disabling writing and deleting text */
  gtk_text_view_set_overwrite (GTK_TEXT_VIEW (*(notebook->textview)), preferences->cursor_mode);
  gtk_text_buffer_get_bounds (*(notebook->buffer), &start, &end);
  gtk_text_buffer_delete (*(notebook->buffer), &start, &end);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (*(notebook->textview)), FALSE);

  /* If it was created a new file it closes */
  if ((*file)->new)
    {
      _ncurrent (file)->new = !(*file)->new;
      _ncurrent (file)->modified = FALSE;
    }

  gtk_notebook_set_tab_label_text (GTK_NOTEBOOK (notebook->notebook),
                                   *(notebook->table),
                                   g_strndup (_("No File Open"), preferences->max_tabchars));

  notebook->current = 0;
}

static void
copy_file_structure (JeexFileInfo * dest, JeexFileInfo * src, GtkWidget * notebook_table)
{
  char *str[2];
  g_return_if_fail (src && dest);

  chk_free (dest->name);
  if (src->name)
    {
      dest->name = g_strdup (src->name);
      str[0] = g_path_get_basename (dest->name);
      if (strlen (str[0]) > preferences->max_tabchars)
        {
          str[1] = g_strndup (str[0], preferences->max_tabchars);
          g_free (str[0]);
          str[0] = g_strdup_printf ("%s...", str[1]);
          g_free (str[1]);
        }

      gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook->notebook),
                                  notebook_table, gtk_label_new (*str));
    }
  else
    dest->name = NULL;

  chk_free (dest->type);
  if (src->type)
    dest->type = g_strdup (src->type);
  else
    dest->type = NULL;

  chk_free (dest->permission);
  if (src->permission)
    dest->permission = g_strdup (src->permission);
  else
    dest->permission = NULL;

  dest->mtime = src->mtime;
  memcpy (&(dest->last_access), &(src->last_access), 50);
  memcpy (&(dest->last_modify), &(src->last_modify), 50);
  dest->size = (src->size > 0) ? src->size : 0;
  dest->new = src->new;
  dest->modified = src->modified;
  memcpy (&(dest->bookmark), &(src->bookmark), sizeof (struct JeexBookmark));
}

/* Function to close the file */
void *
close_file (GtkWidget * widget, void *data)
{
  int total_page, i;

  logging_action (_("Closed file `%s'\n"), file[notebook->current]->name);

  total_page = gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook->notebook)) - 1;
  if (total_page < 1 && check_p)
    {
      remove_last_page ();
      update_status_bar (NULL, NULL);
	  plugin_close_file_emit_signal ();
      return NULL;
    }

  if (notebook->current == total_page)
    {
      remove_current_page update_status_bar (NULL, NULL);
	  plugin_close_file_emit_signal ();
      return NULL;
    }

  /* Scaled elements */
  for (i = notebook->current; i < total_page; i++)
    {
      copy_file_structure (file[i], file[i + 1], notebook->table[i]);

      notebook->page[i] = notebook->page[i + 1];
      notebook->table[i] = notebook->table[i + 1];
      if (GTK_IS_TEXT_BUFFER (notebook->buffer[i + 1]))
        notebook->buffer[i] = notebook->buffer[i + 1];

      notebook->insert[i] = notebook->insert[i + 1];
      notebook->textview[i] = notebook->textview[i + 1];
    }

  remove_current_page update_status_bar (NULL, NULL);
  plugin_close_file_emit_signal ();

  return NULL;
}

/* Function to load file */
unsigned long
load_exe (FILE * fd, unsigned char **data)
{
  unsigned long rval, i, load_max;

  rewind (fd);
  fseek (fd, 0, SEEK_END);
  *data = malloc ((rval = ftell (fd)) + 1);
  rewind (fd);

  for (i = 0; i < rval; ++i)
    {
      if ((i + preferences->max_loadbuff) > rval)
        load_max = rval - i;
      else
        load_max = preferences->max_loadbuff;

      fread (*data + i, 1, load_max, fd);
    }
  (*data)[rval] = '\0';

  if (ftell (fd))
    {
      rewind (fd);
    }

  return rval;
}

/* Function to convert in hexadecimal the bytes */
char *
make_string (unsigned char **data, unsigned long len)
{
  GString *s;
  unsigned long i;

  if (len < 1)
    {
      return g_strdup ("\0");
    }

  s = g_string_new ("");

  /* "for" that converts to hex all the different bytes */
  for (i = 0; i < len; i++)
    {
      g_string_append_printf (s, "%.2X", (*data)[i]);

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

  return s->str;
}


/* Function to control the size */
gboolean
control_size (FILE * of)
{
  extern JeexPreferences *preferences;

  /* Recovery size */
  rewind (of);
  fseek (of, 0, SEEK_END);
  _ncurrent (file)->size = ftell (of);
  rewind (of);

  /* Control the size */
  if (_ncurrent (file)->size > preferences->max_filesize)
    {
      return FALSE;
    }

  return TRUE;
}

/* Function for the returns of the inserted hexadecimal text */
char *
ohfa (int *r)
{
  GtkTextIter start, end;
  char *txtmp, buff[3], *local_str;
  double c, i;
  int mem = 2;

  /* Gets start and end iter of the text */
  gtk_text_buffer_get_start_iter (textview->buffer, &start);
  gtk_text_buffer_get_end_iter (textview->buffer, &end);

  /* Gets text */
  txtmp = gtk_text_buffer_get_text (textview->buffer, &start, &end, TRUE);

  i = strlen (txtmp);

  local_str = (char *) malloc (mem);

  /* "for" for the convertion */
  for (c = *r = 0; c < i;)
    {
      if (txtmp[0] == ' ' || txtmp[0] == '\n')
        {
          txtmp++;
          c++;
          continue;
        }

      buff[0] = txtmp[0];
      buff[1] = txtmp[1];

      local_str[mem - 2] = strtol (buff, NULL, 16);
      local_str = (char *) realloc (local_str, mem++);

      txtmp += 2;
      c += 2;
      (*r)++;
    }
  local_str[mem - 2] = 0x00;

  return local_str;
}

/* Function to read the file */
void *
file_open (GtkWidget * dialog)
{
  unsigned char *ddata;
  unsigned long dlen;
  unsigned int ram_size = 0;
  char *str, **tmp_info, *tmp;
  FILE *of;
  int i;
  GtkTextIter iter, start, end;
  GtkTextMark *mark;
  GFileError error_type;
  extern GtkWidget *jeex_main_window;
  extern JeexPreferences *preferences;
  extern JeexToolbar *jeex_toolbar;
  static struct stat info;

  if (dialog && G_IS_OBJECT (G_OBJECT (dialog)))
    {
      gtk_widget_destroy (dialog);
    }

  logging_action (_("Opened file `%s'\n"), _ncurrent (file)->name);

  stat (_ncurrent (file)->name, &info);
  _ncurrent (file)->mtime = info.st_mtime;

  /* Checks if file exist and it can be read */
  if (!(of = fopen (_ncurrent (file)->name, "rb")))
    {
      /* Checks error type */
      error_type = g_file_error_from_errno (errno);
      file_error_control (error_type, RB_MODE);

      if (gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook->notebook)) > 1 && check_p)
        {
          gtk_notebook_remove_page (GTK_NOTEBOOK (notebook->notebook), notebook->current);
        }
      else
        {
          check_p = FALSE;

          if ((*file)->name)
            {
              (*file)->name = NULL;
            }

          /* Hides table with info */
          if (GTK_IS_WIDGET (textview->insert->table))
            {
              gtk_widget_hide (textview->insert->table);
            }

          if (GTK_IS_WIDGET (textview->regex->table))
            {
              gtk_widget_hide (textview->regex->table);
            }
        }

      update_status_bar (NULL, NULL);

      return ((void **) !NULL);
    }

  /* Checks if file is a folder */
  if (g_file_test (_ncurrent (file)->name, G_FILE_TEST_IS_DIR))
    {
      _error (_("<big><b>Unable to open file</b></big>\n"
                "\nThe file you are trying to open is a directory."));
      fclose (of);

      if (gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook->notebook)) > 1 && check_p)
        {
          gtk_notebook_remove_page (GTK_NOTEBOOK (notebook->notebook), notebook->current);
        }
      else
        {
          check_p = FALSE;

          if ((*file)->name)
            {
              (*file)->name = NULL;
            }

          /* Hides table with info */
          if (GTK_IS_WIDGET (textview->insert->table))
            {
              gtk_widget_hide (textview->insert->table);
            }

          if (GTK_IS_WIDGET (textview->regex->table))
            {
              gtk_widget_hide (textview->regex->table);
            }
        }

      update_status_bar (NULL, NULL);

      return ((void **) !NULL);
    }

  /* Checks size */
  if (!control_size (of))
    {
      _error (g_strdup_printf (_("<big><b>File too big!</b></big>\n"
                                 "\nThe file size exceeds <i>%s</i>."),
                               g_format_size_for_display ((goffset) preferences->max_filesize)));
      fclose (of);
      logging_action (_("File opened is too big!\n"));

      if (gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook->notebook)) > 1 && check_p)
        {
          gtk_notebook_remove_page (GTK_NOTEBOOK (notebook->notebook), notebook->current);
        }
      else
        {
          check_p = FALSE;

          if ((*file)->name)
            {
              (*file)->name = NULL;
            }

          /* Hides table with info */
          if (GTK_IS_WIDGET (textview->insert->table))
            {
              gtk_widget_hide (textview->insert->table);
            }

          if (GTK_IS_WIDGET (textview->regex->table))
            {
              gtk_widget_hide (textview->regex->table);
            }
        }

      update_status_bar (NULL, NULL);

      return ((void **) !NULL);
    }

  /* Checks that opened file isn't too big for the system */
  if (get_ram_size_from_proc (&ram_size) < _ncurrent (file)->size)
    {
      _error (g_strdup_printf (_("<big><b>File too big!</b></big>\n"
                                 "\nThe file is too big for the system!\n"
                                 "You have only <b>%s</b> of RAM."),
                               g_format_size_for_display ((goffset) ram_size)));
      fclose (of);
      logging_action (_("File opened is too big for the system!\n"));

      if (gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook->notebook)) > 1 && check_p)
        {
          gtk_notebook_remove_page (GTK_NOTEBOOK (notebook->notebook), notebook->current);
        }
      else
        {
          check_p = FALSE;

          if ((*file)->name)
            {
              (*file)->name = NULL;
            }

          /* Hides table with info */
          if (GTK_IS_WIDGET (textview->insert->table))
            {
              gtk_widget_hide (textview->insert->table);
            }

          if (GTK_IS_WIDGET (textview->regex->table))
            {
              gtk_widget_hide (textview->regex->table);
            }
        }

      update_status_bar (NULL, NULL);

      return ((void **) !NULL);
    }

  /* Reset text */
  gtk_text_buffer_get_bounds (_ncurrent (notebook->buffer), &start, &end);
  gtk_text_buffer_delete (_ncurrent (notebook->buffer), &start, &end);

  /* Recovery data to insert a new text */
  _ncurrent (notebook->buffer) =
    gtk_text_view_get_buffer (GTK_TEXT_VIEW (_ncurrent (notebook->textview)));
  mark = gtk_text_buffer_get_insert (_ncurrent (notebook->buffer));
  gtk_text_buffer_get_iter_at_mark (_ncurrent (notebook->buffer), &iter, mark);

  /* Setting color */
  gdk_color_parse (preferences->color, &(textview->color));
  gtk_widget_modify_text (_ncurrent (notebook->textview), GTK_STATE_NORMAL, &(textview->color));

  /* Loading of text */
  dlen = load_exe (of, &ddata);
  info_type (FALSE, ddata, dlen);
  str = make_string (&ddata, dlen);
  fclose (of);

  gtk_text_buffer_set_text (_ncurrent (notebook->buffer), str, -1);

  g_free (str);
  free (ddata);

  /* Makes the textview editable and place the pointer in overwrite
   * modality.
   */
  gtk_text_view_set_editable (GTK_TEXT_VIEW (_ncurrent (notebook->textview)), TRUE);
  gtk_text_view_set_overwrite (GTK_TEXT_VIEW (_ncurrent (notebook->textview)), preferences->cursor_mode);

  tmp_info = inap (_ncurrent (file)->name);
  tmp = g_strdup_printf ("%s ( %s ) - %s", tmp_info[0], tmp_info[1], Jeex);
  gtk_window_set_title (GTK_WINDOW (jeex_main_window), tmp);
  g_free (tmp);

  /* Adds to recent file */
  add_recent_file (tmp_info[0], tmp_info[1]);

  /* Resize string if length > preferences->max_tabchars */
  if ((i = strlen (tmp_info[0])) > preferences->max_tabchars)
    {
      tmp = g_strndup (tmp_info[0], preferences->max_tabchars);
      g_free (tmp_info[0]);
      tmp_info[0] = g_strdup_printf ("%s...", tmp);
      g_free (tmp);
    }
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook->notebook),
                              _ncurrent (notebook->table), gtk_label_new (tmp_info[0]));

  g_free (tmp_info[0]);
  g_free (tmp_info[1]);
  g_free (tmp_info);

  _ncurrent (file)->new = FALSE;
  _ncurrent (file)->modified = FALSE;

  /* Abling buttons regarding operations with File */
  jeex_widget_set_sensitive (TRUE, jeex_menu->file_menu.save,
                             jeex_menu->file_menu.save_as,
                             jeex_menu->file_menu.save_html,
                             jeex_menu->tools_menu.split,
                             jeex_menu->tools_menu.file_structures,
                             jeex_menu->view_menu.hash,
                             jeex_menu->file_menu.info,
                             jeex_menu->file_menu.remove,
                             jeex_menu->file_menu.close,
                             jeex_menu->file_menu.append,
                             jeex_menu->edit_menu.copy,
                             jeex_menu->edit_menu.delete,
                             jeex_menu->edit_menu.select_all,
                             jeex_menu->edit_menu.cut,
                             jeex_menu->edit_menu.paste,
                             jeex_menu->edit_menu.reset,
                             jeex_menu->search_menu.replace,
                             jeex_menu->edit_menu.revert,
                             jeex_menu->search_menu.regex,
                             jeex_menu->edit_menu.insert,
                             jeex_menu->edit_menu.insert_from_bit,
                             jeex_menu->search_menu.go_offset,
                             jeex_menu->search_menu.search,
                             jeex_menu->edit_menu.update,
                             jeex_menu->tools_menu.strings,
                             jeex_menu->view_menu.stats,
                             jeex_menu->view_menu.add_view,
                             jeex_menu->bookmark_menu.manage,
                             jeex_toolbar->replace,
                             jeex_menu->bookmark_menu.add,
                             _ncurrent (notebook->textview),
                             jeex_toolbar->save_file, jeex_toolbar->search,
                             jeex_toolbar->copy, jeex_toolbar->cut,
                             jeex_toolbar->paste,
                             jeex_menu->view_menu.file_in_other_format,
                             jeex_menu->view_menu.difference, NULL);

  plugin_open_file_emit_signal ();

  update_status_bar (NULL, NULL);

  return NULL;
}

/* Function to append the content of select file to content of current file */
void *
file_append (GtkWidget * widget, void *data)
{
  GtkWidget *dialog;
  extern GtkWidget *jeex_main_window;
  int result;
  char *str, *filename;
  FILE *of;
  GFileError error_type;
  unsigned char *ddata;
  unsigned long dlen;
  GtkTextIter iter;

  dialog =
    gtk_file_chooser_dialog_new (_("Open File..."),
                                 GTK_WINDOW (jeex_main_window),
                                 GTK_FILE_CHOOSER_ACTION_OPEN,
                                 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                 GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);

  if (_ncurrent (file)->name)
    {
      str = g_path_get_dirname (_ncurrent (file)->name);
      gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), str);
      g_free (str);
    }

  /* Disabling multiple choice */
  gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (dialog), FALSE);

  if ((result = gtk_dialog_run (GTK_DIALOG (dialog))) == GTK_RESPONSE_ACCEPT)
    {
      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
      if (!(of = fopen (filename, "rb")))
        {
          /* Checks error type */
          error_type = g_file_error_from_errno (errno);
          file_error_control (error_type, RB_MODE);

          g_free (filename);
          gtk_widget_destroy (dialog);

          return NULL;
        }
      g_free (filename);
      dlen = load_exe (of, &ddata);
      str = make_string (&ddata, dlen);
      fclose (of);
      free (ddata);
      gtk_text_buffer_get_end_iter (_ncurrent (notebook->buffer), &iter);
      gtk_text_buffer_insert (_ncurrent (notebook->buffer), &iter, str, -1);
      g_free (str);
    }
  gtk_widget_destroy (dialog);
  return NULL;
}


/* Function to choose the file */
void *
open_file (GtkWidget * widget, void *data)
{
  GtkWidget *dialog;
  int result, tot, i;
  char *str;
  GSList *filenames;
  extern GtkWidget *jeex_main_window;

  dialog =
    gtk_file_chooser_dialog_new (_("Open File..."),
                                 GTK_WINDOW (jeex_main_window),
                                 GTK_FILE_CHOOSER_ACTION_OPEN,
                                 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                 GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);

  if (_ncurrent (file)->name)
    {
      str = g_path_get_dirname (_ncurrent (file)->name);
      gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), str);
      g_free (str);
    }

  /* Disabling multiple choice */
  gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (dialog), FALSE);

  result = gtk_dialog_run (GTK_DIALOG (dialog));

  if (result == GTK_RESPONSE_ACCEPT)
    {
      filenames = gtk_file_chooser_get_filenames (GTK_FILE_CHOOSER (dialog));

      str = (char *) g_path_get_basename (filenames->data);

      if (check_p)
        {
          /* Checks if the file was opened */
          tot = gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook->notebook));

          for (i = 0; i < tot; ++i)
            {
              if (!(file[i]->name))
                {
                  continue;
                }

              if (!strcmp (file[i]->name, filenames->data))
                {
                  /* If was opened in previous time, position current tab
                   * on tab where it's and closes dialog.
                   */
                  gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook->notebook), i);
                  gtk_widget_destroy (dialog);

                  return NULL;
                }
            }

          make_new_notebook_page (str);
        }
      else
        {
          check_p = TRUE;
        }

      g_free (str);
      _ncurrent (file)->name = (char *) filenames->data;
      _ncurrent (file)->bookmark.n = 0;

      file_open (dialog);

      gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook->notebook), notebook->current);

    }
  else
    {
      gtk_widget_destroy (dialog);
      update_status_bar (NULL, NULL);
    }

  return NULL;
}

/* Function to open a position */
void *
open_position (GtkWidget * widget, void *data)
{
  char *str, *fstr;
  DIR *input;
  int result, x = 0, c;
  gboolean chk = FALSE;
  GtkWidget *dialog;
  extern GtkWidget *jeex_main_window;
  struct dirent *dirent;

  dialog =
    gtk_file_chooser_dialog_new (_("Select Folder..."),
                                 GTK_WINDOW (jeex_main_window),
                                 GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                 GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);

  if (_ncurrent (file)->name)
    {
      str = g_path_get_dirname (_ncurrent (file)->name);
      gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), str);
      g_free (str);
    }

  /* Disabling multiple choice */
  gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (dialog), FALSE);

  result = gtk_dialog_run (GTK_DIALOG (dialog));

  if (result == GTK_RESPONSE_ACCEPT)
    {
      str = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
      if (!str)
        {
          gtk_widget_destroy (dialog);
          return NULL;
        }

      logging_action (_("Opened position `%s'\n"), str);
      if (!(input = opendir (str)))
        {
          _error (_("Cannot open input folder."));
          g_free (str);
          gtk_widget_destroy (dialog);
          return NULL;
        }

      while ((dirent = readdir (input)))
        {
          if (dirent->d_type == DT_REG)
            {
              fstr = g_strdup_printf ("%s/%s", str, dirent->d_name);
              if (check_p)
                {
                  /* Checking if file was opened */
                  x = gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook->notebook));
                  for (c = 0; c < x; ++c)
                    {
                      if (!(file[c]->name))
                        continue;

                      if (!strcmp (file[c]->name, fstr))
                        chk = TRUE;
                    }

                  if (!chk)
                    make_new_notebook_page (str);
                }
              else
                check_p = TRUE;

              if (!chk)
                {
                  file[x]->name = g_strdup (fstr);
                  if (!file_open (NULL))
                    x++;
                }
              g_free (fstr);
            }
        }
      g_free (str);
      closedir (input);
    }

  gtk_widget_destroy (dialog);

  return NULL;
}

/* Function to save the file */
void *
save_file (GtkWidget * widget, void *data)
{
  char **info, *str, *value;
  int c, i, save_len;
  FILE *of;
  GFileError error_type;
  extern GtkWidget *jeex_main_window;
  extern JeexToolbar *jeex_toolbar;
  static struct stat file_info;

  if (!_ncurrent (file)->new)
    /* If the file is going to save is not new .. */
    {
      if (g_file_test (_ncurrent (file)->name, G_FILE_TEST_EXISTS) && preferences->backup_copy)
        {
          str = g_strdup_printf ("%s~", _ncurrent (file)->name);
          logging_action (_("Saving backup copy of `%s'..\n"), _ncurrent (file)->name);
          if (-1 == rename (_ncurrent (file)->name, str))
            {
              _error (_("<big><b>Error to making a backup copy</b></big>"));

#if defined(DEBUG_ENABLE) && DEBUG_ENABLE
              g_debug ("%s:%d - save_file:\n"
                       "\t\t(i) rename (%s, %s): %s\n",
                       __FILE__, __LINE__, _ncurrent (file)->name, str, strerror (errno));
#endif
              logging_action (_("Backup copy not created. An unknow error has occurred.\n"));
            }
          else
            logging_action (_("Backup copy created. No error has occurred.\n"));

          g_free (str);
        }

      logging_action (_("Saving file `%s'..\n"), _ncurrent (file)->name);
      if (!(of = fopen (_ncurrent (file)->name, "wb+")))
        /* Opening the file for writing */
        {
          logging_action (_("Error has occurred while saving the file `%s'\n"), _ncurrent (file)->name);
          error_type = g_file_error_from_errno (errno);
          file_error_control (error_type, WB_MODE);
        }
      else
        {
          str = ohfa (&c);
          if (save_format == GENERIC_FORMAT)
            {
              for (i = 0; i < c;)
                {
                  if ((i + preferences->max_savebuff) > c)
                    save_len = c - i;
                  else
                    save_len = preferences->max_savebuff;

                  fwrite (str + i, sizeof (unsigned char), save_len, of);

                  i += save_len;
                }
            }
          else
            {
              for (i = 0; i < c; i++)
                {
                  switch (save_format)
                    {
                    case JASCII:
                      if (g_ascii_isprint (str[i]) || str[i] == 0x20)
                        value = g_strdup_printf ("%c", str[i]);
                      else
                        value = g_strdup (".");
                      fwrite (value, sizeof (char), 1, of);
                      break;
                    case JOCT:
                      value = g_strdup_printf ("%3.3o", str[i]);
                      fwrite (value, sizeof (char), 3, of);
                      break;
                    case JHEX:
                      value = g_strdup_printf ("%2.2X", str[i]);
                      fwrite (value, sizeof (char), 2, of);
                      break;
                    case JDEC:
                      value = g_strdup_printf ("%3.3d", str[i]);
                      fwrite (value, sizeof (char), 3, of);
                      break;
                    case JBIN:
                      value = int_to_binary (str[i]);
                      fwrite (value, sizeof (char), 8, of);
                      break;
                    case GENERIC_FORMAT:
                    default:
                      break;
                    }
                  g_free (value);

                  /* Insert one space every 4 bytes */
                  if (!((i + 1) % 4) && i && ((i + 1) % 16) && i != c - 1)
                    {
                      value = g_strdup (" ");
                      fwrite (value, sizeof (char), 1, of);
                      g_free (value);
                    }

                  /* Case in who has taken 16 bytes */
                  if (!((i + 1) % 16) && i && i != c - 1)
                    {
                      value = g_strdup ("\n");
                      fwrite (value, sizeof (char), 1, of);
                      g_free (value);
                    }
                  else if (i != c - 1)
                    {
                      value = g_strdup (" ");
                      fwrite (value, sizeof (char), 1, of);
                      g_free (value);
                    }
                }
            }

          free (str);
          fclose (of);
          logging_action (_("File `%s' has been saved.\n"), _ncurrent (file)->name);

          /* Text updating with the saved text, useful if the saved
           * text does not correspond to what we wanted to save,
           * this is equivalent to some bugs in saving of jeex
           */
          stat (_ncurrent (file)->name, &file_info);
          _ncurrent (file)->mtime = file_info.st_mtime;
          _ncurrent (file)->modified = FALSE;
          _ncurrent (file)->new = FALSE;
          _update (NULL, NULL);
          update_status_bar (NULL, NULL);

          info = inap (_ncurrent (file)->name);
          str = g_strdup_printf ("%s ( %s ) - %s", info[0], info[1], Jeex);
          gtk_window_set_title (GTK_WINDOW (jeex_main_window), str);
          g_free (str);
          g_free (info[0]);
          g_free (info[1]);
          g_free (info);
          able_file_widget (NULL, NULL);

          jeex_widget_set_sensitive (TRUE, jeex_menu->edit_menu.update,
                                     jeex_menu->file_menu.remove, NULL);
        }
    }
  else
    {
      /* In case the file was created by 0 refers the
       * screen to decide where to save it.
       */
      save_file_as (NULL, NULL);
    }

  return NULL;
}

/* Function to create a filter for files showed when
 * the current file is being saved
 */
static void
make_filter (GtkFileFilter * filter, unsigned int type)
{
  const char *pattern[6] = { "*", "*.jascii", "*.joct", "*.jhex", "*.jdec", "*.jbin" }, *human_text[6] =
  {
  N_("Generic File"),
      N_("Jeex File: Ascii ( *.jascii )"),
      N_("Jeex File: Octal ( *.joct )"),
      N_("Jeex File: Hexadecimal ( *.jhex )"),
      N_("Jeex File: Decimal ( *.jdec )"), N_("Jeex File: Binary ( *.jbin )")};

  gtk_file_filter_set_name (filter, _(human_text[type]));
  gtk_file_filter_add_pattern (filter, pattern[type]);
}

/* Funzion to change the extension in the text entered into
 * file chooser passed as parameter.
 */
static void
change_extension (GtkFileChooser * file_chooser)
{
  char *file_name, *format;
  int i, len;
  gboolean pchk;

  file_name = gtk_file_chooser_get_filename (file_chooser);
  if (!file_name)
    file_name = g_strdup (_("Untitle File"));
  else
    file_name = g_path_get_basename (file_name);

  for (len = strlen (file_name) - 1, i = len; len > 0; len--)
    {
      if (file_name[len] == '.')
        break;
    }
  pchk = len ? TRUE : FALSE;
  len = len ? len : i;

  switch (save_format)
    {
    case GENERIC_FORMAT:
      format = g_strdup ("");
      break;
    case JASCII:
      format = g_strdup (".jascii");
      break;
    case JOCT:
      format = g_strdup (".joct");
      break;
    case JHEX:
      format = g_strdup (".jhex");
      break;
    case JDEC:
      format = g_strdup (".jdec");
      break;
    case JBIN:
      format = g_strdup (".jbin");
      break;
    default:
      format = g_strdup ("");
      break;
    }

  file_name = g_strdup_printf ("%s%s", g_strndup (file_name, pchk ? len : len + 1), format);
  g_free (format);

  gtk_file_chooser_set_current_name (file_chooser, file_name);
  g_free (file_name);
}

static int
get_format_number_from_string (char *string)
{
  const char *extension[6] = { "", "jascii", "joct", "jhex", "jdec", "jbin" };
  int i;

  g_return_val_if_fail (string, -1);

  for (i = 0; i < 6; ++i)
    {
      if (!strcmp (string, extension[i]))
        return i;
    }

  return -1;
}

/* Function to apply the select user format */
static void
selected_format (GtkTreeSelection * treeselection, void *user_data)
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  char *value;
  int n;

  if (gtk_tree_selection_get_selected (GTK_TREE_SELECTION (treeselection), &model, &iter))
    {
      gtk_tree_model_get (model, &iter, 1, &value, -1);
      n = get_format_number_from_string (value);
      g_free (value);

      switch (n)
        {
        case 1:
          save_format = JASCII;
          break;
        case 2:
          save_format = JOCT;
          break;
        case 3:
          save_format = JHEX;
          break;
        case 4:
          save_format = JDEC;
          break;
        case 5:
          save_format = JBIN;
          break;
        default:
          save_format = GENERIC_FORMAT;
          break;
        }

      change_extension ((GtkFileChooser *) user_data);
    }
}

/* Function to make an area where the user can select
 * the save format.
 */
static GtkWidget *
make_select_format_area (GtkFileChooser * file_chooser)
{
  GtkWidget *expander, *treeview, *scroll;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  GtkTreeSelection *selection;
  GtkListStore *store;
  GtkTreeIter iter;
  int i;
  const char *extension[6] = { "", "jascii", "joct", "jhex", "jdec", "jbin" }, *description[6] =
  {
  N_("Generic File"),
      N_("Jeex Ascii File"),
      N_("Jeex Octal File"),
      N_("Jeex Hexadecimal File"), N_("Jeex Decimal File"), N_("Jeex Binary File")};

  expander = gtk_expander_new (_("Select File Type ( By Extension )"));

  /* TreeView */
  treeview = gtk_tree_view_new ();
  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview), TRUE);
  renderer = gtk_cell_renderer_text_new ();
  store = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_STRING);

  /* Description */
  column = gtk_tree_view_column_new_with_attributes (_("Description"), renderer, "text", 0, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
  gtk_tree_view_column_set_resizable (column, TRUE);

  /* Extension */
  column = gtk_tree_view_column_new_with_attributes (_("Extension"), renderer, "text", 1, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
  gtk_tree_view_column_set_resizable (column, TRUE);

#define msfa_space "                             "

  for (i = 0; i < 6; i++)
    {
      gtk_list_store_append (store, &iter);
      if (!i)
        gtk_list_store_set (store, &iter, 0,
                            g_strdup_printf ("%s%s", _(description[i]),
                                             msfa_space), 1, extension[i], -1);
      else
        gtk_list_store_set (store, &iter, 0, _(description[i]), 1, extension[i], -1);
    }

  gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
  g_object_unref (store);

  /* TreeView Scrollbar */
  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_size_request (scroll, -1, 155);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scroll), treeview);
  gtk_container_add (GTK_CONTAINER (expander), scroll);

  gtk_widget_show_all (expander);

  /* Signals */
  g_signal_connect (G_OBJECT (selection), "changed", G_CALLBACK (selected_format), file_chooser);

  return expander;
}

/* Function to choose where to save */
void *
save_file_as (GtkWidget * widget, void *data)
{
  GtkWidget *dialog;
  int result, i = -1;
  extern GtkWidget *jeex_main_window;
  GtkFileFilter *filter_file[6];

  if ((!_ncurrent (file)->name || !strcmp (_ncurrent (file)->name, "")) && !_ncurrent (file)->new)
    {
      _error (_("<big><b>Unable to save the file</b></big>.\n" "\nNo files were opened."));
    }
  else
    {
      dialog =
        gtk_file_chooser_dialog_new (_("Save File As.."),
                                     GTK_WINDOW (jeex_main_window),
                                     GTK_FILE_CHOOSER_ACTION_SAVE,
                                     GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                     GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);

      if (!_ncurrent (file)->new)
        {
          gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog),
                                               g_path_get_dirname (_ncurrent (file)->name));
        }

      gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);

      /* Making filter */
      filter_file[++i] = gtk_file_filter_new ();
      make_filter (filter_file[i], i);
      gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filter_file[i]);

      filter_file[++i] = gtk_file_filter_new ();
      make_filter (filter_file[i], i);
      gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filter_file[i]);

      filter_file[++i] = gtk_file_filter_new ();
      make_filter (filter_file[i], i);
      gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filter_file[i]);

      filter_file[++i] = gtk_file_filter_new ();
      make_filter (filter_file[i], i);
      gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filter_file[i]);

      filter_file[++i] = gtk_file_filter_new ();
      make_filter (filter_file[i], i);
      gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filter_file[i]);

      filter_file[++i] = gtk_file_filter_new ();
      make_filter (filter_file[i], i);
      gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filter_file[i]);

      /* Making an extra widget */
      gtk_file_chooser_set_extra_widget (GTK_FILE_CHOOSER (dialog),
                                         make_select_format_area (GTK_FILE_CHOOSER (dialog)));

      result = gtk_dialog_run (GTK_DIALOG (dialog));

      if (result == GTK_RESPONSE_ACCEPT)
        {
          _ncurrent (file)->name = g_strdup (gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog)));
          _ncurrent (file)->new = FALSE;
          _ncurrent (file)->modified = FALSE;

          gtk_widget_destroy (dialog);
          save_file (NULL, NULL);
        }
      else
        gtk_widget_destroy (dialog);
    }

  return NULL;
}

/* Restart Jeex */
void *
_restart (GtkWidget * widget, void *data)
{
  if (!g_file_test (JEEX_PATH_BIN "/" LINK_NAME, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_EXECUTABLE))
    {
#if defined(DEBUG_ENABLE) && DEBUG_ENABLE
      g_debug ("%s:%d - _restart:\n"
               "\t\t(c) Executable = (char *) \"%s\"\n", __FILE__, __LINE__,
               JEEX_PATH_BIN "/" LINK_NAME);
#endif
      _error (_("<big><b>Cannot restart Jeex</b></big>\n" "\nLink not found."));
    }
  else
    {
      logging_action (_("Restarted jeex..\n"));
      logging_unload ();
      execl (JEEX_PATH_BIN "/" LINK_NAME, JEEX_PATH_BIN "/" LINK_NAME, NULL, NULL);
    }

  return NULL;
}

/* Function to remove the opened file. */
void *
remove_open_file (GtkWidget * widget, void *data)
{

  if (!_info (_("Are you sure you want to delete the currently open file?"), TRUE))
    {
      _info (_("File wasn't deleted."), FALSE);
    }
  else
    {
      if (remove (_ncurrent (file)->name))
        {
          _info (_("Cannot delete the file."), FALSE);
        }
      else
        {
          logging_action (_("Removed file `%s'\n"), _ncurrent (file)->name);
          close_file (NULL, NULL);
          _info (_("File has been deleted."), FALSE);
        }
    }
  return NULL;
}


/* Function to make a new file */
void *
new_file (GtkWidget * widget, void *data)
{
  GtkTextIter iter;
  GtkTextMark *mark;
  extern JeexPreferences *preferences;
  extern JeexToolbar *jeex_toolbar;

  logging_action (_("Created a new file.\n"));
  if (check_p)
    {
      make_new_notebook_page (_("New File"));
    }
  else
    {
      gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook->notebook),
                                  _ncurrent (notebook->table),
                                  gtk_label_new (g_strndup (_("New File"), preferences->max_tabchars)));
      check_p = TRUE;
    }

  /* Gets various data to insert a new text */
  _ncurrent (notebook->buffer) =
    gtk_text_view_get_buffer (GTK_TEXT_VIEW (_ncurrent (notebook->textview)));
  mark = gtk_text_buffer_get_insert (_ncurrent (notebook->buffer));
  gtk_text_buffer_get_iter_at_mark (_ncurrent (notebook->buffer), &iter, mark);

  /* Setting color */
  gdk_color_parse (preferences->color, &(textview->color));
  gtk_widget_modify_text (_ncurrent (notebook->textview), GTK_STATE_NORMAL, &(textview->color));

  gtk_text_view_set_editable (GTK_TEXT_VIEW (_ncurrent (notebook->textview)), TRUE);
  gtk_text_view_set_overwrite (GTK_TEXT_VIEW (_ncurrent (notebook->textview)), preferences->cursor_mode);

  jeex_widget_set_sensitive (TRUE, jeex_menu->file_menu.save,
                             jeex_menu->file_menu.save_as,
                             jeex_menu->tools_menu.split,
                             jeex_menu->view_menu.hash,
                             jeex_menu->file_menu.info,
                             jeex_menu->tools_menu.file_structures,
                             jeex_menu->file_menu.save_html,
                             jeex_menu->file_menu.close,
                             jeex_menu->file_menu.append,
                             jeex_menu->edit_menu.copy,
                             jeex_menu->edit_menu.cut,
                             jeex_menu->edit_menu.paste,
                             jeex_menu->edit_menu.delete,
                             jeex_menu->edit_menu.select_all,
                             jeex_menu->edit_menu.reset,
                             jeex_menu->search_menu.search,
                             jeex_menu->search_menu.replace,
                             jeex_menu->search_menu.regex,
                             jeex_menu->edit_menu.insert,
                             jeex_menu->edit_menu.insert_from_bit,
                             jeex_menu->edit_menu.revert,
                             jeex_menu->search_menu.go_offset,
                             jeex_menu->tools_menu.strings,
                             jeex_menu->view_menu.stats,
                             jeex_menu->view_menu.add_view,
                             jeex_menu->bookmark_menu.add,
                             jeex_menu->bookmark_menu.manage,
                             _ncurrent (notebook->textview),
                             jeex_toolbar->save_file, jeex_toolbar->search,
                             jeex_toolbar->replace, jeex_toolbar->copy,
                             jeex_toolbar->cut, jeex_toolbar->paste,
                             jeex_menu->view_menu.file_in_other_format,
                             jeex_menu->view_menu.difference, NULL);

  if (!_ncurrent (file)->new)
    {
      _ncurrent (file)->new = TRUE;
      _ncurrent (file)->modified = TRUE;
    }

  /* NULL-ing var */
  _ncurrent (file)->name = NULL;
  _ncurrent (file)->type = NULL;
  _ncurrent (file)->permission = NULL;

  update_status_bar (NULL, NULL);

  _ncurrent (file)->bookmark.n = 0;

  if (check_p)
    {
      gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook->notebook), notebook->current);
    }

  return NULL;
}

/* Show properties of current opened file */
void *
properties (GtkWidget * widget, void *data)
{
  GtkWidget *win, *title, *table, *close, *info, *frame;
  extern GtkWidget *jeex_main_window;
  char *t;

  /* Window */
  win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_transient_for (GTK_WINDOW (win), GTK_WINDOW (jeex_main_window));
  gtk_window_set_position (GTK_WINDOW (win), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_title (GTK_WINDOW (win), _("Properties"));
  gtk_container_set_border_width (GTK_CONTAINER (win), 8);
  gtk_window_set_default_size (GTK_WINDOW (win), 315, -1);
  gtk_widget_show (win);

  /* Table */
  table = gtk_table_new (3, 1, FALSE);
  gtk_container_add (GTK_CONTAINER (win), table);
  gtk_widget_show (table);

  /* Title */
  title = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (title), _("<big><b>Properties</b></big>"));
  gtk_table_attach (GTK_TABLE (table), title, 0, 1, 0, 1, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
  gtk_widget_show (title);

  /* Frame */
  frame = gtk_frame_new (_("Information"));
  gtk_table_attach (GTK_TABLE (table), frame, 0, 1, 1, 2, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 10);
  gtk_widget_show (frame);

  /* Info */
  info = gtk_label_new (NULL);
  t = obtain_info ();
  gtk_label_set_markup (GTK_LABEL (info), t);
  g_free (t);
  gtk_label_set_selectable (GTK_LABEL (info), TRUE);
  gtk_label_set_ellipsize (GTK_LABEL (info), PANGO_ELLIPSIZE_MIDDLE);
  gtk_container_add (GTK_CONTAINER (frame), info);
  gtk_widget_show (info);

  /* CLOSE Button */
  close = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  gtk_table_attach (GTK_TABLE (table), close, 0, 1, 2, 3, GTK_FILL | GTK_EXPAND, GTK_EXPAND, 0, 0);
  g_signal_connect (G_OBJECT (close), "clicked", G_CALLBACK (destroy_this), win);
  gtk_widget_show (close);

  return NULL;
}
