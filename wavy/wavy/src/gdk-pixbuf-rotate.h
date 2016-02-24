/* GdkPixbuf library - image transformation using 2x2 arbitrary matrix
 *
 * Copyright (C) 1999 The Free Software Foundation
 *
 * Authors: Oleg Klimov <quif@land.ru>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef GDK_PIXBUF_ROTATE_H
#define GDK_PIXBUF_ROTATE_H

#include "gdk-pixbuf/gdk-pixbuf.h"

G_BEGIN_DECLS


gdouble*	gdk_pixbuf_rotate_matrix_new				(void);
void		gdk_pixbuf_rotate_martix_fill_for_rotation	(gdouble* matrix, gdouble angle);
void		gdk_pixbuf_rotate_matrix_mult_number		(gdouble* matrix, gdouble mult);
void		gdk_pixbuf_rotate_matrix_mult_matrix		(gdouble* dst_matrix, const gdouble* src1, const gdouble* src2);
gboolean	gdk_pixbuf_rotate_matrix_reverse			(gdouble* dest,const gdouble* src);
void		gdk_pixbuf_rotate_matrix_transpose			(gdouble* dest,const gdouble* src);
gdouble		gdk_pixbuf_rotate_matrix_determinant		(const gdouble*	matrix);


void gdk_pixbuf_rotate		(GdkPixbuf*	dst,
							 gdouble	dst_x,
							 gdouble	dst_y,
							 gdouble*	matrix,
							 GdkPixbuf*	src,
							 gdouble	src_center_x,
							 gdouble	src_center_y,
							 gint*		result_rect_x,
							 gint*		result_rect_y,
							 gint*		result_rect_width,
							 gint*		result_rect_height,
							 GdkInterpType	interp_type);
							 
void gdk_pixbuf_rotate_vector	(gdouble*	dst_x,
								 gdouble*	dst_y,
								 const gdouble*	matrix,
								 gdouble	src_vector_x,
								 gdouble	src_vector_y);

G_END_DECLS

#endif /* GDK_PIXBUF_ROTATE_H */

