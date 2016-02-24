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

/* 1997-1999  original code was written
 * 2001-08-12 ported to GdkImage
 * 2003-01-31 ported to GdkPixbuf
 */

#include <math.h>
#include "gdk-pixbuf-rotate.h"
#include "gdk-pixbuf-private.h"

/**
 * gdk_pixbuf_rotate_matrix_new:
 *
 * Creates a new 2x2 matrix. Allocates 4*sizeof(gdouble) buffer,
 * fills it with 1 on the main dialonal.
 * | 1 0 |
 * | 0 1 |
 * Static gdouble[4] buffer can be used instead.
 *
 * Return value: initialized matrix, destroy with g_free()
 **/
gdouble* gdk_pixbuf_rotate_matrix_new()
{
	gdouble* matrix;
	matrix = g_malloc(sizeof(gdouble)*4);
	matrix[0] = 1; matrix[1] = 0;
	matrix[2] = 0; matrix[3] = 1;
	return matrix;
}

/**
 * gdk_pixbuf_rotate_matrix_fill_for_rotation:
 * @matrix: destination matrix
 * @angle: angle (radian)
 *
 * Prepare 2x2 matrix for rotation.
 * | cos(a) -sin(a) |
 * | sin(a)  cos(a) |
 **/
void gdk_pixbuf_rotate_martix_fill_for_rotation(gdouble* matrix, gdouble angle)
{
	gdouble sa = sin(angle), ca = cos(angle);
	matrix[0] = ca; matrix[1] = -sa;
	matrix[2] = sa; matrix[3] = ca;
}

/**
 * gdk_pixbuf_rotate_matrix_mult_number:
 * @matrix: matrix to multiply
 * @mult: multiplier
 *
 * Multiply 2x2 matrix.
 * | *=mult *=mult |
 * | *=mult *=mult |
 * Use to scale image.
 **/
void gdk_pixbuf_rotate_matrix_mult_number(gdouble* matrix, gdouble mult)
{
	matrix[0] *= mult; matrix[1] *= mult;
	matrix[2] *= mult; matrix[3] *= mult;
}

/**
 * gdk_pixbuf_rotate_matrix_mult_matrix:
 * @dst: matrix to store the result
 * @src1: left matrix to multiply
 * @src2: right matrix to multiply
 *
 * Multiply two 2x2 matrixes. Destination and any of the
 * sources can be one the same.
 * Use to combine two transformations.
 **/
void gdk_pixbuf_rotate_matrix_mult_matrix(
	gdouble* dst,
	const gdouble* src1, const gdouble* src2)
{
	gdouble ans[4];
	ans[0] = src1[0]*src2[0] + src1[1]*src2[2];
	ans[1] = src1[0]*src2[1] + src1[1]*src2[3];
	ans[2] = src1[2]*src2[0] + src1[3]*src2[2];
	ans[3] = src1[2]*src2[1] + src1[3]*src2[3];
	dst[0] = ans[0];
	dst[1] = ans[1];
	dst[2] = ans[2];
	dst[3] = ans[3];
}

/**
 * gdk_pixbuf_rotate_vector:
 * @dst_x: pointer to store X coord of the result vector
 * @dst_y: pointer to store Y coord of the result vector
 * @matrix: matrix to use
 * @src_x: X coord of source vector
 * @src_y: Y coord of source vector
 *
 * Convenience: the same matrix for transforming vectors.
 * Imagine you have a spaceship image, you rotate it and
 * now want to calculate where its spaceguns located.
 **/
void gdk_pixbuf_rotate_vector(
	gdouble* dst_x, gdouble* dst_y,
	const gdouble* matrix,
	gdouble src_x, gdouble src_y)
{
	*dst_x = matrix[0]*src_x + matrix[1]*src_y;
	*dst_y = matrix[2]*src_x + matrix[3]*src_y;
}

inline gdouble gdouble_abs(gdouble d) { return d>0 ? d : -d; } 
inline gdouble gdouble_min(gdouble a, gdouble b) { return a>b ? b : a; } 
inline gdouble gdouble_max(gdouble a, gdouble b) { return a>b ? a : b; } 

/**
 * gdk_pixbuf_rotate_matrix_reverse:
 * @dest: matrix to store the result
 * @src: matrix to reverse
 *
 * Calculates reversed matrix.
 *
 * Return value: TRUE on success, FALSE when reverse
 * matrix doen't exist.
 **/
