/*
 * "info.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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
#include <libintl.h>
#include <glib/gi18n.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "header.h"

static void
jeex_launch_url (GtkAboutDialog * about, const char *link_, void *data)
{
  char *paths = getenv ("PATH");
  static char *browser[] = { "firefox", "konqueror", "opera", "iceweasel", "epiphany-browser" };
  char **path = NULL, *t;
  int mem = 1, i, c = 0;
  pid_t pid;

  if (!paths)
    {
#if defined(DEBUG_ENABLE) && DEBUG_ENABLE
      g_debug ("%s:%d - jeex_launch_url:\n" "\t\t(i) getenv(\"PATH\") = NULL", __FILE__, __LINE__);
#endif
      return;
    }

  path = g_strsplit (paths, ":", 0);
  mem = (int) g_strv_length (path);

  for (i = 0; i < mem; ++i)
    {
      if (c != -1)
        {
          for (c = 0; c < 5; ++c)
            {
              t = g_strdup_printf ("%s/%s", path[i], browser[c]);
              if (g_file_test (t, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_EXECUTABLE))
                {
                  logging_action (_("Open the jeex project homepage with %s browser.\n"), browser[c]);
                  pid = fork ();
                  if (pid == -1)
                    {
#if defined(DEBUG_ENABLE) && DEBUG_ENABLE
                      g_debug ("%s:%d - jeex_launch_url:\n"
                               "\t\t(i) Cannot create a child process. fork () has return -1.",
                               __FILE__, __LINE__);
#endif
                      g_free (t);
                      c = -1;
                      break;
                    }
                  if (!pid)
                    {
                      execl (t, t, link_, NULL, NULL);
                      exit (0);
                    }
                  g_free (t);
                  c = -1;
                  break;
                }
            }
        }
    }

  g_strfreev (path);
}

void *
credit (GtkWidget * widget, void *data)
{
  GtkWidget *dialog;
  GdkPixbuf *logo;
  const char *authors[] = {
    "Davide Francesco \"HdS619\" Merico ( hds619@gmail.com )",
    "Package:",
    "David Paleino ( d.paleino@gmail.com )",
    "Max Cavallo ( ixamit@gmail.com )",
    NULL
  };
  static char *GNU_General_Public_License;

  GNU_General_Public_License =
    g_strdup_printf ("%s%s%s",
                     _("\n"
                       " Jeex is free software; you can redistribute it and/or modify\n"
                       " it under the terms of the GNU General Public License as published\n"
                       " by the Free Software Foundation; either version 3 of the License,\n"
                       " or (at your option) any later version.\n" "\n"),
                     _
                     (" Jeex is distributed in the hope that it will be useful,\n"
                      " but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
                      " MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
                      " GNU General Public License for more details.\n" "\n"),
                     _
                     (" You should have received a copy of the GNU General Public License\n"
                      " along with this program; if not, write to the Free Software \n"
                      " Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,\n"
                      " MA 02110-1301, USA."));

  logo = gdk_pixbuf_new_from_file (JEEX_PATH_IMG "/jeex.png", NULL);
  dialog = gtk_about_dialog_new ();

  gtk_about_dialog_set_program_name (GTK_ABOUT_DIALOG (dialog), _NAME);
  gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (dialog), _VER);
  gtk_about_dialog_set_logo (GTK_ABOUT_DIALOG (dialog), logo);
  gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (dialog),
                                  "(C) 2008 - 2012 Davide Francesco \"HdS619\" Merico ( hds619@gmail.com )");
  gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG (dialog),
                                 _("Jeex is a small hexadecimal editor\n" "created with Gtk+."));
  gtk_about_dialog_set_translator_credits (GTK_ABOUT_DIALOG (dialog),
                                           "Strings Revision:\n"
                                           "David Paleino ( d.paleino@gmail.com )\n\n"
                                           "Greek Translation:\n"
                                           "Alkis Mavridis ( alkismavridis@hotmail.com )\n");
  gtk_about_dialog_set_license (GTK_ABOUT_DIALOG (dialog), GNU_General_Public_License);

  gtk_about_dialog_set_url_hook (jeex_launch_url, NULL, NULL);
  gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (dialog), "http://www.hds619.net/jeex.php");
  gtk_about_dialog_set_website_label (GTK_ABOUT_DIALOG (dialog), "Jeex Project HomePage");

  gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (dialog), authors);

  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);

  return NULL;
}
