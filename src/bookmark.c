/*
 * "bookmark.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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
#include <libintl.h>
#include <glib/gi18n.h>
#include <locale.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

#define bookmark_elem(elem) _ncurrent(file)->bookmark.elem

/* Structure to get the GtkEntry values.
 * Relative to bookmarks.
 */
static struct bookmark_widget
{
  GtkWidget *e_name;
  GtkWidget *e_addr;
  GtkWidget *e_tips;
} *bwidget;

/* Structure to manage window where the users manage the bookmarks. */
struct bookmarks_gui
{
  GtkWidget *win, *table, *treelist, *title, *label, *scroll, *sep;
  GtkWidget *l_name, *l_addr, *l_tips;
  GtkWidget *remove, *up, *down, *remove_i, *up_i, *down_i;
  GtkWidget *ok, *apply, *close, *box, *hbox;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  GtkListStore *store;
  GtkTreeIter iter;
  GtkTreeSelection *selection;
} *bg = NULL;

int current_bookmark = 0;
extern JeexMenu *jeex_menu;
extern JeexFileInfo *file[64];

static void *selected_bookmark (GtkWidget *, void *);
static void *add_bookmark (GtkWidget *, GtkWidget *);
static void *remove_bookmark (GtkWidget *, void *);
static void *go_to_bookmark (GtkWidget *, int);
static void *move_bookmark_up (GtkWidget *, void *);
static void *move_bookmark_down (GtkWidget *, void *);
static void *apply_bookmark_changes (GtkWidget *, void *);
static void *apply_and_close_manage_bookmarks_window (GtkWidget *, void *);
void delete_bookmarks (void);
void load_bookmarks (void);
void reload_bookmarks (void);

/* Gets next default name for the bookmark. */
const char *
next_bookmarks_name (void)
{
  int i, c;
  char *str;
  gboolean check = FALSE;

  if (bookmark_elem (n) < 1)
    {
      return _("New Bookmark");
    }

  for (i = 0; !check; ++i)
    {
      str = g_strdup_printf (_("New Bookmark %d"), i + 1);

      for (c = 0; c < bookmark_elem (n); ++c)
        {
          if (!strcmp (str, bookmark_elem (str[c])))
            {
              check = FALSE;
              break;
            }
          else
            {
              check = TRUE;
            }
        }

      if (!check)
        {
          g_free (str);
        }
      else
        {
          break;
        }
    }

  return (const char *) str;
}

