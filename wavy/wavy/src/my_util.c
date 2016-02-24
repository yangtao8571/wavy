#define MY_LOG_LEVEL 3

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gtk/gtk.h>
#include <glib-object.h>
#include "my_math.h"
#include "my_debug.h"
#include "my_point.h"
#include "my_vector.h"
#include "my_util.h"


gchar *		chinese			(gchar *sourceUtf8Bom) {


	return g_convert(sourceUtf8Bom,-1,"UTF-8","GB2312",NULL,NULL,NULL);
}


int 		my_util_atoi	(const char *src) {
	

	return atoi(src);
}


char *		my_util_int_to_str		(int i) {
	
	size_t size = 20;
	char *buf = (char *) g_malloc ((size+1) * sizeof (char));
	sprintf_s(buf, size, "%d", i);
	return buf;
}


char *		my_util_double_to_str		(double d) {

	size_t size = 20;
	char *buf = (char *) g_malloc ((size+1) * sizeof (char));
	sprintf_s(buf, size, "%.3f", d);
	return buf;
}


double		my_util_str_to_double		(const char *src) {

	double d;
	d = atof (src);
	//sscanf(src, "%f", d);
	return d;
}


/**
 * Frees a NULL-terminated array of strings, but not the array itself.
 * If called on a NULL value, my_util_free_str_arr() simply returns.
 *
 * strArr	NULL-terminated array to g_free
 */
void		my_util_free_str_arr		(char **strArr) {
	
	int i;
	if (!strArr)
		return;
	for (i = 0; strArr[i]; i++) {
		g_free (strArr[i]);
	}
}


gdouble		my_util_rotate_point			(gdouble fi, gdouble x, gdouble y, gdouble *rotatedX, gdouble *rotatedY) {


	*rotatedX = cos (fi) * x - sin (fi) * y;
	*rotatedY = sin (fi) * x + cos (fi) * y;
}


gdouble		my_util_rotate_point_with_center	(gdouble fi, gdouble centerX, gdouble centerY, gdouble x, gdouble y, gdouble *rotatedX, gdouble *rotatedY) {


	gdouble relX = x - centerX;
	gdouble relY = y - centerY;
	my_util_rotate_point (fi, relX, relY, &relX, &relY);
	*rotatedX = centerX + relX;
	*rotatedY = centerY + relY;
}

/*
gdouble		my_util_triangle_area2 (gdouble x1,
									gdouble y1,
									gdouble x2,
									gdouble y2,
									gdouble x3,
									gdouble y3) {
									
	return 0.5*(x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));						
}*/


double		my_util_triangle_area	(double ax, double ay, double bx, double by, double cx, double cy) {

    double A = sqrt((double)(bx-ax) * (bx-ax) + (by-ay) * (by-ay));
    double B = sqrt((double)(bx-cx) * (bx-cx) + (by-cy) * (by-cy));
    double C = sqrt((double)(ax-cx) * (ax-cx) + (ay-cy) * (ay-cy));
    double height = 0;
 
    // Heron's formula for area calculation
    // area = sqrt( s * (s-a) * (s-b) * (s-c))
    double s = (A + B + C) / 2;   
    double area = sqrt( s * (s-A) * (s-B) * (s-C));
	return area;
}


gdouble		my_util_rectangle_area (gdouble x1,
									gdouble y1,
									gdouble x2,
									gdouble y2,
									gdouble x3,
									gdouble y3,
									gdouble x4,
									gdouble y4) {
									
	return 0.5*abs((y1 - y3) * (x4 - x2) + (y2 - y4) * (x1 - x3));
}
/**
 * 正规矩形是否包含点
 */
gboolean 		my_util_is_point_in_rect	(	gdouble rectX, 
												gdouble rectY, 
												gdouble width, 
												gdouble height, 
												gdouble x, 
												gdouble y) {

	gboolean isInRect;
	if (rectX <= x && x <= (rectX + width)
		&& rectY <= y && y <= (rectY + height)) {
		isInRect = TRUE;
	} else {
		isInRect = FALSE;
	}
	my_debug ("Is point (%f,%f) in rect (%f,%f,%f,%f) ? %d", x, y, rectX, rectY, width, height, isInRect);
	return isInRect;
}


