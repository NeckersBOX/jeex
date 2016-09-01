/*
 * "terminal-actions.h" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#ifndef _TERMINAL_ACTIONS_HEADER
#define _TERMINAL_ACTIONS_HEADER

/* show_bytes ()
 *
 * Shows the file's bytes.
 * Return TRUE if no error occurred otherwise FALSE.
 */
extern gboolean show_bytes (char *);

/* show_struct ()
 *
 * Shows the file's data in structures with types specified.
 * Return TRUE if no error occurred otherwise FALSE.
 */
extern gboolean show_struct (char *);

/* show_portion ()
 *
 * Shows the file's portion.
 */
extern gboolean show_portion (char *);

extern gboolean check_valid_type (char **, JeexTypeCode **);

/* rand_name ()
 *
 * Returns a string with five random characters.
 */
extern char *rand_name (void);

/* types_analyze ()
 * 
 * Returns a strings array with the used types in the
 * file's structure or NULL if an error occurred.
 */
extern char **types_analyze (char **strings);

#endif