/* Shows the window to add a bookmark. */
void *
add_bookmark_gui (GtkWidget * widget, void *data)
{
  GtkWidget *window, *table, *title, *sep;
  GtkWidget *l_name, *l_addr, *l_tips;
  GtkWidget *add, *close, *box, *hbox;
  extern GtkWidget *jeex_main_window;
  extern JeexPreferences *preferences;
  GtkTextMark *mark;
  GtkTextIter iter;
  char *addr;

  /* Checks that the existent bookmarks is < preferences->max_bookmarks.. */
  if (bookmark_elem (n) >= preferences->max_bookmarks)
    {
      addr =
        g_strdup_printf (_("<big><b>Unable to add bookmark</b></big>\n" "\n"
                           "You can insert max %d bookmarks in list,\n"
                           "to add another should delete some with\n"
                           "\"<i>Manage Bookmarks</i>\"."), preferences->max_bookmarks);
      _error (_(addr));
      g_free (addr);

      return NULL;
    }

  bwidget = (struct bookmark_widget *) g_malloc (sizeof (struct bookmark_widget));

  /* Window */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), _("Bookmarks"));
  gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (jeex_main_window));
  gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_default_size (GTK_WINDOW (window), 342, 158);
  gtk_container_set_border_width (GTK_CONTAINER (window), 5);
  gtk_widget_show (window);

  /* Table */
  table = gtk_table_new (6, 2, FALSE);
  gtk_container_add (GTK_CONTAINER (window), table);
  gtk_widget_show (table);

  /* Title */
  title = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (title),
                        g_markup_printf_escaped (_("<big><b>Add a bookmark</b></big>\n")));
  gtk_table_attach (GTK_TABLE (table), title, 0, 2, 0, 1, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
  gtk_widget_show (title);

  /* Row for bookmark name */
  l_name = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (l_name), g_markup_printf_escaped (_("<b>Bookmark Name:</b>")));
  gtk_misc_set_alignment (GTK_MISC (l_name), 0.0, 0.5);
  gtk_table_attach (GTK_TABLE (table), l_name, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (l_name);

  bwidget->e_name = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (bwidget->e_name), next_bookmarks_name ());
  gtk_entry_set_max_length (GTK_ENTRY (bwidget->e_name), 30);
  gtk_table_attach (GTK_TABLE (table), bwidget->e_name, 1, 2, 1, 2,
                    GTK_FILL | GTK_EXPAND, GTK_FILL, 10, 0);
  gtk_widget_show (bwidget->e_name);

  /* Row for bookmark address */
  l_addr = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (l_addr), g_markup_printf_escaped (_("<b>Bookmark Address:</b>")));
  gtk_misc_set_alignment (GTK_MISC (l_addr), 0.0, 0.5);
  gtk_table_attach (GTK_TABLE (table), l_addr, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (l_addr);

  bwidget->e_addr = gtk_entry_new ();
  mark = gtk_text_buffer_get_insert (textview->buffer);
  gtk_text_buffer_get_iter_at_mark (textview->buffer, &iter, mark);
  addr = g_strdup_printf ("%8.8X", 0x10 * (gtk_text_iter_get_line (&iter) + 1));
  gtk_entry_set_text (GTK_ENTRY (bwidget->e_addr), addr);
  gtk_entry_select_region (GTK_ENTRY (bwidget->e_addr), 0, strlen (addr));
  g_free (addr);
  gtk_entry_set_max_length (GTK_ENTRY (bwidget->e_addr), 8);
  gtk_table_attach (GTK_TABLE (table), bwidget->e_addr, 1, 2, 2, 3,
                    GTK_FILL | GTK_EXPAND, GTK_FILL, 10, 0);
  gtk_widget_show (bwidget->e_addr);

  /* Row for bookmark tooltips */
  l_tips = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (l_tips), g_markup_printf_escaped (_("<b>Bookmark Description:</b>")));
  gtk_misc_set_alignment (GTK_MISC (l_tips), 0.0, 0.5);
  gtk_table_attach (GTK_TABLE (table), l_tips, 0, 1, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (l_tips);

  bwidget->e_tips = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE (table), bwidget->e_tips, 1, 2, 3, 4,
                    GTK_FILL | GTK_EXPAND, GTK_FILL, 10, 0);
  gtk_widget_show (bwidget->e_tips);

  /* Separator for button */
  sep = gtk_hseparator_new ();
  gtk_table_attach (GTK_TABLE (table), sep, 0, 2, 4, 5, GTK_FILL | GTK_EXPAND,
                    GTK_FILL | GTK_EXPAND, 0, 0);
  gtk_widget_show (sep);

  /* Horizontal Box */
  hbox = gtk_hbox_new (TRUE, 0);
  gtk_table_attach (GTK_TABLE (table), hbox, 0, 2, 5, 6, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 2);
  gtk_widget_show (hbox);

  /* Button Box */
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

  /* Add Button */
  add = gtk_button_new_from_stock (GTK_STOCK_ADD);
  GTK_WIDGET_SET_FLAGS (add, GTK_CAN_DEFAULT);
  gtk_widget_grab_default (add);
  gtk_container_add (GTK_CONTAINER (box), add);
  gtk_widget_show (add);

  /* Signals */
  g_signal_connect (G_OBJECT (close), "clicked", G_CALLBACK (destroy_this), window);
  g_signal_connect (G_OBJECT (add), "clicked", G_CALLBACK (add_bookmark), window);

  return NULL;
}

/* Adds a bookmark */
static void *
add_bookmark (GtkWidget * widget, GtkWidget * win)
{
  int i, addr_d, len, res;
  char *text, *tips, *addr, c;

  text = (char *) gtk_entry_get_text (GTK_ENTRY (bwidget->e_name));
  tips = (char *) gtk_entry_get_text (GTK_ENTRY (bwidget->e_tips));
  addr = (char *) gtk_entry_get_text (GTK_ENTRY (bwidget->e_addr));

  if (*text == '\0' || *addr == '\0' || strisnull (text))
    {
      _error (_("<big><b>Unable to add bookmark</b></big>\n" "\n"
                "It wasn't possible to add the bookmark because\n"
                "the fields \"Name\" and/or \"Address\" were empty."));
      return NULL;
    }

  addr = g_utf8_normalize (addr, -1, G_NORMALIZE_DEFAULT);
  len = strlen (addr);

  /* Checking if addr is empty */
  if (strisnull (addr))
    {
      _error (_("<big><b>Unable to add bookmark</b></big>\n" "\n" "The entered address is empty.\n"));
      return NULL;
    }

  addr = g_ascii_strup (addr, -1);

  for (i = 0; i < len; ++i)
    {
      if (!g_ascii_isxdigit (addr[i]))
        {
          _error (_("<big><b>Unable to add bookmark</b></big>\n" "\n"
                    "The entered address is not valid."));
          return NULL;
        }
    }

  /* Converting from hexadecimal to decimal */
  addr_d = 0;
  for (i = 0; i < len; ++i)
    {
      if (addr[i] >= 0x30 && addr[i] <= 0x39)
        {
          c = addr[i] - 0x30;
        }
      else
        {
          c = addr[i] - 0x37;
        }
      addr_d += c * pow (16, (len - 1) - i);
    }
  addr_d /= 16;

  /* Control that there isn't another similar bookmark */
  for (i = 0; i < bookmark_elem (n); ++i)
    {
      if (!strcmp (text, bookmark_elem (str[i])))
        {
          _error (_("<big><b>Unable to add bookmark</b></big>\n" "\n"
                    "The name chosen for this bookmark is already used\n" "by another."));
          return NULL;
        }

      if (bookmark_elem (addr[i]) == addr_d)
        {
          res = _info (_("Another bookmark was added with the same address.\n\n" "Add it?"), TRUE);

          if (res)
            {
              break;
            }
          else
            {
              return NULL;
            }
        }
    }

  i = bookmark_elem (n);

  /* Adding bookmark */
  bookmark_elem (addr[i]) = addr_d;
  bookmark_elem (str[i]) = g_strdup (text);
  bookmark_elem (tips[i]) = (*tips == '\0') ? NULL : g_strdup (tips);

  reload_bookmarks ();

  gtk_widget_destroy (win);
  g_free (bwidget);

  delete_bookmarks ();

  bookmark_elem (n)++;

  load_bookmarks ();

  return NULL;
}

