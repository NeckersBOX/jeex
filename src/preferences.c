/*
 * "preferences.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#define _GNU_SOURCE
#include <gtk/gtk.h>
#include <ctype.h>
#include <libintl.h>
#include <errno.h>
#include <glib/gi18n.h>
#include <locale.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "header.h"

#define JEEX_MAX_UNSIGNED_INT (pow (2, sizeof (unsigned int) * 8) - 1)
#define G_KEY_FILE_PREFERENCES_GROUP _("Preferences")

#define jeex_key_file_set_dbl(key_file, var_name, var_value) \
 g_key_file_set_double (key_file, G_KEY_FILE_PREFERENCES_GROUP, var_name, (double) var_value);

#define jeex_key_file_set_int(key_file, var_name, var_value) \
 g_key_file_set_integer (key_file, G_KEY_FILE_PREFERENCES_GROUP, var_name, (int) var_value);

#define jeex_key_file_set_bln(key_file, var_name, var_value) \
 g_key_file_set_boolean (key_file, G_KEY_FILE_PREFERENCES_GROUP, var_name, (gboolean) var_value);

#define jeex_key_file_set_str(key_file, var_name, var_value) \
 g_key_file_set_string (key_file, G_KEY_FILE_PREFERENCES_GROUP, var_name, (char *) var_value);

/* Check that no error occurred, if they are occurred setting
 * `var' with `set' casting at `type' specified, otherwise check
 * that `var' not exceeded `min' or `max` value, if exceeded
 * setting `var' with `set'.
 */
#define _CHECK_CORRECT_GET(err, var, type, min, set, max) \
        if ( !err && min && max ) { \
             if ( var < ((type) min) || var > ((type) max) ) \
                  var = ((type) set); \
        } \
        if ( err ) { \
               var = ((type) set);
#if defined(DEBUG_ENABLE) && DEBUG_ENABLE
# define CHECK_CORRECT_GET(err, var, type, min, set, max) \
         _CHECK_CORRECT_GET(err, var, type, min, set, max) \
             g_debug ("%s:%d - CHECK_CORRECT_GET:\n" \
                      "\t(i) variable not found into preferences file.", __FILE__, __LINE__); \
         }
#else
# define CHECK_CORRECT_GET(err, var, type, min, set, max) \
         _CHECK_CORRECT_GET(err, var, type, min, set, max) \
         }
#endif

typedef struct
{
  GtkWidget *window;
  struct
  {
    GtkWidget *filesize, *combosize[3], *buffloadsize, *buffsavesize;
    GtkWidget *bookmarks, *recentfile, *charstab;
    GtkWidget *check_bcopy, *check_logging;
  } page1;
  struct
  {
    GtkWidget *font, *l_color, *b_color;
    GtkWidget *theme, *transparency;
    GtkWidget *check_insert_mode;
    GtkWidget *check_systray_icon;
    GtkWidget *check_toolbar;
    GtkWidget *check_prefix;
  } page2;
} JeexPreferencesWindow;

JeexPreferencesWindow *JPW = NULL;

typedef enum
{
  G_KEY_INT,
  G_KEY_UINT,
  G_KEY_DBL,
  G_KEY_STR,
  G_KEY_BLN
} GKeyType;

static void make_first_notebook_page (GtkNotebook *);
static void make_second_notebook_page (GtkNotebook *);
static void apply_preferences (void);
static void ok_preferences (GtkButton *, void *);

static int previous_combo_box_active[3] = { 1, 1, 1 };

extern gboolean support_opacity;
static gboolean pref_check = TRUE;

JeexPreferences *preferences;

