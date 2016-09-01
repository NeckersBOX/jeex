/*
 * "search.h" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#ifndef _SEARCH_HEADER
#define _SEARCH_HEADER

/* ac_strcasecmp ()
 *
 * Redirect functions to strcasecmp, created especially to
 * work as GCompareDataFunc paramater.
 * Returns 0 if strings are equal, otherwhise 1 or -1.
 */
extern int ac_strcasecmp (const void *, const void *);

/* ac_add_string ()
 *
 * Add the passed string as parameter to the list of the
 * automatic completion strings.
 */
extern void ac_add_string (char *);

/* ac_string_exist ()
 *
 * Tests that the string passed as parameter is in the 
 * list of the strings in the automatic completion list.
 * Returns TRUE if the string is present, otherwhise FALSE.
 */
extern gboolean ac_string_exist (char *);

/* regex_search_text () */
extern void *regex_search_text (GtkWidget *, void *);

/* regex_entry_clear () */
extern void *regex_entry_clear (GtkWidget *, void *);

/* find_text ()
 *
 * Show the dialog for text search. Paramters can be 
 * omitted with NULL.
 * Returns NULL.
 */
extern void *find_text (GtkWidget *, void *);

/* find_this_text ()
 *
 * Find the text inserted in research dialog, in the hexadecimal
 * text and tags apply. Second parameters must be a GtkEntry which
 * get the search text, the first can be omitted with NULL.
 * Returns NULL.
 */
extern void *find_this_text (GtkWidget *, void *);

/* reset_tag ()
 *
 * Delete all tags applied to the text. Parameters can be
 * omitted with NULL.
 * Returns NULL.
 */
extern void *reset_tag (GtkWidget *, void *);

/* find_method ()
 *
 * Change the research method dependent of the second parameter:
 * FALSE - forward, TRUE - backward. First parameter can be
 * omitted with NULL.
 * Returns NULL.
 */
extern void *find_method (GtkWidget *, gboolean);

/* jump_to_offset ()
 *
 * Jump to offset inserted in the GtkEntry second parameter.
 * First parameter can be omitted with NULL.
 * Returns NULL.
 */
extern void *jump_to_offset (GtkWidget *, GtkEntry *);

/* jump_to_offset_gui ()
 *
 * Show a window where you can choose the offset address in 
 * which place the scrollbar. Parameters can be omitted with NULL.
 * Returns NULL. 
 */
extern void *jump_to_offset_gui (GtkWidget *, void **);

/* replace_text ()
 *
 * Show the dialog where search and replace text. Parameters
 * can be omitted with NULL.
 * Returns NULL.
 */
extern void *replace_text (GtkWidget *, void *);

/* regex_show_bar ()
 *
 * Show the regex bar to search a byte using the regex.
 * Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *regex_show_bar (GtkWidget *, void **);

/* regex_close_bar ()
 *
 * Hide the regex bar. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *regex_close_bar (GtkWidget *, void **);

/* search_and_replace ()
 *
 * Search and replace all occurence that find in the text.
 * Second parameters must be a search_and_replace structure,
 * the first can be omitted with NULL.
 * Returns NULL.
 */
extern void *search_and_replace (GtkWidget *, void *);

/* single_replace ()
 *
 * Search and replace the first occurrence that find in text.
 * Second parameters must be a search_and_replace structure,
 * the first can be omitted with NULL.
 * Returns NULL.
 */
extern void *single_replace (GtkWidget *, void *);


#endif