/* Function for reload the bookmarks */
void
reload_bookmarks (void)
{
  delete_bookmarks ();
  load_bookmarks ();
}

/* Function to delete the bookmarks */
void
delete_bookmarks (void)
{
  register int i;

  for (i = 0; i < bookmark_elem (n); ++i)
    {
      if (bookmark_elem (bookmark[i]) && GTK_IS_WIDGET (bookmark_elem (bookmark[i])))
        {
          gtk_widget_destroy (bookmark_elem (bookmark[i]));
        }
    }

  gtk_widget_hide (jeex_menu->bookmark_menu.sep);
  gtk_widget_set_sensitive (jeex_menu->bookmark_menu.next, FALSE);
  gtk_widget_set_sensitive (jeex_menu->bookmark_menu.prev, FALSE);
}

/* Function to load the bookmarks */
void
load_bookmarks (void)
{
  register int i;
  register GtkWidget *image;

  for (i = 0; i < bookmark_elem (n); ++i)
    {
      if (!i)
        {
          gtk_widget_show (jeex_menu->bookmark_menu.sep);
          gtk_widget_set_sensitive (jeex_menu->bookmark_menu.prev, TRUE);
          gtk_widget_set_sensitive (jeex_menu->bookmark_menu.next, TRUE);
        }

      bookmark_elem (bookmark[i]) = gtk_image_menu_item_new_with_label (bookmark_elem (str[i]));
      image = gtk_image_new_from_file (JEEX_PATH_IMG "/list_menu.png");
      gtk_widget_show (image);
      gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (bookmark_elem (bookmark[i])), image);

      gtk_menu_shell_insert (GTK_MENU_SHELL
                             (jeex_menu->bookmark_menu.bookmark), bookmark_elem (bookmark[i]), i + 3);

      if (bookmark_elem (tips[i]))
        {
          gtk_widget_set_tooltip_text (bookmark_elem (bookmark[i]),
                                       g_strdup_printf (_
                                                        ("\"%s\"\n"
                                                         "Go to address."), bookmark_elem (tips[i])));
        }
      else
        {
          gtk_widget_set_tooltip_text (bookmark_elem (bookmark[i]), _("Go to address."));
        }

      gtk_widget_show (bookmark_elem (bookmark[i]));

      g_signal_connect (G_OBJECT (bookmark_elem (bookmark[i])), "activate",
                        G_CALLBACK (go_to_bookmark), (void *) bookmark_elem (addr[i]));
    }
}

/* Function to position the scrollbar in bookmark offset. */
static void *
go_to_bookmark (GtkWidget * widget, int addr)
{
  GtkTextIter iter;
  GtkTextMark *mark;

  gtk_text_buffer_get_iter_at_mark (textview->buffer, &iter,
                                    gtk_text_buffer_get_insert (textview->buffer));

  gtk_text_iter_set_line (&iter, addr);
  mark = gtk_text_buffer_create_mark (textview->buffer, "mark_offset", &iter, FALSE);
  if (GTK_IS_TEXT_MARK (mark))
    {
      gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (textview->textview), mark);
    }

  return NULL;
}

