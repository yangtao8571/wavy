#include <glib-object.h>
#include "my_vector.h"
#include "my_bezier_util.h"

MyVector	calculateBezierPoint	(gdouble t, MyVector p0, MyVector p1, MyVector p2, MyVector p3) {

  gdouble u = 1 - t;
  gdouble tt = t*t;
  gdouble uu = u*u;
  gdouble uuu = uu * u;
  gdouble ttt = tt * t;
 
  MyVector p = my_vector_scalar_mul (p0, uuu); //first term
  p = my_vector_add ( p, my_vector_scalar_mul (p1, 3 * uu * t));//second term
  p = my_vector_add ( p, my_vector_scalar_mul (p2, 3 * u * tt));//third term
  p = my_vector_add ( p, my_vector_scalar_mul (p3, ttt));//fourth term
 
  return p;
}