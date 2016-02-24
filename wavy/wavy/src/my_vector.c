#define MY_LOG_LEVEL 4

#include <glib-object.h>
#include <math.h>
#include "my_vector.h"
#include "my_debug.h"

MyVector	my_vector_new	(gdouble x1, gdouble y1, gdouble x2, gdouble y2) {

	MyVector z;
	z.x = x2 - x1;
	z.y = y2 - y1;
	return z;
}

MyVector	my_vector_add	(MyVector v, MyVector w) {
	MyVector z;
	z.x = v.x + w.x;
	z.y = v.y + w.y;
	return z;
}

MyVector	my_vector_sub	(MyVector v, MyVector w) {
	MyVector z;
	z.x = v.x - w.x;
	z.y = v.y - w.y;
	return z;
}

MyVector	my_vector_scalar_mul	(MyVector v, gdouble a) {
	MyVector z;
	z.x = a * v.x;
	z.y = a * v.y;
	return z;
}


gdouble		my_vector_dot_mul		(MyVector v, MyVector w) {

	gdouble mul = v.x * w.x + v.y * w.y;
	return mul;
}


gdouble		my_vector_length	(MyVector v) {
	gdouble l = sqrt (v.x*v.x + v.y*v.y);
	my_debug ("vector [%f,%f] length %f", v.x, v.y, l);
	return l;
}

MyVector	my_vector_norm	(MyVector v) {
	MyVector w = my_vector_scalar_mul (v, 1 / my_vector_length (v));
	my_debug ("vector [%f,%f] norm [%f,%f]", v.x, v.y, w.x, w.y);
	return w;
}

/**
 * 转90度,延正向
 */
MyVector	my_vector_perpendicular	(MyVector v) {
	MyVector w;
	w.x = -1 * v.y;
	w.y = v.x;
	my_debug ("vector [%f,%f] perpendicular [%f,%f]", v.x, v.y, w.x, w.y);
	return w;
}


gdouble		my_vector_cos_to_vector	(MyVector v, MyVector w) {

	gdouble cos = my_vector_dot_mul (v, w) / my_vector_length (v) / my_vector_length(w);
	return cos;
}


MyVector	my_vector_project		(MyVector v, MyVector toVector) {

	
	MyVector proj = my_vector_scalar_mul (toVector, my_vector_dot_mul (v, toVector) / my_vector_dot_mul (toVector, toVector));
	return proj;
}