/* Function to save the preferences */
static void
write_new_preferences (void)
{
  char *pfile, *pfolder;
  GString *str_error;
  GKeyFile *kfile;
  GIOChannel *of;
  GError *error = NULL;
  gsize byte;
  GIOStatus res;

  return_if_fail (pref_check);

  /* Getting name of the folder and file to use as preferences. */
  pfolder = g_strdup_printf ("%s/.config/jeex", g_get_home_dir ());
  pfile = g_strdup_printf ("%s/preferences.conf", pfolder);

  /* Checking if exist a jeex folder. */
  if (!g_file_test (pfolder, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR))
    {
      /* Making folder */
      if (mkdir (pfolder, 0777) == -1)
        {
          _error (_("<big><b>Error while saving the preferences</b></big>\n"
                    "\nUnable to create the <i>.config/jeex</i> folder in your home.\n"
                    "This makes it impossible to save your preferences\nfor future sessions."));
          g_free (pfile);
          g_free (pfolder);
          pref_check = FALSE;
          return;
        }
    }

  /* Making preferences file. */
  kfile = g_key_file_new ();
  jeex_key_file_set_dbl (kfile, "max filesize", preferences->max_filesize - 1);
  jeex_key_file_set_dbl (kfile, "max loadbuff", preferences->max_loadbuff - 1);
  jeex_key_file_set_dbl (kfile, "max savebuff", preferences->max_savebuff - 1);
  jeex_key_file_set_int (kfile, "max bookmarks", preferences->max_bookmarks);
  jeex_key_file_set_int (kfile, "max recentfile", preferences->max_recentfile);
  jeex_key_file_set_int (kfile, "max tabchars", preferences->max_tabchars);
  jeex_key_file_set_bln (kfile, "backup copy", preferences->backup_copy);
  jeex_key_file_set_str (kfile, "font", preferences->font);
  jeex_key_file_set_str (kfile, "color", preferences->color);
  jeex_key_file_set_dbl (kfile, "transparency", preferences->transparency);
  jeex_key_file_set_int (kfile, "current theme", preferences->theme);
  jeex_key_file_set_bln (kfile, "cursor mode", preferences->cursor_mode);
  jeex_key_file_set_bln (kfile, "jeex icon", preferences->jeex_icon);
  jeex_key_file_set_bln (kfile, "toolbar", preferences->toolbar);
  jeex_key_file_set_bln (kfile, "zero prefix", preferences->zero_prefix);
  jeex_key_file_set_bln (kfile, "logging", preferences->logging);

  /* Saving new preferences. */
  if (!(of = g_io_channel_new_file (pfile, "w", &error)))
    {
      file_error_control (error->code, WB_MODE);

      g_error_free (error);
      g_free (pfile);
      g_free (pfolder);
      g_key_file_free (kfile);
      pref_check = FALSE;
      return;
    }
  res = g_io_channel_write_chars (of, g_key_file_to_data (kfile, NULL, NULL), -1, &byte, &error);

  /* Checking that there are no errors in the saving. */
  switch (res)
    {
    case G_IO_STATUS_ERROR:
      str_error = g_string_new (_("<big><b>Error while saving the preferences</b></big>\n\n"));
      switch (error->code)
        {
        case G_IO_CHANNEL_ERROR_IO:
          g_string_append (str_error,
                           _
                           ("An I/O error has occurred. Check that the device\n"
                            "you're trying to save to is not damaged."));
          break;
        case G_IO_CHANNEL_ERROR_ISDIR:
          g_string_append (str_error, _("The file you are trying to save to is a directory."));
          break;
        case G_IO_CHANNEL_ERROR_NXIO:
          g_string_append (str_error, _("The file cannot be open because it cannot be found."));
          break;
        default:
          g_string_append (str_error, _("An unknown error has occurred while file saving."));
          break;
        }

      _error (str_error->str);
      g_error_free (error);
      g_string_free (str_error, TRUE);
      break;
    case G_IO_STATUS_AGAIN:
      str_error = g_string_new (_("<big><b>Error while saving the preferences</b></big>\n\n"));
      g_string_append (str_error, _("Resource temporarily unavailable."));

      _error (str_error->str);
      g_error_free (error);
      g_string_free (str_error, TRUE);
      break;
    case G_IO_STATUS_NORMAL:
      break;
    case G_IO_STATUS_EOF:
      break;
    }

  g_io_channel_close (of);

  g_free (pfile);
  g_free (pfolder);
  g_key_file_free (kfile);

  pref_check = TRUE;
}

/* Function that setting the preferences variables, first parameter is the variabile type
 * to gets, second parameter is the GKeyFile from which to obtain the variables, third
 * parameter the variable name, quarter parameter the variable to set and last parameter
 * is the variabile to set TRUE if error has occurred.
 */
static void
jeex_key_file_get_value (GKeyType t, GKeyFile * f, char *var, void *val, gboolean * err)
{
  GString *str_error;
  GError *error = NULL;
  static union
  {
    char *str;
    unsigned long int uint32;
    double dbl;
    long int int32;
    gboolean bln;
  } GKFV;
  static char *group;

  group = G_KEY_FILE_PREFERENCES_GROUP;
  *err = FALSE;

  /* Get the value */
  switch (t)
    {
    case G_KEY_INT:
      GKFV.int32 = (long int) g_key_file_get_integer (f, group, var, &error);
      break;
    case G_KEY_DBL:
      GKFV.dbl = g_key_file_get_double (f, group, var, &error);
      break;
    case G_KEY_STR:
      GKFV.str = g_key_file_get_string (f, group, var, &error);
      break;
    case G_KEY_UINT:
      GKFV.uint32 = (unsigned long int) g_key_file_get_double (f, group, var, &error);
      break;
    case G_KEY_BLN:
      GKFV.bln = g_key_file_get_boolean (f, group, var, &error);
      break;
    }

  /* Check that no error occurred */
  if (error)
    {
      str_error = g_string_new (_("<big><b>Error while loading the preferences</b></big>\n\n"));
      g_string_append (str_error, error->message);
      _error (str_error->str);
      g_string_free (str_error, TRUE);
      g_error_free (error);
      *err = TRUE;
    }

  /* Set value */
  switch (t)
    {
    case G_KEY_INT:
      *((int *) val) = *err ? 0 : GKFV.int32;
      break;
    case G_KEY_DBL:
      *((double *) val) = *err ? 0 : GKFV.dbl;
      break;
    case G_KEY_STR:
      *((char **) val) = *err ? NULL : g_strdup (GKFV.str);
      break;
    case G_KEY_UINT:
      *((unsigned long int *) val) = *err ? 0 : GKFV.uint32;
      break;
    case G_KEY_BLN:
      *((gboolean *) val) = *err ? FALSE : GKFV.bln;
      break;
    }
}

