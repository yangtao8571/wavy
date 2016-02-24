#include "my_math.h"

/**
 * 计算平方根倒数
 * http://www.cnblogs.com/pkuoliver/archive/2010/10/06/sotry-about-sqrt.html
 */
float InvSqrt(float x)
{
	float xhalf = 0.5f*x;
	int i = *(int*)&x; // get bits for floating VALUE 
	i = 0x5f375a86- (i>>1); // gives initial guess y0
	x = *(float*)&i; // convert bits BACK to float
	x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy
	return x;
}  