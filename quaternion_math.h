// Source code by Shawn Halayka
// Source code is in the public domain

#ifndef QUATERNION_MATH_H
#define QUATERNION_MATH_H


// For console debug purposes only
//#include <iostream>
//using namespace std;


#include "primitives.h"

#include <string>
#include <cmath>


class quaternion_math
{
public:
	quaternion_math(void)
	{
		temp_a_x = temp_a_y = temp_a_z = temp_a_w = 0;
		temp_b_x = temp_b_y = temp_b_z = temp_b_w = 0;
	}

	void sin(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void sinh(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void exsin(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void exsinh(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void coversin(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void coversinh(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void cos(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void cosh(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void excos(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void excosh(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void versin(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void versinh(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void add(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void sub(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void ln(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void exp(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void sqrt(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void inverse(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void conjugate(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void copy(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void copy_masked(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void swizzle(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void mul(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void pow(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void div(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void sinc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void sinhc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void csc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void csch(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void excsc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void excsch(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void covercsc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void covercsch(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void cscc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void cschc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void cosc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void coshc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void sec(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void sech(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void exsec(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void exsech(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void vercsc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void vercsch(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void secc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void sechc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void tan(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void tanh(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void extan(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void extanh(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void covertan(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void covertanh(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void tanc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void tanhc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void cot(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void coth(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void excot(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void excoth(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void covercot(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void covercoth(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void cotc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);
	void cothc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut);

protected:
	float temp_a_x, temp_a_y, temp_a_z, temp_a_w;
	float temp_b_x, temp_b_y, temp_b_z, temp_b_w;
};



#endif
