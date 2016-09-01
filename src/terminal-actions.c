/*
 * "terminal-actions.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#include <stdio.h>
#include <ctype.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <libintl.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "header.h"

#define xrand(min,max) \
        ((rand () % ((max + 1) - (min))) + min)

static char *get_filename_and_type (char *, char ***);
static gboolean read_files_structure (FILE *, int, JeexTypeCode *);

gboolean
show_bytes (char *file)
{
  FILE *of;
  unsigned char *data;
  int file_len, i = 0, c, t;

  g_return_val_if_fail (file, FALSE);

  if (!(of = fopen (file, "rb")))
    {
      g_printerr (_("Cannot open stream.\n"));
      return FALSE;
    }
  g_print (_("Reading Bytes...\n"));
  fseek (of, 0, SEEK_END);
  data = (unsigned char *) g_malloc ((file_len = ftell (of)) * sizeof (unsigned char));
  rewind (of);
  fread (data, sizeof (char), file_len, of);
  fclose (of);

  g_print (_("Formatting Bytes...\n\n"));

  while (i < file_len)
    {
      printf ("%8.8X | ", i + 16);
      for (c = (t = i) + 16; i < c && i < file_len; i++)
        printf ("%.2X ", data[i]);
      if (i == file_len)
        while (i++ < c)
          printf ("   ");
      printf (" | ");
      for (i = t; i < c && i < file_len; i++)
        printf ("%c", isprint (data[i]) ? data[i] : '.');
      printf ("\n");
    }
  free (data);

  g_print ("\n\n%s " Jeex "\n", _("Generated with"));

  return TRUE;
}

gboolean
show_struct (char *file_and_type)
{
  FILE *of;
  char *filename, **type = NULL;
  int size_types = 0, dlen, i;
  JeexTypeCode *type_code;

  g_return_val_if_fail (file_and_type, FALSE);

  if (!(filename = get_filename_and_type (file_and_type, &type)))
    return FALSE;

  if (!check_valid_type (type, &type_code))
    {
      g_free (filename);
      g_strfreev (type);

      return FALSE;
    }
  g_strfreev (type);

  if (!(of = fopen (filename, "rb")))
    {
      g_printerr (_("Cannot open stream.\n"));
      g_free (filename);

      return FALSE;
    }
  fseek (of, 0, SEEK_END);
  dlen = ftell (of);
  rewind (of);

  for (i = 0; type_code[i] != -1; i++)
    {
      switch (type_code[i])
        {
        case TYPE_CHAR:
          size_types += sizeof (char);
          break;
        case TYPE_INT:
          size_types += sizeof (int);
          break;
        case TYPE_DOUBLE:
          size_types += sizeof (double);
          break;
        case TYPE_FLOAT:
          size_types += sizeof (float);
          break;
        case TYPE_LONG:
          size_types += sizeof (long);
          break;
        case TYPE_UNSIGNED:
          size_types += sizeof (unsigned);
          break;
        case TYPE_SHORT:
          size_types += sizeof (short);
          break;
        case TYPE_SIGNED:
          size_types += sizeof (signed);
          break;
        }
    }

  if (dlen % size_types)
    {
      g_printerr (_("Not Valid Structure.\n"));
      g_free (filename);
      fclose (of);

      return FALSE;
    }

  if (!read_files_structure (of, dlen, type_code))
    {
      free (type_code);
      fclose (of);

      return FALSE;
    }

  free (type_code);
  fclose (of);

  return TRUE;
}

static char *
get_filename_and_type (char *str, char ***p_type)
{
  char *filename;
  int i, len = strlen (str);

  /* Validating String.. */
  if (!g_regex_match_simple ("^[^\t\n\r\f\v]+:[a-z,]+[a-z]$", str, 0, 0))
    {
      g_printerr (_("Not valid argument.\n"));
      return NULL;
    }

  filename = g_strndup (str, len - (i = strlen (strrchr (str, ':'))));
  *p_type = g_strsplit (str + (len - i) + 1, ",", -1);

  return filename;
}

gboolean
check_valid_type (char **type, JeexTypeCode ** code)
{
  int i, s, mem = 1;
  gboolean chk = FALSE;
  const char *supported_type[] = { "char", "int", "double",
    "float", "long", "unsigned",
    "short", "signed", NULL
  };

  return_val_if_fail (type, FALSE);

  *code = (JeexTypeCode *) malloc (mem * sizeof (JeexTypeCode));
  **code = -1;

  for (i = 0; type[i]; i++, chk = FALSE)
    {
      for (s = 0; supported_type[s]; s++)
        {
          if (!strcmp (supported_type[s], type[i]))
            {
              (*code)[mem - 1] = s;
              *code = realloc (*code, ++mem * sizeof (int));
              (*code)[mem - 1] = -1;
              chk = TRUE;
            }
        }
      if (!chk)
        {
          g_printerr (_("Unsupported Type \"%s\".\n" "Usable types: "), type[i]);
          for (s = 0; supported_type[s]; s++)
            g_print ("%s ", supported_type[s]);
          putchar ('\n');

          return FALSE;
        }
    }

  return TRUE;
}

