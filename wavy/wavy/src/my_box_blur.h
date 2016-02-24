#ifndef __MY_BOX_BLUR_H__
#define __MY_BOX_BLUR_H__

#include <glib.h>


void	my_box_blur_horizontal	(unsigned char* src,
								  unsigned char* dst,
								  int width,
								  int height,
								  int stride,
								  int radius);
								  
void	my_box_blur_vertical	(unsigned char* src,
								  unsigned char* dst,
								  int width,
								  int height,
								  int stride,
								  int radius);
								  
#endif /* __MY_BOX_BLUR_H__ */