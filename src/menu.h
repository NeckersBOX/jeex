/*
 * "menu.h" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#ifndef _MENU_HEADER
#define _MENU_HEADER

/* menu structure of the main window. */
typedef struct
{
  GtkWidget *barra;
  GtkWidget *file, *edit, *view, *search, *bookmark, *tools, *info;
  struct menu_file
  {
    GtkWidget *file;
    GtkWidget *new;
    GtkWidget *open;
    GtkWidget *open_position;
    GtkWidget *append;
    GtkWidget *save;
    GtkWidget *save_as;
    GtkWidget *save_html;
    GtkWidget *remove;
    GtkWidget *quit;
    GtkWidget *restart;
    GtkWidget *info;
    GtkWidget *close;
    GtkWidget *sep;
  } file_menu;
  struct menu_edit
  {
    GtkWidget *edit;
    GtkWidget *copy;
    GtkWidget *cut;
    GtkWidget *paste;
    GtkWidget *delete;
    GtkWidget *select_all;
    GtkWidget *sep;
    GtkWidget *update;
    GtkWidget *reset;
    GtkWidget *revert;
    GtkWidget *insert;
    GtkWidget *insert_from_bit;
    GtkWidget *preferences;
  } edit_menu;
  struct menu_view
  {
    GtkWidget *view;
    GtkWidget *add_view;
    GtkWidget *difference;
    GtkWidget *chars_table;
    GtkWidget *file_in_other_format;
    GtkWidget *stats;
    GtkWidget *hash;
  } view_menu;
  struct menu_search
  {
    GtkWidget *search_m;
    GtkWidget *search;
    GtkWidget *replace;
    GtkWidget *reset_high;
    GtkWidget *go_offset;
    GtkWidget *regex;
  } search_menu;
  struct menu_bookmark
  {
    GtkWidget *bookmark;
    GtkWidget *manage;
    GtkWidget *add;
    GtkWidget *sep;
    GtkWidget *next;
    GtkWidget *prev;
  } bookmark_menu;
  struct menu_tools
  {
    GtkWidget *tools;
    GtkWidget *strings;
    GtkWidget *split;
    GtkWidget *join;
    GtkWidget *file_structures;
  } tools_menu;
  struct menu_info
  {
    GtkWidget *info;
    GtkWidget *credit;
  } info_menu;
} JeexMenu;

/* make_menu ()
 *
 * Make the main window menu. Uses mkm, a global variable, that must be
 * initialize first to be called.
 * Returns NULL.
 */
extern GtkWidget *jeex_menu_new (void);

/*
 * jeex_menu_label_with_stock_append ()
 *
   + Function to make a personalize menu element with the choosed STOCK.
     Received as parameters:

    * menu:  The menu into add the new item
    * label: The text to show as label.
    * stock_id: The STOCK icon to associate at element.
    * func: The function to call when element is selected.
    *       NULL if function doesn't exist.
    * accel_group: The accelerator group into insert a shourtcut for the element.
    *              NULL if accelerator group doesn't exist.
    * accel_id: The shortcut to associate at element.
    *           0 if accelerator group doesn't exist..
    * mod_type: The mask to apply at accelerator.
    * tooltip: The tooltips to associate at element.
               NULL if isn't exist an associate tooltip.
*/
extern GtkWidget *jeex_menu_label_with_stock_append (GtkMenu * menu,
                                                     const char *label,
                                                     const char *stock_id,
                                                     GCallback func,
                                                     GtkAccelGroup *
                                                     accel_group,
                                                     unsigned int accel_id,
                                                     GdkModifierType mod_type, char *tooltip);

/*
 * jeex_menu_stock_append ()
 *
   + Like previous function, only difference is the label parameter beacuse it isn't necessary.
     The element text is take from STOCK.
 */
extern GtkWidget *jeex_menu_stock_append (GtkMenu * menu,
                                          const char *stock_id,
                                          GCallback func,
                                          GtkAccelGroup * accel_group,
                                          unsigned int accel_id,
                                          GdkModifierType mod_type, char *tooltip);


/*
 * jeex_menu_append ()
 *
   + Function to append a menu in a menu bar.
     Received as parameter:

    * bar: The menu bar.
    * menu: The menu to insert.
    * name: The name to show in the bar.
    * tooltip: The tooltip to associate.
*/
extern GtkWidget *jeex_menu_append (GtkMenuBar * bar, GtkWidget * menu,
                                    const char *name, char *tooltip);

/*
 * jeex_menu_separator_append ()
 *
 * Append a separator item into menu.
*/
extern GtkWidget *jeex_menu_separator_append (GtkMenu * menu);

#endif
