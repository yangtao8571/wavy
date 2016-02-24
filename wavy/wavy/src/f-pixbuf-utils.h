/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* f-pixbuf-utils.h
 *
 * Copyright (C) 2003  Ettore Perazzoli
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: Ettore Perazzoli <ettore@ximian.com>
 */

#ifndef _F_PIXBUF_UTILS_H_
#define _F_PIXBUF_UTILS_H_

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <cairo.h>


/* Return the largest between height and width of the specified PIXBUF.  */
int f_pixbuf_get_image_size (GdkPixbuf *pixbuf);

/* Return the normalized width for the specified PIXBUF at the specified
   thumbnail SIZE.  */
int f_pixbuf_get_scaled_width (GdkPixbuf *pixbuf, int size);

/* Return the normalized height for the specified PIXBUF at the specified
   thumbnail SIZE.  */
int f_pixbuf_get_scaled_height (GdkPixbuf *pixbuf, int size);

cairo_surface_t *
f_pixbuf_to_cairo_surface (GdkPixbuf *pixbuf);

GdkPixbuf *
f_pixbuf_from_cairo_surface (cairo_surface_t *source);

#endif