/*
 * "html-export.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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
#include <ctype.h>
#include <string.h>
#include <strings.h>
#include <glib/gi18n.h>
#include <stdlib.h>
#include <libintl.h>
#include <locale.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "header.h"

#define HTML_NOT_CORRISPONDENT(c) ( c != 0x22 && c != 0x26 && \
                                    c != 0x3C && c != 0x3E && \
                                    c != 0x0A )

#define html_free_header(header) \
        g_free (header.start);   \
        g_free (header.css);     \
        g_free (header.title);

struct header
{
  char *start;
  char *css;
  char *title;
};

typedef struct
{
  GtkWidget *window;
  GtkWidget *outfilename[2];
  GtkWidget *rows_for_page;
  GtkWidget *ascii_ext;
  GtkWidget *font_size;
  GtkWidget *bg_color[2];
  GtkWidget *b_color[2];
  GtkWidget *border_width;
  GtkWidget *border_style;
} JHtmlExportPageWidget;

static void html_init_header (struct header *, JHtmlExportPage *, char *);
static gboolean html_generate_page (struct header *, char *, char *,
                                    unsigned char *, int, int, int, int, int, JHtmlExportPage *);
static void *html_window_get_setting (GtkWidget *, void *);

static void *
html_color_change (GtkWidget * widget, void *data)
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

/* Function to show a window where user can
 * choose how to export in html
 */
