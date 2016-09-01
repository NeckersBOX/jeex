/*
 * "html-export.h" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#ifndef _HTML_EXPORT_HEADER
#define _HTML_EXPORT_HEADER

/* html_export ()
 *
 * Function to export a file in html. Receive as parameter in order:
 *  1. String contains path and name of input file
 *  2. String contains path and name of output file
 *  3. Data of input file
 *  4. Length of data
 *  5. A setted JHtmlExportPage variable.
 * Returns TRUE if export has been success, otherwise FALSE.
 */
extern gboolean html_export (char *, char *, unsigned char *, int, JHtmlExportPage *);

/* html_export_window ()
 * Show dialog to choose path to save the file as html. Parameters can be 
 * omitted with NULL.
 * Returns NULL.
 */
extern void *html_export_window (GtkWidget *, void *);

#endif
