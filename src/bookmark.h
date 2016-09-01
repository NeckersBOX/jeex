/*
 * "bookmark.h" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#ifndef _BOOK_HEADER
#define _BOOK_HEADER

/* add_bookmark_gui ()
 *
 * Show the window that allow of insert a bookmark in the bookmarks
 * of the current file. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *add_bookmark_gui (GtkWidget *, void *);

/* delete_bookmarks ()
 *
 * Delete the present bookmarks in the list.
 */
extern void delete_bookmarks (void);

/* load_bookmakrs ()
 *
 * Load the bookmarks of the current file. 
 */
extern void load_bookmarks (void);

/* manage_bookmarks_gui ()
 *
 * Show the window for manage the created bookmarks. 
 * Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *manage_bookmarks_gui (GtkWidget *, void *);

/* n_bookmark ()
 * 
 * Go to the next bookmark. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *n_bookmark (GtkWidget *, void *);

/* p_bookmark ()
 *
 * Go to the previous bookmark. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *p_bookmark (GtkWidget *, void *);

/* reload_bookmarks ()
 *
 * Allow of reload the bookmarks.
 */
extern void reload_bookmarks (void);

#endif
