/*
 * "header.h" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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
#define MALLOC_CHECK_ 1
#define gettext_noop(String) String

typedef enum
{ RB_MODE = 0, WB_MODE } FileMode;

typedef enum
{ HEX, OCT, DEC, BIN, ASCII } StringFormat;


typedef enum jeex_type_code
{
  TYPE_CHAR = 0,
  TYPE_INT,
  TYPE_DOUBLE,
  TYPE_FLOAT,
  TYPE_LONG,
  TYPE_UNSIGNED,
  TYPE_SHORT,
  TYPE_SIGNED
} JeexTypeCode;

typedef struct GtkTextInsert
{
  GtkWidget *table;
  GtkWidget *b_close, *i_close;
  GtkWidget *l_insert, *e_insert, *b_insert, *i_insert;
  GtkWidget *o_hex, *o_oct, *o_str, *o_bin, *o_dec;
  int format;
} GtkTextInsert;

typedef struct GtkTextRegex
{
  GtkWidget *table;
  GtkWidget *b_close, *i_close;
  GtkWidget *l_regex, *e_regex;
  GtkWidget *b_find, *i_find;
  GtkWidget *b_clear, *i_clear;
} GtkTextRegex;

typedef struct
{
  int num;
  gboolean ascii_ext;
  struct
  {
    char *background;
    char *border;
  } color;
  struct
  {
    int width;
    int style;
  } border;
  int font_size;
} JHtmlExportPage;

/* Structure for bookmarks manage */
struct JeexBookmark
{
  int n;
  int *addr;
  char **str;
  char **tips;
  GtkWidget **bookmark;
};

struct show_file_struct
{
  GtkWidget *main_widget;
  GtkWidget *e_types, *b_types, *error_label;
  GtkWidget *textview, *scrolled_window;
  GtkTextBuffer *buffer;
  GtkTextTag *syntax_highlight[4];
  GtkWidget *b_prev, *b_next, *label;
};

typedef struct _toolbar
{
  GtkWidget *toolbar_widget;
  GtkToolItem *new_file;
  GtkToolItem *open_file;
  GtkToolItem *save_file;
  GtkToolItem *cut;
  GtkToolItem *copy;
  GtkToolItem *paste;
  GtkToolItem *search;
  GtkToolItem *replace;
  GtkToolItem *separator;
  struct
  {
    GtkWidget *menu;
  } menu;
} JeexToolbar;

#include "edit.h"
#include "file.h"
#include "menu.h"
#include "search.h"
#include "general-function.h"
#include "notebook-manage.h"
#include "html-export.h"
#include "info.h"
#include "preferences.h"
#include "view.h"
#include "bookmark.h"
#include "logging.h"
#include "terminal-actions.h"
#include "tools.h"
#include "plugin.h"

#define _NAME           "Jeex"
#define _VER            "12.6.1"
#define Jeex            _NAME" "_VER
#define LINK_NAME        "jeex"

#define PACKAGE         "jeex"
#ifndef DESTDIR
# define DESTDIR
#endif

#define JEEX_PATH_BIN   DESTDIR "/usr/bin"
#ifdef SYSCONFDIR
# define JEEX_PATH_CONF  SYSCONFDIR 
#else
# define JEEX_PATH_CONF  DESTDIR "/etc/jeex"
#endif
#ifdef DATADIR
# define JEEX_PATH_IMG DATADIR 
#else
# define JEEX_PATH_IMG   DESTDIR "/usr/share/jeex"
#endif
#ifndef LOCALEDIR
# define LOCALEDIR       DESTDIR "/usr/share/locale"
#endif

#define return_val_if_fail(condition, val) if ( !(condition) ) { \
                                                 return(val); \
                                            }
#define return_if_fail(condition) if ( !(condition) ) { \
                                       return; \
                                  }
#define jeex_widget_hide(widget) \
        if ( GTK_IS_WIDGET (widget) && GTK_WIDGET_VISIBLE (widget) ) { \
             gtk_widget_hide (widget); \
        }
#define jeex_widget_show(widget) \
        if ( GTK_IS_WIDGET (widget) && !GTK_WIDGET_VISIBLE (widget) ) { \
             gtk_widget_show (widget); \
        }

/* Structure for byte info manage */
struct info_byte
{
  GtkWidget *l_gint8, *e_gint8;
  GtkWidget *l_guint8, *e_guint8;
  GtkWidget *l_ascii, *e_ascii;
  GtkWidget *l_binary, *e_binary;
  GtkWidget *l_octal, *e_octal;
  GtkWidget *l_hex, *e_hex;
} byte_info;

/* Structure for textview manage */
typedef struct info_textview
{
  GtkTextBuffer *buffer;
  GdkColor color;
  PangoFontDescription *font;
  GtkTextTag *tag;
  gboolean find_type;
  GtkTextInsert *insert;
  GtkTextRegex *regex;
  GtkWidget *textview;
} textview_info;

textview_info *textview;

/* These widget are used in the main window. */
GtkWidget *bar, *viewport, *table_info;
GtkStatusIcon *sysicon;

/* Structure for autocompletion manage */
struct _ac
{
  GtkListStore *list;
  GtkTreeIter iter;
  GSList *string_list;
    gboolean (*string_exist) (char *);
  void (*add_string) (char *);
  int (*strcasecmp) (gconstpointer a, gconstpointer b);
} *ac;

typedef struct _jeex_types {
          GtkWidget *main_window;
          JeexPreferences *preferences;
          JeexFileInfo **file;
          JeexFileRecent *file_recent;
          JeexMenu *menu;
          GtkWidget *main_window_panel;
          JeexToolbar *toolbar;
          struct jeex_notebook *notebook;
          JeexLog *log;
        } JeexTypes;

typedef struct _plugin_list {
          char *plugin_name;
          GtkWidget *menu_widget;
          gboolean (*start_plugin_function) (JeexTypes *);
          void (*open_file_plugin_signal) (void);
          void (*close_file_plugin_signal) (void);
          void (*change_tab_plugin_signal) (void);
          void (*end_plugin_function) (gboolean);
        } PluginList;
