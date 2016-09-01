/*
 * "general-function.h" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#ifndef _GENERAL_FUNCTION_HEADER
#define _GENERAL_FUNCTION_HEADER

/* _error ()
 *
 * Function for errors. Shows a dialog with the string passed
 * as parameter ( may use markup ). 
 * Returns NULL.
 */
extern void *_error (char *);

/* callback_widget_hide ()
 *
 * Hides the widget passed as parameter into g_signal_connect-like functions.
 */
extern void callback_widget_hide (GtkWidget *, void *);

/* _info ()
 *
 * Function for info message. Shows a dialog with the string passed
 * as first parameter. Can be used as boolean choose dialog ( FALSE
 * if user click on CANCEL or TRUE if user click on OK ) if the last
 * parameter is TRUE.
 */
extern gboolean _info (char *, gboolean);

/* able_file_widget ()
 *
 * Able the widgets usable only when the file wasn't modified. 
 * Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *able_file_widget (GtkWidget *, void *);

/* abspath ()
 *
 * Gets absolute path of a file. First parameter is a string with the 
 * relative path of file. 
 * Returns string with absolute path.
 */
extern char *abspath (char *);

/* destroy_this ()
 *
 * Destroy the widget passed as second parameter. First parameter can be
 * omitted with NULL.
 * Returns NULL.
 */
extern void *destroy_this (GtkWidget *, GtkWidget *);

/* disable_file_widget ()
 *
 * Disable the widget usable only when the file wasn't modified.
 * Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *disable_file_widget (GtkWidget *, void *);

/* get_ram_size_from_proc ()
 *
 * Get the info on system memory looking into /proc virtual filesystem.
 * Parameter can be omitted with NULL.
 */
extern unsigned int get_ram_size_from_proc (unsigned int *);

/* int_to_binary ()
 *
 * Convert a integer number in binary and returns as string.
 */
extern char *int_to_binary (int);

/* hide_or_view_window ()
 *
 * Hides or shows the main window. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *hide_or_view_window (GtkWidget *, void *);

/* inap ()
 * 
 * Split file path and name from a string passed as parameter. 
 * Returns a array containing at first element the file name, and
 * at second element the path. 
 */
extern char **inap (char *str);

/* jeex_widget_set_sensitive ()
 *
 * Able ( if first parameters is TRUE ) or Disable ( if is FALSE ) the
 * widgets passed as parameters. Last parameters must be NULL.
 * Returns NULL.
 */
extern void *jeex_widget_set_sensitive (gboolean, ...);

/* obtain_value ()
 *
 * Obtain hexadecimal value of the inserted ascii, binary, decimal
 * octal, etc... text. First parameter is the type from which
 * must be converter, as second the originally string and as 
 * third the address of a gboolean variable which will be set
 * to TRUE in the case the string should not be freed with g_free.
 * Returns the string that contain the hexadecimal value.
 */
extern char *obtain_value (int, char *, gboolean *);

/* secure_quit ()
 *
 * Shows the dialog for secure quit. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *secure_quit (GtkWidget *, void *);

/* strisnull ()
 *
 * Checks that string passed as parameter isn't empty. 
 * Returns TRUE if string is empty, otherwise FALSE.
 */
extern gboolean strisnull (void *);

/* strpos ()
 *
 * Search the second string in the first, if find it returns the position from 
 * which begins in the first string, otherwise -1.
*/
extern int strpos (char *, char *);

/* strreplace ()
 *
 * Replace the second string with the third string in main string.
 * Returns NULL if there is any mistake, otherwise the string with replacements.
 */
extern char *strreplace (char *, char *, char *);

/* update_status_bar ()
 *
 * Update the status bar. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *update_status_bar (GtkWidget *, void *);
#endif