void *
html_export_window (GtkWidget * widget, void *data)
{
  GtkWidget *label, *vbox, *hbox, *button;
  JHtmlExportPageWidget *j;
  GdkColor color;
  extern GtkWidget *jeex_main_window;
  extern JeexFileInfo *file[64];

  j = (JHtmlExportPageWidget *) g_malloc (sizeof (JHtmlExportPageWidget));

  /* Window */
  j->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_transient_for (GTK_WINDOW (j->window), GTK_WINDOW (jeex_main_window));
  gtk_window_set_position (GTK_WINDOW (j->window), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_modal (GTK_WINDOW (j->window), TRUE);
  gtk_window_set_default_size (GTK_WINDOW (j->window), 465, -1);
  gtk_window_set_title (GTK_WINDOW (j->window), _("Html Export"));
  gtk_container_set_border_width (GTK_CONTAINER (j->window), 5);

  /* Vertical Box ( the main box ) */
  vbox = gtk_vbox_new (FALSE, 2);
  gtk_container_add (GTK_CONTAINER (j->window), vbox);

  /* Title */
  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<big><b>HTML Export</b></big>\n"
                                             "<i>Dialog to setting the preferences</i>"));
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_CENTER);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, TRUE, 4);

  gtk_box_pack_start (GTK_BOX (vbox), gtk_hseparator_new (), FALSE, TRUE, 2);

  /* Output Filename Row */
  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Output Folder:</b>"));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);

  j->outfilename[0] =
    gtk_file_chooser_button_new (_("Choose File..."), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
  gtk_box_pack_start (GTK_BOX (hbox), j->outfilename[0], TRUE, TRUE, 2);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Name:</b>"));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 2);

  j->outfilename[1] = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (j->outfilename[1]), g_path_get_basename (_ncurrent (file)->name));
  gtk_box_pack_start (GTK_BOX (hbox), j->outfilename[1], TRUE, TRUE, 2);

  gtk_box_pack_start (GTK_BOX (vbox), gtk_hseparator_new (), FALSE, TRUE, 2);

  /* Title */
  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<big><b>Page Style</b></big>\n"));
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, TRUE, 4);

  /* Row "Rows for Page" */
  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Rows for page:</b>\t\t\t\t"));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);

  j->rows_for_page = gtk_spin_button_new_with_range (16, 2048, 16);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (j->rows_for_page), 64);
  gtk_box_pack_start (GTK_BOX (hbox), j->rows_for_page, FALSE, TRUE, 4);

  /* Row to choose the font size */
  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Font Size:</b>\t\t\t\t\t"));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);

  j->font_size = gtk_spin_button_new_with_range (2, 72, 1);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (j->font_size), 11);
  gtk_box_pack_start (GTK_BOX (hbox), j->font_size, FALSE, TRUE, 4);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _(" <i>pixel</i>"));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);

  /* Row to enable or disable the viewing of extended ascii */
  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Show extended ascii characters:</b>\t"));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);

  j->ascii_ext = gtk_combo_box_new_text ();
  gtk_combo_box_append_text (GTK_COMBO_BOX (j->ascii_ext), _("Disable"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (j->ascii_ext), _("Enable"));
  gtk_combo_box_set_active (GTK_COMBO_BOX (j->ascii_ext), 0);
  gtk_box_pack_start (GTK_BOX (hbox), j->ascii_ext, FALSE, TRUE, 4);

  gtk_box_pack_start (GTK_BOX (vbox), gtk_hseparator_new (), FALSE, TRUE, 2);

  /* Row to choose the border width */
  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Border width:</b>\t\t\t\t\t"));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);

  j->border_width = gtk_spin_button_new_with_range (0, 20, 1);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON (j->border_width), 2);
  gtk_box_pack_start (GTK_BOX (hbox), j->border_width, FALSE, TRUE, 4);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _(" <i>pixel</i>"));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);

  /* Row to choose the border style */
  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Border style:</b>\t\t\t\t\t"));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);

  j->border_style = gtk_combo_box_new_text ();
  gtk_combo_box_append_text (GTK_COMBO_BOX (j->border_style), "none");
  gtk_combo_box_append_text (GTK_COMBO_BOX (j->border_style), "hidden");
  gtk_combo_box_append_text (GTK_COMBO_BOX (j->border_style), "dotted");
  gtk_combo_box_append_text (GTK_COMBO_BOX (j->border_style), "dashed");
  gtk_combo_box_append_text (GTK_COMBO_BOX (j->border_style), "solid");
  gtk_combo_box_append_text (GTK_COMBO_BOX (j->border_style), "double");
  gtk_combo_box_append_text (GTK_COMBO_BOX (j->border_style), "groove");
  gtk_combo_box_append_text (GTK_COMBO_BOX (j->border_style), "ridge");
  gtk_combo_box_append_text (GTK_COMBO_BOX (j->border_style), "inset");
  gtk_combo_box_append_text (GTK_COMBO_BOX (j->border_style), "outset");
  gtk_combo_box_set_active (GTK_COMBO_BOX (j->border_style), 4);
  gtk_box_pack_start (GTK_BOX (hbox), j->border_style, FALSE, TRUE, 4);

  gtk_box_pack_start (GTK_BOX (vbox), gtk_hseparator_new (), FALSE, TRUE, 2);

  /* Row to choose the background color */
  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Font color:</b>\t\t\t\t\t"));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);

  j->bg_color[0] = gtk_color_button_new ();
  gdk_color_parse ("#000000", &color);
  gtk_color_button_set_color (GTK_COLOR_BUTTON (j->bg_color[0]), &color);
  gtk_box_pack_start (GTK_BOX (hbox), j->bg_color[0], FALSE, TRUE, 4);

  j->bg_color[1] = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (j->bg_color[1]), "#000000");
  gtk_widget_set_size_request (j->bg_color[1], 60, -1);
  gtk_editable_set_editable (GTK_EDITABLE (j->bg_color[1]), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), j->bg_color[1], FALSE, TRUE, 2);

  g_signal_connect (G_OBJECT (j->bg_color[0]), "color-set",
                    G_CALLBACK (html_color_change), (void *) j->bg_color[1]);

  /* Row to choose the border color */
  hbox = gtk_hbox_new (FALSE, 2);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);

  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), _("<b>Border color:</b>\t\t\t\t\t"));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);

  j->b_color[0] = gtk_color_button_new ();
  gdk_color_parse ("#990000", &color);
  gtk_color_button_set_color (GTK_COLOR_BUTTON (j->b_color[0]), &color);
  gtk_box_pack_start (GTK_BOX (hbox), j->b_color[0], FALSE, TRUE, 4);

  j->b_color[1] = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (j->b_color[1]), "#990000");
  gtk_widget_set_size_request (j->b_color[1], 60, -1);
  gtk_editable_set_editable (GTK_EDITABLE (j->b_color[1]), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), j->b_color[1], FALSE, TRUE, 2);

  g_signal_connect (G_OBJECT (j->b_color[0]), "color-set",
                    G_CALLBACK (html_color_change), (void *) j->b_color[1]);

  gtk_box_pack_start (GTK_BOX (vbox), gtk_hseparator_new (), FALSE, TRUE, 2);

  /* Button */
  hbox = gtk_hbutton_box_new ();
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbox), GTK_BUTTONBOX_END);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);
  gtk_box_set_spacing (GTK_BOX (hbox), 2);

  button = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
  gtk_container_add (GTK_CONTAINER (hbox), button);

  g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (destroy_this), (void *) j->window);

  button = gtk_button_new_from_stock (GTK_STOCK_OK);
  gtk_container_add (GTK_CONTAINER (hbox), button);

  g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (html_window_get_setting), (void *) j);

  gtk_widget_show_all (j->window);

  return NULL;
}

