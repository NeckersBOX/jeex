/*
 * "plugin.h" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#ifndef _PLUGIN_HEADER_
#define _PLUGIN_HEADER_

/* plugin_load ()
 *
 * Load all plugin.
 */
extern void plugin_load (void);

/* plugin_load_unload ()
 *
 * Load or Unload a single plugin.
 * Plugin name must be memorized in data.
 */
extern void *plugin_load_unload (GtkWidget *widget, void *data);

/* plugin_open_file_emit_signal ()
 *
 * Emit a signal when jeex open a file.
 */
extern void plugin_open_file_emit_signal (void);

/* plugin_close_file_emit_signal ()
 *
 * Emit a signal when jeex close all file.
 */
extern void plugin_close_file_emit_signal (void);

/* plugin_change_tab_emit_signal ()
 *
 * Emit a signal when jeex change tab.
 */
extern void plugin_change_tab_emit_signal (void);

/* plugin_unlod ()
 *
 * Unload all plugin.
 */
extern void plugin_unload (void);

#endif
