/*
 * "logging.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#include <gtk/gtk.h>
#include <stdarg.h>
#include <glib/gi18n.h>
#include <libintl.h>
#include <locale.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "header.h"

JeexLog *jeex_log = NULL;

void
logging_load (void)
{
  char *folder;
  GTimeVal time_val;

  g_return_if_fail (!jeex_log);

  folder = g_strdup_printf ("%s/.config/jeex/log", g_get_home_dir ());

  /* Check if exist a logging folder */
  if (!g_file_test (folder, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR))
    {
      /* Making folder */
      if (mkdir (folder, 0777) == -1)
        {
          _error (_("<big><b>Error while saving log</b></big>\n"
                    "\nUnable to make the <i>.config/jeex/log</i> folder in your home.\n"));
          g_free (folder);
          return;
        }
    }
  g_free (folder);

  jeex_log = (JeexLog *) g_malloc (sizeof (JeexLog));

  /* Prepare new log file name */
  g_get_current_time (&time_val);
  jeex_log->log_date = g_date_new ();
  g_date_set_time_val (jeex_log->log_date, &time_val);
  jeex_log->log_filename = g_strdup_printf ("%s/.config/jeex/log/%.2d%.2d%.4d.log", g_get_home_dir (),
                                            g_date_get_day (jeex_log->log_date),
                                            g_date_get_month (jeex_log->log_date),
                                            g_date_get_year (jeex_log->log_date));

  /* Open File */
  if (!(jeex_log->log_file = fopen (jeex_log->log_filename, "a")))
    {
      _error (_("<big><b>Error while saving log</b></big>\n"
                "\nUnable to open a log file for logging your action with\n"
                "jeex. Check the permission."));
      g_free (jeex_log->log_filename);
      g_free (jeex_log);
      jeex_log = NULL;
      return;
    }

  jeex_log->log_buffer = g_string_new ("");
}

void
logging_unload (void)
{
  g_return_if_fail (jeex_log);

  g_free (jeex_log->log_filename);
  if (jeex_log->log_buffer->len > 0)
    fwrite (jeex_log->log_buffer->str, sizeof (char), jeex_log->log_buffer->len, jeex_log->log_file);
  g_string_free (jeex_log->log_buffer, TRUE);
  fclose (jeex_log->log_file);
  g_free (jeex_log);
  jeex_log = NULL;
}

void
logging_action (const char *string, ...)
{
  va_list arg_list;
  GTimeVal time_val;
  GDate *date;
  struct tm *time_s;
  time_t t;
  char buff[64];
  extern JeexPreferences *preferences;
  
  if ( !preferences->logging )
       return;
       
  g_return_if_fail (string && jeex_log);
  
  /* If is begin a new day, close old log file and open
   * a new log file for the new day.
   * Isn't romantic begin a new day with jeex? :')
   */
  g_get_current_time (&time_val);
  date = g_date_new ();
  g_date_set_time_val (date, &time_val);
  if (!g_date_compare (date, jeex_log->log_date))
    {
      g_date_set_dmy (jeex_log->log_date, g_date_get_day (date),
                      g_date_get_month (date), g_date_get_year (date));
      logging_unload ();
      logging_load ();
    }
  g_date_free (date);

  /* Get current time */
  t = time (NULL);
  time_s = localtime (&t);
  strftime (buff, sizeof (buff), "%T", time_s);

  va_start (arg_list, string);
  g_string_append_printf (jeex_log->log_buffer, "( Jeex:%d ) - [ %s ] - ", getpid (), buff);
  g_string_append_vprintf (jeex_log->log_buffer, string, arg_list);
  va_end (arg_list);
}
