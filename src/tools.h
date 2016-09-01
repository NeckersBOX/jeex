/*
 * "tools.h" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#ifndef _TOOLS_HEADER
#define _TOOLS_HEADER

struct jeex_split
{
  char *output;
  int bytes;
};

typedef struct
{
  GtkWidget *window;
  GtkWidget *table;
  GtkWidget *hbox;
  GtkWidget *cbox;
  GtkWidget *split;
  GtkWidget *cancel;
  GtkWidget *l_output, *m_output;
  GtkWidget *t_output, *e_output;
  GtkWidget *l_bytes, *s_bytes;
  GtkWidget *check_default_output;
  gboolean output;
  struct jeex_split *opt;
} GtkSplit;

extern void *show_file_structures (GtkWidget *, void *);

/* split_disable_output ()
 *
 * Disable widget for output in the window created with split_window_file () function.
 * First parameter can be omitted with NULL, second must be a GtkSplit structure.
 * Returns NULL.
 */
extern void *split_disable_output (GtkWidget *, GtkSplit *);

/* split_file ()
 *
 * Split original file in other smaller files. First parameter can be omitted
 * with NULL, second must be a GtkSplit structure.
 * Returns NULL.
*/
extern void *split_file (GtkWidget *, GtkSplit *);

/* split_window_file ()
 *
 * Show window per setting the varius parameters to split the current file.
 * Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *split_window_file (GtkWidget *, void *);

extern void show_file_struct_search (void);

/* change_structure ()
 *
 * Change current structure with the next or previous.
 * First parameter can be omitted with NULL, last must be
 * 0 to change with next structure, otherwise 1.
 * Returns NULL.
 */
extern void *change_structure (GtkWidget *, void *);

#endif