/* Loads preferences if there are, otherwise initializes the various folders and files
 * to be able to save and reload later.
 */
void
old_preferences_load (void)
{
  char *pfolder, *pfile;
  gboolean chk;
  GString *str_error;
  GKeyFile *kfile;
  GError *error = NULL;

  /* Getting name of the folder and file to use as preferences. */
  pfolder = g_strdup_printf ("%s/.config/jeex", g_get_home_dir ());
  pfile = g_strdup_printf ("%s/preferences.conf", pfolder);

  /* Checking if exist a jeex folder. */
  if (!g_file_test (pfolder, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR))
    {
      /* if folder doesn't exist, i create it */
      if (mkdir (pfolder, 0777) == -1)
        {
          _error (_("<big><b>Error while saving the preferences</b></big>\n"
                    "\nUnable to make the <i>.jeex</i> folder in your home.\n"
                    "This makes it impossible to save your preferences\nfor future sessions."));
          g_free (pfile);
          g_free (pfolder);
          pref_check = FALSE;
          return;
        }
    }

  /* Checking if the preferences file exist. */
  if (!g_file_test (pfile, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR))
    {
      preferences->max_filesize = JEEX_MAX_UNSIGNED_INT;
      preferences->max_loadbuff = JEEX_MAX_UNSIGNED_INT;
      preferences->max_savebuff = JEEX_MAX_UNSIGNED_INT;
      preferences->max_bookmarks = 15;
      preferences->max_recentfile = 5;
      preferences->max_tabchars = 18;
      preferences->backup_copy = FALSE;
      preferences->font = g_strdup ("Monospace 10");
      preferences->color = g_strdup ("#2A00FF");
      preferences->transparency = 100.0;
      preferences->theme = 5;
      preferences->cursor_mode = FALSE;
      preferences->jeex_icon = TRUE;
      preferences->toolbar = TRUE;
      preferences->zero_prefix = TRUE;
      preferences->logging = TRUE;

      write_new_preferences ();
    }
  else
    {
      /* Loading file to recovery the values of old choosed preferences and
       * checking that there are no errors.
       */
      kfile = g_key_file_new ();
      chk = g_key_file_load_from_file (kfile, pfile, G_KEY_FILE_KEEP_COMMENTS, &error);
      if (!chk)
        {
          str_error = g_string_new (_("<big><b>Error while loading the preferences</b></big>\n\n"));

          switch (error->code)
            {
            case G_KEY_FILE_ERROR_UNKNOWN_ENCODING:
            case G_KEY_FILE_ERROR_PARSE:
              g_string_append (str_error,
                               _
                               ("Preferences file that you trying to examine\n"
                                "is in a unknown format."));
              break;
            case G_KEY_FILE_ERROR_NOT_FOUND:
              g_string_append (str_error, _("Preferences file wasn't found."));
              break;
            default:
              g_string_append (str_error, _("Unknown Error."));
              break;
            }

          _error (str_error->str);

          g_error_free (error);
          g_string_free (str_error, TRUE);
          g_free (pfile);
          g_free (pfolder);
          g_key_file_free (kfile);
          pref_check = FALSE;
          return;
        }

      g_free (pfile);
      g_free (pfolder);

      /* Recovery values of keys  */
      jeex_key_file_get_value (G_KEY_UINT, kfile, "max filesize", &preferences->max_filesize, &chk);
      CHECK_CORRECT_GET (chk, preferences->max_filesize, unsigned int, 1,
                         JEEX_MAX_UNSIGNED_INT - 1, JEEX_MAX_UNSIGNED_INT);
      jeex_key_file_get_value (G_KEY_UINT, kfile, "max loadbuff", &preferences->max_loadbuff, &chk);
      CHECK_CORRECT_GET (chk, preferences->max_loadbuff, unsigned int, 1,
                         JEEX_MAX_UNSIGNED_INT - 1, JEEX_MAX_UNSIGNED_INT);
      jeex_key_file_get_value (G_KEY_UINT, kfile, "max savebuff", &preferences->max_savebuff, &chk);
      CHECK_CORRECT_GET (chk, preferences->max_savebuff, unsigned int, 1,
                         JEEX_MAX_UNSIGNED_INT - 1, JEEX_MAX_UNSIGNED_INT);
      jeex_key_file_get_value (G_KEY_INT, kfile, "max bookmarks", &preferences->max_bookmarks, &chk);
      CHECK_CORRECT_GET (chk, preferences->max_bookmarks, int, 1, 15, 30);
      jeex_key_file_get_value (G_KEY_INT, kfile, "max recentfile", &preferences->max_recentfile, &chk);
      CHECK_CORRECT_GET (chk, preferences->max_recentfile, int, 2, 5, 9);
      jeex_key_file_get_value (G_KEY_INT, kfile, "max tabchars", &preferences->max_tabchars, &chk);
      CHECK_CORRECT_GET (chk, preferences->max_tabchars, int, 1, 18, 255);
      jeex_key_file_get_value (G_KEY_BLN, kfile, "backup copy", &preferences->backup_copy, &chk);
      CHECK_CORRECT_GET (chk, preferences->backup_copy, gboolean, NULL, FALSE, NULL);
      jeex_key_file_get_value (G_KEY_STR, kfile, "font", &preferences->font, &chk);
      CHECK_CORRECT_GET (chk, preferences->font, char *, NULL, g_strdup ("Monospace 10"), NULL);
      jeex_key_file_get_value (G_KEY_STR, kfile, "color", &preferences->color, &chk);
      CHECK_CORRECT_GET (chk, preferences->color, char *, NULL, g_strdup ("#2A00FF"), NULL);
      jeex_key_file_get_value (G_KEY_DBL, kfile, "transparency", &preferences->transparency, &chk);
      CHECK_CORRECT_GET (chk, preferences->transparency, double, 10.0, 100.0, 100.0);
      jeex_key_file_get_value (G_KEY_INT, kfile, "current theme", &preferences->theme, &chk);
      CHECK_CORRECT_GET (chk, preferences->theme, int, 0, 5, 5);
      jeex_key_file_get_value (G_KEY_BLN, kfile, "cursor mode", &preferences->cursor_mode, &chk);
      CHECK_CORRECT_GET (chk, preferences->cursor_mode, gboolean, NULL, FALSE, NULL);
      jeex_key_file_get_value (G_KEY_BLN, kfile, "jeex icon", &preferences->jeex_icon, &chk);
      CHECK_CORRECT_GET (chk, preferences->jeex_icon, gboolean, NULL, TRUE, NULL);
      jeex_key_file_get_value (G_KEY_BLN, kfile, "toolbar", &preferences->toolbar, &chk);
      CHECK_CORRECT_GET (chk, preferences->toolbar, gboolean, NULL, TRUE, NULL);
      jeex_key_file_get_value (G_KEY_BLN, kfile, "zero prefix", &preferences->zero_prefix, &chk);
      CHECK_CORRECT_GET (chk, preferences->zero_prefix, gboolean, NULL, TRUE, NULL);
      jeex_key_file_get_value (G_KEY_BLN, kfile, "logging", &preferences->logging, &chk);
      CHECK_CORRECT_GET (chk, preferences->logging, gboolean, NULL, TRUE, NULL);

      g_key_file_free (kfile);
      pref_check = TRUE;
    }

  apply_preferences ();
}

