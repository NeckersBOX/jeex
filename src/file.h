/*
 * "file.h" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#ifndef _FILE_HEADER
#define _FILE_HEADER

/* Structure for file info manage */
typedef struct info_file
{
  char *name;
  char *type;
  char *permission;
  time_t mtime;
  char last_access[50], last_modify[50];
  unsigned long size;
  gboolean new;
  gboolean modified;
  struct JeexBookmark bookmark;
} JeexFileInfo;

typedef struct
{
  gboolean check;
  gboolean count_check;
  int count;
  char **name;
  GtkWidget **elem[2];
} JeexFileRecent;

/* _restart ()
 *
 * Restart Jeex. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *_restart (GtkWidget *, void *);

/* close_file ()
 *
 * Close current file. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *close_file (GtkWidget *, void *);

/* control_size ()
 *
 * Check size of opened file. First parameters is the file
 * opened for binary write.
 * Returns TRUE if the file size is > 104857600 byte,
 * otherwhise FALSE.
 */
extern gboolean control_size (FILE *);

/* file_append ()
 *
 * Append the content of selected file at current file open.
 * Parameters can be omitted with NULL.
 */
extern void *file_append (GtkWidget *, void *);

/* file_error_control ()
 *
 * Check the error type and show an error dialog.
 * First parameter must be a FILE *Error and second parameter
 * the way was open file:
 * WB_MODE ( Writing Mode ), RB_MODE ( Reading Mode ).
 * Returns NULL.
 */
extern void *file_error_control (GFileError, FileMode);

/* file_open ()
 *
 * Open the choosen file in open_file function.
 * First parameter is the dialog widget where was the
 * choice or NULL.
 * Returns NULL.
 */
extern void *file_open (GtkWidget *);

/* fthx ()
 *
 * Check if the beginning of string, passed as first parameter,
 * is equal to string passed as second parameter.
 * Returns TRUE if they are equal, otherwhise FALSE.
 */
extern gboolean fthx (unsigned char *str, const char *type);

/* info_type ()
 *
 * Gets info on the current file if called with FALSE ( must
 * specify the file data and length of data ), otherwhise
 * returns info as string, if called with TRUE.
 */
extern char *info_type (gboolean mode, ...);

/* load_exe ()
 *
 * Load opened file. First parameter is the opened file for binary
 * writing, second parameter is the address of unsigned char * to store
 * loaded text.
 * Returns file length.
 */
extern unsigned long load_exe (FILE * fd, unsigned char **data);

/* make_string ()
 *
 * Create hexadecimal string from binary string. First parameter
 * is the address of binary string, second string length.
 * Returns hexadecimal string.
 */
extern char *make_string (unsigned char **data, unsigned long len);

/* new_file ()
 *
 * Make a new file. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *new_file (GtkWidget *, void *);

/* obtain_info ()
 *
 * Get info on the current file ( path, name, size ) and make the
 * format string.
 * Returns a string to use as label markup.
 */
extern char *obtain_info (void);

/* ohfa ()
 *
 * Returns binary text from hexadecimal text inserted in
 * the textview.
 */
extern char *ohfa (int *);

/* open_file ()
 *
 * Show dialog to open a new file. Parameters can be omitte with NULL.
 */
extern void *open_file (GtkWidget *, void *);

/* open_position ()
 *
 * Show dialog to select a folder and open all files that it contain.
 * Parameters can be omitted with NULL.
 */
extern void *open_position (GtkWidget *, void *);

/* parse_element ()
 *
 * Check if the passed item to address as parameter is printable on
 * screen, if it isn't printable is replaced by '.'.
 * Returns NULL.
 */
extern void *parse_element (unsigned char *);

/* properties ()
 *
 * Show info on opened file. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *properties (GtkWidget *, void *);

/* remove_open_file ()
 *
 * Remove opened file. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *remove_open_file (GtkWidget *, void *);

/* save_as_html ()
 *
 * Show dialog to choose path to save the file as html. Parameters can be
 * omitted with NULL.
 * Returns NULL.
 */
extern void *save_as_html (GtkWidget *, void *);

/* save_file ()
 *
 * Save current file. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *save_file (GtkWidget *, void *);

/* save_file_as ()
 *
 * Show dialog to choose path to save the file. Parameters can be omitted with NULL.
 * Returns NULL.
 */
extern void *save_file_as (GtkWidget *, void *);

#endif