gboolean		my_util_is_rect_intersection	(	gdouble x0,
													gdouble y0,
													gdouble width0,
													gdouble height0,
													gdouble x1,
													gdouble y1,
													gdouble width1,
													gdouble height1) {
													
	if (my_util_is_point_in_rect (x0, y0, width0, height0, x1, y1)
		|| my_util_is_point_in_rect (x0, y0, width0, height0, x1 + width1, y1)
		|| my_util_is_point_in_rect (x0, y0, width0, height0, x1 + width1, y1 + height1)
		|| my_util_is_point_in_rect (x0, y0, width0, height0, x1, y1 + height1)
		|| my_util_is_point_in_rect (x1, y1, width1, height1, x0, y0)
		|| my_util_is_point_in_rect (x1, y1, width1, height1, x0 + width0, y0)
		|| my_util_is_point_in_rect (x1, y1, width1, height1, x0 + width0, y0 + height0)
		|| my_util_is_point_in_rect (x1, y1, width1, height1, x0, y0 + height0)) {
		return TRUE;
	}
	return FALSE;
}													
													
/**
 * 矩形是否包含点，包括在边上
 * 1-4为参数为顺时针或逆时针方向4个点
 */
/*
gboolean 	my_util_is_point_inside_rect	(gdouble x,
											 gdouble y,
											 gdouble x1,
											 gdouble y1,
											 gdouble x2,
											 gdouble y2,
											 gdouble x3,
											 gdouble y3,
											 gdouble x4,
											 gdouble y4) {
	
	gdouble triangleAreaSum = 
		my_util_triangle_area (x, y, x1, y1, x2, y2) +
		my_util_triangle_area (x, y, x2, y2, x3, y3) +
		my_util_triangle_area (x, y, x3, y3, x4, y4) +
		my_util_triangle_area (x, y, x4, y4, x1, y1);
		
	gdouble rectangleAreaSum = my_util_rectangle_area (x1, y1, x2, y2, x3, y3, x4, y4);
	
	my_debug ("triangleAreaSum: %f", triangleAreaSum);
	my_debug ("rectangleAreaSum: %f", rectangleAreaSum);
	return triangleAreaSum < rectangleAreaSum;
}*/
gboolean 	my_util_is_point_inside_rect	(gdouble x,
											 gdouble y,
											 gdouble x1,
											 gdouble y1,
											 gdouble x2,
											 gdouble y2,
											 gdouble x3,
											 gdouble y3,
											 gdouble x4,
											 gdouble y4) {
	MyVector v1, v2, v3, v4, f1, f2, f3, f4;
	MyVector w1, w2, w3, w4;
	gdouble cosA;
	v1 = my_vector_new (x1, y1, x2, y2);
	v2 = my_vector_new (x2, y2, x3, y3);
	v3 = my_vector_new (x3, y3, x4, y4);
	v4 = my_vector_new (x4, y4, x1, y1);
	f1 = v2;
	f2 = v3;
	f3 = v4;
	f4 = v1;	
	w1 = my_vector_new (x1, y1, x, y);
	w2 = my_vector_new (x2, y2, x, y);
	w3 = my_vector_new (x3, y3, x, y);
	w4 = my_vector_new (x4, y4, x, y);
	cosA = my_vector_cos_to_vector (w1, f1);
	if (0 <= cosA && cosA <= 1) {		
		cosA = my_vector_cos_to_vector (w2, f2);
		if (0 <= cosA && cosA <= 1) {			
			cosA = my_vector_cos_to_vector (w3, f3);
			if (0 <= cosA && cosA <= 1) {				
				cosA = my_vector_cos_to_vector (w4, f4);
				if (0 <= cosA && cosA <= 1) {
					return TRUE;
				} else {
					return FALSE;
				}
			} else {
				return FALSE;
			}
		} else {
			return FALSE;
		}
	} else {
		return FALSE;
	}
}