static void *
html_window_get_setting (GtkWidget * widget, void *data)
{
  char *outfilename, *t;
  unsigned char *_data;
  gboolean chk;
  JHtmlExportPage jhep;
  JHtmlExportPageWidget *j = (JHtmlExportPageWidget *) data;
  extern JeexFileInfo *file[64];
  int len, l;

  t = (char *) gtk_entry_get_text (GTK_ENTRY (j->outfilename[1]));
  if (!t || *t == '\0')
    {
      _error (_("<big><b>Output Filename is empty!</b></big>\n"));
      return NULL;
    }
  if ((len = strlen (t)) > 5 && fthx ((unsigned char *) t + (len - 5), ".html"))
    {
      outfilename =
        g_strdup_printf ("%s/%s",
                         gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER (j->outfilename[0])), t);
    }
  else
    {
      outfilename =
        g_strdup_printf ("%s/%s.html",
                         gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER (j->outfilename[0])), t);
    }

  _data = (unsigned char *) ohfa (&len);
  jhep.num = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (j->rows_for_page));

  jhep.border.style = gtk_combo_box_get_active (GTK_COMBO_BOX (j->border_style));

  jhep.border.width = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (j->border_width));

  jhep.font_size = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (j->font_size));

  l = gtk_combo_box_get_active (GTK_COMBO_BOX (j->ascii_ext));
  jhep.ascii_ext = l ? TRUE : FALSE;

  t = (char *) gtk_entry_get_text (GTK_ENTRY (j->bg_color[1]));
  jhep.color.background = g_strdup (t);

  t = (char *) gtk_entry_get_text (GTK_ENTRY (j->b_color[1]));
  jhep.color.border = g_strdup (t);

  gtk_widget_destroy (j->window);
  g_free (j);

  if ((len / (jhep.num * 16)) > 500)
    {
      chk =
        _info (_
               ("If you press OK, after this message,\n"
                "jeex maked more 500 files!\n" "\n" "Continue?"), TRUE);
      if (chk)
        {
          logging_action (_("Exported `%s' to HTML `%s'\n"), _ncurrent (file)->name, outfilename);
          html_export (_ncurrent (file)->name, outfilename, _data, len, &jhep);
        }
    }
  else
    {
      logging_action (_("Exported `%s' to HTML `%s'\n"), _ncurrent (file)->name, outfilename);
      html_export (_ncurrent (file)->name, outfilename, _data, len, &jhep);
    }

  return NULL;
}

/* Main function to export in html formt */
gboolean
html_export (char *filename, char *outfilename, unsigned char *data, int len, JHtmlExportPage * jhep)
{
  static struct header header;
  gboolean chk;

  /* Get html page to generate */
  unsigned int page_num = len / (jhep->num * 16);
  int i, page_current = 1;

  html_init_header (&header, jhep, filename);

  for (i = 0; i < len; i += (jhep->num * 16))
    {
      chk =
        html_generate_page (&header, filename, outfilename, data,
                            page_current, page_num, i, len, (int) jhep->num * 16, jhep);
      if (!chk)
        {
          html_free_header (header);
          return FALSE;
        }
      page_current++;
    }

  return TRUE;
}

