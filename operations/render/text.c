/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Copyright 2006 Øyvind Kolås <pippin@gimp.org>
 */
#ifdef GEGL_CHANT_PROPERTIES

gegl_chant_string (string, "Hello", "utf8 string to display")
gegl_chant_double (size, 1.0, 2048.0, 10.0, "approximate height of text in pixels")
gegl_chant_int    (width, 0, 1000000, 0, "private")
gegl_chant_int    (height, 0, 1000000, 0, "private")

#else

#define GEGL_CHANT_SOURCE
#define GEGL_CHANT_NAME            text
#define GEGL_CHANT_DESCRIPTION     "Display a string of text using cairo"
#define GEGL_CHANT_SELF            "text.c"
#define GEGL_CHANT_CATEGORIES      "sources:render"
#include "gegl-chant.h"

#include <cairo.h>

static void text_layout_text (cairo_t     *cr,
                              const gchar *utf8,
                              gdouble      rowstride,
                              gdouble     *width,
                              gdouble     *height)
{
  cairo_text_extents_t extent;

  cairo_text_extents (cr, utf8, &extent);

  if (width && height)
    {
      *width = extent.width+extent.x_bearing;
      *height = extent.height;
    }
  else
    {
      cairo_move_to (cr, 0, 0-extent.y_bearing);
      cairo_show_text (cr, utf8);
    }
}

static gboolean
process (GeglOperation *operation,
         const gchar   *output_prop)
{
  GeglOperationSource     *op_source = GEGL_OPERATION_SOURCE(operation);
  GeglChantOperation *self = GEGL_CHANT_OPERATION (operation);
  gint       width;
  gint       height;

  if(strcmp("output", output_prop))
    return FALSE;

  if (op_source->output)
    g_object_unref (op_source->output);
  op_source->output=NULL;

  width = self->width;
  height = self->height;

    op_source->output = g_object_new (GEGL_TYPE_BUFFER,
                                      "format", babl_format ("R'G'B'A u8"),
                                      "x",      0,
                                      "y",      0,
                                      "width",  width,
                                      "height", height,
                                      NULL);

  {
    guchar *data = g_malloc0 (width * height * 4);
    cairo_t *cr;

    cairo_surface_t *surface = cairo_image_surface_create_for_data (data, CAIRO_FORMAT_ARGB32, width, height, width * 4);
    cr = cairo_create (surface);
    cairo_set_font_size (cr, self->size);
    cairo_move_to (cr, 0, height);

    cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 1.0);
    text_layout_text (cr, self->string, 0, NULL, NULL);

    gegl_buffer_set_fmt (op_source->output, data,
        babl_format_new (babl_model ("R'G'B'A"),
                         babl_type ("u8"),
                         babl_component ("B'"),
                         babl_component ("G'"),
                         babl_component ("R'"),
                         babl_component ("A"),
                         NULL));

    cairo_destroy (cr);
    cairo_surface_destroy (surface);
    g_free (data);
  }
  return  TRUE;
}


static GeglRect
get_defined_region (GeglOperation *operation)
{
  GeglRect result = {0,0,0,0};
  GeglChantOperation *self = GEGL_CHANT_OPERATION (operation);
  gint status = FALSE;

  { /* get extents */
    cairo_t *cr;
    gdouble width, height;

    cairo_surface_t *surface  = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
        1, 1);
    cr = cairo_create (surface);
    cairo_set_font_size (cr, self->size);
    text_layout_text (cr, self->string, 0, &width, &height);
    result.w = width;
    result.h = height;

    /* store the measured size for later use */
    self->width = width;
    self->height = height;

    cairo_destroy (cr);
    cairo_surface_destroy (surface);
  }

  if (status)
    {
      g_warning ("calc have rect of text '%s' failed", self->string);
    }
  else
    {
    }

  return result;
}

#endif