/* Function for bookmarks manage. */
void *
manage_bookmarks_gui (GtkWidget * widget, void *data)
{
  int i;
  extern GtkWidget *jeex_main_window;

  if (bookmark_elem (n) < 1)
    {
      _error (_("<b><big>Unable to organize bookmarks</big></b>\n" "\n"
                "It wasn't possible to organize the bookmarks because\n"
                "there are no bookmarks saved.\n"));
      return NULL;
    }

  bwidget = (struct bookmark_widget *) g_malloc (sizeof (struct bookmark_widget));

  if (!bg)
    {
      bg = (struct bookmarks_gui *) g_malloc (sizeof (struct bookmarks_gui));
    }

  /* Window */
  bg->win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (bg->win), _("Manage Bookmarks"));
  gtk_window_set_transient_for (GTK_WINDOW (bg->win), GTK_WINDOW (jeex_main_window));
  gtk_window_set_position (GTK_WINDOW (bg->win), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_default_size (GTK_WINDOW (bg->win), 370, 162);
  gtk_container_set_border_width (GTK_CONTAINER (bg->win), 5);
  gtk_widget_show (bg->win);

  /* Table */
  bg->table = gtk_table_new (8, 5, FALSE);
  gtk_container_add (GTK_CONTAINER (bg->win), bg->table);
  gtk_widget_show (bg->table);

  /* Window Title */
  bg->title = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (bg->title),
                        g_markup_printf_escaped (_("<big><b>Manage Bookmarks</b></big>\n")));
  gtk_table_attach (GTK_TABLE (bg->table), bg->title, 0, 5, 0, 1, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
  gtk_widget_show (bg->title);

  /* List of created bookmarks */
  bg->label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (bg->label),
                        g_markup_printf_escaped (_("<i>Select the bookmark:</i>")));
  gtk_misc_set_alignment (GTK_MISC (bg->label), 0.0, 0.5);
  gtk_table_attach (GTK_TABLE (bg->table), bg->label, 0, 3, 1, 2, GTK_FILL | GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (bg->label);

  /* Making list */
  bg->treelist = gtk_tree_view_new ();
  gtk_widget_set_size_request (bg->treelist, 140, -1);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (bg->treelist), FALSE);
  bg->renderer = gtk_cell_renderer_text_new ();
  bg->store = gtk_list_store_new (1, G_TYPE_STRING);
  bg->column = gtk_tree_view_column_new_with_attributes ("Bookmark", bg->renderer, "text", 0, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (bg->treelist), bg->column);

  for (i = 0; i < bookmark_elem (n); ++i)
    {
      gtk_list_store_append (bg->store, &(bg->iter));
      gtk_list_store_set (bg->store, &(bg->iter), 0, bookmark_elem (str[i]), -1);
    }

  gtk_tree_view_set_model (GTK_TREE_VIEW (bg->treelist), GTK_TREE_MODEL (bg->store));
  gtk_widget_show (bg->treelist);

  /* Creating and connecting the scrollbar to list */
  bg->scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (bg->scroll),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (bg->scroll), GTK_SHADOW_ETCHED_IN);
  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (bg->scroll), bg->treelist);
  gtk_table_attach (GTK_TABLE (bg->table), bg->scroll, 0, 3, 2, 5, GTK_FILL,
                    GTK_FILL | GTK_EXPAND, 4, 0);
  gtk_widget_show (bg->scroll);

  /* Creating buttons to manage the elements of list */
  /* "Remove" Button */
  bg->remove = gtk_button_new ();
  bg->remove_i = gtk_image_new_from_stock (GTK_STOCK_REMOVE, GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (bg->remove_i);
  gtk_button_set_image (GTK_BUTTON (bg->remove), bg->remove_i);
  gtk_button_set_relief (GTK_BUTTON (bg->remove), GTK_RELIEF_NONE);
  gtk_table_attach (GTK_TABLE (bg->table), bg->remove, 0, 1, 5, 6, GTK_FILL, GTK_FILL, 5, 5);
  gtk_widget_show (bg->remove);

  /* "Up" Button */
  bg->up = gtk_button_new ();
  bg->up_i = gtk_image_new_from_stock (GTK_STOCK_GOTO_TOP, GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (bg->up_i);
  gtk_button_set_image (GTK_BUTTON (bg->up), bg->up_i);
  gtk_button_set_relief (GTK_BUTTON (bg->up), GTK_RELIEF_NONE);
  gtk_table_attach (GTK_TABLE (bg->table), bg->up, 1, 2, 5, 6, GTK_FILL, GTK_FILL, 5, 5);
  gtk_widget_show (bg->up);

  /* "Down" Button */
  bg->down = gtk_button_new ();
  bg->down_i = gtk_image_new_from_stock (GTK_STOCK_GOTO_BOTTOM, GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (bg->down_i);
  gtk_button_set_image (GTK_BUTTON (bg->down), bg->down_i);
  gtk_button_set_relief (GTK_BUTTON (bg->down), GTK_RELIEF_NONE);
  gtk_table_attach (GTK_TABLE (bg->table), bg->down, 2, 3, 5, 6, GTK_FILL, GTK_FILL, 5, 5);
  gtk_widget_show (bg->down);

  /* Row for the bookmark name */
  bg->l_name = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (bg->l_name), g_markup_printf_escaped (_("<b>Bookmark Name:</b>")));
  gtk_misc_set_alignment (GTK_MISC (bg->l_name), 0.0, 0.5);
  gtk_table_attach (GTK_TABLE (bg->table), bg->l_name, 3, 4, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (bg->l_name);

  bwidget->e_name = gtk_entry_new ();
  gtk_entry_set_max_length (GTK_ENTRY (bwidget->e_name), 30);
  gtk_table_attach (GTK_TABLE (bg->table), bwidget->e_name, 4, 5, 2, 3,
                    GTK_FILL | GTK_EXPAND, GTK_FILL, 10, 0);
  gtk_widget_show (bwidget->e_name);

  /* Row for the bookmark address */
  bg->l_addr = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (bg->l_addr), g_markup_printf_escaped (_("<b>Bookmark Address:</b>")));
  gtk_misc_set_alignment (GTK_MISC (bg->l_addr), 0.0, 0.5);
  gtk_table_attach (GTK_TABLE (bg->table), bg->l_addr, 3, 4, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (bg->l_addr);

  bwidget->e_addr = gtk_entry_new ();
  gtk_entry_set_max_length (GTK_ENTRY (bwidget->e_addr), 8);
  gtk_table_attach (GTK_TABLE (bg->table), bwidget->e_addr, 4, 5, 3, 4,
                    GTK_FILL | GTK_EXPAND, GTK_FILL, 10, 0);
  gtk_widget_show (bwidget->e_addr);

  /* Row for the bookmark tooltip */
  bg->l_tips = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (bg->l_tips),
                        g_markup_printf_escaped (_("<b>Bookmark Description:</b>")));
  gtk_misc_set_alignment (GTK_MISC (bg->l_tips), 0.0, 0.5);
  gtk_table_attach (GTK_TABLE (bg->table), bg->l_tips, 3, 4, 4, 5, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (bg->l_tips);

  bwidget->e_tips = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE (bg->table), bwidget->e_tips, 4, 5, 4, 5,
                    GTK_FILL | GTK_EXPAND, GTK_FILL, 10, 0);
  gtk_widget_show (bwidget->e_tips);

  /* Separator */
  bg->sep = gtk_hseparator_new ();
  gtk_table_attach (GTK_TABLE (bg->table), bg->sep, 0, 5, 6, 7, GTK_FILL, GTK_FILL, 10, 0);
  gtk_widget_show (bg->sep);

  /* Horizontal Box */
  bg->hbox = gtk_hbox_new (FALSE, 0);
  gtk_table_attach_defaults (GTK_TABLE (bg->table), bg->hbox, 0, 5, 7, 8);
  gtk_widget_show (bg->hbox);

  /* Button Box */
  bg->box = gtk_hbutton_box_new ();
  gtk_container_set_border_width (GTK_CONTAINER (bg->box), 2);
  gtk_box_set_spacing (GTK_BOX (bg->box), 5);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (bg->box), GTK_BUTTONBOX_END);
  gtk_container_add (GTK_CONTAINER (bg->hbox), bg->box);
  gtk_widget_show (bg->box);

  /* Close Button */
  bg->close = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  GTK_WIDGET_SET_FLAGS (bg->close, GTK_CAN_DEFAULT);
  gtk_widget_grab_default (bg->close);
  gtk_container_add (GTK_CONTAINER (bg->box), bg->close);
  gtk_widget_show (bg->close);

  /* Apply Button */
  bg->apply = gtk_button_new_from_stock (GTK_STOCK_APPLY);
  gtk_container_add (GTK_CONTAINER (bg->box), bg->apply);
  gtk_widget_show (bg->apply);

  /* Ok Button */
  bg->ok = gtk_button_new_from_stock (GTK_STOCK_OK);
  gtk_container_add (GTK_CONTAINER (bg->box), bg->ok);
  gtk_widget_show (bg->ok);

  /* List selection */
  bg->selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (bg->treelist));

  /* Disabling unusable buttons */
  jeex_widget_set_sensitive (FALSE, bg->ok, bg->apply, bwidget->e_tips,
                             bwidget->e_addr, bwidget->e_name, bg->down, bg->up, bg->remove, NULL);

  /* Signals */
  g_signal_connect (G_OBJECT (bg->close), "clicked", G_CALLBACK (destroy_this), (void *) bg->win);

  g_signal_connect (G_OBJECT (bg->selection), "changed", G_CALLBACK (selected_bookmark), NULL);

  g_signal_connect (G_OBJECT (bg->remove), "clicked", G_CALLBACK (remove_bookmark), NULL);

  g_signal_connect (G_OBJECT (bg->up), "clicked", G_CALLBACK (move_bookmark_up), NULL);

  g_signal_connect (G_OBJECT (bg->down), "clicked", G_CALLBACK (move_bookmark_down), NULL);

  g_signal_connect (G_OBJECT (bg->apply), "clicked", G_CALLBACK (apply_bookmark_changes), NULL);

  g_signal_connect (G_OBJECT (bg->ok), "clicked",
                    G_CALLBACK (apply_and_close_manage_bookmarks_window), NULL);

  return NULL;
}