gboolean gdk_pixbuf_rotate_matrix_reverse(gdouble* dest, const gdouble* src)
{
	gdouble ans[4];
	gdouble det;
	gdouble consider_as_zero;
	det = gdk_pixbuf_rotate_matrix_determinant(src);
	consider_as_zero =
		(gdouble_abs(src[0]) +
		 gdouble_abs(src[1]) +
		 gdouble_abs(src[2]) +
		 gdouble_abs(src[3])) * 1e-11;
	if (gdouble_abs(det)<consider_as_zero) return FALSE;
	ans[0] = src[3];
	ans[1] = -src[1];
	ans[2] = -src[2];
	ans[3] = src[0];
	gdk_pixbuf_rotate_matrix_mult_number(ans, 1.0/det);
	dest[0] = ans[0];
	dest[1] = ans[1];
	dest[2] = ans[2];
	dest[3] = ans[3];
	return TRUE;
}

/**
 * gdk_pixbuf_rotate_matrix_transpose:
 * @dest: matrix to store the result
 * @src: matrix to transpose
 *
 * Calculates transposed matrix.
 **/
void gdk_pixbuf_rotate_matrix_transpose(
	gdouble* dest,
	const gdouble* src)
{
	gdouble t;
	t = dest[2];
	dest[2] = dest[1];
	dest[1] = t;
}

/**
 * gdk_pixbuf_rotate_matrix_determinant:
 * @matrix: matrix
 *
 * Calculates determinant.
 *
 * Return value: determinant.
 **/
gdouble gdk_pixbuf_rotate_matrix_determinant(const gdouble* matrix)
{
	return matrix[0]*matrix[3] - matrix[1]*matrix[2];
}


#define ROTATE_CUT_EDGES 0.1 /* cut small part of texel on edges - to avoid artefacts */

/**
 * gdk_pixbuf_rotate:
 * @dst_pixbuf: destination pixbuf
 * @dst_x: X coord in @dst_pixbuf
 * @dst_y: Y coord in @dst_pixbuf
 * @matrix: transformation matrix
 * @src_pixbuf: a pixbuf
 * @src_center_x: X coord of center in @src_pixbuf
 * @src_center_y: Y coord of center in @src_pixbuf
 * @result_rect_x: pointer to X coord of rectangle to store result bounds
 * @result_rect_y: pointer to Y coord of rectangle to store result bounds
 * @result_rect_width: pointer to Width of rectangle to store result bounds
 * @result_rect_height: pointer to Height of rectangle to store result bounds
 * @interp_type: GDK_INTERP_NEAREST
 *
 * Transform a src_pixbuf into dst_pixbuf using arbitrary matrix.
 * GDK_INTERP_NEAREST only supported
 * GDK_COLORSPACE_RGB only supported
 * only 8 bits per sample supported
 * only 3 or 4 channels supported (with or without alpha)
 **/
