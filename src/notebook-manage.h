/*
 * "notebook-manage.h" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#ifndef _NOTEBOOK_MANAGE
#define _NOTEBOOK_MANAGE

#define _ncurrent(element) element[notebook->current]

/* Structure for notebook manage */
struct jeex_notebook
{
  GtkWidget *notebook;
  GtkWidget *page[64];
  GtkWidget *table[64];
  GtkTextBuffer *buffer[64];
  GtkTextInsert *insert[64];
  GtkTextRegex *regex[64];
  GtkWidget *textview[64];
  GtkWidget *update[64];
  int n_page;
  int current;
};

struct jeex_notebook *notebook;


/* make_new_notebook_page ()
 *
 * Make the necessary space , in notebook structure, to manage a new page.
 * First, and last, parameter is the label to show as new page title.
 * Returns NULL.
 */
extern void *make_new_notebook_page (char *label);

/* notebook_change ()
 *
 * Manage the change of notebook page. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *notebook_change (GtkNotebook *, GtkNotebookPage *, int);

#endif
