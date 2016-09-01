/*
 * "loggin.h" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#ifndef _LOGGING_HEADER_
#define _LOGGING_HEADER_

typedef struct _logging
{
  GString *log_buffer;
  FILE *log_file;
  char *log_filename;
  GDate *log_date;
} JeexLog;

/* logging_load ()
 *
 * Load and prepare the log file. 
 */
extern void logging_load (void);

/* logging_action ()
 *
 * Save in the buffer the action description.
 * Used as printf-like format.
 */
extern void logging_action (const char *string, ...);

/* logging_unload ()
 *
 * Write log on created file and free 
 * all structures and variables used.
 */
extern void logging_unload (void);

#endif
