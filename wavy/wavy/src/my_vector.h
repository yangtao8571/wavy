#ifndef __MY_VECTOR_H__
#define __MY_VECTOR_H__

#include <glib-object.h>

/*
 * ¶þÎ¬ÏòÁ¿
 */
typedef struct _MyVector {
	gdouble x;
	gdouble y;
} MyVector;

MyVector	my_vector_new			(gdouble x1, gdouble y1, gdouble x2, gdouble y2);
MyVector	my_vector_add			(MyVector v, MyVector w);
MyVector	my_vector_sub			(MyVector v, MyVector w);
MyVector	my_vector_scalar_mul	(MyVector v, gdouble a);
gdouble		my_vector_dot_mul		(MyVector v, MyVector w);
gdouble		my_vector_length		(MyVector v);
MyVector	my_vector_norm			(MyVector v);
MyVector	my_vector_perpendicular	(MyVector v);
gdouble		my_vector_cos_to_vector	(MyVector v, MyVector w);
MyVector	my_vector_project		(MyVector v, MyVector toVector);

#endif /*__MY_VECTOR_H__*/