/* Initialize the html header for all generated pages */
static void
html_init_header (struct header *h, JHtmlExportPage * jhep, char *filename)
{
  char **t = inap (filename);
  static const char *style[] = {
    "none", "hidden", "dotted", "dashed",
    "solid", "double", "groove", "ridge",
    "inset", "outset"
  };

  h->start =
    g_strdup
    ("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n"
     "            \"http://www.w3.org/TR/html4/loose.dtd\">\n" "<html>\n<head>\n");

  h->css =
    g_strdup_printf ("<style type=\"text/css\">\n" "<!--\n" " .jeex-html {\n"
                     "              font_size:   %dpx;\n"
                     "              font-family: Monospace;\n"
                     "              color: %s;\n"
                     "              border-color: %s;\n"
                     "              border-style: %s;\n"
                     "              border-width: %dpx;\n" " }\n" "-->\n"
                     "</style>\n", jhep->font_size, jhep->color.background,
                     jhep->color.border, style[jhep->border.style], jhep->border.width);

  h->title =
    g_strdup_printf ("<title>%s `%s' ( %s ) - " Jeex "</title>\n", _("Hexadecimal Dump of"), t[0], t[1]);

  g_free (t[0]);
  g_free (t[1]);
  g_free (t);
}

static char *
html_char (unsigned char c, gboolean ext)
{
  char *t;
  static const char *html_code[] = {
    "nbsp", "iexcl", "cent", "pound",
    "curren", "yen", "brvbar", "sect",
    "uml", "copy", "ordf", "laquo",
    "not", "shy", "reg", "macr", "deg",
    "plusmn", "sup2", "sup3", "acute",
    "micro", "para", "middot", "cedil",
    "sup1", "ordm", "raquo", "frac14",
    "frac12", "frac34", "iquest",
    "Agrave", "Aacute", "Acirc", "Atilde",
    "Auml", "Aring", "AElig", "Ccedil",
    "Egrave", "Eacute", "Ecirc", "Euml",
    "Igrave", "Iacute", "Icirc", "Iuml",
    "ETH", "Ntilde", "Ograve", "Oacute",
    "Ocirc", "Otilde", "Ouml", "times",
    "Oslash", "Ugrave", "Uacute", "Ucirc",
    "Uuml", "Yacute", "THORN", "szlig",
    "agrave", "aacute", "acirc", "atilde",
    "auml", "aring", "aelig", "ccedil",
    "egrave", "eacute", "ecirc", "euml",
    "igrave", "iacute", "icirc", "iuml",
    "eth", "ntilde", "ograve", "oacute",
    "ocirc", "otilde", "ouml", "divide",
    "oslash", "ugrave", "uacute", "ucirc",
    "uuml", "yacute", "thorn", "yuml"
  };

  if (g_ascii_isgraph ((char) c) && HTML_NOT_CORRISPONDENT (c))
    {
      return g_strdup_printf ("%c", c);
    }

  switch (c)
    {
    case 0x20:
      t = g_strdup ("&nbsp;");
      break;
    case 0x22:
      t = g_strdup ("&quot;");
      break;
    case 0x26:
      t = g_strdup ("&amp;");
      break;
    case 0x3C:
      t = g_strdup ("&lt;");
      break;
    case 0x3E:
      t = g_strdup ("&gt;");
      break;
    default:
      if (c > 0x9F && ext)
        {
          t = g_strdup_printf ("&%s;", html_code[c - 0xA0]);
        }
      else
        {
          t = g_strdup (".");
        }
      break;
    }

  return t;
}