void *
preferences_window (GtkWidget * widget, void *data)
{
  extern GtkWidget *jeex_main_window;
  GtkWidget *vbox, *notebook, *hbutton, *generic_widget;

  if (!JPW)
    {
      JPW = (JeexPreferencesWindow *) g_malloc (sizeof (JeexPreferencesWindow));
    }

  /* Preferences Window */
  JPW->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_modal (GTK_WINDOW (JPW->window), TRUE);
  gtk_window_set_transient_for (GTK_WINDOW (JPW->window), GTK_WINDOW (jeex_main_window));
  gtk_window_set_position (GTK_WINDOW (JPW->window), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_title (GTK_WINDOW (JPW->window), _("Preferences"));
  gtk_container_set_border_width (GTK_CONTAINER (JPW->window), 10);
  gtk_window_set_default_size (GTK_WINDOW (JPW->window), 350, 380);

  /* Making Notebook Area */
  vbox = gtk_vbox_new (FALSE, 2);
  gtk_container_add (GTK_CONTAINER (JPW->window), vbox);

  generic_widget = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (generic_widget), _("<big><b>Setting your preferences</b></big>"));
  gtk_label_set_justify (GTK_LABEL (generic_widget), GTK_JUSTIFY_CENTER);
  gtk_box_pack_start (GTK_BOX (vbox), generic_widget, FALSE, TRUE, 2);

  gtk_box_pack_start (GTK_BOX (vbox), gtk_hseparator_new (), FALSE, TRUE, 2);

  notebook = gtk_notebook_new ();
  make_first_notebook_page (GTK_NOTEBOOK (notebook));
  make_second_notebook_page (GTK_NOTEBOOK (notebook));
  gtk_box_pack_start (GTK_BOX (vbox), notebook, FALSE, TRUE, 2);

  /* Button area */
  hbutton = gtk_hbutton_box_new ();
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbutton), GTK_BUTTONBOX_END);
  gtk_box_set_spacing (GTK_BOX (hbutton), 3);
  gtk_box_pack_start (GTK_BOX (vbox), hbutton, FALSE, TRUE, 0);

  /* OK Button to save the preferences */
  generic_widget = gtk_button_new_from_stock (GTK_STOCK_OK);
  gtk_box_pack_start (GTK_BOX (hbutton), generic_widget, FALSE, TRUE, 0);
  g_signal_connect (G_OBJECT (generic_widget), "clicked", G_CALLBACK (ok_preferences), NULL);

  /* CLOSE Button to close the preferences window */
  generic_widget = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  gtk_box_pack_start (GTK_BOX (hbutton), generic_widget, FALSE, TRUE, 0);
  g_signal_connect (G_OBJECT (generic_widget), "clicked", G_CALLBACK (destroy_this), JPW->window);

  gtk_widget_show_all (JPW->window);

  return NULL;
}

