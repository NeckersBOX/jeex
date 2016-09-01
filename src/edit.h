/*
 * "edit.h" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#ifndef _EDIT_HEADER
#define _EDIT_HEADER

/* cut ()
 *
 * Cut the selected text. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *cut (GtkWidget *, void *);

/* copy ()
 *
 * Copy the selected text. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *copy (GtkWidget *, void *);

/* insert_from_bits_show_dialog ()
 *
 * Show the dialog to operate with bits. Parameters can be omitted with NULL.
 */
extern void *insert_from_bits_show_dialog (GtkWidget * widget, void *data);

/* paste ()
 *
 * Paste the selected text. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *paste (GtkWidget *, void *);

/* reset ()
 *
 * Delete all text entered. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *reset (GtkWidget *, void *);

/* delete ()
 *
 * Delete selected text. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *delete (GtkWidget *, void *);

/* _update ()
 *
 * Refresh the text, reading it from the original file. 
 * Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *_update (GtkWidget *, void *);

/* insert_a ()
 *
 * Show the insertion bar. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *insert_a (GtkWidget *, void **);

/* insert_d ()
 *
 * Hide the insertion bar. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *insert_d (GtkWidget *, void **);

/* insert_format ()
 *
 * Change the inserts format, with that passed as second parameter.
 * First parameter can be omitted with NULL.
 * Returns NULL.
 */
extern void *insert_format (GtkWidget *, int);

/* insert_text ()
 *
 * Insert a text of any format ( binary, octal, hexadecimal,
 * ascii or decimal ) in the text field where are displayed the
 * file bytes ( converted in hexadecimal ). First 
 * parameter can be omitted with NULL, the second must be a
 * GtkTextInsert struct.
 * Returns NULL.
 */
extern void *insert_text (GtkWidget *, GtkTextInsert *);

/* reverse_byte ()
 *
 * Reverse the selected text. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *reverse_byte (GtkWidget *, void *);

/* select_all ()
 *
 * Select all text. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *select_all (GtkWidget *, void *);

#endif
