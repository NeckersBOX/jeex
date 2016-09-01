/*
 * "preferences.h" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#ifndef _PREFERENCES_HEADER
#define _PREFERENCES_HEADER

typedef struct
{
  unsigned int max_filesize;
  unsigned int max_loadbuff;
  unsigned int max_savebuff;
  int max_bookmarks;
  int max_recentfile;
  int max_tabchars;
  gboolean backup_copy;
  gboolean logging;
  char *font;
  char *color;
  double transparency;
  int theme;
  gboolean cursor_mode;
  gboolean jeex_icon;
  gboolean toolbar;
  gboolean zero_prefix;
} JeexPreferences;

/* imad ()
 *
 * Able or diable the chars overwrite mode. Parameters can be omitted
 * with NULL.
 * Returns NULL. 
 */
extern void *imad (GtkWidget *, void *);

/* old_preferences_load ()
 *
 * Load preferences from the preferences.conf file in ~/.config/jeex
 */
extern void old_preferences_load (void);

/* preferences_window ()
 *
 * Make and show the preferences window. Parameters can be omitted 
 * with NULL.
 * Returns NULL.
 */
extern void *preferences_window (GtkWidget *, void *);

#endif