/**
 * 判断给定点(x,y)是否落在线段内
 * strokeWidth	半径
 */
gboolean		my_util_is_point_inside_segment		(gdouble x0, gdouble y0, gdouble x1, gdouble y1, gdouble x, gdouble y, gdouble strokeWidth) {

  gdouble x3, y3, x4, y4, x5, y5, x6, y6, 
		rate, dx, dy, distance;
  gboolean isInRect;
  gdouble theLastX = x0;
  gdouble theLastY = y0;
  gdouble currentX = x1;
  gdouble currentY = y1;
	if (theLastX == currentX) { // 线段垂直情况
		x3 = theLastX - strokeWidth;
		y3 = theLastY;
		x4 = theLastX + strokeWidth;
		y4 = theLastY;
		x5 = x4;
		y5 = currentY;
		x6 = x3;
		y6 = currentY;
	} else {
		rate = (currentY - theLastY) / (currentX - theLastX);
		distance = sqrt((currentY - theLastY) * (currentY - theLastY) + (currentX - theLastX) * (currentX - theLastX));
		my_debug ("rate: %f, distance: %f", rate, distance);
		dx = abs( strokeWidth * (currentY - theLastY) / distance);
		dy = abs( strokeWidth * (currentX - theLastX) / distance);
		my_debug ("dx: %f, dy: %f", dx, dy);
		if (rate >= 0) {
			x3 = theLastX - dx;
			y3 = theLastY + dy;
			x4 = theLastX + dx;
			y4 = theLastY - dy;
			x5 = currentX + dx;
			y5 = currentY - dy;
			x6 = currentX - dx;
			y6 = currentY + dy;
		} else {
			x3 = theLastX - dx;
			y3 = theLastY - dy;
			x4 = theLastX + dx;
			y4 = theLastY + dy;
			x5 = currentX + dx;
			y5 = currentY + dy;
			x6 = currentX - dx;
			y6 = currentY - dy;
		}
	}
	my_debug ("x: %f, y: %f, x3: %f, y3: %f, x4: %f, y4: %f, x5: %f, y5: %f, x6: %f, y6: %f,", 
									x, y,
									x3, y3,
									x4, y4,
									x5, y5,
									x6, y6);
	isInRect = my_util_is_point_inside_rect (	
									x, y,
									x3, y3,
									x4, y4,
									x5, y5,
									x6, y6);
	if (isInRect) {
		my_debug ("line segment: (%f,%f) (%f,%f), mouse: (%f,%f). Select the target. OOOOOOOOO ", 
				theLastX, theLastY, currentX, currentY, x, y);
		
		return TRUE;
	}
	return FALSE;
}

/**
 * 把线段P1P2扩成矩形
 */
void		my_util_segment_to_rect			(gdouble x1, gdouble y1, 
											gdouble x2, gdouble y2,
											gdouble *Ax, gdouble *Ay,
											gdouble *Bx, gdouble *By,
											gdouble *Cx, gdouble *Cy,
											gdouble *Dx, gdouble *Dy,
											gdouble radius) {
	gdouble x3, y3;
	gdouble a = atan2 (y2 - y1, x2 - x1);
	gdouble b = PI/2 - a;

	my_util_rotate_point_with_center (b, x1, y1, x2, y2, &x3, &y3);
	*Ax = x3 - radius;
	*Ay = y3 + radius;
	*Bx = x3 + radius;
	*By = y3 + radius;
	*Cx = x1 + radius;
	*Cy = y1 - radius;
	*Dx = x1 - radius;
	*Dy = y1 - radius;
		
	my_util_rotate_point_with_center (-b, x1, y1, *Ax, *Ay, Ax, Ay);
	my_util_rotate_point_with_center (-b, x1, y1, *Bx, *By, Bx, By);
	my_util_rotate_point_with_center (-b, x1, y1, *Cx, *Cy, Cx, Cy);
	my_util_rotate_point_with_center (-b, x1, y1, *Dx, *Dy, Dx, Dy);
}


