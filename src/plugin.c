/*
 * "plugin.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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
#include "header.h"

extern JeexTypes *jeex_types;
extern PluginList plugin_list[];
static gboolean *activate_plugin = NULL;
static int length_activate_plugin = 1;

extern void plugin_menu_make (gboolean *, int);

void plugin_load (void)
{
 int i = 0;
 gboolean chk;

 g_return_if_fail (plugin_list);

 activate_plugin = (gboolean *) g_malloc (length_activate_plugin * sizeof (gboolean));
 while ( plugin_list[i++].start_plugin_function ) {
         chk = plugin_list[i - 1].start_plugin_function (jeex_types);
         activate_plugin[length_activate_plugin - 1] = chk;
         activate_plugin = (gboolean *) g_realloc (activate_plugin, length_activate_plugin++ * sizeof (gboolean));
 }

 plugin_menu_make (activate_plugin, length_activate_plugin);
}

void *plugin_load_unload (GtkWidget *widget, void *data)
{
 char *plugin_name = (char *) data;
 int i;

 for ( i = 0; i < length_activate_plugin; i++ ) {
       if ( !strcmp (plugin_name ? plugin_name : "",
                     plugin_list[i].plugin_name ? plugin_list[i].plugin_name : "") )
            break;
 }

 if ( strcmp (plugin_name ? plugin_name : "",
              plugin_list[i].plugin_name ? plugin_list[i].plugin_name : "") )
      return NULL;

 if ( activate_plugin[i] ) {
      plugin_list[i].end_plugin_function (activate_plugin[i]);
      activate_plugin[i] = FALSE;
      return NULL;
 }

 activate_plugin[i] = plugin_list[i].start_plugin_function (jeex_types);
 gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (plugin_list[i].menu_widget), activate_plugin[i]);

 return NULL;
}

void plugin_open_file_emit_signal (void)
{
 int i = 0;

 g_return_if_fail (plugin_list);

 while ( plugin_list[i++].start_plugin_function ) {
         if ( plugin_list[i - 1].open_file_plugin_signal && activate_plugin[i - 1] )
              plugin_list[i - 1].open_file_plugin_signal ();
 }
}

void plugin_close_file_emit_signal (void)
{
 int i = 0;

 g_return_if_fail (plugin_list);

 while ( plugin_list[i++].start_plugin_function ) {
         if ( plugin_list[i - 1].close_file_plugin_signal && activate_plugin[i - 1] )
              plugin_list[i - 1].close_file_plugin_signal ();
 }
}

void plugin_change_tab_emit_signal (void)
{
 int i = 0;

 g_return_if_fail (plugin_list);
 while ( plugin_list[i++].start_plugin_function ) {
         if ( plugin_list[i - 1].change_tab_plugin_signal && activate_plugin[i - 1] )
              plugin_list[i - 1].change_tab_plugin_signal ();
 }
}

void plugin_unload (void)
{
 int i = 0;

 g_return_if_fail (plugin_list);

 while ( plugin_list[i++].end_plugin_function )
         plugin_list[i - 1].end_plugin_function (activate_plugin[i - 1]);

 g_free (activate_plugin);
 length_activate_plugin = 1;
}