/* Gets current bookmark position in the list.
 * If not found or if any error occurs returns -1.
 */
static int
bookmarks_get_number_from_string (char *s)
{
  int i;

  g_return_val_if_fail (s, -1);

  for (i = 0; i < bookmark_elem (n); ++i)
    {
      if (!strcmp (bookmark_elem (str[i]), s))
        {
          return i;
        }
    }

  return -1;
}

/* It handles the selected bookmark  */
static void *
selected_bookmark (GtkWidget * widget, void *data)
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  char *value;
  int n;

  /* Gets the current bookmark number in bookmark list */
  if (gtk_tree_selection_get_selected (GTK_TREE_SELECTION (widget), &model, &iter))
    {
      gtk_tree_model_get (model, &iter, 0, &value, -1);
      n = bookmarks_get_number_from_string (value);
      if (n == -1)
        {
          _error (_("<big><b>Bookmark not found</b></big>\n\n"
                    "Selected bookmark wasn't found, probably this is a <b>bug</b>.\n"
                    "Please report at: <i>hds619@gmail.com</i>."));
          g_free (value);

          return NULL;
        }
      else
        {
          current_bookmark = n;
        }
      g_free (value);
    }
  else
    {
      jeex_widget_set_sensitive (FALSE, bg->ok, bg->apply, bwidget->e_tips,
                                 bwidget->e_addr, bwidget->e_name, bg->down, bg->up, bg->remove, NULL);
      return NULL;
    }

  /* Sets bookmarks field */
  gtk_entry_set_text (GTK_ENTRY (bwidget->e_name), (const char *) bookmark_elem (str[n]));

  value = g_strdup_printf ("%8.8X", 0x10 * bookmark_elem (addr[n]));
  gtk_entry_set_text (GTK_ENTRY (bwidget->e_addr), (const char *) value);
  g_free (value);

  if (bookmark_elem (tips[n]))
    {
      gtk_entry_set_text (GTK_ENTRY (bwidget->e_tips), (const char *) bookmark_elem (tips[n]));
    }
  else
    {
      gtk_entry_set_text (GTK_ENTRY (bwidget->e_tips), "\0");
    }

  /* Active text fields and buttons to confirm and implement 
   * any changes.
   */
  gtk_widget_set_sensitive (bg->ok, TRUE);
  gtk_widget_set_sensitive (bg->apply, TRUE);
  gtk_widget_set_sensitive (bg->remove, TRUE);
  gtk_widget_set_sensitive (bwidget->e_tips, TRUE);
  gtk_widget_set_sensitive (bwidget->e_addr, TRUE);
  gtk_widget_set_sensitive (bwidget->e_name, TRUE);

  if (n > 0)
    {
      gtk_widget_set_sensitive (bg->up, TRUE);
    }
  else
    {
      gtk_widget_set_sensitive (bg->up, FALSE);
    }

  if (n < (bookmark_elem (n) - 1))
    {
      gtk_widget_set_sensitive (bg->down, TRUE);
    }
  else
    {
      gtk_widget_set_sensitive (bg->down, FALSE);
    }

  return NULL;
}

