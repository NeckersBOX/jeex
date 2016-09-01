/*
 * "main.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <libintl.h>
#include <getopt.h>
#include <locale.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "header.h"

GtkWidget *jeex_main_window;
gboolean support_opacity = TRUE;
JeexToolbar *jeex_toolbar;
JeexTypes *jeex_types;

static void *make_toolbar (GtkWidget *);
static void *make_bytes_info_field (void);
static void *make_personalized_structure_paned (GtkWidget **);

/* Start :) */
int
main (int argc, char **argv)
{
  int i, tot = 0, c, x, cnt;
  int opt_index = 0, z;
  char *filename;
  extern GtkTextInsert *gti_find;
  extern GtkTextInsert *gti_replace;
  extern gboolean check_p;
  extern JeexPreferences *preferences;
  extern JeexFileInfo *file[64];
  extern JeexFileRecent *recent_file;
  extern JeexMenu *jeex_menu;
  extern struct jeex_notebook *notebook;
  extern JeexLog *jeex_log;
  GtkWidget *table, *menu_bar, *widget, *hbox_panel;
  gboolean chk = FALSE, opt_chk = TRUE;
  struct option jeex_long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"help-gtk", no_argument, 0, 'g'},
    {"show-struct", required_argument, 0, 's'},
    {"show-bytes", required_argument, 0, 'b'},
    {"show-portion", required_argument, 0, 'p'},
    {"class", required_argument, 0, 0},
    {"name", required_argument, 0, 0},
    {"display", required_argument, 0, 0},
    {"screen", required_argument, 0, 0},
    {"sync", no_argument, 0, 0},
    {"gtk-module", required_argument, 0, 0},
    {"g-fatal-warnings", no_argument, 0, 0},
    {"gdk-debug", required_argument, 0, 0},
    {"gdk-no-debug", required_argument, 0, 0},
    {"no-xshm", no_argument, 0, 0},
    {"gxid_host", required_argument, 0, 0},
    {"gxid_port", required_argument, 0, 0},
    {"xim-preedit", required_argument, 0, 0},
    {"xim-status", required_argument, 0, 0},
    {"gtk-debug", required_argument, 0, 0},
    {"gtk-no-debug", required_argument, 0, 0},
    {"version", no_argument, 0, 'v'},
    {0, 0, 0, 0}
  };

  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);
  g_set_prgname (Jeex);

  while (opt_chk)
    {
      z = getopt_long (argc, argv, "hgvb:s:p:", jeex_long_options, &opt_index);

      switch (z)
        {
        case 'h':
          g_print (_("Usage:\n"
                     "  jeex [OPTION...] [FILE 1] [FILE 2] [...] [FILE 64]\n\n"
                     "Help Options:\n"
                     "  -h, --help\n"
                     "\tShows this help.\n"
                     "  -g, --help-gtk\n"
                     "\tShows the Gtk+ Options.\n\n"
                     "Jeex Options:\n"
                     "  -v, --version\n"
                     "\tShows the Jeex version.\n"
                     "  -b [FILE], --show-bytes [FILE]\n"
                     "\tShows the file's bytes.\n"
                     "  -s [FILE:type1,type2,...], --show-struct [FILE:type1,type2,...]\n"
                     "\tShows the file's portion.\n"
                     "  -p [FILE:start_byte,end_byte], --show-portion [FILE:start_byte,end_byte]\n"
                     "\tShows the file's data in structures with types specified.\n\n"));
          exit (0);
          break;
        case 'g':
          g_print (_
                   ("Usage:\n"
                    "  jeex [OPTION...] [FILE 1] [FILE 2] [...] [FILE 64]\n"
                    "\n" "Gtk+ Options:\n" "\n"
                    "  --class=CLASS         Program class as used by the window manager\n"
                    "  --name=NAME           Program name as used by the window manager\n"
                    "  --display=DISPLAY     X display to use\n"
                    "  --screen=SCREEN       X screen to use\n"
                    "  --sync                Make X calls synchronous\n"
                    "  --gtk-module=MODULES  Load additional GTK+ modules\n"
                    "  --g-fatal-warnings    Make all warnings fatal\n"
                    "  --gdk-debug=FLAGS     Gdk debugging flags to set\n"
                    "  --gdk-no-debug=FLAGS  Gdk debugging flags to unset\n"
                    "  --no-xshm             Don't use X shared memory extesion\n"
                    "  --gxid_host=HOST\n" "  --gxid_port=PORT\n"
                    "  --xim-preedit=STYLE\n" "  --xim-status=STYLE\n"
                    "  --gtk-debug=FLAGS     Gtk+ debugging flags to set\n"
                    "  --gtk-no-debug=FLAGS  Gtk+ debugging flags to unset\n\n"));
          exit (0);
          break;
        case 'v':
          g_print (Jeex
                   "\nCopyright (C) 2008 Davide Francesco \"HdS619\" Merico " "<hds619@gmail.com>\n\n");
          exit (0);
          break;
        case 'b':
          if (!show_bytes (optarg))
            exit (1);
          exit (0);
          break;
        case 'p':
          if (!show_portion (optarg))
            exit (1);
          exit (0);
          break;
        case 's':
          if (!show_struct (optarg))
            exit (1);
          exit (0);
          break;
        default:
          opt_chk = FALSE;
          break;
        }
    }

  gtk_init (&argc, &argv);

  textview = (textview_info *) g_malloc (sizeof (textview_info));
  gti_find = (GtkTextInsert *) g_malloc (sizeof (GtkTextInsert));
  gti_replace = (GtkTextInsert *) g_malloc (sizeof (GtkTextInsert));
  preferences = (JeexPreferences *) g_malloc (sizeof (JeexPreferences));

  /* Initialize autocompletion structure */
  ac = (struct _ac *) g_malloc0 (sizeof (struct _ac));
  ac->list = gtk_list_store_new (1, G_TYPE_STRING);
  ac->string_exist = ac_string_exist;
  ac->add_string = ac_add_string;
  ac->string_list = g_slist_alloc ();
  ac->strcasecmp = ac_strcasecmp;

  /* Making icon for systray */
  sysicon = gtk_status_icon_new_from_file (JEEX_PATH_IMG "/icon/jeex.png");
  gtk_status_icon_set_tooltip (sysicon, _("Allows you to show or hide the jeex\n" "main window."));

  /* Main Window */
  jeex_main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_name (jeex_main_window, "window");
  gtk_window_set_title (GTK_WINDOW (jeex_main_window), Jeex);
  gtk_window_set_default_size (GTK_WINDOW (jeex_main_window), 434, 506);
  gtk_window_set_default_icon_from_file (JEEX_PATH_IMG "/images/jeex.png", NULL);
  gtk_widget_show (jeex_main_window);

  /* Parsing theme file. */
  if (g_file_test (g_strdup_printf ("%s/.config/jeex/jeex.rc", g_get_home_dir()), G_FILE_TEST_EXISTS))
      gtk_rc_parse (g_strdup_printf ("%s/.config/jeex/jeex.rc", g_get_home_dir()));
	
  /* Main Table */
  table = gtk_table_new (6, 3, FALSE);
  gtk_container_add (GTK_CONTAINER (jeex_main_window), table);
  gtk_widget_show (table);

  /* Making Menu */
  menu_bar = jeex_menu_new ();
  gtk_widget_show (menu_bar);
  gtk_table_attach (GTK_TABLE (table), menu_bar, 0, 2, 0, 1, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

  /* Toolbar */
  jeex_toolbar = (JeexToolbar *) g_malloc (sizeof (JeexToolbar));
  make_toolbar (table);

  /* Personalized Structure's Paned */
  make_personalized_structure_paned (&widget);
  gtk_table_attach (GTK_TABLE (table), widget, 1, 2, 2, 5, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

  /* Plugin avaiable label */
  hbox_panel = gtk_hbox_new (FALSE, 2);
  gtk_table_attach (GTK_TABLE (table), hbox_panel, 2, 3, 2, 5, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

  /* Bytes Info */
  table_info = gtk_table_new (3, 4, FALSE);
  gtk_table_attach (GTK_TABLE (table), table_info, 0, 1, 4, 5, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
  gtk_widget_hide (table_info);
  make_bytes_info_field ();

  /* status bar */
  bar = gtk_statusbar_new ();
  gtk_table_attach (GTK_TABLE (table), bar, 0, 2, 5, 6, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
  gtk_widget_show (bar);

  /* Signals */
  g_signal_connect (G_OBJECT (jeex_main_window), "delete_event", G_CALLBACK (gtk_main_quit), NULL);
  g_signal_connect (G_OBJECT (sysicon), "activate", G_CALLBACK (hide_or_view_window), NULL);

  /* Initialize notebook */
  notebook = (struct jeex_notebook *) g_malloc (sizeof (struct jeex_notebook));

  /* Making notebook and parameters setting. */
  notebook->notebook = gtk_notebook_new ();
  gtk_widget_show (notebook->notebook);
  gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook->notebook), GTK_POS_TOP);
  gtk_notebook_set_homogeneous_tabs (GTK_NOTEBOOK (notebook->notebook), FALSE);
  gtk_table_attach_defaults (GTK_TABLE (table), notebook->notebook, 0, 1, 2, 3);
  gtk_notebook_set_scrollable (GTK_NOTEBOOK (notebook->notebook), TRUE);

  /* Initialize Notebook Page */
  notebook->n_page = 1;
  notebook->current = 0;

  /* Loading Preferences */
  old_preferences_load ();

  logging_action (_("Start jeex session! :)\n"));

  /* Making first page */
  make_new_notebook_page (_("No File Open"));

  textview->insert = *(notebook->insert);
  textview->regex = *(notebook->regex);
  textview->buffer = *(notebook->buffer);
  textview->textview = *(notebook->textview);

  /* Notebook Signals */
  g_signal_connect (G_OBJECT (notebook->notebook), "switch-page", G_CALLBACK (notebook_change), NULL);

  _ncurrent (file)->name = NULL;
  _ncurrent (file)->new = FALSE;
  _ncurrent (file)->bookmark.addr = (int *) g_malloc (preferences->max_bookmarks * sizeof (int));
  _ncurrent (file)->bookmark.str = (char **) g_malloc (preferences->max_bookmarks * sizeof (char *));
  _ncurrent (file)->bookmark.tips = (char **) g_malloc (preferences->max_bookmarks * sizeof (char *));
  _ncurrent (file)->bookmark.bookmark =
    (GtkWidget **) g_malloc (preferences->max_bookmarks * sizeof (GtkWidget *));

  recent_file = (JeexFileRecent *) g_malloc (sizeof (JeexFileRecent));
  recent_file->check = FALSE;
  recent_file->count_check = FALSE;
  recent_file->count = 0;
  recent_file->name = (char **) g_malloc0 (preferences->max_recentfile * sizeof (char *));
  recent_file->elem[0] = (GtkWidget **) g_malloc0 (preferences->max_recentfile * sizeof (GtkWidget *));
  recent_file->elem[1] = (GtkWidget **) g_malloc0 (preferences->max_recentfile * sizeof (GtkWidget *));

  jeex_types = (JeexTypes *) g_malloc (sizeof (JeexTypes));
  jeex_types->main_window = jeex_main_window;
  jeex_types->preferences = preferences;
  jeex_types->file = (JeexFileInfo **) file;
  jeex_types->file_recent = recent_file;
  jeex_types->main_window_panel = hbox_panel;
  jeex_types->menu = jeex_menu;
  jeex_types->toolbar = jeex_toolbar;
  jeex_types->notebook = notebook;
  jeex_types->log = jeex_log;

  plugin_load ();

  /* Opening file\s, if specified as parameter */
  if (argc > 1)
    {
      for (i = 1, x = 0, cnt = 0; i < argc; ++i, ++x)
        {
          if (*(argv[i]) != '/')
            filename = g_strdup_printf ("%s/%s", g_get_current_dir (), argv[i]);
          else
            filename = g_strdup (argv[i]);

          filename = abspath (filename);
          if (g_file_test (filename, G_FILE_TEST_IS_DIR))
            {
              g_free (filename);
              continue;
            }

          if (check_p)
            {
              /* Checking if file was opened */
              tot = gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook->notebook));

              for (c = 0; c < tot; ++c)
                {
                  if (!(file[c]->name))
                    continue;

                  if (!strcmp (file[c]->name, filename))
                    chk = TRUE;
                }

              if (!chk)
                make_new_notebook_page (*(inap (filename)));
            }
          else
            check_p = TRUE;

          if (!chk)
            {
              file[cnt++]->name = g_strdup (filename);
              file_open (NULL);
            }

          g_free (filename);
        }

      gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook->notebook), ((i > tot) ? tot : i) - 1);
      notebook->current = ((i > tot) ? tot : i) - 1;
      if (notebook->current < 0)
        notebook->current = 0;
    }
  else
    update_status_bar (NULL, NULL);

  /* Setting opacity */
  support_opacity = gtk_widget_is_composited (jeex_main_window);
  if (support_opacity)
    gtk_window_set_opacity (GTK_WINDOW (jeex_main_window), preferences->transparency / 100);

  gtk_main ();

  plugin_unload ();
  g_free (jeex_types);
  logging_action (_("End jeex session! :(\n"));
  if ( preferences->logging )
       logging_unload ();

  return 0;
}