void gdk_pixbuf_rotate(
	GdkPixbuf* dst_pixbuf, gdouble dst_x, gdouble dst_y,
	gdouble* matrix,
	GdkPixbuf* src_pixbuf, gdouble src_center_x, gdouble src_center_y,
	gint* result_rect_x,
	gint* result_rect_y,
	gint* result_rect_width,
	gint* result_rect_height,
	GdkInterpType interp_type)
{
	gdouble reverse[4];
	guchar *dst_buf, *dst;
	guchar *src_buf, *src;
	gint src_bpl = src_pixbuf->rowstride;
	gint dst_bpl = dst_pixbuf->rowstride;

	gboolean lines4 = TRUE;		/* two cases: 4-lines or 2-lines */
	gdouble px[4], py[4];		/* edge points on dest */
	gint line_l1, line_l2 = -1;	/* line indexes, 2 left + 2 right or 1 left + 1 right */
	gint line_r1, line_r2 = -1;
	gdouble k[4], b[4];		/* x = k*y + b - bounding line equations */
	gint ili = -1;			/* invalid line index - to restore witch 2 lines valid [2-lines case] */

	gdouble left, right, top, bottom;	/* resulting rect */
	gint int_left, int_right, int_top, int_bottom;
	gint li =0, ri =0, ti =0, bi =0;	/* point indexes */

	gdouble src_x, src_y;			/* first point in a row */
	gdouble pixel_right_x, pixel_right_y;	/* one-pixel-to-the-left vector on source */
	gint32  pixel_right_ix, pixel_right_iy;	/* fixed-point, the same */

	gint c, x, y; /* bunch of temprorary variables */
	gdouble tmpd;
	gint tmpi;

	if (!gdk_pixbuf_rotate_matrix_reverse(reverse, matrix)) return;
	/* No reverse matrix means we have image collapsed into
	 * line or point
	 */

	dst_buf = (guchar*) dst_pixbuf->pixels;
	src_buf = (guchar*) src_pixbuf->pixels;

	/* 01  map source edge points to dest
	 * 32
	 */
	gdk_pixbuf_rotate_vector(&px[0], &py[0], matrix,
		- src_center_x + ROTATE_CUT_EDGES,
		- src_center_y + ROTATE_CUT_EDGES);
	gdk_pixbuf_rotate_vector(&px[1], &py[1], matrix,
		src_pixbuf->width - src_center_x - ROTATE_CUT_EDGES,
		- src_center_y + ROTATE_CUT_EDGES);
	gdk_pixbuf_rotate_vector(&px[2], &py[2], matrix,
		src_pixbuf->width - src_center_x -ROTATE_CUT_EDGES,
		src_pixbuf->height - src_center_y - ROTATE_CUT_EDGES);
	gdk_pixbuf_rotate_vector(&px[3], &py[3], matrix,
		- src_center_x + ROTATE_CUT_EDGES,
		src_pixbuf->height - src_center_y - ROTATE_CUT_EDGES);

	for (c=0; c<4; c++) {
		tmpi = (c+1)%4;
		tmpd = py[tmpi] - py[c];
		if (gdouble_abs(tmpd)<1e-4) {
			lines4 = FALSE;
			ili = c;
		} else {
			k[c] = (px[tmpi]-px[c])/tmpd;
			b[c] = px[c] - k[c]*py[c];
		}
	}

	left = px[0];
	right = px[0];
	top = py[0];
	bottom = py[0];
	for (c=1; c<4; c++) {
		if (left>=px[c]) { left = px[c]; li = c; }
		if (right<=px[c]) { right = px[c]; ri = c; }
		if (top>=py[c]) { top = py[c]; ti = c; }
		if (bottom<=py[c]) { bottom = py[c]; bi = c; }
	}
	int_top = (gint) (dst_y + py[ti] + 0.5);
	int_top = CLAMP(int_top, 0, dst_pixbuf->height);
	int_bottom = (gint) (dst_y + py[bi] + 0.5);
	int_bottom = CLAMP(int_bottom, 0, dst_pixbuf->height);
	int_left = (gint) (dst_x + px[li] + 0.5);
	int_left = CLAMP(int_left, 0, dst_pixbuf->width);
	int_right = (gint) (dst_x + px[ri] + 0.5);
	int_right = CLAMP(int_right, 0, dst_pixbuf->width);
	if (result_rect_x) *result_rect_x = int_left;
	if (result_rect_y) *result_rect_y = int_top;
	if (result_rect_width) *result_rect_width = int_right - int_left;
	if (result_rect_height) *result_rect_height = int_bottom - int_top;

	if (int_right - int_left == 0 || int_bottom - int_top == 0) return;

	if (lines4) {
		line_l1 = li;
		line_r1 = ri;
		/*    ti
		 *  li    ri
		 *      bi
		 */
		if (((ti+1)%4)==ri) {
			line_l2 = bi; /* clockwise */
			line_r2 = ti;
			/*   line_l1 /\  line_r2
			 *           \  \ 
			 *    line_l2  \/  line_r1
			 */
		} else {
			line_l2 = ti; /* anticlockwise */
			line_r2 = bi;
			/*   line_l2 /\  line_r1
			 *           \  \ 
			 *    line_l1  \/  line_r2
			 */
		}
	} else {
		/* 2 lines case
		 *          |   |
		 * line_l1  |   |  line_r1
		 */
		tmpd = (px[0]+px[1]+px[2]+px[3])/4.0; /* middle x */
		tmpi = (ili+1)%4; /* valid for sure */
		if (px[tmpi]<tmpd) {
			line_l1 = tmpi;
			line_r1 = (ili+3)%4;
		} else {
			line_r1 = tmpi;
			line_l1 = (ili+3)%4;
		}
	}

	gdk_pixbuf_rotate_vector(
		&pixel_right_x, &pixel_right_y,
		reverse,
		1, 0);
	pixel_right_ix = (gint32) (pixel_right_x*0x10000);
	pixel_right_iy = (gint32) (pixel_right_y*0x10000);

	for (y=int_top; y<int_bottom; y++) {
		gdouble x1r, x2r;
		gint x1, x2;
		gint32 pixel_ix, pixel_iy;

		/* calc line x1..x2 on dest */
		tmpd = y - dst_y;
		if (lines4) {
			x1r = gdouble_max(
				k[line_l1]*(tmpd+0.5) + b[line_l1],
				k[line_l2]*(tmpd+0.5) + b[line_l2] );
			x2r = gdouble_min(
				k[line_r1]*(tmpd+0.5) + b[line_r1],
				k[line_r2]*(tmpd+0.5) + b[line_r2] );
		} else {
			x1r = k[line_l1]*tmpd + b[line_l1];
			x2r = k[line_r1]*tmpd + b[line_r1];
		}
		x1r += dst_x;
		x2r += dst_x;
		x1r = gdouble_max(x1r, 0);
		x2r = gdouble_min(x2r, dst_pixbuf->width);
		x1 = (gint) (x1r+0.5);
		x2 = (gint) (x2r+0.5);
		if (x2<=x1) continue;

		/* calc y:x1 point on source */
		gdk_pixbuf_rotate_vector(
			&src_x, &src_y,
			reverse,
			x1-dst_x + 0.5,
			tmpd + 0.5);
		src_x += src_center_x;
		src_y += src_center_y;

		/* fixed point */
		pixel_ix = (gint32) (src_x*0x10000);
		pixel_iy = (gint32) (src_y*0x10000);

		dst = dst_buf + (y*dst_bpl + x1*dst_pixbuf->n_channels);

#define INNER_LOOP(DST_CHANNELS, SRC_CHANNELS) \
		for (x=x1; x<x2; x++) { \
			src = src_buf \
				+ (pixel_iy>>16)*src_bpl \
				+ (pixel_ix>>16)*SRC_CHANNELS; \
			if (DST_CHANNELS==3 && SRC_CHANNELS==3) { \
				dst[0] = src[0]; \
				dst[1] = src[1]; \
				dst[2] = src[2]; \
			} \
			if (DST_CHANNELS==4 && SRC_CHANNELS==3) { \
				dst[0] = src[0]; \
				dst[1] = src[1]; \
				dst[2] = src[2]; \
				dst[3] = 0xFF; \
			} \
			if (DST_CHANNELS==3 && SRC_CHANNELS==4) { \
				guint a0, a1, t; \
				a0 = (guint) src[3]; \
				a1 = 0xff - a0; \
				t = a0*src[0] + a1*dst[0] + 0x80; \
				dst[0] = (t + (t>>8)) >> 8; \
				t = a0*src[1] + a1*dst[1] + 0x80; \
				dst[1] = (t + (t>>8)) >> 8; \
				t = a0*src[2] + a1*dst[2] + 0x80; \
				dst[2] = (t + (t>>8)) >> 8; \
			} \
			if (DST_CHANNELS==4 && SRC_CHANNELS==4) { \
				guint a0 = (guint) src[3]; \
				if (a0==255) { \
					dst[0] = src[0]; \
					dst[1] = src[1]; \
					dst[2] = src[2]; \
					dst[3] = 0xFF; \
				} if (a0>0) { \
					guint w0 = 0xff * a0; \
					guint w1 = (0xff - a0) * dst[3]; \
					guint w = w0 + w1; \
					dst[0] = (w0*src[0] + w1*dst[0]) / w; \
					dst[1] = (w0*src[1] + w1*dst[1]) / w; \
					dst[2] = (w0*src[2] + w1*dst[2]) / w; \
					dst[3] = w / 0xff; \
				} \
			} \
			pixel_ix += pixel_right_ix; \
			pixel_iy += pixel_right_iy; \
			dst += DST_CHANNELS; \
		}

		if (dst_pixbuf->n_channels==3 && src_pixbuf->n_channels==3)
			INNER_LOOP(3, 3)
		else if (dst_pixbuf->n_channels==3 && src_pixbuf->n_channels==4)
			INNER_LOOP(3, 4)
		else if (dst_pixbuf->n_channels==4 && src_pixbuf->n_channels==3)
			INNER_LOOP(4, 3)
		else if (dst_pixbuf->n_channels==4 && src_pixbuf->n_channels==4)
			INNER_LOOP(4, 4)
	}
}

