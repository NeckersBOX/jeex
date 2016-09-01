/*
 * "view.h" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#ifndef _VIEW_HEADER
#define _VIEW_HEADER

/* chars_table ()
 * 
 * Show a table containing the list of all characters ( ASCII 127 ),
 * and their hexadecimal, binary, octal and decimal value. Parameters
 * can be omitted with NULL.
 * Returns NULL.
 */
extern void *chars_table (GtkWidget *, void *);

/* file_difference ()
 *
 * Show the current file differences with other file.
 * Parameter can be omitted with NULL.
 * Returns NULL.
 */
extern void *file_difference (GtkWidget *, void *);

/* show_in_other_format ()
 *
 * Show the current file in other format, example: ascii, octal, decimal..
 * Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *show_in_other_format (GtkWidget *, void *);

/* stats ()
 *
 * Show Document Statistics. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *stats (GtkWidget *, void *);

/* strings_extract ()
 *
 * Extract all strings containing in the opened file and shows in a 
 * list. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *strings_extract (GtkWidget *, void *);

/* md5_file ()
 *
 * Show dialog with data hash ( md5, sha-1, sha-256 ). Parameters 
 * can be omitted with NULL.
 * Returns NULL.
 */
extern void *md5_file (GtkWidget *, void *);

extern void *add_view (GtkWidget *, void *);

#endif