/**
 * 点(x,y)是否在路径上
 */

gboolean		my_util_is_point_in_path		(GQueue *points, gdouble x, gdouble y, gdouble strokeWidth) {

	
	guint pointCount, i, j, k;
	MyPoint *point;
	gdouble x3, y3, x4, y4, x5, y5, x6, y6, 
		rate, dx, dy, distance,
		currentX, currentY, theLastX, theLastY;
	gboolean isInRect;
	
	for (k = 0, pointCount = g_queue_get_length (points), isInRect = FALSE; k < pointCount; k++) {
		point = g_queue_peek_nth (points, k);
		currentX = point->x;
		currentY = point->y;
		if (k != 0) { // 从第二个点开始有线段

			my_util_segment_to_rect (theLastX, theLastY, 
									currentX, currentY, 
									&x3, &y3, 
									&x4, &y4, 
									&x5, &y5, 
									&x6, &y6,
									0.5 * strokeWidth);
			isInRect = my_util_is_point_inside_rect (
											x, y,
											x3, y3,
											x4, y4,
											x5, y5,
											x6, y6);
			if (isInRect) {
				return TRUE;
			}
		}
		theLastX = currentX;
		theLastY = currentY;
	}
	return FALSE;
}
/*
gboolean		my_util_is_point_in_path		(GQueue *points, gdouble x, gdouble y, gdouble strokeWidth) {

	
  guint pointCount, i, j, k;
  MyPoint *point;
  gdouble x3, y3, x4, y4, x5, y5, x6, y6, 
		rate, dx, dy, distance,
		currentX, currentY, theLastX, theLastY;
  gboolean isInRect;


					for (k = 0, pointCount = g_queue_get_length (points), isInRect = FALSE; k < pointCount; k++) {
						my_debug ("loop points count: %d", k);
						point = g_queue_peek_nth (points, k);
						currentX = point->x;
						currentY = point->y;
						if (k != 0) { // 从第二个点开始有线段
							if (theLastX == currentX) { // 线段垂直情况
								x3 = theLastX - strokeWidth;
								y3 = theLastY;
								x4 = theLastX + strokeWidth;
								y4 = theLastY;
								x5 = x4;
								y5 = currentY;
								x6 = x3;
								y6 = currentY;
							} else {
								rate = (currentY - theLastY) / (currentX - theLastX);
								distance = sqrt((currentY - theLastY) * (currentY - theLastY) + (currentX - theLastX) * (currentX - theLastX));
								my_debug ("rate: %f, distance: %f", rate, distance);
								dx = abs( strokeWidth * (currentY - theLastY) / distance);
								dy = abs( strokeWidth * (currentX - theLastX) / distance);
								my_debug ("dx: %f, dy: %f", dx, dy);
								if (rate >= 0) {
									x3 = theLastX - dx;
									y3 = theLastY + dy;
									x4 = theLastX + dx;
									y4 = theLastY - dy;
									x5 = currentX + dx;
									y5 = currentY - dy;
									x6 = currentX - dx;
									y6 = currentY + dy;
								} else {
									x3 = theLastX - dx;
									y3 = theLastY - dy;
									x4 = theLastX + dx;
									y4 = theLastY + dy;
									x5 = currentX + dx;
									y5 = currentY + dy;
									x6 = currentX - dx;
									y6 = currentY - dy;
								}
							}
							my_debug ("x: %f, y: %f, x3: %f, y3: %f, x4: %f, y4: %f, x5: %f, y5: %f, x6: %f, y6: %f,", 
															x, y,
															x3, y3,
															x4, y4,
															x5, y5,
															x6, y6);
							isInRect = my_util_is_point_inside_rect (	
															x, y,
															x3, y3,
															x4, y4,
															x5, y5,
															x6, y6);
							if (isInRect) {
								my_debug ("self %#x, id: %d, line segment: (%f,%f) (%f,%f), mouse: (%f,%f). Select the target. OOOOOOOOO ", 
										self, shape->id, theLastX, theLastY, currentX, currentY, x, y);
								
								my_debug ("is curve chosen done.");
								return TRUE;
							}
						}
						theLastX = currentX;
						theLastY = currentY;
					}
	my_debug ("is curve chosen done.");
	return FALSE;
}
*/


