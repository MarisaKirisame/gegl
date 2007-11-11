/* This file is part of GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright 2006 Øyvind Kolås
 */
#ifndef _GEGL_OPERATION_POINT_FILTER_H
#define _GEGL_OPERATION_POINT_FILTER_H

#include <glib-object.h>
#include "gegl-types.h"
#include "buffer/gegl-buffer.h"
#include "gegl-operation.h"
#include "gegl-operation-filter.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GEGL_TYPE_OPERATION_POINT_FILTER           (gegl_operation_point_filter_get_type ())
#define GEGL_OPERATION_POINT_FILTER(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GEGL_TYPE_OPERATION_POINT_FILTER, GeglOperationPointFilter))
#define GEGL_OPERATION_POINT_FILTER_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST ((klass),  GEGL_TYPE_OPERATION_POINT_FILTER, GeglOperationPointFilterClass))
#define GEGL_OPERATION_POINT_FILTER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj),  GEGL_TYPE_OPERATION_POINT_FILTER, GeglOperationPointFilterClass))

typedef struct _GeglOperationPointFilter  GeglOperationPointFilter;
struct _GeglOperationPointFilter
{
    GeglOperationFilter  operation;
    gint                 samples;
};

typedef struct _GeglOperationPointFilterClass GeglOperationPointFilterClass;
struct _GeglOperationPointFilterClass
{
   GeglOperationFilterClass operation_class;
   gboolean (*process) (GeglOperation *self,      /* for parameters      */
                        void          *in_buf,    /* input buffer */
                        void          *out_buf,   /* output buffer */
                        glong          samples);  /* number of samples   */

};

GType gegl_operation_point_filter_get_type (void) G_GNUC_CONST;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