/* Convert the value entered in the type choosed */
static void
change_range (GtkComboBox * widget, void *data, int n)
{
  GtkSpinButton *spin = GTK_SPIN_BUTTON ((GtkWidget *) data);
  int choose, i;
  unsigned int range_max = JEEX_MAX_UNSIGNED_INT, range_current;
  choose = gtk_combo_box_get_active (widget);
  range_current = (unsigned int) gtk_spin_button_get_value (spin);

  for (i = 0; i < choose; ++i)
    range_max /= 1024;

  if (previous_combo_box_active[n] < choose)
    {
      for (i = previous_combo_box_active[n]; i < choose; ++i)
        range_current /= 1024;
    }
  else
    {
      for (i = previous_combo_box_active[n]; i > choose; --i)
        range_current *= 1024;
    }

  if (range_current < 1)
    {
      range_current = 1;
    }

  gtk_spin_button_set_range (spin, 1, range_max);
  gtk_spin_button_set_value (spin, range_current);

  previous_combo_box_active[n] = choose;
}

static void
change_range_1 (GtkComboBox * widget, void *data)
{
  change_range (widget, data, 0);
}

static void
change_range_2 (GtkComboBox * widget, void *data)
{
  change_range (widget, data, 1);
}

static void
change_range_3 (GtkComboBox * widget, void *data)
{
  change_range (widget, data, 2);
}