/**
 * x1,y1	起点
 * x2,y2	终点
 * side		1表示尖在线的右边，-1表示尖在线的左边
 */
MyPoint		my_util_jmline_point	(gdouble x1, gdouble y1, gdouble x2, gdouble y2, gboolean side) {

	MyPoint p;
	p.x = (x1 + x2) / 2 + side * sqrt(3.) / 2 * (y1 - y2);
	p.y = (y1 + y2) / 2 + side * sqrt(3.) / 2 * (x2 - x1);
	return p;
}

/**
 * 找到线段上的点，该点到线段起点的距离为partLen
 * partLen	距离
 * len		线段长度
 */
MyPoint		my_util_part_segment	(gdouble x1, gdouble y1, gdouble x2, gdouble y2, gdouble len, gdouble partLen) {
	
	MyPoint p;
	p.x = x1 + partLen / len * (x2 - x1);
	p.y = y1 + partLen / len * (y2 - y1);
	return p;
}

GQueue * 		my_util_delete_queue_element	(GQueue *queue,
											 MyQueueDeleteFunc deleteFunc,
											 gpointer user_data) {
	
	guint i, len;
	GQueue *tmpQ;
	gpointer data;
	tmpQ = g_queue_new();
	// 双队列实现删除
	for (i = 0, len = g_queue_get_length (queue); i < len; i++){
		data = g_queue_peek_nth (queue, i);
		if (deleteFunc (data, user_data)) {
			my_debug ("deleting data %#x", data);
		} else {
			g_queue_push_tail (tmpQ, data);
		}
	}
	return tmpQ;
}

/*
void			my_util_extend_rect		(gdouble rectX, gdouble rectY, gdouble rectWidth, gdouble rectHeight, gdouble x, gdouble y) {
	
	if (x < rectX) {
		if (rectWidth > 0) {
			rectWidth += (rectX - x);
		} else {
			rectWidth = 1;
		}
		rectX = x;
	}
	if (x > rectX + rectWidth) {
		rectWidth = (x - rectX);
	}
	if (y < rectY) {
		if (rectHeight > 0) {
			rectHeight += (rectY - y);
		} else {
			rectHeight = 1;
		}
		rectY = y;
	}
	if (y > rectY + rectHeight) {
		rectHeight = (y - rectY);
	}
}
*/