/* Make the toolbar into main jeex window. */
static void *
make_toolbar (GtkWidget * table)
{
  int i = 0;

  /* Making toolbar */
  jeex_toolbar->toolbar_widget = gtk_toolbar_new ();
  gtk_toolbar_set_orientation (GTK_TOOLBAR (jeex_toolbar->toolbar_widget), GTK_ORIENTATION_HORIZONTAL);
  gtk_toolbar_set_show_arrow (GTK_TOOLBAR (jeex_toolbar->toolbar_widget), TRUE);
  gtk_table_attach (GTK_TABLE (table), jeex_toolbar->toolbar_widget, 0, 2, 1,
                    2, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
  gtk_widget_show (jeex_toolbar->toolbar_widget);

  /* Making button to make a new file */
  jeex_toolbar->new_file = gtk_tool_button_new_from_stock (GTK_STOCK_NEW);
  gtk_tool_item_set_homogeneous (jeex_toolbar->new_file, FALSE);
  gtk_tool_item_set_tooltip_text (jeex_toolbar->new_file, _("Create a new file"));
  gtk_tool_item_set_visible_horizontal (jeex_toolbar->new_file, TRUE);
  gtk_toolbar_insert (GTK_TOOLBAR (jeex_toolbar->toolbar_widget), jeex_toolbar->new_file, i++);
  g_signal_connect (G_OBJECT (jeex_toolbar->new_file), "clicked", G_CALLBACK (new_file), NULL);
  gtk_widget_show (GTK_WIDGET (jeex_toolbar->new_file));

  /* Making button to open a file */
  jeex_toolbar->open_file = gtk_menu_tool_button_new_from_stock (GTK_STOCK_OPEN);
  gtk_tool_item_set_homogeneous (jeex_toolbar->open_file, FALSE);
  gtk_tool_item_set_tooltip_text (jeex_toolbar->open_file, _("Open a file"));
  gtk_tool_item_set_visible_horizontal (jeex_toolbar->open_file, TRUE);
  gtk_toolbar_insert (GTK_TOOLBAR (jeex_toolbar->toolbar_widget), jeex_toolbar->open_file, i++);
  g_signal_connect (G_OBJECT (jeex_toolbar->open_file), "clicked", G_CALLBACK (open_file), NULL);
  gtk_widget_show (GTK_WIDGET (jeex_toolbar->open_file));

  /* Making button to save the current file */
  jeex_toolbar->save_file = gtk_tool_button_new_from_stock (GTK_STOCK_SAVE);
  gtk_tool_item_set_homogeneous (jeex_toolbar->save_file, FALSE);
  gtk_tool_item_set_tooltip_text (jeex_toolbar->save_file, _("Save the current file"));
  gtk_tool_item_set_visible_horizontal (jeex_toolbar->save_file, TRUE);
  gtk_toolbar_insert (GTK_TOOLBAR (jeex_toolbar->toolbar_widget), jeex_toolbar->save_file, i++);
  g_signal_connect (G_OBJECT (jeex_toolbar->save_file), "clicked", G_CALLBACK (save_file), NULL);
  gtk_widget_set_sensitive (GTK_WIDGET (jeex_toolbar->save_file), FALSE);
  gtk_widget_show (GTK_WIDGET (jeex_toolbar->save_file));

  /* Separator */
  jeex_toolbar->separator = gtk_separator_tool_item_new ();
  gtk_tool_item_set_visible_horizontal (jeex_toolbar->separator, TRUE);
  gtk_toolbar_insert (GTK_TOOLBAR (jeex_toolbar->toolbar_widget), jeex_toolbar->separator, i++);
  gtk_widget_show (GTK_WIDGET (jeex_toolbar->separator));

  /* Making button to cut bytes in the current file */
  jeex_toolbar->cut = gtk_tool_button_new_from_stock (GTK_STOCK_CUT);
  gtk_tool_item_set_homogeneous (jeex_toolbar->cut, FALSE);
  gtk_tool_item_set_tooltip_text (jeex_toolbar->cut, _("Update the current file"));
  gtk_tool_item_set_visible_horizontal (jeex_toolbar->cut, TRUE);
  gtk_toolbar_insert (GTK_TOOLBAR (jeex_toolbar->toolbar_widget), jeex_toolbar->cut, i++);
  g_signal_connect (G_OBJECT (jeex_toolbar->cut), "clicked", G_CALLBACK (cut), NULL);
  gtk_widget_set_sensitive (GTK_WIDGET (jeex_toolbar->cut), FALSE);
  gtk_widget_show (GTK_WIDGET (jeex_toolbar->cut));

  /* Making button to cut bytes in the current file */
  jeex_toolbar->copy = gtk_tool_button_new_from_stock (GTK_STOCK_COPY);
  gtk_tool_item_set_homogeneous (jeex_toolbar->copy, FALSE);
  gtk_tool_item_set_tooltip_text (jeex_toolbar->copy, _("Update the current file"));
  gtk_tool_item_set_visible_horizontal (jeex_toolbar->copy, TRUE);
  gtk_toolbar_insert (GTK_TOOLBAR (jeex_toolbar->toolbar_widget), jeex_toolbar->copy, i++);
  g_signal_connect (G_OBJECT (jeex_toolbar->copy), "clicked", G_CALLBACK (copy), NULL);
  gtk_widget_set_sensitive (GTK_WIDGET (jeex_toolbar->copy), FALSE);
  gtk_widget_show (GTK_WIDGET (jeex_toolbar->copy));

  /* Making button to cut bytes in the current file */
  jeex_toolbar->paste = gtk_tool_button_new_from_stock (GTK_STOCK_PASTE);
  gtk_tool_item_set_homogeneous (jeex_toolbar->paste, FALSE);
  gtk_tool_item_set_tooltip_text (jeex_toolbar->paste, _("Update the current file"));
  gtk_tool_item_set_visible_horizontal (jeex_toolbar->paste, TRUE);
  gtk_toolbar_insert (GTK_TOOLBAR (jeex_toolbar->toolbar_widget), jeex_toolbar->paste, i++);
  g_signal_connect (G_OBJECT (jeex_toolbar->paste), "clicked", G_CALLBACK (paste), NULL);
  gtk_widget_set_sensitive (GTK_WIDGET (jeex_toolbar->paste), FALSE);
  gtk_widget_show (GTK_WIDGET (jeex_toolbar->paste));

  /* Separator */
  jeex_toolbar->separator = gtk_separator_tool_item_new ();
  gtk_tool_item_set_visible_horizontal (jeex_toolbar->separator, TRUE);
  gtk_toolbar_insert (GTK_TOOLBAR (jeex_toolbar->toolbar_widget), jeex_toolbar->separator, i++);
  gtk_widget_show (GTK_WIDGET (jeex_toolbar->separator));

  /* Making button to search bytes in the current file */
  jeex_toolbar->search = gtk_tool_button_new_from_stock (GTK_STOCK_FIND);
  gtk_tool_item_set_homogeneous (jeex_toolbar->search, FALSE);
  gtk_tool_item_set_tooltip_text (jeex_toolbar->search, _("Search more bytes in the current file"));
  gtk_tool_item_set_visible_horizontal (jeex_toolbar->search, TRUE);
  gtk_toolbar_insert (GTK_TOOLBAR (jeex_toolbar->toolbar_widget), jeex_toolbar->search, i++);
  g_signal_connect (G_OBJECT (jeex_toolbar->search), "clicked", G_CALLBACK (find_text), NULL);
  gtk_widget_set_sensitive (GTK_WIDGET (jeex_toolbar->search), FALSE);
  gtk_widget_show (GTK_WIDGET (jeex_toolbar->search));

  /* Making button to replace bytes in the current file */
  jeex_toolbar->replace = gtk_tool_button_new_from_stock (GTK_STOCK_FIND_AND_REPLACE);
  gtk_tool_item_set_homogeneous (jeex_toolbar->replace, FALSE);
  gtk_tool_button_set_label (GTK_TOOL_BUTTON (jeex_toolbar->replace), _("Replace"));
  gtk_tool_item_set_tooltip_text (jeex_toolbar->replace,
                                  _("Search and Replace more bytes in the current file"));
  gtk_tool_item_set_visible_horizontal (jeex_toolbar->replace, TRUE);
  gtk_toolbar_insert (GTK_TOOLBAR (jeex_toolbar->toolbar_widget), jeex_toolbar->replace, i++);
  g_signal_connect (G_OBJECT (jeex_toolbar->replace), "clicked", G_CALLBACK (replace_text), NULL);
  gtk_widget_set_sensitive (GTK_WIDGET (jeex_toolbar->replace), FALSE);
  gtk_widget_show (GTK_WIDGET (jeex_toolbar->replace));

  return NULL;
}

static void *
make_bytes_info_field (void)
{
  /* 8 bit row */
  byte_info.l_gint8 = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (byte_info.l_gint8), _(" <i>Signed 8 bit:</i> "));
  gtk_misc_set_alignment (GTK_MISC (byte_info.l_gint8), 0.0, 0.5);
  gtk_widget_show (byte_info.l_gint8);
  gtk_table_attach (GTK_TABLE (table_info), byte_info.l_gint8, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
  byte_info.e_gint8 = gtk_entry_new ();
  gtk_widget_set_size_request (byte_info.e_gint8, 100, -1);
  gtk_widget_show (byte_info.e_gint8);
  gtk_table_attach (GTK_TABLE (table_info), byte_info.e_gint8, 1, 2, 0, 1,
                    GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

  byte_info.l_guint8 = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (byte_info.l_guint8), _(" <i>Unsigned 8 bit:</i> "));
  gtk_misc_set_alignment (GTK_MISC (byte_info.l_guint8), 0.0, 0.5);
  gtk_widget_show (byte_info.l_guint8);
  gtk_table_attach (GTK_TABLE (table_info), byte_info.l_guint8, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
  byte_info.e_guint8 = gtk_entry_new ();
  gtk_widget_set_size_request (byte_info.e_guint8, 100, -1);
  gtk_widget_show (byte_info.e_guint8);
  gtk_table_attach (GTK_TABLE (table_info), byte_info.e_guint8, 3, 4, 0, 1,
                    GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

  /* Ascii and binary row */
  byte_info.l_ascii = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (byte_info.l_ascii), _(" <i>Ascii:</i> "));
  gtk_misc_set_alignment (GTK_MISC (byte_info.l_ascii), 0.0, 0.5);
  gtk_widget_show (byte_info.l_ascii);
  gtk_table_attach (GTK_TABLE (table_info), byte_info.l_ascii, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
  byte_info.e_ascii = gtk_entry_new ();
  gtk_widget_set_size_request (byte_info.e_ascii, 100, -1);
  gtk_widget_show (byte_info.e_ascii);
  gtk_table_attach (GTK_TABLE (table_info), byte_info.e_ascii, 1, 2, 1, 2,
                    GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

  byte_info.l_binary = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (byte_info.l_binary), _(" <i>Binary:</i> "));
  gtk_misc_set_alignment (GTK_MISC (byte_info.l_binary), 0.0, 0.5);
  gtk_widget_show (byte_info.l_binary);
  gtk_table_attach (GTK_TABLE (table_info), byte_info.l_binary, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
  byte_info.e_binary = gtk_entry_new ();
  gtk_widget_set_size_request (byte_info.e_binary, 100, -1);
  gtk_widget_show (byte_info.e_binary);
  gtk_table_attach (GTK_TABLE (table_info), byte_info.e_binary, 3, 4, 1, 2,
                    GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

  /* Octal and hexadecimal row */
  byte_info.l_octal = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (byte_info.l_octal), _(" <i>Octal:</i> "));
  gtk_misc_set_alignment (GTK_MISC (byte_info.l_octal), 0.0, 0.5);
  gtk_widget_show (byte_info.l_octal);
  gtk_table_attach (GTK_TABLE (table_info), byte_info.l_octal, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
  byte_info.e_octal = gtk_entry_new ();
  gtk_widget_set_size_request (byte_info.e_octal, 100, -1);
  gtk_widget_show (byte_info.e_octal);
  gtk_table_attach (GTK_TABLE (table_info), byte_info.e_octal, 1, 2, 2, 3,
                    GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

  byte_info.l_hex = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (byte_info.l_hex), _(" <i>Hexadecimal:</i> "));
  gtk_misc_set_alignment (GTK_MISC (byte_info.l_hex), 0.0, 0.5);
  gtk_widget_show (byte_info.l_hex);
  gtk_table_attach (GTK_TABLE (table_info), byte_info.l_hex, 2, 3, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
  byte_info.e_hex = gtk_entry_new ();
  gtk_widget_set_size_request (byte_info.e_hex, 100, -1);
  gtk_widget_show (byte_info.e_hex);
  gtk_table_attach (GTK_TABLE (table_info), byte_info.e_hex, 3, 4, 2, 3,
                    GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);

  return NULL;
}

static void *
make_personalized_structure_paned (GtkWidget ** widget)
{
  extern struct show_file_struct show_file_struct;
  extern JeexPreferences *preferences;
  GtkWidget *hbox, *image, *scrolled_window, *close;

  *widget = gtk_vbox_new (FALSE, 3);
  gtk_widget_set_size_request (*widget, 250, -1);
  show_file_struct.main_widget = *widget;
  gtk_container_set_border_width (GTK_CONTAINER (*widget), 10);

  /* Structure's Types */
  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (*widget), hbox, FALSE, TRUE, 0);
  gtk_widget_show (hbox);

  show_file_struct.label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (show_file_struct.label), _("<b>Structure's types:</b>"));
  gtk_widget_show (show_file_struct.label);
  gtk_box_pack_start (GTK_BOX (hbox), show_file_struct.label, FALSE, TRUE, 0);

  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (*widget), hbox, FALSE, TRUE, 0);
  gtk_widget_show (hbox);

  show_file_struct.e_types = gtk_entry_new ();
  gtk_widget_set_tooltip_text (show_file_struct.e_types, _("Example: int,double(repeat_n),char,int"));
  gtk_widget_show (show_file_struct.e_types);
  gtk_box_pack_start (GTK_BOX (hbox), show_file_struct.e_types, FALSE, TRUE, 2);

  show_file_struct.b_types = gtk_button_new ();
  image = gtk_image_new_from_stock (GTK_STOCK_FIND, GTK_ICON_SIZE_MENU);
  gtk_widget_show (image);
  gtk_button_set_image (GTK_BUTTON (show_file_struct.b_types), image);
  gtk_widget_show (show_file_struct.b_types);
  gtk_box_pack_start (GTK_BOX (hbox), show_file_struct.b_types, FALSE, TRUE, 0);

  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (*widget), hbox, FALSE, TRUE, 0);
  gtk_widget_show (hbox);

  show_file_struct.error_label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (show_file_struct.error_label),
                        _("<b><span color=\"#990000\">Values <u>NOT</u> valid.</span></b>"));
  gtk_box_pack_start (GTK_BOX (hbox), show_file_struct.error_label, FALSE, TRUE, 0);

  /* TextView */
  scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  show_file_struct.textview = gtk_text_view_new ();
  gtk_container_set_border_width (GTK_CONTAINER (show_file_struct.textview), 3);
  gtk_widget_show (show_file_struct.textview);
  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window),
                                         show_file_struct.textview);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (show_file_struct.textview), GTK_WRAP_NONE);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (show_file_struct.textview), FALSE);
  gtk_text_view_set_left_margin (GTK_TEXT_VIEW (show_file_struct.textview), 3);
  gtk_text_view_set_pixels_below_lines (GTK_TEXT_VIEW (show_file_struct.textview), 2);
  gtk_widget_modify_font (show_file_struct.textview, pango_font_description_from_string ("Monospace 8"));
  gtk_box_pack_start (GTK_BOX (*widget), scrolled_window, TRUE, TRUE, 3);
  show_file_struct.scrolled_window = scrolled_window;

  /* Tag TextView */
  show_file_struct.buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (show_file_struct.textview));

  show_file_struct.syntax_highlight[0] = gtk_text_buffer_create_tag (show_file_struct.buffer, "key_word",
                                                                     "foreground", "#990000",
                                                                     "weight", PANGO_WEIGHT_BOLD, NULL);

  show_file_struct.syntax_highlight[1] = gtk_text_buffer_create_tag (show_file_struct.buffer, "types",
                                                                     "foreground", "#2e8b57",
                                                                     "weight", PANGO_WEIGHT_BOLD, NULL);

  show_file_struct.syntax_highlight[2] = gtk_text_buffer_create_tag (show_file_struct.buffer, "chars",
                                                                     "foreground", "#990000",
                                                                     "weight", PANGO_WEIGHT_NORMAL,
                                                                     NULL);

  show_file_struct.syntax_highlight[3] = gtk_text_buffer_create_tag (show_file_struct.buffer, "numbers",
                                                                     "foreground", "#ff00ff",
                                                                     "weight", PANGO_WEIGHT_NORMAL,
                                                                     NULL);

  /* Scrolled Structures */
  hbox = gtk_hbox_new (FALSE, 3);
  gtk_box_pack_start (GTK_BOX (*widget), hbox, FALSE, TRUE, 0);
  gtk_widget_show (hbox);

  show_file_struct.b_prev = gtk_button_new ();
  image = gtk_image_new_from_stock (GTK_STOCK_GO_BACK, GTK_ICON_SIZE_MENU);
  gtk_widget_show (image);
  gtk_button_set_image (GTK_BUTTON (show_file_struct.b_prev), image);
  gtk_box_pack_start (GTK_BOX (hbox), show_file_struct.b_prev, FALSE, TRUE, 0);

  show_file_struct.label = gtk_label_new (_("Structure 1/1"));
  gtk_box_pack_start (GTK_BOX (hbox), show_file_struct.label, TRUE, TRUE, 0);

  show_file_struct.b_next = gtk_button_new ();
  image = gtk_image_new_from_stock (GTK_STOCK_GO_FORWARD, GTK_ICON_SIZE_MENU);
  gtk_widget_show (image);
  gtk_button_set_image (GTK_BUTTON (show_file_struct.b_next), image);
  gtk_box_pack_start (GTK_BOX (hbox), show_file_struct.b_next, FALSE, TRUE, 0);

  /* Closed Area */
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_end (GTK_BOX (*widget), hbox, FALSE, TRUE, 0);
  gtk_widget_show (hbox);

  close = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  gtk_widget_show (close);
  gtk_box_pack_end (GTK_BOX (hbox), close, FALSE, TRUE, 0);

  g_signal_connect (G_OBJECT (close), "clicked", G_CALLBACK (callback_widget_hide), *widget);
  g_signal_connect (G_OBJECT (show_file_struct.b_next), "clicked", G_CALLBACK (change_structure),
                    GINT_TO_POINTER (0));
  g_signal_connect (G_OBJECT (show_file_struct.b_prev), "clicked", G_CALLBACK (change_structure),
                    GINT_TO_POINTER (1));
  g_signal_connect (G_OBJECT (show_file_struct.b_types), "clicked", G_CALLBACK (show_file_struct_search),
                    NULL);

  return NULL;
}