static void
make_first_notebook_page (GtkNotebook * notebook)
{
  GtkWidget *hbox, *vbox, *label;
  unsigned int current_value;

  vbox = gtk_vbox_new (FALSE, 2);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 5);

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>File Loading</b>"));
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 1);

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (_("\tMax file size: "));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 2);

  JPW->page1.filesize =
    gtk_spin_button_new_with_range (1, (pow (2, sizeof (unsigned int) * 8) / 1024) - 1, 1);
  current_value = preferences->max_filesize / 1024;
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (JPW->page1.filesize),
                             (current_value < 2) ? 1 : current_value - 1);
  gtk_box_pack_start (GTK_BOX (hbox), JPW->page1.filesize, FALSE, TRUE, 0);

  JPW->page1.combosize[0] = gtk_combo_box_new_text ();
  gtk_combo_box_append_text (GTK_COMBO_BOX (JPW->page1.combosize[0]), "byte");
  gtk_combo_box_append_text (GTK_COMBO_BOX (JPW->page1.combosize[0]), "kB");
  gtk_combo_box_append_text (GTK_COMBO_BOX (JPW->page1.combosize[0]), "MB");
  gtk_combo_box_set_active (GTK_COMBO_BOX (JPW->page1.combosize[0]), 1);
  gtk_box_pack_start (GTK_BOX (hbox), JPW->page1.combosize[0], FALSE, TRUE, 0);

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (_("\tMax loading buffer: "));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 2);

  JPW->page1.buffloadsize =
    gtk_spin_button_new_with_range (1, (pow (2, sizeof (unsigned int) * 8) / 1024) - 1, 1);
  current_value = preferences->max_loadbuff / 1024;
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (JPW->page1.buffloadsize),
                             (current_value < 2) ? 1 : current_value - 1);
  gtk_box_pack_start (GTK_BOX (hbox), JPW->page1.buffloadsize, FALSE, TRUE, 0);


  JPW->page1.combosize[1] = gtk_combo_box_new_text ();
  gtk_combo_box_append_text (GTK_COMBO_BOX (JPW->page1.combosize[1]), "byte");
  gtk_combo_box_append_text (GTK_COMBO_BOX (JPW->page1.combosize[1]), "kB");
  gtk_combo_box_append_text (GTK_COMBO_BOX (JPW->page1.combosize[1]), "MB");
  gtk_combo_box_set_active (GTK_COMBO_BOX (JPW->page1.combosize[1]), 1);
  gtk_box_pack_start (GTK_BOX (hbox), JPW->page1.combosize[1], FALSE, TRUE, 0);

  gtk_box_pack_start (GTK_BOX (vbox), gtk_hseparator_new (), FALSE, TRUE, 2);

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Max Elements</b>"));
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 1);

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (_("\tBookmarks: "));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 2);

  JPW->page1.bookmarks = gtk_spin_button_new_with_range (1, 30, 1);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (JPW->page1.bookmarks), preferences->max_bookmarks);
  gtk_box_pack_start (GTK_BOX (hbox), JPW->page1.bookmarks, FALSE, TRUE, 0);

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (_("\tRecent files: "));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 2);

  JPW->page1.recentfile = gtk_spin_button_new_with_range (2, 9, 1);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (JPW->page1.recentfile), preferences->max_recentfile);
  gtk_box_pack_start (GTK_BOX (hbox), JPW->page1.recentfile, FALSE, TRUE, 0);

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (_("\tChars in the tab title: "));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 2);

  JPW->page1.charstab = gtk_spin_button_new_with_range (1, 255, 1);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (JPW->page1.charstab), preferences->max_tabchars);
  gtk_box_pack_start (GTK_BOX (hbox), JPW->page1.charstab, FALSE, TRUE, 0);

  gtk_box_pack_start (GTK_BOX (vbox), gtk_hseparator_new (), FALSE, TRUE, 2);

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>File Saving</b>"));
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 1);

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (_("\tMax saving buffer: "));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 2);

  JPW->page1.buffsavesize =
    gtk_spin_button_new_with_range (1, (pow (2, sizeof (unsigned int) * 8) / 1024) - 1, 1);
  current_value = preferences->max_savebuff / 1024;
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (JPW->page1.buffsavesize),
                             (current_value < 2) ? 1 : current_value - 1);
  gtk_box_pack_start (GTK_BOX (hbox), JPW->page1.buffsavesize, FALSE, TRUE, 0);

  JPW->page1.combosize[2] = gtk_combo_box_new_text ();
  gtk_combo_box_append_text (GTK_COMBO_BOX (JPW->page1.combosize[2]), "byte");
  gtk_combo_box_append_text (GTK_COMBO_BOX (JPW->page1.combosize[2]), "kB");
  gtk_combo_box_append_text (GTK_COMBO_BOX (JPW->page1.combosize[2]), "MB");
  gtk_combo_box_set_active (GTK_COMBO_BOX (JPW->page1.combosize[2]), 1);
  gtk_box_pack_start (GTK_BOX (hbox), JPW->page1.combosize[2], FALSE, TRUE, 0);

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new ("       ");
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 2);

  JPW->page1.check_bcopy =
    gtk_check_button_new_with_label (_("Create a backup copy of files before saving"));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (JPW->page1.check_bcopy), preferences->backup_copy);
  gtk_box_pack_start (GTK_BOX (hbox), JPW->page1.check_bcopy, FALSE, TRUE, 0);

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new ("       ");
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 2);

  JPW->page1.check_logging =
    gtk_check_button_new_with_label (_("Log actions"));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (JPW->page1.check_logging), preferences->logging);
  gtk_box_pack_start (GTK_BOX (hbox), JPW->page1.check_logging, FALSE, TRUE, 0);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Editor</b>"));
  gtk_notebook_append_page (notebook, vbox, label);

  g_signal_connect (G_OBJECT (JPW->page1.combosize[0]), "changed",
                    G_CALLBACK (change_range_1), (void *) JPW->page1.filesize);
  g_signal_connect (G_OBJECT (JPW->page1.combosize[1]), "changed",
                    G_CALLBACK (change_range_2), (void *) JPW->page1.buffloadsize);
  g_signal_connect (G_OBJECT (JPW->page1.combosize[2]), "changed",
                    G_CALLBACK (change_range_3), (void *) JPW->page1.buffsavesize);
}

static void *
preferences_color_change (GtkWidget * widget, void *data)
{
  GdkColor color;
  GtkEntry *entry = GTK_ENTRY ((GtkWidget *) data);
  char *s, *t;

  gtk_color_button_get_color (GTK_COLOR_BUTTON (widget), &color);
  t = gdk_color_to_string (&color);
  s = g_strdup_printf ("#%c%c%c%c%c%c", toupper (t[1]),
                       toupper (t[2]), toupper (t[5]),
                       toupper (t[6]), toupper (t[9]),
                       toupper (t[10]));
  gtk_entry_set_text (entry, (const char *) s);
  g_free (s);

  return NULL;
}

