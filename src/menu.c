/*
 * "menu.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <libintl.h>
#include <locale.h>
#include "header.h"

JeexMenu *jeex_menu;

GtkWidget *
jeex_menu_separator_append (GtkMenu * menu)
{
  GtkWidget *widget;

  widget = gtk_separator_menu_item_new ();
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), widget);
  gtk_widget_show (widget);

  return widget;
}

static GtkWidget *
jeex_menu_check_append (GtkMenu * menu, gboolean status, char *plugin_name)
{
 GtkWidget *widget;

 widget = gtk_check_menu_item_new_with_label (plugin_name);
 gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (widget), status);
 gtk_menu_shell_append (GTK_MENU_SHELL (menu), widget);
 gtk_widget_show (widget);

 g_signal_connect (G_OBJECT (widget), "toggled",
                   G_CALLBACK (plugin_load_unload), plugin_name);

 return widget;
}

GtkWidget *
jeex_menu_new (void)
{
  GtkAccelGroup *group;
  extern GtkWidget *jeex_main_window;
  GtkWidget *file_menu, *view_menu, *edit_menu;
  GtkWidget *book_menu, *info_menu, *search_menu;
  GtkWidget *tools_menu;

  jeex_menu = (JeexMenu *) g_malloc (sizeof (JeexMenu));
  jeex_menu->barra = gtk_menu_bar_new ();

  /* Acceleration group */
  group = gtk_accel_group_new ();
  gtk_window_add_accel_group (GTK_WINDOW (jeex_main_window), group);

  /* Making File Menu */
  jeex_menu->file_menu.file = gtk_menu_new ();
  file_menu = jeex_menu->file_menu.file;

  jeex_menu->file_menu.new =
    jeex_menu_stock_append (GTK_MENU (file_menu), GTK_STOCK_NEW, G_CALLBACK (new_file), group,
                            GDK_N, GDK_CONTROL_MASK, _("Make a new file"));
  jeex_menu->file_menu.open =
    jeex_menu_stock_append (GTK_MENU (file_menu), GTK_STOCK_OPEN, G_CALLBACK (open_file), group,
                            GDK_A, GDK_CONTROL_MASK, _("Open a file"));
  jeex_menu->file_menu.open_position =
    jeex_menu_label_with_stock_append (GTK_MENU (file_menu), _("Open position.."), GTK_STOCK_OPEN,
                                       G_CALLBACK (open_position), group, GDK_L, GDK_CONTROL_MASK,
                                       _("Open all files of the selected folder."));
  jeex_menu->file_menu.append =
    jeex_menu_label_with_stock_append (GTK_MENU (file_menu), _("Append File"), GTK_STOCK_OPEN,
                                       G_CALLBACK (file_append), group, GDK_I, GDK_MOD1_MASK,
                                       _("Append the selected file at end of current file."));

  jeex_menu_separator_append (GTK_MENU (file_menu));

  jeex_menu->file_menu.save =
    jeex_menu_stock_append (GTK_MENU (file_menu), GTK_STOCK_SAVE, G_CALLBACK (save_file), group,
                            GDK_S, GDK_CONTROL_MASK, _("Save the current file"));
  jeex_menu->file_menu.save_as =
    jeex_menu_stock_append (GTK_MENU (file_menu), GTK_STOCK_SAVE_AS, G_CALLBACK (save_file_as),
                            group, GDK_S, GDK_CONTROL_MASK | GDK_SHIFT_MASK,
                            _("Save the current file as..."));
  jeex_menu->file_menu.save_html =
    jeex_menu_label_with_stock_append (GTK_MENU (file_menu), _("Export to HTML"), GTK_STOCK_EDIT,
                                       G_CALLBACK (html_export_window), group, GDK_S,
                                       GDK_SHIFT_MASK | GDK_MOD1_MASK,
                                       _("Export the current file in HTML"));

  jeex_menu_separator_append (GTK_MENU (file_menu));

  jeex_menu->file_menu.remove =
    jeex_menu_stock_append (GTK_MENU (file_menu), GTK_STOCK_DELETE, G_CALLBACK (remove_open_file),
                            group, GDK_R, GDK_CONTROL_MASK | GDK_SHIFT_MASK,
                            _("Remove the current file from hard disk"));
  jeex_menu->file_menu.info =
    jeex_menu_stock_append (GTK_MENU (file_menu), GTK_STOCK_PROPERTIES, G_CALLBACK (properties),
                            group, GDK_P, GDK_CONTROL_MASK, _("View file's properties"));
  jeex_menu->file_menu.close =
    jeex_menu_stock_append (GTK_MENU (file_menu), GTK_STOCK_CLOSE, G_CALLBACK (close_file),
                            group, GDK_W, GDK_CONTROL_MASK, _("Close the current file"));

  jeex_menu_separator_append (GTK_MENU (file_menu));

  jeex_menu->file_menu.restart =
    jeex_menu_label_with_stock_append (GTK_MENU (file_menu), _("Restart Jeex"), GTK_STOCK_REDO,
                                       G_CALLBACK (_restart), group, GDK_R,
                                       GDK_MOD1_MASK | GDK_CONTROL_MASK,
                                       _("Restart Jeex\n" "( Unsaved changes will be lost )"));
  jeex_menu->file_menu.quit =
    jeex_menu_stock_append (GTK_MENU (file_menu), GTK_STOCK_QUIT, G_CALLBACK (secure_quit), group, GDK_Q,
                            GDK_CONTROL_MASK, NULL);

  jeex_menu->file = jeex_menu_append (GTK_MENU_BAR (jeex_menu->barra), file_menu, _("File"), NULL);

  /* Making Edit Menu */
  jeex_menu->edit_menu.edit = gtk_menu_new ();
  edit_menu = jeex_menu->edit_menu.edit;

  jeex_menu->edit_menu.copy =
    jeex_menu_stock_append (GTK_MENU (edit_menu), GTK_STOCK_COPY, G_CALLBACK (copy), group, GDK_C,
                            GDK_CONTROL_MASK, _("Copy the selected text"));
  jeex_menu->edit_menu.cut =
    jeex_menu_stock_append (GTK_MENU (edit_menu), GTK_STOCK_CUT, G_CALLBACK (cut), group, GDK_X,
                            GDK_CONTROL_MASK, _("Cut the selected text"));
  jeex_menu->edit_menu.paste =
    jeex_menu_stock_append (GTK_MENU (edit_menu), GTK_STOCK_PASTE, G_CALLBACK (paste), group, GDK_V,
                            GDK_CONTROL_MASK, _("Paste the cutted or copied text"));
  jeex_menu->edit_menu.delete =
    jeex_menu_stock_append (GTK_MENU (edit_menu), GTK_STOCK_DELETE, G_CALLBACK (delete), group, GDK_D,
                            GDK_CONTROL_MASK, _("Delete the selected text"));
  jeex_menu->edit_menu.insert =
    jeex_menu_label_with_stock_append (GTK_MENU (edit_menu), _("Insert"), GTK_STOCK_EDIT,
                                       G_CALLBACK (insert_a), group, GDK_I, GDK_CONTROL_MASK,
                                       _("Insert a binary, hexadecimal, ascii\n"
                                         "or octal text into current file."));
  jeex_menu->edit_menu.insert_from_bit =
    jeex_menu_label_with_stock_append (GTK_MENU (edit_menu), _("Insert from bit operation"),
                                       GTK_STOCK_EDIT, G_CALLBACK (insert_from_bits_show_dialog), group,
                                       GDK_I, GDK_CONTROL_MASK | GDK_MOD1_MASK | GDK_SHIFT_MASK,
                                       _("Insert a byte result from your operation with bits"));
  jeex_menu->edit_menu.revert =
    jeex_menu_label_with_stock_append (GTK_MENU (edit_menu), _("Reverse"), GTK_STOCK_REVERT_TO_SAVED,
                                       G_CALLBACK (reverse_byte), group, GDK_B, GDK_CONTROL_MASK,
                                       _("Reverse the selected text"));

  jeex_menu_separator_append (GTK_MENU (edit_menu));

  jeex_menu->edit_menu.select_all =
    jeex_menu_stock_append (GTK_MENU (edit_menu), GTK_STOCK_SELECT_ALL, G_CALLBACK (select_all),
                            group, GDK_A, GDK_MOD1_MASK | GDK_CONTROL_MASK, NULL);

  jeex_menu_separator_append (GTK_MENU (edit_menu));

  jeex_menu->edit_menu.update =
    jeex_menu_stock_append (GTK_MENU (edit_menu), GTK_STOCK_REFRESH, G_CALLBACK (_update), group,
                            GDK_U, GDK_CONTROL_MASK, NULL);
  jeex_menu->edit_menu.reset =
    jeex_menu_stock_append (GTK_MENU (edit_menu), GTK_STOCK_CLEAR, G_CALLBACK (reset), group, GDK_C,
                            GDK_SHIFT_MASK | GDK_CONTROL_MASK,
                            _("Delete all the contents of the file.\nThe original file can be "
                              "restored by\nclicking on the refresh button."));

  jeex_menu_separator_append (GTK_MENU (edit_menu));

  jeex_menu->edit_menu.preferences =
    jeex_menu_stock_append (GTK_MENU (edit_menu), GTK_STOCK_PREFERENCES,
                            G_CALLBACK (preferences_window), group, GDK_P, GDK_SHIFT_MASK,
                            _("Change jeex settings"));

  jeex_menu->edit = jeex_menu_append (GTK_MENU_BAR (jeex_menu->barra), edit_menu, _("Edit"), NULL);

  /* Making View Menu */
  jeex_menu->view_menu.view = gtk_menu_new ();
  view_menu = jeex_menu->view_menu.view;

  jeex_menu->view_menu.add_view =
    jeex_menu_label_with_stock_append (GTK_MENU (view_menu), _("Add View"),
                                       GTK_STOCK_ZOOM_IN, G_CALLBACK (add_view),
                                       group, GDK_A, GDK_SHIFT_MASK | GDK_CONTROL_MASK, NULL);
  jeex_menu->view_menu.hash =
    jeex_menu_label_with_stock_append (GTK_MENU (view_menu), _("Show Hash"),
                                       GTK_STOCK_DIALOG_AUTHENTICATION, G_CALLBACK (md5_file),
                                       group, GDK_H, GDK_CONTROL_MASK,
                                       _("View MD5, SHA-1 and SHA-256 hashes\n" "of the current file"));
  jeex_menu->view_menu.stats =
    jeex_menu_label_with_stock_append (GTK_MENU (view_menu), _("Document Statistics"),
                                       GTK_STOCK_JUSTIFY_LEFT, G_CALLBACK (stats), group, GDK_D,
                                       GDK_SHIFT_MASK, NULL);
  jeex_menu->view_menu.difference =
    jeex_menu_label_with_stock_append (GTK_MENU (view_menu), _("Difference with another file"),
                                       GTK_STOCK_UNINDENT, G_CALLBACK (file_difference), group,
                                       GDK_D, GDK_SHIFT_MASK | GDK_CONTROL_MASK,
                                       _("View the current file differences\n"
                                         "with another chosen file"));
  jeex_menu->view_menu.file_in_other_format =
    jeex_menu_label_with_stock_append (GTK_MENU (view_menu), _("Show in other format"),
                                       GTK_STOCK_ZOOM_FIT, G_CALLBACK (show_in_other_format),
                                       group, GDK_F, GDK_CONTROL_MASK | GDK_SHIFT_MASK,
                                       _("View the current file in other format, example:\n"
                                         "ascii, octal, decimal, etc.."));

  jeex_menu_separator_append (GTK_MENU (view_menu));

  jeex_menu->view_menu.chars_table =
    jeex_menu_label_with_stock_append (GTK_MENU (view_menu), _("Characters Table"),
                                       GTK_STOCK_ITALIC, G_CALLBACK (chars_table), group,
                                       GDK_C, GDK_MOD1_MASK,
                                       _("View the characters table converted in binary,\n"
                                         "hexadecimal, octal and decimal."));

  jeex_menu->view = jeex_menu_append (GTK_MENU_BAR (jeex_menu->barra), view_menu, _("View"), NULL);

  /* Making Search Menu */
  jeex_menu->search_menu.search_m = gtk_menu_new ();
  search_menu = jeex_menu->search_menu.search_m;

  jeex_menu->search_menu.search =
    jeex_menu_stock_append (GTK_MENU (search_menu), GTK_STOCK_FIND, G_CALLBACK (find_text), group,
                            GDK_F, GDK_CONTROL_MASK, _("Search bytes in the current file"));
  jeex_menu->search_menu.replace =
    jeex_menu_stock_append (GTK_MENU (search_menu), GTK_STOCK_FIND_AND_REPLACE,
                            G_CALLBACK (replace_text), group, GDK_F, GDK_CONTROL_MASK |
                            GDK_MOD1_MASK, _("Search and replace bytes in the current file"));
  jeex_menu->search_menu.regex =
    jeex_menu_label_with_stock_append (GTK_MENU (search_menu), _("Search byte with regex"),
                                       GTK_STOCK_SORT_ASCENDING, G_CALLBACK (regex_show_bar), group,
                                       GDK_X, GDK_SHIFT_MASK, NULL);

  jeex_menu_separator_append (GTK_MENU (search_menu));

  jeex_menu->search_menu.reset_high =
    jeex_menu_label_with_stock_append (GTK_MENU (search_menu), _("Clear Highlight"), GTK_STOCK_INDEX,
                                       G_CALLBACK (reset_tag), group, GDK_K, GDK_CONTROL_MASK, NULL);

  jeex_menu_separator_append (GTK_MENU (search_menu));

  jeex_menu->search_menu.go_offset =
    jeex_menu_label_with_stock_append (GTK_MENU (search_menu), _("Go to offset"), GTK_STOCK_JUMP_TO,
                                       G_CALLBACK (jump_to_offset_gui), group, GDK_O,
                                       GDK_SHIFT_MASK, _("Moves the scrollbar into chosen offset."));

  jeex_menu->search = jeex_menu_append (GTK_MENU_BAR (jeex_menu->barra), search_menu, _("Search"), NULL);

  /* Making Bookmark Menu */
  jeex_menu->bookmark_menu.bookmark = gtk_menu_new ();
  book_menu = jeex_menu->bookmark_menu.bookmark;

  jeex_menu->bookmark_menu.manage =
    jeex_menu_label_with_stock_append (GTK_MENU (book_menu), _("Manage Bookmarks"),
                                       GTK_STOCK_EXECUTE, G_CALLBACK (manage_bookmarks_gui), group,
                                       GDK_B, GDK_SHIFT_MASK, _("Manage the bookmarks."));
  jeex_menu->bookmark_menu.add =
    jeex_menu_label_with_stock_append (GTK_MENU (book_menu), _("Add bookmark"), GTK_STOCK_ADD,
                                       G_CALLBACK (add_bookmark_gui), group, GDK_B, GDK_MOD1_MASK,
                                       _("Add a bookmark."));

  jeex_menu_separator_append (GTK_MENU (book_menu));

  jeex_menu->bookmark_menu.sep = jeex_menu_separator_append (GTK_MENU (book_menu));
  gtk_widget_hide (jeex_menu->bookmark_menu.sep);

  jeex_menu->bookmark_menu.next =
    jeex_menu_label_with_stock_append (GTK_MENU (book_menu), _("Next bookmark"),
                                       GTK_STOCK_GO_FORWARD, G_CALLBACK (n_bookmark), group, GDK_N,
                                       GDK_MOD1_MASK, _("Go to the next bookmark."));

  jeex_menu->bookmark_menu.prev =
    jeex_menu_label_with_stock_append (GTK_MENU (book_menu), _("Previous bookmark"),
                                       GTK_STOCK_GO_BACK, G_CALLBACK (p_bookmark), group, GDK_P,
                                       GDK_MOD1_MASK, _("Go to the previous bookmark."));

  jeex_menu->bookmark =
    jeex_menu_append (GTK_MENU_BAR (jeex_menu->barra), book_menu, _("Bookmarks"), NULL);

  /* Making Tools Menu */
  jeex_menu->tools_menu.tools = gtk_menu_new ();
  tools_menu = jeex_menu->tools_menu.tools;


  jeex_menu->tools_menu.strings =
    jeex_menu_label_with_stock_append (GTK_MENU (tools_menu), _("Extracts all Strings"),
                                       GTK_STOCK_INDENT, G_CALLBACK (strings_extract), group, GDK_E,
                                       GDK_CONTROL_MASK, _("Extract all strings in the current file"));
  jeex_menu->tools_menu.split =
    jeex_menu_label_with_stock_append (GTK_MENU (tools_menu), _("Split the current file"),
                                       GTK_STOCK_DND_MULTIPLE, G_CALLBACK (split_window_file),
                                       group, GDK_S, GDK_SHIFT_MASK | GDK_MOD1_MASK | GDK_CONTROL_MASK,
                                       _("Split the current file in files with fix size"));
  jeex_menu->tools_menu.file_structures =
    jeex_menu_label_with_stock_append (GTK_MENU (tools_menu), _("Show file's structures"),
                                       GTK_STOCK_UNINDENT, G_CALLBACK (show_file_structures), group,
                                       GDK_E, GDK_SHIFT_MASK | GDK_CONTROL_MASK,
                                       _("Show the file's data in personalized structures"));

  jeex_menu->tools = jeex_menu_append (GTK_MENU_BAR (jeex_menu->barra), tools_menu, _("Tools"), NULL);

  jeex_widget_set_sensitive (FALSE, jeex_menu->file_menu.close, jeex_menu->file_menu.save,
                             jeex_menu->file_menu.save_as, jeex_menu->file_menu.save_html,
                             jeex_menu->file_menu.append, jeex_menu->view_menu.file_in_other_format,
                             jeex_menu->file_menu.remove, jeex_menu->file_menu.info,
                             jeex_menu->edit_menu.copy, jeex_menu->edit_menu.cut,
                             jeex_menu->edit_menu.paste, jeex_menu->edit_menu.delete,
                             jeex_menu->edit_menu.select_all, jeex_menu->edit_menu.update,
                             jeex_menu->edit_menu.reset, jeex_menu->edit_menu.revert,
                             jeex_menu->search_menu.replace, jeex_menu->edit_menu.insert,
                             jeex_menu->edit_menu.insert_from_bit, jeex_menu->search_menu.regex,
                             jeex_menu->search_menu.go_offset, jeex_menu->search_menu.reset_high,
                             jeex_menu->search_menu.search, jeex_menu->view_menu.hash,
                             jeex_menu->tools_menu.strings, jeex_menu->view_menu.stats,
                             jeex_menu->bookmark_menu.manage, jeex_menu->bookmark_menu.add,
                             jeex_menu->bookmark_menu.next, jeex_menu->bookmark_menu.prev,
                             jeex_menu->view_menu.difference, jeex_menu->tools_menu.split,
                             jeex_menu->tools_menu.file_structures, jeex_menu->view_menu.add_view, NULL);

  return jeex_menu->barra;
}