void		my_util_block_position_in_buffer	(unsigned char *buf,
												int buf_stride,
												int buf_height,
												int block_index_x,
												int block_index_y,
												int block_width_byte,
												int block_height_byte,
												int extra_left_byte,
												int extra_top_byte,
												int extra_right_byte,
												int extra_bottom_byte,
												int *out_x,
												int *out_y,
												int *out_width_byte,
												int *out_height_byte,
												int *out_inner_left_byte,
												int *out_inner_top_byte,
												int *out_inner_right_byte,
												int *out_inner_bottom_byte) {
	
	int x, y, width_byte, height_byte;

	assert (buf_stride >= 0);
	assert (buf_height >= 0);
	assert (block_width_byte >= 0);
	assert (block_height_byte >= 0);
	assert (block_index_x >= 0);
	assert (block_index_y >= 0);
	assert (extra_left_byte >= 0);
	assert (extra_top_byte >= 0);
	assert (extra_right_byte >= 0);
	assert (extra_bottom_byte >= 0);
	
	// calculate x
	x = block_index_x * block_width_byte;
	assert (x < buf_stride);
	if (x + block_width_byte < buf_stride) {
		width_byte = block_width_byte;
	} else {
		width_byte = buf_stride - x;
	}
	if (x - extra_left_byte >= 0) {
		x -= extra_left_byte;
		width_byte += extra_left_byte;
		*out_inner_left_byte = extra_left_byte;
	} else {
		*out_inner_left_byte = 0;
	}
	if (x + width_byte + extra_right_byte <= buf_stride) {
		width_byte += extra_right_byte;
		*out_inner_right_byte = extra_right_byte;
	} else {
		*out_inner_right_byte = 0;
	}
	// calculate y
	y = block_index_y * block_height_byte;
	assert (y < buf_height);
	if (y + block_height_byte < buf_height) {
		height_byte = block_height_byte;
	} else {
		height_byte = buf_height - y;
	}
	if (y - extra_top_byte >= 0) {
		y -= extra_top_byte;
		height_byte += extra_top_byte;
		*out_inner_top_byte = extra_top_byte;
	} else {
		*out_inner_top_byte = 0;
	}
	if (y + height_byte + extra_bottom_byte <= buf_height) {
		height_byte += extra_bottom_byte;
		*out_inner_bottom_byte = extra_bottom_byte;
	} else {
		*out_inner_bottom_byte = 0;
	}
	*out_x = x;
	*out_y = y;
	*out_width_byte = width_byte;
	*out_height_byte = height_byte;
}


void 		my_util_memcpy_box_to_box			(unsigned char *src,
												int src_stride,
												int src_height_byte,
												int box_src_x,
												int box_src_y,
												int box_width_byte,
												int box_height_byte,
												unsigned char *dst,
												int box_dst_x,
												int box_dst_y,
												int dst_stride,
												int dst_height_byte) {
		
	int i, j;
	unsigned char *p;	// points to start address of each row of src
	unsigned char *q;	// points to start address of each row of dst

	assert (box_src_x >= 0);
	assert (box_src_y >= 0);
	assert (box_src_x + box_width_byte <= src_stride);
	assert (box_src_y + box_height_byte <= src_height_byte);
	assert (box_dst_x >= 0);
	assert (box_dst_y >= 0);
	assert (box_dst_x + box_width_byte <= dst_stride);
	assert (box_dst_y + box_height_byte <= dst_height_byte);
	
	p = src + box_src_y * src_stride + box_src_x;
	q = dst + box_dst_y * dst_stride + box_dst_x;
	for (i = 0; i < box_height_byte; i++) {
		for (j = 0; j < box_width_byte; j++) {
			q[j] = p[j];
		}
		p += src_stride;
		q += dst_stride;
	}
}


void 		my_util_memcpy_box_to_continuous	(unsigned char *src,
												int src_stride,
												int src_height_byte,
												int box_x,
												int box_y,
												int box_width_byte,
												int box_height_byte,
												unsigned char *dst,
												int dst_offset) {
			
	int i, j;
	unsigned char *p;	// points to start address of each row of src
	unsigned char *q;	// points to address of dst

	assert (box_x >= 0);
	assert (box_y >= 0);
	assert (box_x + box_width_byte <= src_stride);
	assert (box_y + box_height_byte <= src_height_byte);
	assert (dst_offset >= 0);
	
	p = src + box_y * src_stride + box_x;
	q = dst + dst_offset;
	for (i = 0; i < box_height_byte; i++) {
		for (j = 0; j < box_width_byte; j++) {
			*q = p[j];
			q ++;
		}
		p += src_stride;
	}
}

int 	my_util_index_of	(char *str1,char *str2)  
{  
    char *p=str1;  
    int i=0;  
    p=strstr(str1,str2);  
    if(p==NULL)  
        return -1;  
    else{  
        while(str1!=p)  
        {  
            str1++;  
            i++;  
        }  
    }  
    return i;  
}  

