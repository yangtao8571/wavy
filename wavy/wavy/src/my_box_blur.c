#define MY_LOG_LEVEL 30

#include <gtk/gtk.h>
#include <math.h>
#include "my_math.h"
#include "my_debug.h"
#include "my_box_blur.h"


void	my_box_blur_horizontal	(unsigned char* src,
								  unsigned char* dst,
								  int width,
								  int height,
								  int stride,
								  int radius) {
	
	
	int x, y, k; // pixel coordinary
	int boxSize = radius * 2 + 1;
	unsigned int totalR, totalG, totalB, totalA;
	unsigned char *p, *q;
	
	my_debug ("my_box_blur_horizontal from %#x to %#x, %d %d %d", src, dst, stride, rowCount, radius);
	g_assert (radius >= 1);

	for (y = 0; y < height; y++) {
		totalR = 0;
		totalG = 0;
		totalB = 0;
		totalA = 0;
		for (k = 0; k < my_min (width, radius); k++) {
			p = src + y * stride + k * 4;
			totalR += p[0];
			totalG += p[1];
			totalB += p[2];
			totalA += p[3];
		}
		for (x = 0; x < width; x++) {
			if (x - radius - 1 >= 0) {
				p = src + y * stride + (x - radius - 1) * 4;
				totalR -= p[0];
				totalG -= p[1];
				totalB -= p[2];
				totalA -= p[3];
			}
			if (x + radius < width) {
				p = src + y * stride + (x + radius) * 4;
				totalR += p[0];
				totalG += p[1];
				totalB += p[2];
				totalA += p[3];
			}
			q = dst + y * stride + x * 4;
			q[0] = totalR / boxSize;
			q[1] = totalG / boxSize;
			q[2] = totalB / boxSize;
			q[3] = totalA / boxSize;
		}
	}
}


void	my_box_blur_vertical	(unsigned char* src,
								  unsigned char* dst,
								  int width,
								  int height,
								  int stride,
								  int radius) {
	
	int x, y, k;
	int boxSize = radius * 2 + 1;
	unsigned int totalR, totalG, totalB, totalA;
	unsigned char *p,	// src pointer, points to address of pixel
				*q;		// dst pointer
	
	for (x = 0; x < width; x++) {
		totalR = 0;
		totalG = 0;
		totalB = 0;
		totalA = 0;
		for (k = 0; k < my_min (height, radius); k++) {
			p = src + k * stride + x * 4;
			totalR += p[0];
			totalG += p[1];
			totalB += p[2];
			totalA += p[3];
		}
		for (y = 0; y < height; y++) {
			if (y - radius - 1 >= 0) {
				p = src + (y - radius - 1) * stride + x * 4;
				totalR -= p[0];
				totalG -= p[1];
				totalB -= p[2];
				totalA -= p[3];
			}
			if (y + radius < height) {
				p = src + (y + radius) * stride + x * 4;
				totalR += p[0];
				totalG += p[1];
				totalB += p[2];
				totalA += p[3];
			}
			q = dst + y * stride + x * 4;
			q[0] = totalR / boxSize;
			q[1] = totalG / boxSize;
			q[2] = totalB / boxSize;
			q[3] = totalA / boxSize;			
		}
	}
}