/* Function to remove a bookmark */
static void *
remove_bookmark (GtkWidget * widget, void *data)
{
  int i = current_bookmark;
  GtkTreeIter iter;
  GtkTreeModel *model;

  /* Removing the current element, climbing the other */
  delete_bookmarks ();
  for (; i < (bookmark_elem (n) - 1); ++i)
    {
      bookmark_elem (addr[i]) = bookmark_elem (addr[i + 1]);
      bookmark_elem (str[i]) = g_strdup (bookmark_elem (str[i + 1]));
      if (!(bookmark_elem (tips[i + 1])))
        {
          bookmark_elem (tips[i]) = NULL;
        }
      else
        {
          bookmark_elem (tips[i]) = g_strdup (bookmark_elem (tips[i + 1]));
        }
    }
  bookmark_elem (n)--;

  /* Update bookmarks list */
  if (!(bookmark_elem (n)))
    {
      gtk_widget_destroy (bg->win);
      g_free (bg);
      _error (_("<big><b>Unable to organize bookmarks</b></big>\n" "\n"
                "Cannot continue to organize bookmarks because\n" "there are no more bookmarks."));
      load_bookmarks ();
      return NULL;
    }

  gtk_tree_selection_get_selected (GTK_TREE_SELECTION (bg->selection), &model, &iter);
  gtk_list_store_remove (GTK_LIST_STORE (bg->store), &iter);

  load_bookmarks ();

  return NULL;
}


/* Moves current bookmark in previous position. */
static void *
move_bookmark_up (GtkWidget * widget, void *data)
{
  char *str, *tips;
  int i = current_bookmark, addr;
  GtkTreeModel *model;
  GtkTreeIter iter;

  /* Memorizes data from the previous bookmark in 
   * temporary variables for the swap
   */
  addr = bookmark_elem (addr[i - 1]);
  str = g_strdup (bookmark_elem (str[i - 1]));
  if (!(bookmark_elem (tips[i - 1])))
    {
      tips = NULL;
    }
  else
    {
      tips = g_strdup (bookmark_elem (tips[i - 1]));
    }

  /* Free previous bookmark memory.. */
  g_free (bookmark_elem (str[i - 1]));
  if (bookmark_elem (tips[i - 1]))
    {
      g_free (bookmark_elem (tips[i - 1]));
    }

  /* ..and assign it the current data */
  bookmark_elem (str[i - 1]) = g_strdup (bookmark_elem (str[i]));
  bookmark_elem (addr[i - 1]) = bookmark_elem (addr[i]);
  if (!(bookmark_elem (tips[i])))
    {
      bookmark_elem (tips[i - 1]) = NULL;
    }
  else
    {
      bookmark_elem (tips[i - 1]) = g_strdup (bookmark_elem (tips[i]));
    }

  /* Free current bookmark memory.. */
  g_free (bookmark_elem (str[i]));
  if (bookmark_elem (tips[i]))
    {
      g_free (bookmark_elem (tips[i]));
    }

  /* ..and assign it the previous bookmark data */
  bookmark_elem (str[i]) = g_strdup (str);
  bookmark_elem (addr[i]) = addr;
  if (tips)
    {
      bookmark_elem (tips[i]) = NULL;
    }
  else
    {
      bookmark_elem (tips[i]) = g_strdup (tips);
    }

  /* Free temporany variables memory used for the swap */
  g_free (str);
  if (tips)
    {
      g_free (tips);
    }

  /* Reorder list elements */
  gtk_tree_selection_get_selected (GTK_TREE_SELECTION (bg->selection), &model, &iter);

  /* Remove elements from list */
  gtk_tree_model_get_iter_first (model, &iter);
  while (gtk_list_store_remove (GTK_LIST_STORE (bg->store), &iter))
    {
      if (!(gtk_tree_model_get_iter_first (model, &iter)))
        {
          break;
        }
    }

  /* Inserts new elements */
  for (i = 0; i < bookmark_elem (n); ++i)
    {
      gtk_list_store_append (GTK_LIST_STORE (bg->store), &(bg->iter));
      gtk_list_store_set (GTK_LIST_STORE (bg->store), &(bg->iter), 0, bookmark_elem (str[i]), -1);
    }

  /* Reload menu bookmarks */
  reload_bookmarks ();

  return NULL;
}