static void
make_second_notebook_page (GtkNotebook * notebook)
{
  GtkWidget *label, *vbox, *hbox;
  GdkColor color;
  GtkObject *adj;

  vbox = gtk_vbox_new (FALSE, 2);
  gtk_container_border_width (GTK_CONTAINER (vbox), 5);

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Font &amp; Color</b>"));
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 1);

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (_("\tFont: "));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 2);

  JPW->page2.font = gtk_font_button_new_with_font (preferences->font);
  gtk_box_pack_start (GTK_BOX (hbox), JPW->page2.font, FALSE, TRUE, 0);

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (_("\tText color: "));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 2);

  JPW->page2.b_color = gtk_color_button_new ();
  gtk_button_set_relief (GTK_BUTTON (JPW->page2.b_color), GTK_RELIEF_NONE);
  gdk_color_parse (preferences->color, &color);
  gtk_color_button_set_color (GTK_COLOR_BUTTON (JPW->page2.b_color), &color);
  gtk_box_pack_start (GTK_BOX (hbox), JPW->page2.b_color, FALSE, TRUE, 2);

  JPW->page2.l_color = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (JPW->page2.l_color), preferences->color);
  gtk_widget_set_usize (JPW->page2.l_color, 80, -1);
  gtk_entry_set_editable (GTK_ENTRY (JPW->page2.l_color), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), JPW->page2.l_color, FALSE, TRUE, 0);

  gtk_box_pack_start (GTK_BOX (vbox), gtk_hseparator_new (), FALSE, TRUE, 2);

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Appearance Main Window</b>"));
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 1);

  if (support_opacity)
    {
      hbox = gtk_hbox_new (FALSE, 2);
      gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

      label = gtk_label_new (_("\tTransparency: "));
      gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 2);

      adj = gtk_adjustment_new (preferences->transparency, 10.0, 100.0, 1.0, 1.0, 1.0);

      JPW->page2.transparency = gtk_hscale_new (GTK_ADJUSTMENT (adj));
      gtk_scale_set_draw_value (GTK_SCALE (JPW->page2.transparency), TRUE);
      gtk_scale_set_value_pos (GTK_SCALE (JPW->page2.transparency), GTK_POS_RIGHT);
      gtk_box_pack_start (GTK_BOX (hbox), JPW->page2.transparency, TRUE, TRUE, 2);
    }

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (_("\tTheme: "));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 2);

  JPW->page2.theme = gtk_combo_box_new_text ();
  gtk_combo_box_append_text (GTK_COMBO_BOX (JPW->page2.theme), _("Blue Theme"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (JPW->page2.theme), _("Red Theme"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (JPW->page2.theme), _("Dark Red Theme"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (JPW->page2.theme), _("Green Theme"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (JPW->page2.theme), _("White Theme"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (JPW->page2.theme), _("Default Gtk+ Theme"));
  gtk_combo_box_set_active (GTK_COMBO_BOX (JPW->page2.theme), preferences->theme);
  gtk_box_pack_start (GTK_BOX (hbox), JPW->page2.theme, FALSE, TRUE, 0);

  gtk_box_pack_start (GTK_BOX (vbox), gtk_hseparator_new (), FALSE, TRUE, 2);

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Jeex Elements</b>"));
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 1);

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new ("       ");
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 2);

  JPW->page2.check_insert_mode = gtk_check_button_new_with_label (_("Cursor in text insert mode"));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON
                                (JPW->page2.check_insert_mode), preferences->cursor_mode);
  gtk_box_pack_start (GTK_BOX (hbox), JPW->page2.check_insert_mode, FALSE, TRUE, 0);

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new ("       ");
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 2);

  JPW->page2.check_systray_icon = gtk_check_button_new_with_label (_("Show jeex icon in systray"));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON
                                (JPW->page2.check_systray_icon), preferences->jeex_icon);
  gtk_box_pack_start (GTK_BOX (hbox), JPW->page2.check_systray_icon, FALSE, TRUE, 0);

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new ("       ");
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 2);

  JPW->page2.check_toolbar = gtk_check_button_new_with_label (_("Show toolbar"));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (JPW->page2.check_toolbar), preferences->toolbar);
  gtk_box_pack_start (GTK_BOX (hbox), JPW->page2.check_toolbar, FALSE, TRUE, 0);

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new ("       ");
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 2);

  JPW->page2.check_prefix =
    gtk_check_button_new_with_label (_("Show 0x prefix in the byte hexadecimal value"));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (JPW->page2.check_prefix), preferences->zero_prefix);
  gtk_box_pack_start (GTK_BOX (hbox), JPW->page2.check_prefix, FALSE, TRUE, 0);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Appearance</b>"));
  gtk_notebook_append_page (notebook, vbox, label);

  g_signal_connect (G_OBJECT (JPW->page2.b_color), "color-set",
                    G_CALLBACK (preferences_color_change), (void *) JPW->page2.l_color);
}