void plugin_menu_make (gboolean *activated, int length_activated)
{
 extern PluginList plugin_list[];
 extern GtkWidget *jeex_main_window;
 GtkAccelGroup *group;
 GtkWidget *plugin_menu, *info_menu;
 int i;

 if ( length_activated < 2 )
      return;

 plugin_menu = gtk_menu_new ();

 for ( i = 0; i < (length_activated - 1); i++ )
       plugin_list[i].menu_widget =
          jeex_menu_check_append (GTK_MENU (plugin_menu), activated[i], plugin_list[i].plugin_name);

 jeex_menu_append (GTK_MENU_BAR (jeex_menu->barra), plugin_menu, "Plugins", NULL);

 group = gtk_accel_group_new ();
 gtk_window_add_accel_group (GTK_WINDOW (jeex_main_window), group);

 jeex_menu->info_menu.info = gtk_menu_new ();
 info_menu = jeex_menu->info_menu.info;

 jeex_menu->info_menu.credit =
   jeex_menu_stock_append (GTK_MENU (info_menu), GTK_STOCK_INFO, G_CALLBACK (credit), group, GDK_H,
                           GDK_MOD1_MASK, _("View information about Jeex:\n"
                                            "Credits, License and Version."));

 jeex_menu->info = jeex_menu_append (GTK_MENU_BAR (jeex_menu->barra), info_menu, _("Help"), NULL);
 gtk_menu_item_set_right_justified (GTK_MENU_ITEM (jeex_menu->info), TRUE);
}

