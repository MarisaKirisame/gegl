/* This file is part of GEGL editor -- a gtk frontend for GEGL
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Copyright (C) 2003, 2004, 2006 Øyvind Kolås
 */

#include <glib.h>
#include <gegl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "gegl-options.h"

#include <gtk/gtk.h>
gint editor_main (GeglNode    *gegl,
                  const gchar *path);


/*FIXME: this should be in gegl.h*/

GeglNode * gegl_graph_output          (GeglNode     *graph,
                                       const gchar  *name);


GeglNode * gegl_node_get_connected_to (GeglNode     *self,
                                       gchar        *pad_name);

/******************/

static gboolean file_is_gegl_xml (const gchar *path)
{
  gchar *extension;

  extension = strrchr (path, '.');
  if (!extension)
    return FALSE;
  extension++;
  if (extension[0]=='\0')
    return FALSE;
  if (!strcmp (extension, "xml")||
      !strcmp (extension, "XML"))
    return TRUE;
  return FALSE;
}

gint
main (gint    argc,
      gchar **argv)
{
  GeglOptions *o        = NULL;
  GeglNode    *gegl     = NULL;
  gchar       *script   = NULL;
  GError      *err      = NULL;

  gegl_init (&argc, &argv);

  o = gegl_options_parse (argc, argv);

  if (o->xml)
    {
      script = g_strdup (o->xml);
    }
  else if (o->file)
    {
      if (!strcmp (o->file, "-"))  /* read XML from stdin */
        {
          gint  buf_size = 128;
          gchar buf[buf_size];
          GString *acc = g_string_new ("");

          while (fgets (buf, buf_size, stdin))
            {
              g_string_append (acc, buf);
            }
          script = g_string_free (acc, FALSE);
        }
      else if (file_is_gegl_xml (o->file))
        {
          g_file_get_contents (o->file, &script, NULL, &err);
          if (err != NULL)
            {
              g_warning ("Unable to read file: %s", err->message);
            }
        }
      else
        {
          gchar *leaked_string = g_malloc (strlen (o->file + 4));
          GString *acc = g_string_new (""); 

          g_string_append (acc, "<gegl><load path='");
          g_string_append (acc, o->file);
          g_string_append (acc, "'/></gegl>");
          
          script = g_string_free (acc, FALSE);

          leaked_string[0]='\0';
          strcat (leaked_string, o->file);
          strcat (leaked_string, ".xml");
          o->file = leaked_string;
        }
    }
  else
    {
      script = g_strdup ("<gegl></gegl>");
    }

  gegl = gegl_xml_parse (script);

  if (o->rest)
    {
      GeglNode *proxy;
      GeglNode *iter;
     
      gchar **operation = o->rest;
      proxy = gegl_graph_output (gegl, "output");
      iter = gegl_node_get_connected_to (proxy, "input");

      while (*operation)
        {
          GeglNode *new = gegl_graph_new_node (gegl, "operation", *operation, NULL);
          if (iter)
            {
              gegl_node_link_many (iter, new, proxy, NULL);
            }
          else
            {
              gegl_node_link_many (new, proxy, NULL);
            }
          iter = new;
          operation++;
        }
    }

  switch (o->mode)
    {
      case GEGL_RUN_MODE_INTERACTIVE:
          gtk_init (&argc, &argv);
          editor_main (gegl, o->file);
          g_object_unref (gegl);
          g_free (o);
          gegl_exit ();
          return 0;
        break;
      case GEGL_RUN_MODE_XML:
          g_print (gegl_to_xml (gegl));
          gegl_exit ();
          return 0;
        break;
      case GEGL_RUN_MODE_PNG:
        {
          GeglNode *output = gegl_graph_new_node (gegl,
                               "operation", "png-save",
                               "path", o->output,
                               NULL);
          gegl_node_connect (output, "input", gegl_graph_output (gegl, "output"), "output");
          gegl_node_apply (output, "output");

          g_object_unref (gegl);
          g_free (o);
          gegl_exit ();

        }
        break;
      case GEGL_RUN_MODE_HELP:
        break;
      default:
        break;
    }
  return 0;
}
