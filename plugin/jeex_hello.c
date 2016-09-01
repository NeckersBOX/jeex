/*
 * "jeex_hello.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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
#define plugin_header
#include "header.h"

static JeexTypes *plugin_jeex_types;
static int loaded = 0;

gboolean jeex_hello_start (JeexTypes *jeex_types)
{
 /* This not allow to load plugin after call
  * jeex_hello_end function.
  */
 if ( loaded )
      return FALSE;

#ifdef debug_enable
 plugin_jeex_types = jeex_types;
 g_print ("Plugin: jeex_hello: loaded!\n");
#endif
 loaded = 1;

 return TRUE;
}

void jeex_hello_file_open (void)
{
#ifdef debug_enable
 g_print ("Plugin: file opened: %s\n",
          plugin_jeex_types->file[plugin_jeex_types->notebook->current]->name);
#endif
}

void jeex_hello_end (gboolean activated)
{
#ifdef debug_enable
 g_print ("Plugin: jeex_hello: unloaded!\n");
#endif
}