GtkWidget *
jeex_menu_label_with_stock_append (GtkMenu * menu, const char *label,
                                   const char *stock_id, GCallback func,
                                   GtkAccelGroup * accel_group,
                                   unsigned int accel_id, GdkModifierType mod_type, char *tooltip)
{
  GtkWidget *widget, *image;

  image = gtk_image_new_from_stock (stock_id, GTK_ICON_SIZE_MENU);
  gtk_widget_show (image);

  widget = gtk_image_menu_item_new_with_label (label);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (widget), image);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), widget);
  gtk_widget_show (widget);

  if (func)
    g_signal_connect (G_OBJECT (widget), "activate", func, NULL);

  if (accel_id && accel_group)
    gtk_widget_add_accelerator (widget, "activate", accel_group, accel_id, mod_type, GTK_ACCEL_VISIBLE);

  if (tooltip)
    gtk_widget_set_tooltip_text (widget, tooltip);

  return widget;
}

GtkWidget *
jeex_menu_stock_append (GtkMenu * menu, const char *stock_id, GCallback func,
                        GtkAccelGroup * accel_group, unsigned int accel_id,
                        GdkModifierType mod_type, char *tooltip)
{
  GtkWidget *widget;

  widget = gtk_image_menu_item_new_from_stock (stock_id, accel_group);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), widget);
  gtk_widget_show (widget);

  if (func)
    g_signal_connect (G_OBJECT (widget), "activate", func, NULL);

  if (accel_id && accel_group)
    gtk_widget_add_accelerator (widget, "activate", accel_group, accel_id, mod_type, GTK_ACCEL_VISIBLE);

  if (tooltip)
    gtk_widget_set_tooltip_text (widget, tooltip);

  return widget;
}

GtkWidget *
jeex_menu_append (GtkMenuBar * bar, GtkWidget * menu, const char *name, char *tooltip)
{
  GtkWidget *widget;

  widget = gtk_menu_item_new_with_label (name);
  gtk_widget_show (widget);

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (widget), menu);
  gtk_menu_shell_append (GTK_MENU_SHELL (bar), widget);

  if (tooltip)
    gtk_widget_set_tooltip_text (widget, tooltip);

  return widget;
}