static void *
move_bookmark_down (GtkWidget * widget, void *data)
{
  char *str, *tips;
  int i = current_bookmark, addr;
  GtkTreeModel *model;
  GtkTreeIter iter;

  /* Memorizes data from the previous bookmark in 
   * temporary variables for the swap
   */
  addr = bookmark_elem (addr[i + 1]);
  str = g_strdup (bookmark_elem (str[i + 1]));
  if (!(bookmark_elem (tips[i + 1])))
    {
      tips = NULL;
    }
  else
    {
      tips = g_strdup (bookmark_elem (tips[i + 1]));
    }

  /* Free previous bookmark memory.. */
  g_free (bookmark_elem (str[i + 1]));
  if (bookmark_elem (tips[i + 1]))
    {
      g_free (bookmark_elem (tips[i + 1]));
    }

  /* ..and assign it the current data */
  bookmark_elem (str[i + 1]) = g_strdup (bookmark_elem (str[i]));
  bookmark_elem (addr[i + 1]) = bookmark_elem (addr[i]);
  if (!(bookmark_elem (tips[i])))
    {
      bookmark_elem (tips[i + 1]) = NULL;
    }
  else
    {
      bookmark_elem (tips[i + 1]) = g_strdup (bookmark_elem (tips[i]));
    }

  /* Free current bookmark memory.. */
  g_free (bookmark_elem (str[i]));
  if (bookmark_elem (tips[i]))
    {
      g_free (bookmark_elem (tips[i]));
    }

  /* ..and assign it the previous bookmark data */
  bookmark_elem (str[i]) = g_strdup (str);
  bookmark_elem (addr[i]) = addr;
  if (tips)
    {
      bookmark_elem (tips[i]) = NULL;
    }
  else
    {
      bookmark_elem (tips[i]) = g_strdup (tips);
    }

  /* Free temporany variables memory used for the swap */
  g_free (str);
  if (tips)
    {
      g_free (tips);
    }

  /* Reorder list elements */
  gtk_tree_selection_get_selected (GTK_TREE_SELECTION (bg->selection), &model, &iter);

  /* Remove elements from list */
  gtk_tree_model_get_iter_first (model, &iter);
  while (gtk_list_store_remove (GTK_LIST_STORE (bg->store), &iter))
    {
      if (!(gtk_tree_model_get_iter_first (model, &iter)))
        {
          break;
        }
    }

  /* Inserts new elements */
  for (i = 0; i < bookmark_elem (n); ++i)
    {
      gtk_list_store_append (GTK_LIST_STORE (bg->store), &(bg->iter));
      gtk_list_store_set (GTK_LIST_STORE (bg->store), &(bg->iter), 0, bookmark_elem (str[i]), -1);
    }

  /* Reload menu bookmarks */
  reload_bookmarks ();

  return NULL;
}