static gboolean
html_generate_page (struct header *h, char *filename, char *outfilename,
                    unsigned char *data, int current_page, int page_num,
                    int start, int end, int byte_for_page, JHtmlExportPage * j)
{
  int tot = ((start + byte_for_page) > end) ? end : start + byte_for_page;
  int i, len;
  FILE *of;
  char *tmp, *t, *_end, **d, timebuf[20];
  time_t timer = time (NULL);
  struct tm *now = localtime (&timer);

  /* Generate page name */
  d = g_malloc (2 * sizeof (char *));
  d[0] = g_path_get_basename (outfilename);
  d[1] = g_path_get_dirname (outfilename);
  len = strlen (*d) - 5;
  t = g_malloc (len + 1);
  for (i = 0; i < len; ++i)
    {
      t[i] = (*d)[i];
    }
  t[i] = '\0';
  g_free (*d);
  tmp = g_strdup_printf ("%s/%s_%d.html", !strcmp (d[1], "/") ? "" : d[1], t, current_page);
  g_free (d[1]);
  g_free (d);
  g_free (t);

  if (!(of = fopen (tmp, "wb")))
    {
      file_error_control (errno, WB_MODE);
      g_free (tmp);
      return FALSE;
    }

  g_free (tmp);

  /* Writing headers.. */
  d = inap (filename);
  _end =
    g_strdup_printf ("</head>\n<body>\n"
                     "<h1><div align=center>%s - %s %d/%d</div></h1>\n"
                     "<table width=460 class=\"jeex-html\" cellspacing=15 align=center>\n"
                     "<tr>\n", *d, _("Page"), current_page, page_num + 1);
  g_free (*d);
  g_free (d[1]);
  g_free (d);

  tmp = g_strdup_printf ("%s%s%s%s<td>\n", h->start, h->css, h->title, _end);
  g_free (_end);

  fwrite (tmp, sizeof (char), strlen (tmp), of);
  g_free (tmp);

  /* Writing offset */
  for (i = start; i < tot; ++i)
    {
      if ((i != start && !(((i + 1) - start) % 16)) || i == end - 1)
        {
          tmp = g_strdup_printf ("<i>%8.8X</i><br>\n", (i + 1));
          fwrite (tmp, sizeof (char), strlen (tmp), of);
          g_free (tmp);
        }
    }

  fwrite ("</td>\n<td>\n", sizeof (char), 11, of);

  /* Writing hexadecimal byte */
  for (i = start; i < tot; ++i)
    {
      if (i != start && !(((i + 1) - start) % 16) && i != (tot - 1))
        {
          tmp = g_strdup_printf ("%.2X<br>\n", data[i]);
        }
      else
        {
          tmp = g_strdup_printf ("%.2X&nbsp;", data[i]);
        }
      fwrite (tmp, sizeof (char), strlen (tmp), of);
      g_free (tmp);
    }

  fwrite ("\n</td>\n<td>\n", sizeof (char), 12, of);

  /* Writing characters */
  for (i = start; i < tot; ++i)
    {
      t = html_char (data[i], j->ascii_ext);
      if (i != start && !(((i + 1) - start) % 16) && i != (tot - 1))
        {
          tmp = g_strdup_printf ("%s<br>\n", t);
        }
      else
        {
          tmp = g_strdup_printf ("%s", t);
        }
      g_free (t);
      fwrite (tmp, sizeof (char), strlen (tmp), of);
      g_free (tmp);
    }

  fwrite ("\n</td>\n</tr>\n</table><br><br>\n"
          "<div style=\"font-family: Monospace\">", sizeof (char), 66, of);

  /* Previous Page */
  if (current_page > 1)
    {
      t = g_strdup (outfilename);
      t[strlen (t) - 5] = '\0';
      tmp =
        g_strdup_printf ("<a href=\"%s_%d.html\">%s</a>&nbsp;", t, current_page - 1, _("Previous Page"));
      g_free (t);
      fwrite (tmp, sizeof (char), strlen (tmp), of);
      g_free (tmp);
    }

  /* Next Page */
  if (current_page <= page_num)
    {
      t = g_strdup (outfilename);
      t[strlen (t) - 5] = '\0';
      tmp = g_strdup_printf ("<a href=\"%s_%d.html\">%s</a>", t, current_page + 1, _("Next Page"));
      g_free (t);
      fwrite (tmp, sizeof (char), strlen (tmp), of);
      g_free (tmp);
    }

  /* Gets current date and time */
  strftime (timebuf, sizeof (timebuf), _("%d/%m/%Y %X"), now);
  tmp =
    g_strdup_printf
    ("</div><div style=\"font-family: Monospace\"><i>%s <b>%s</b></i>", _("Generate Date:"), timebuf);
  fwrite (tmp, sizeof (char), strlen (tmp), of);
  g_free (tmp);

  tmp = g_strdup ("</font>\n<br><br>\n</body>\n</html>");
  fwrite (tmp, sizeof (char), 32, of);

  fclose (of);

  return TRUE;
}