static void
gets_preferences (void)
{
  GtkSpinButton *spin;
  GtkToggleButton *toggle;
  int i;

  spin = GTK_SPIN_BUTTON (JPW->page1.filesize);
  preferences->max_filesize = (unsigned int) gtk_spin_button_get_value (spin);
  for (i = previous_combo_box_active[0]; i > 0; --i)
    preferences->max_filesize *= 1024;

  spin = GTK_SPIN_BUTTON (JPW->page1.buffloadsize);
  preferences->max_loadbuff = (unsigned int) gtk_spin_button_get_value (spin);
  for (i = previous_combo_box_active[1]; i > 0; --i)
    preferences->max_loadbuff *= 1024;

  spin = GTK_SPIN_BUTTON (JPW->page1.buffsavesize);
  preferences->max_savebuff = (unsigned int) gtk_spin_button_get_value (spin);
  for (i = previous_combo_box_active[2]; i > 0; --i)
    preferences->max_savebuff *= 1024;

  spin = GTK_SPIN_BUTTON (JPW->page1.bookmarks);
  preferences->max_bookmarks = (int) gtk_spin_button_get_value (spin);

  spin = GTK_SPIN_BUTTON (JPW->page1.recentfile);
  preferences->max_recentfile = (int) gtk_spin_button_get_value (spin);

  spin = GTK_SPIN_BUTTON (JPW->page1.charstab);
  preferences->max_tabchars = (int) gtk_spin_button_get_value (spin);

  toggle = GTK_TOGGLE_BUTTON (JPW->page1.check_bcopy);
  preferences->backup_copy = gtk_toggle_button_get_active (toggle);

  toggle = GTK_TOGGLE_BUTTON (JPW->page1.check_logging);
  preferences->logging = gtk_toggle_button_get_active (toggle);

  g_free (preferences->font);
  preferences->font = g_strdup (gtk_font_button_get_font_name (GTK_FONT_BUTTON (JPW->page2.font)));

  g_free (preferences->color);
  preferences->color = g_strdup (gtk_entry_get_text (GTK_ENTRY (JPW->page2.l_color)));

  if (support_opacity)
    {
      preferences->transparency = gtk_range_get_value (GTK_RANGE (JPW->page2.transparency));
    }

  preferences->theme = gtk_combo_box_get_active (GTK_COMBO_BOX (JPW->page2.theme));

  toggle = GTK_TOGGLE_BUTTON (JPW->page2.check_insert_mode);
  preferences->cursor_mode = gtk_toggle_button_get_active (toggle);

  toggle = GTK_TOGGLE_BUTTON (JPW->page2.check_systray_icon);
  preferences->jeex_icon = gtk_toggle_button_get_active (toggle);

  toggle = GTK_TOGGLE_BUTTON (JPW->page2.check_toolbar);
  preferences->toolbar = gtk_toggle_button_get_active (toggle);

  toggle = GTK_TOGGLE_BUTTON (JPW->page2.check_prefix);
  preferences->zero_prefix = gtk_toggle_button_get_active (toggle);
}

static void
apply_preferences (void)
{
  int tot, i;
  char *text;
  FILE *rc;
  GFileError error_type;
  GFile *system_conf, *user_conf;
  char *style_string[] = { "wblue", "wred", "wdarkred", "wgreen", "wwhite", "wdefault" };
  extern GtkWidget *jeex_main_window;
  extern JeexToolbar *jeex_toolbar;

  tot = gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook->notebook));

  gtk_status_icon_set_visible (sysicon, preferences->jeex_icon);

  if (preferences->toolbar && GTK_IS_WIDGET (jeex_toolbar->toolbar_widget))
    {
      gtk_widget_show (jeex_toolbar->toolbar_widget);
    }
  else if (GTK_IS_WIDGET (jeex_toolbar->toolbar_widget))
    {
      gtk_widget_hide (jeex_toolbar->toolbar_widget);
    }

  if ( !preferences->logging )
       logging_unload ();
  else logging_load ();

  textview->font = pango_font_description_from_string (preferences->font);

  gdk_color_parse (preferences->color, &(textview->color));

  for (i = 0; i < tot; ++i)
    {
      gtk_text_view_set_overwrite (GTK_TEXT_VIEW (notebook->textview[i]), preferences->cursor_mode);
      gtk_widget_modify_font (notebook->textview[i], textview->font);
      gtk_widget_modify_text (textview->textview, GTK_STATE_NORMAL, &(textview->color));
    }

  if (support_opacity)
    gtk_window_set_opacity (GTK_WINDOW (jeex_main_window), preferences->transparency / 100);


  text = g_strdup_printf ("%s/.config/jeex/jeex.rc", g_get_home_dir());
  if (!g_file_test(text, G_FILE_TEST_EXISTS))
    {
      system_conf = g_file_new_for_path (g_strdup_printf ("%s/jeex.rc", JEEX_PATH_CONF));
      user_conf = g_file_new_for_path (text);
      g_file_copy (system_conf, user_conf, 0, NULL, NULL, NULL, NULL);

      g_object_unref(system_conf);
      g_object_unref(user_conf);
    }

  if (!(rc = fopen (text, "r+")))
    {
      error_type = g_file_error_from_errno (errno);
      file_error_control (error_type, RB_MODE);
    }
  else
    {
      fseek (rc, 0x554, SEEK_SET);
      fprintf (rc, "\nwidget \"window.*\" style \"%s\"          ", style_string[preferences->theme]);
      fclose (rc);
    }
  g_free (text);
}

static void
ok_preferences (GtkButton * button, void *data)
{
  gets_preferences ();

  apply_preferences ();

  write_new_preferences ();

  gtk_widget_destroy (JPW->window);
  g_free (JPW);
  JPW = NULL;
}