/**
 * 计算过P0与P1点的圆的圆心。
 * 返回 TRUE -> 较精确
 *      FALSE -> 计算有错误,返回中点
 */
gboolean	my_util_circle_center	(gdouble x0, gdouble y0, gdouble x1, gdouble y1, gdouble radius, int isFiSide, gdouble *circelCenterX, gdouble *circelCenterY) {


	MyVector v = my_vector_new (x0, y0, x1, y1);
	MyVector fi = my_vector_perpendicular (v);
	MyVector w;
	gdouble a;
	gdouble distance;

	a = my_vector_length (v) / 2;
	if (radius >= a) {
		distance = sqrt (radius * radius - a * a);
		if (isFiSide) {
			w = my_vector_scalar_mul (my_vector_norm (fi), distance);
		}else {
			w = my_vector_scalar_mul (my_vector_scalar_mul (my_vector_norm (fi), -1), distance);
		}
		*circelCenterX = (x0 + x1) / 2 + w.x;
		*circelCenterY = (y0 + y1) / 2 + w.y;
		return TRUE;
	} else {
		*circelCenterX = (x0 + x1) / 2;
		*circelCenterY = (y0 + y1) / 2;
		return FALSE;
	}
}

/**
 * 点在圆的位置,距离为半径+-1即为在圆上
 *
 * 返回	1 -> 点在圆内
 *      0 -> 点在圆上
 *     -1 -> 点在圆外
 */
int			my_util_point_position_to_circle		(gdouble x, gdouble y, gdouble centerX, gdouble centerY, gdouble radius) {


	gdouble distance = sqrt ((x-centerX)*(x-centerX) + (y-centerY)*(y-centerY));
	if (radius - 1 <= distance && distance <= radius + 1) {
		return 0;
	} else if (distance > radius) {
		return -1;
	} else {
		return 1;
	}
}

/**
 * 有向线段与圆交点
 *
 * 返回 0 -> 无交点
 *      1 -> 1个交点
 *      2 -> 2个交点
 */
int			my_util_segment_inter_circle			(gdouble x0, gdouble y0,
													gdouble x1, gdouble y1,
													gdouble centerX, gdouble centerY,
													gdouble radius,
													gdouble *outX1, gdouble *outY1,
													gdouble *outX2, gdouble *outY2) {

	gdouble delta;
	gdouble t1, t2; // 参数方程的两个解
	gdouble a,b,c,d;
	int solveCount = 0;
	a = -x0 + x1;
	b = x0 - centerX;
	c = -y0 + y1;
	d = y0 - centerY;
	delta = (2*a*b+2*c*d)*(2*a*b+2*c*d) - 4*(a*a+c*c)*(b*b+d*d-radius*radius);
	if (delta > 0) {
		t1 = (-2*(a*b+c*d)-sqrt(delta)) / (2*(a*a+c*c));
		t2 = (-2*(a*b+c*d)+sqrt(delta)) / (2*(a*a+c*c));
		if (0 <= t1 && t1 <= 1) {
			solveCount++;
			*outX1 = (1-t1)*x0+t1*x1;
			*outY1 = (1-t1)*y0+t1*y1;
		}
		if (0 <= t2 && t2 <= 1) {
			solveCount++;
			if (solveCount == 1) {
				*outX1 = (1-t2)*x0+t2*x1;
				*outY1 = (1-t2)*y0+t2*y1;
			} else {
				*outX2 = (1-t2)*x0+t2*x1;
				*outY2 = (1-t2)*y0+t2*y1;
			}
		}
	} else if (delta == 0) {
		t1 = t2 = (-2*(a*b+c*d)) / (2*(a*a+c*c));
		if (0 <= t1 && t1 <= 1) {
			*outX1 = (1-t1)*x0+t1*x1;
			*outY1 = (1-t1)*y0+t1*y1;
			solveCount++;
		}
	} else {	// 无解
	}
	return solveCount;
}