/* Apply changes to the bookmark currently selected */
static void *
apply_bookmark_changes (GtkWidget * widget, void *data)
{
  char *str, *tips, *addr;
  int i, c, len, addr_d, res;
  GtkTreeModel *model;
  GtkTreeIter iter;

  str = (char *) gtk_entry_get_text (GTK_ENTRY (bwidget->e_name));
  tips = (char *) gtk_entry_get_text (GTK_ENTRY (bwidget->e_tips));
  addr = (char *) gtk_entry_get_text (GTK_ENTRY (bwidget->e_addr));

  if (*str == '\0' || *addr == '\0' || strisnull (str))
    {
      _error (_("<big><b>Unable to add bookmark</b></big>\n\n"
                "It wasn't possible to add the bookmark because\n"
                "the fields \"Name\" and/or \"Address\" were empty."));
      return NULL;
    }

  addr = g_utf8_normalize (addr, -1, G_NORMALIZE_DEFAULT);
  len = strlen (addr);

  /* Checking if addr is empty */
  if (strisnull (addr))
    {
      _error (_("<big><b>Unable to add bookmark</b></big>\n\n" "The entered address is empty.\n"));
      return NULL;
    }

  addr = g_ascii_strup (addr, -1);

  for (i = 0; i < len; ++i)
    {
      if (!g_ascii_isxdigit (addr[i]))
        {
          _error (_("<big><b>Unable to add bookmark</b></big>\n\n" "The entered address is not valid."));
          return NULL;
        }
    }

  /* Converting from hexadecimal to decimal */
  addr_d = 0;
  for (i = 0; i < len; ++i)
    {
      if (addr[i] >= 0x30 && addr[i] <= 0x39)
        {
          c = addr[i] - 0x30;
        }
      else
        {
          c = addr[i] - 0x37;
        }
      addr_d += c * pow (16, (len - 1) - i);
    }
  addr_d /= 16;

  /* Control that there isn't another similar bookmark */
  for (i = 0; i < bookmark_elem (n); ++i)
    {
      if (i == current_bookmark)
        {
          continue;
        }

      if (!strcmp (str, bookmark_elem (str[i])))
        {
          _error (_("<big><b>Unable to add bookmark</b></big>\n\n"
                    "The name chosen for this bookmark is already used\n" "by another."));
          return NULL;
        }

      if (bookmark_elem (addr[i]) == addr_d)
        {
          res = _info (_("Another bookmark was added with the same address.\n\n" "Modify it?"), TRUE);

          if (res)
            {
              break;
            }
          else
            {
              return NULL;
            }
        }
    }

  i = current_bookmark;

  /* Changing the bookmark */
  bookmark_elem (addr[i]) = addr_d;
  bookmark_elem (str[i]) = g_strdup (str);
  bookmark_elem (tips[i]) = (*tips == '\0') ? NULL : g_strdup (tips);

  /* Reorders elements in the list */
  gtk_tree_selection_get_selected (GTK_TREE_SELECTION (bg->selection), &model, &iter);

  /* Removing elements from list */
  gtk_tree_model_get_iter_first (model, &iter);
  while (gtk_list_store_remove (GTK_LIST_STORE (bg->store), &iter))
    {
      if (!(gtk_tree_model_get_iter_first (model, &iter)))
        {
          break;
        }
    }

  /* Inserts a new elements */
  for (i = 0; i < bookmark_elem (n); ++i)
    {
      gtk_list_store_append (GTK_LIST_STORE (bg->store), &(bg->iter));
      gtk_list_store_set (GTK_LIST_STORE (bg->store), &(bg->iter), 0, bookmark_elem (str[i]), -1);
    }

  /* Reloads menu bookmarks */
  reload_bookmarks ();

  return NULL;
}

/* Apply the changes made to the currently selected bookmark
 * and close window to manage bookmarks.
 */
static void *
apply_and_close_manage_bookmarks_window (GtkWidget * widget, void *data)
{
  apply_bookmark_changes (NULL, NULL);
  gtk_widget_destroy (bg->win);

  return NULL;
}

/* Position scollbar on offset choosen */
static void
position_scrollbar (int off)
{
  GtkTextIter iter;
  GtkTextMark *mark;

  /* Positioning on offset chosen */
  gtk_text_buffer_get_iter_at_mark (textview->buffer, &iter,
                                    gtk_text_buffer_get_insert (textview->buffer));
  gtk_text_iter_set_line (&iter, off);
  mark = gtk_text_buffer_create_mark (textview->buffer, "mark_offset", &iter, FALSE);

  if (GTK_IS_TEXT_MARK (mark))
    {
      gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (textview->textview), mark);
    }
}

/* Compare two bookmarks address */
static int
bcompare (const void *a, const void *b)
{
  if (*((const int *) a) < *((const int *) b))
    {
      return -1;
    }
  if (*((const int *) a) > *((const int *) b))
    {
      return 1;
    }

  return 0;
}

/* Go to the next ( if second parameters is 0 ) or
 * previous ( if second parameters is 1 ) bookmark.
 */
static void
np_bookmark (int n)
{
  int i, c, *ord;
  GtkTextMark *mark;
  GtkTextIter iter;

  ord = (int *) g_malloc (bookmark_elem (n) * sizeof (int));

  for (i = 0; i < bookmark_elem (n); ++i)
    {
      ord[i] = bookmark_elem (addr[i]);
    }

  qsort (ord, i, sizeof (int), bcompare);

  mark = gtk_text_buffer_get_insert (_ncurrent (notebook->buffer));
  gtk_text_buffer_get_iter_at_mark (textview->buffer, &iter, mark);
  c = gtk_text_iter_get_line (&iter) + 1;

  if (!n)
    {
      for (i = 0; i < bookmark_elem (n); i++)
        {
          if (ord[i] > c)
            {
              position_scrollbar (ord[i]);
              break;
            }
        }
    }
  else
    {
      for (i = bookmark_elem (n) - 1; i > -1; i--)
        {
          if (ord[i] < c)
            {
              position_scrollbar (ord[i]);
              break;
            }
        }
    }

  g_free (ord);
}

/* Go to the next bookmark */
void *
n_bookmark (GtkWidget * widget, void *data)
{
  np_bookmark (0);
  return NULL;
}

/* Go to the previous bookmark */
void *
p_bookmark (GtkWidget * widget, void *data)
{
  np_bookmark (1);
  return NULL;
}