char **types_analyze (char **strings)
{
 char **types;
 char *number;
 int mem_types = 1, j, k, min = -1, max = -1, len;
 
 types = (char **) g_malloc (sizeof (char *));
 *types = NULL;
 
 for ( j = 0; strings[j]; j++ ) {
       len = strlen (strings[j]);
       
       for ( k = 0; k < len; k++ ) {
             if ( strings[j][k] == '(' )
                  min = k;
             
             if ( strings[j][k] == ')' )
                  max = k;
       }
       
       if ( min == max && min == -1 ) {
            types[mem_types - 1] = g_strdup (strings[j]);
            types = (char **) g_realloc (types, ++mem_types * sizeof (char *));
            types[mem_types - 1] = NULL;
            
            continue;
       }
            
       if ( min > max ) {
            g_strfreev (types);
            return NULL;
       }
       
       number = g_strndup (strings[j] + (min + 1), max - (min + 1));
       len = atoi (number);
       g_free (number);
       
       if ( !len )
            continue;
       
       number = g_strndup (strings[j], min);
       for ( k = 0; k < len; k++ ) {
             types[mem_types - 1] = g_strdup (number);
             types = (char **) g_realloc (types, ++mem_types * sizeof (char *));
       }
       types[mem_types - 1] = NULL;
       g_free (number);
 }
 
 return types;
}

char *
rand_name (void)
{
  char *str = (char *) malloc (6 * sizeof (char));

  str[0] = (rand () & 0x01) ? xrand ('a', 'z') : xrand ('0', '9');
  str[1] = (rand () & 0x01) ? xrand ('a', 'z') : xrand ('0', '9');
  str[2] = (rand () & 0x01) ? xrand ('a', 'z') : xrand ('0', '9');
  str[3] = (rand () & 0x01) ? xrand ('a', 'z') : xrand ('0', '9');
  str[4] = (rand () & 0x01) ? xrand ('a', 'z') : xrand ('0', '9');
  str[5] = '\0';

  return str;
}

static gboolean
read_files_structure (FILE * of, int len, JeexTypeCode * code)
{
  int j, b = 0, i;
  char c;
  double d;
  float f;
  long l;
  unsigned u;
  signed s;
  short h;

  srand (time (NULL));

  do
    {
      g_print ("struct _jeex%s {\n", rand_name ());
      for (j = 0; code[j] != -1; j++)
        {
          switch (code[j])
            {
            case TYPE_CHAR:
              fread (&c, sizeof (char), 1, of);
              b += sizeof (char);

              if (isprint (c))
                g_print ("       char %s = '%c';\n", rand_name (), c);
              else
                g_print ("       char %s = '\\x%.2X';\n", rand_name (), c);

              break;
            case TYPE_INT:
              fread (&i, sizeof (int), 1, of);
              b += sizeof (int);

              g_print ("       int %s = %i;\n", rand_name (), i);

              break;
            case TYPE_DOUBLE:
              fread (&d, sizeof (double), 1, of);
              b += sizeof (double);

              g_print ("       double %s = %lf;\n", rand_name (), d);

              break;
            case TYPE_FLOAT:
              fread (&f, sizeof (float), 1, of);
              b += sizeof (float);

              g_print ("       float %s = %f;\n", rand_name (), f);

              break;
            case TYPE_LONG:
              fread (&l, sizeof (long), 1, of);
              b += sizeof (long);

              g_print ("       long %s = %ld;\n", rand_name (), l);

              break;
            case TYPE_UNSIGNED:
              fread (&u, sizeof (unsigned), 1, of);
              b += sizeof (unsigned);

              g_print ("       unsigned %s = %u;\n", rand_name (), u);

              break;
            case TYPE_SHORT:
              fread (&h, sizeof (short), 1, of);
              b += sizeof (short);

              g_print ("       short %s = %hd;\n", rand_name (), h);

              break;
            case TYPE_SIGNED:
              fread (&s, sizeof (signed), 1, of);
              b += sizeof (signed);

              g_print ("       signed %s = %d;\n", rand_name (), s);

              break;
            }
        }

      g_print ("};\n");
    }
  while (b < len);

  return TRUE;
}

gboolean show_portion (char *start_and_end)
{
 int _start, _end, len = strlen (start_and_end), i, t, k;
 char *filename, *data, c;
 FILE *of;

 if (!g_regex_match_simple ("^[^\t\n\r\f\v]+:[0-9]+,[0-9]+$", start_and_end, 0, 0))
   {
     g_printerr (_("Not valid argument.\n"));
     return FALSE;
   }

 filename = g_strndup (start_and_end, len - (i = strlen (strrchr (start_and_end, ':'))));
 sscanf (start_and_end + (len - i), ":%d,%d", &_start, &_end);

 if ( _start > _end ) {
      g_printerr (_("Start byte (%d) > End byte (%d)\n"), _start, _end);
      g_free (filename);

      return FALSE;
 }
 if ( _start == _end ) {
      g_printerr (_("Start byte (%d) == End byte (%d)\n"), _start, _end);
      g_free (filename);

      return FALSE;
 }
 if ( !(of = fopen (filename, "rb")) ) {
      g_printerr (_("Cannot open stream.\n"));
      g_free (filename);

      return FALSE;
 }
 g_free (filename);

 fseek (of, 0, SEEK_END);
 if ( ftell (of) < _end ) {
      g_printerr (_("End byte (%d) > Length File (%d)\n"), _end, (int) ftell (of));
      fclose (of);

      return FALSE;
 }
 fseek (of, _start, SEEK_SET);

 data = (char *) g_malloc (_end - _start);
 fread (data, sizeof (char), _end - _start, of);
 fclose (of);

 for ( i = 0, len = (_end - _start); i < len; ) 
    {
      k = i + 0x10;
      printf ("%8.8X | ", k);
      for ( t = i; i < k; i++ ) 
         {
           if ( i < len ) 
                printf ("%.2X ", (unsigned char) data[i]);
           else printf ("   ");
         }
      printf ("| ");
      for ( ; t < k && t < len; t++ )
         putchar (isprint (data[t]) ? data[t] : '.');
      putchar ('\n');
    }
  g_free (data);

  g_print ("\n\n%s " Jeex "\n", _("Generated with"));


 return TRUE;
}
