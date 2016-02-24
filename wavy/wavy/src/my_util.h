#ifndef __MY_UTIL_H__
#define __MY_UTIL_H__

#define MY_BLOCK_BEGIN {
#define MY_BLOCK_END }

gchar *		chinese	(gchar *sourceUtf8Bom);

int 		my_util_atoi			(const char *src);
char *		my_util_int_to_str		(int i);
char *		my_util_double_to_str	(double d);
double		my_util_str_to_double	(const char *src);
void		my_util_free_str_arr	(char **strArr);

gdouble		my_util_rotate_point			(gdouble fi, gdouble x, gdouble y, gdouble *rotatedX, gdouble *rotatedY);
gdouble		my_util_rotate_point_with_center(gdouble fi, gdouble centerX, gdouble centerY, gdouble x, gdouble y, gdouble *rotatedX, gdouble *rotatedY);

void		my_util_segment_to_rect			(gdouble x1, gdouble y1, gdouble x2, gdouble y2,
											gdouble *Ax,
											gdouble *Ay,
											gdouble *Bx,
											gdouble *By,
											gdouble *Cx,
											gdouble *Cy,
											gdouble *Dx,
											gdouble *Dy,
											gdouble radius);

gboolean 	my_util_is_point_inside_rect	(gdouble x,
											 gdouble y,
											 gdouble x1,
											 gdouble y1,
											 gdouble x2,
											 gdouble y2,
											 gdouble x3,
											 gdouble y3,
											 gdouble x4,
											 gdouble y4);
											 
gboolean		my_util_is_point_inside_segment	(	gdouble x0, 
													gdouble y0, 
													gdouble x1, 
													gdouble y1, 
													gdouble x, 
													gdouble y, 
													gdouble strokeWidth);

gboolean 		my_util_is_point_in_rect	(	gdouble rectX, 
												gdouble rectY, 
												gdouble width, 
												gdouble height, 
												gdouble x, 
												gdouble y);
												
gboolean		my_util_is_rect_intersection	(	gdouble x0,
													gdouble y0,
													gdouble width0,
													gdouble height0,
													gdouble x1,
													gdouble y1,
													gdouble width1,
													gdouble height1);

gboolean		my_util_is_point_in_path		(GQueue *points, gdouble x, gdouble y, gdouble strokeWidth);
												
MyPoint			my_util_jmline_point	(gdouble x1, gdouble y1, gdouble x2, gdouble y2, gboolean side);

MyPoint			my_util_part_segment	(gdouble x1, gdouble y1, gdouble x2, gdouble y2, gdouble len, gdouble partLen);

typedef gboolean 	(*MyQueueDeleteFunc) (gpointer data, gpointer user_data);

GQueue *		my_util_delete_queue_element	(GQueue *queue, MyQueueDeleteFunc func, gpointer user_data);

/*
void			my_util_extend_rect		(gdouble rectX, gdouble rectY, gdouble rectWidth, gdouble rectHeight, gdouble x, gdouble y);
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
												int *out_extra_left_byte,
												int *out_extra_top_byte,
												int *out_extra_right_byte,
												int *out_extra_bottom_byte);
										
		
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
												int dst_height_byte);


void 		my_util_memcpy_box_to_continuous	(unsigned char *src,
												int src_stride,
												int src_height_byte,
												int box_x,
												int box_y,
												int box_width_byte,
												int box_height_byte,
												unsigned char *dst,
												int dst_offset);


int 		my_util_index_of	(char *str1,char *str2);


gboolean	my_util_circle_center	(gdouble x0, gdouble y0, gdouble x1, gdouble y1, gdouble radius, int isFiSide, gdouble *circelCenterX, gdouble *circelCenterY);

int			my_util_point_position_to_circle		(gdouble x, gdouble y, gdouble centerX, gdouble centerY, gdouble radius);

int			my_util_segment_inter_circle			(gdouble x0, gdouble y0,
													gdouble x1, gdouble y1,
													gdouble centerX, gdouble centerY,
													gdouble radius,
													gdouble *x2, gdouble *y2,
													gdouble *x3, gdouble *y3);
/**
 * 两点间线段长度
 */
#define		my_util_segment_length(x0,y0,x1,y1) sqrt(((x0)-(x1))*((x0)-(x1)) + ((y0)-(y1))*((y0)-(y1)))

#define		my_util_swap(a,b,tmp)	{tmp=a;a=b;b=tmp;}

#endif /* __MY_UTIL_H__ */