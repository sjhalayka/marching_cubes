// Source code by Shawn Halayka
// Source code is in the public domain


#include "quaternion_math.h"


void quaternion_math::sin(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	//|V| = sqrt(v.x^2 + v.y^2 + v.z^2)
	//sin(q) = sin(float) * cosh(|V|), cos(float) * sinh(|V|) * V / |V|

	float mag_vector = std::sqrt(qA->y*qA->y + qA->z*qA->z + qA->w*qA->w);

	temp_a_x = qA->x;

	qOut->x = std::sin(temp_a_x) * std::cosh(mag_vector);
	qOut->y = std::cos(temp_a_x) * std::sinh(mag_vector) * qA->y / mag_vector;
	qOut->z = std::cos(temp_a_x) * std::sinh(mag_vector) * qA->z / mag_vector;
	qOut->w = std::cos(temp_a_x) * std::sinh(mag_vector) * qA->w / mag_vector;
}

void quaternion_math::sinh(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	//|V| = sqrt(v.x^2 + v.y^2 + v.z^2)
	//sin(q) = sin(float) * cosh(|V|), cos(float) * sinh(|V|) * V / |V|

	float mag_vector = std::sqrt(qA->y*qA->y + qA->z*qA->z + qA->w*qA->w);

	temp_a_x = qA->x;

	qOut->x = std::sinh(temp_a_x) * std::cos(mag_vector);
	qOut->y = std::cosh(temp_a_x) * std::sin(mag_vector) * qA->y / mag_vector;
	qOut->z = std::cosh(temp_a_x) * std::sin(mag_vector) * qA->z / mag_vector;
	qOut->w = std::cosh(temp_a_x) * std::sin(mag_vector) * qA->w / mag_vector;
}

void quaternion_math::exsin(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion sin_quat;

	sin(qA, 0, &sin_quat);
	sub(&sin_quat, &one_quat, qOut);
}

void quaternion_math::exsinh(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion sinh_quat;

	sinh(qA, 0, &sinh_quat);
	sub(&sinh_quat, &one_quat, qOut);
}

void quaternion_math::coversin(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion sin_quat;

	sin(qA, 0, &sin_quat);
	sub(&one_quat, &sin_quat, qOut);
}

void quaternion_math::coversinh(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion sinh_quat;

	sinh(qA, 0, &sinh_quat);
	sub(&one_quat, &sinh_quat, qOut);
}

void quaternion_math::cos(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	//|V| = sqrt(v.x^2 + v.y^2 + v.z^2)
	//cos(q) = cos(float) * cosh(|V|), -sin(float) * sinh(|V|) * V / |V|

	float mag_vector = std::sqrt(qA->y*qA->y + qA->z*qA->z + qA->w*qA->w);

	temp_a_x = qA->x;

	qOut->x =  std::cos(temp_a_x) * std::cosh(mag_vector);
	qOut->y = -std::sin(temp_a_x) * std::sinh(mag_vector) * qA->y / mag_vector;
	qOut->z = -std::sin(temp_a_x) * std::sinh(mag_vector) * qA->z / mag_vector;
	qOut->w = -std::sin(temp_a_x) * std::sinh(mag_vector) * qA->w / mag_vector;
}

void quaternion_math::cosh(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	//|V| = sqrt(v.x^2 + v.y^2 + v.z^2)
	//cos(q) = cos(float) * cosh(|V|), -sin(float) * sinh(|V|) * V / |V|

	float mag_vector = std::sqrt(qA->y*qA->y + qA->z*qA->z + qA->w*qA->w);

	temp_a_x = qA->x;

	qOut->x = std::cosh(temp_a_x) * std::cos(mag_vector);
	qOut->y = std::sinh(temp_a_x) * std::sin(mag_vector) * qA->y / mag_vector;
	qOut->z = std::sinh(temp_a_x) * std::sin(mag_vector) * qA->z / mag_vector;
	qOut->w = std::sinh(temp_a_x) * std::sin(mag_vector) * qA->w / mag_vector;
}

void quaternion_math::excos(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion cos_quat;

	cos(qA, 0, &cos_quat);
	sub(&cos_quat, &one_quat, qOut);
}

void quaternion_math::excosh(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion cosh_quat;

	cosh(qA, 0, &cosh_quat);
	sub(&cosh_quat, &one_quat, qOut);
}

void quaternion_math::versin(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion cos_quat;

	cos(qA, 0, &cos_quat);
	sub(&one_quat, &cos_quat, qOut);
}

void quaternion_math::versinh(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion cosh_quat;

	cosh(qA, 0, &cosh_quat);
	sub(&one_quat, &cosh_quat, qOut);
}

void quaternion_math::add(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	// Don't need temp variables here, even if the out pointer is equal to the a or b pointer.
	// (this op is too simple to mess up)

	//temp_a_x = qA->x;
	//temp_a_y = qA->y;
	//temp_a_z = qA->z;
	//temp_a_w = qA->w;

	//temp_b_x = qB->x;
	//temp_b_y = qB->y;
	//temp_b_z = qB->z;
	//temp_b_w = qB->w;

	//qOut->x = temp_a_x + temp_b_x;
	//qOut->y = temp_a_y + temp_b_y;
	//qOut->z = temp_a_z + temp_b_z;
	//qOut->w = temp_a_w + temp_b_w;

	qOut->x = qA->x + qB->x;
	qOut->y = qA->y + qB->y;
	qOut->z = qA->z + qB->z;
	qOut->w = qA->w + qB->w;
}

void quaternion_math::sub(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	// Don't need temp variables here, even if the out pointer is equal to the a or b pointer.
	// (this op is too simple to mess up)

	//temp_a_x = qA->x;
	//temp_a_y = qA->y;
	//temp_a_z = qA->z;
	//temp_a_w = qA->w;

	//temp_b_x = qB->x;
	//temp_b_y = qB->y;
	//temp_b_z = qB->z;
	//temp_b_w = qB->w;

	//qOut->x = temp_a_x - temp_b_x;
	//qOut->y = temp_a_y - temp_b_y;
	//qOut->z = temp_a_z - temp_b_z;
	//qOut->w = temp_a_w - temp_b_w;

	qOut->x = qA->x - qB->x;
	qOut->y = qA->y - qB->y;
	qOut->z = qA->z - qB->z;
	qOut->w = qA->w - qB->w;
}

void quaternion_math::ln(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	temp_a_x = qA->x;
	temp_a_y = qA->y;
	temp_a_z = qA->z;
	temp_a_w = qA->w;

	float quat_length = std::sqrt(temp_a_x*temp_a_x + temp_a_y*temp_a_y + temp_a_z*temp_a_z + temp_a_w*temp_a_w);

	// make into unit quaternion_t if necessary
	if(1 != quat_length)
	{
		temp_a_x /= quat_length;
		temp_a_y /= quat_length;
		temp_a_z /= quat_length;
		temp_a_w /= quat_length;
	}

	//ln(q) = 0.5 * ln(float^2 + V.V), atan2(|V|, float) * V / |V|
	float vector_dot_prod = temp_a_y*temp_a_y + temp_a_z*temp_a_z + temp_a_w*temp_a_w;

	float vector_length = std::sqrt(vector_dot_prod);

	qOut->x = 0.5f * std::log(temp_a_x*temp_a_x + vector_dot_prod);
	qOut->y = (std::atan2(vector_length, temp_a_x) * temp_a_y) / vector_length;
	qOut->z = (std::atan2(vector_length, temp_a_x) * temp_a_z) / vector_length;
	qOut->w = (std::atan2(vector_length, temp_a_x) * temp_a_w) / vector_length;
}

void quaternion_math::exp(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	//exp(q) = exp(float) * cos(|V|), exp(float) * sin(|V|) * V / |V|

	float mag_vector = std::sqrt(qA->y*qA->y + qA->z*qA->z + qA->w*qA->w);

	temp_a_x = qA->x;

	qOut->x = std::exp(temp_a_x) * std::cos(mag_vector);
	qOut->y = std::exp(temp_a_x) * std::sin(mag_vector) * qA->y / mag_vector;
	qOut->z = std::exp(temp_a_x) * std::sin(mag_vector) * qA->z / mag_vector;
	qOut->w = std::exp(temp_a_x) * std::sin(mag_vector) * qA->w / mag_vector;
}

void quaternion_math::sqrt(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	if(qA->y == 0 && qA->z == 0 && qA->w == 0)
	{
		if(qA->x >= 0)
		{
			qOut->x = std::sqrt(qA->x);
			qOut->y = 0;
			qOut->z = 0;
			qOut->w = 0;
		}
		else
		{
			qOut->x = std::sqrt(-qA->x); //0;
			qOut->y = 0; //std::sqrt(-qA->x);
			qOut->z = 0;
			qOut->w = 0;
		}
	}
	else
	{
		temp_a_x = std::sqrt(qA->y*qA->y + qA->z*qA->z + qA->w*qA->w);

		if(qA->x >= 0)
		{
			float m = std::sqrt(0.5f * (std::sqrt(qA->x*qA->x + temp_a_x*temp_a_x) + qA->x));
			float l = temp_a_x / (2 * m);
			float t = l / temp_a_x;

			qOut->x = m;
			qOut->y = qA->y * t;
			qOut->z = qA->z * t;
			qOut->w = qA->w * t;
		}
		else
		{
			float l = std::sqrt(0.5f * (std::sqrt(qA->x*qA->x + temp_a_x*temp_a_x) - qA->x));
			float m = temp_a_x / (2 * l);
			float t = l / temp_a_x;

			qOut->x = m;
			qOut->y = qA->y * t;
			qOut->z = qA->z * t;
			qOut->w = qA->w * t;
		}
	}
}

void quaternion_math::inverse(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	// inv(a) = conjugate(a) / norm(a)

	float temp_a_norm = qA->x*qA->x + qA->y*qA->y + qA->z*qA->z + qA->w*qA->w;

	temp_a_x =  qA->x;
	temp_a_y = -qA->y;
	temp_a_z = -qA->z;
	temp_a_w = -qA->w;

	qOut->x = temp_a_x / temp_a_norm;
	qOut->y = temp_a_y / temp_a_norm;
	qOut->z = temp_a_z / temp_a_norm;
	qOut->w = temp_a_w / temp_a_norm;
}

void quaternion_math::conjugate(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	qOut->x =  qA->x;
	qOut->y = -qA->y;
	qOut->z = -qA->z;
	qOut->w = -qA->w;
}

void quaternion_math::copy(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	qOut->x = qA->x;
	qOut->y = qA->y;
	qOut->z = qA->z;
	qOut->w = qA->w;
}

void quaternion_math::copy_masked(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	temp_a_x = qA->x;
	temp_a_y = qA->y;
	temp_a_z = qA->z;
	temp_a_w = qA->w;

	if(qB->x != 0.0)
	{
		if(qB->x == 1.0)
			qOut->x = temp_a_x;
		else if(qB->x == -1.0)
			qOut->x = -temp_a_x;
		else if(qB->x == 2.0)
			qOut->x = temp_a_y;
		else if(qB->x == -2.0)
			qOut->x = -temp_a_y;
		else if(qB->x == 3.0)
			qOut->x = temp_a_z;
		else if(qB->x == -3.0)
			qOut->x = -temp_a_z;
		else if(qB->x == 4.0)
			qOut->x = temp_a_w;
		else if(qB->x == -4.0)
			qOut->x = -temp_a_w;
	}

	if(qB->y != 0.0)
	{
		if(qB->y == 1.0)
			qOut->y = temp_a_x;
		else if(qB->y == -1.0)
			qOut->y = -temp_a_x;
		else if(qB->y == 2.0)
			qOut->y = temp_a_y;
		else if(qB->y == -2.0)
			qOut->y = -temp_a_y;
		else if(qB->y == 3.0)
			qOut->y = temp_a_z;
		else if(qB->y == -3.0)
			qOut->y = -temp_a_z;
		else if(qB->y == 4.0)
			qOut->y = temp_a_w;
		else if(qB->y == -4.0)
			qOut->y = -temp_a_w;
	}

	if(qB->z != 0.0)
	{
		if(qB->z == 1.0)
			qOut->z = temp_a_x;
		else if(qB->z == -1.0)
			qOut->z = -temp_a_x;
		else if(qB->z == 2.0)
			qOut->z = temp_a_y;
		else if(qB->z == -2.0)
			qOut->z = -temp_a_y;
		else if(qB->z == 3.0)
			qOut->z = temp_a_z;
		else if(qB->z == -3.0)
			qOut->z = -temp_a_z;
		else if(qB->z == 4.0)
			qOut->z = temp_a_w;
		else if(qB->z == -4.0)
			qOut->z = -temp_a_w;
	}

	if(qB->w != 0.0)
	{
		if(qB->w == 1.0)
			qOut->w = temp_a_x;
		else if(qB->w == -1.0)
			qOut->w = -temp_a_x;
		else if(qB->w == 2.0)
			qOut->w = temp_a_y;
		else if(qB->w == -2.0)
			qOut->w = -temp_a_y;
		else if(qB->w == 3.0)
			qOut->w = temp_a_z;
		else if(qB->w == -3.0)
			qOut->w = -temp_a_z;
		else if(qB->w == 4.0)
			qOut->w = temp_a_w;
		else if(qB->w == -4.0)
			qOut->w = -temp_a_w;
	}

}

void quaternion_math::swizzle(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	temp_a_x = qA->x;
	temp_a_y = qA->y;
	temp_a_z = qA->z;
	temp_a_w = qA->w;

	if(qB->x == 1.0)
		qOut->x = temp_a_x;
	else if(qB->x == 2.0)
		qOut->x = temp_a_y;
	else if(qB->x == 3.0)
		qOut->x = temp_a_z;
	else
		qOut->x = temp_a_w;

	if(qB->y == 1.0)
		qOut->y = temp_a_x;
	else if(qB->y == 2.0)
		qOut->y = temp_a_y;
	else if(qB->y == 3.0)
		qOut->y = temp_a_z;
	else
		qOut->y = temp_a_w;

	if(qB->z == 1.0)
		qOut->z = temp_a_x;
	else if(qB->z == 2.0)
		qOut->z = temp_a_y;
	else if(qB->z == 3.0)
		qOut->z = temp_a_z;
	else
		qOut->z = temp_a_w;

	if(qB->w == 1.0)
		qOut->w = temp_a_x;
	else if(qB->w == 2.0)
		qOut->w = temp_a_y;
	else if(qB->w == 3.0)
		qOut->w = temp_a_z;
	else
		qOut->w = temp_a_w;
}

void quaternion_math::mul(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	// in case qA and qOut point to the same variable...
	temp_a_x = qA->x;
	temp_a_y = qA->y;
	temp_a_z = qA->z;
	temp_a_w = qA->w;

	temp_b_x = qB->x;
	temp_b_y = qB->y;
	temp_b_z = qB->z;
	temp_b_w = qB->w;

	// perform multiply
	qOut->x = temp_a_x*temp_b_x - temp_a_y*temp_b_y - temp_a_z*temp_b_z - temp_a_w*temp_b_w;
	qOut->y = temp_a_x*temp_b_y + temp_a_y*temp_b_x + temp_a_z*temp_b_w - temp_a_w*temp_b_z;
	qOut->z = temp_a_x*temp_b_z - temp_a_y*temp_b_w + temp_a_z*temp_b_x + temp_a_w*temp_b_y;
	qOut->w = temp_a_x*temp_b_w + temp_a_y*temp_b_z - temp_a_z*temp_b_y + temp_a_w*temp_b_x;
}

void quaternion_math::pow(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	long unsigned int exp = static_cast<long unsigned int>(fabs(qB->x));

	if(0 == exp)
	{
		qOut->x = 1;
		qOut->y = 0;
		qOut->z = 0;
		qOut->w = 0;
	}
	else if(1 == exp)
	{
		qOut->x = qA->x;
		qOut->y = qA->y;
		qOut->z = qA->z;
		qOut->w = qA->w;
	}
	else
	{
		static quaternion temp_quat;
		temp_quat = *qOut = *qA;

		for(long unsigned int i = 1; i < exp; i++)
		{
			mul(qOut, &temp_quat, qOut);
		}
	}
}

void quaternion_math::div(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	// c = a/b

	// c = inv(b) * a
	// inv(b) = conjugate(b) / norm(b)
	// c = (conjugate(b) / norm(b)) * a

	float temp_b_norm = qB->x*qB->x + qB->y*qB->y + qB->z*qB->z + qB->w*qB->w;

	temp_b_x =  qB->x;
	temp_b_y = -qB->y;
	temp_b_z = -qB->z;
	temp_b_w = -qB->w;

	temp_b_x /= temp_b_norm;
	temp_b_y /= temp_b_norm;
	temp_b_z /= temp_b_norm;
	temp_b_w /= temp_b_norm;

	temp_a_x = qA->x;
	temp_a_y = qA->y;
	temp_a_z = qA->z;
	temp_a_w = qA->w;

	qOut->x = temp_b_x*temp_a_x - temp_b_y*temp_a_y - temp_b_z*temp_a_z - temp_b_w*temp_a_w;
	qOut->y = temp_b_x*temp_a_y + temp_b_y*temp_a_x + temp_b_z*temp_a_w - temp_b_w*temp_a_z;
	qOut->z = temp_b_x*temp_a_z - temp_b_y*temp_a_w + temp_b_z*temp_a_x + temp_b_w*temp_a_y;
	qOut->w = temp_b_x*temp_a_w + temp_b_y*temp_a_z - temp_b_z*temp_a_y + temp_b_w*temp_a_x;
}

void quaternion_math::sinc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	if(qA->x == 0)// && qA->y == 0 && qA->z == 0 && qA->w == 0)
	{
		qOut->x = 1;
		qOut->y = 0;
		qOut->z = 0;
		qOut->w = 0;
	}
	else
	{
		quaternion sin_quat;

		sin(qA, 0, &sin_quat);
		div(&sin_quat, qA, qOut);
	}
}

void quaternion_math::sinhc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	if(qA->x == 0)// && qA->y == 0 && qA->z == 0 && qA->w == 0)
	{
		qOut->x = 1;
		qOut->y = 0;
		qOut->z = 0;
		qOut->w = 0;
	}
	else
	{
		quaternion sinh_quat;

		sinh(qA, 0, &sinh_quat);
		div(&sinh_quat, qA, qOut);
	}
}

void quaternion_math::csc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion sin_quat;

	sin(qA, 0, &sin_quat);
	div(&one_quat, &sin_quat, qOut);
}

void quaternion_math::csch(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion sinh_quat;

	sinh(qA, 0, &sinh_quat);
	div(&one_quat, &sinh_quat, qOut);
}

void quaternion_math::excsc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion csc_quat;

	csc(qA, 0, &csc_quat);
	sub(&csc_quat, &one_quat, qOut);
}

void quaternion_math::excsch(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion csch_quat;

	csch(qA, 0, &csch_quat);
	sub(&csch_quat, &one_quat, qOut);
}

void quaternion_math::covercsc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion csc_quat;

	csc(qA, 0, &csc_quat);
	sub(&one_quat, &csc_quat, qOut);
}

void quaternion_math::covercsch(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion csch_quat;

	csch(qA, 0, &csch_quat);
	sub(&one_quat, &csch_quat, qOut);
}

void quaternion_math::cscc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	if(qA->x == 0)// && qA->y == 0 && qA->z == 0 && qA->w == 0)
	{
		qOut->x = 1;
		qOut->y = 0;
		qOut->z = 0;
		qOut->w = 0;
	}
	else
	{
		quaternion csc_quat;

		csc(qA, 0, &csc_quat);
		div(&csc_quat, qA, qOut);
	}
}

void quaternion_math::cschc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	if(qA->x == 0)// && qA->y == 0 && qA->z == 0 && qA->w == 0)
	{
		qOut->x = 1;
		qOut->y = 0;
		qOut->z = 0;
		qOut->w = 0;
	}
	else
	{
		quaternion csch_quat;

		csch(qA, 0, &csch_quat);
		div(&csch_quat, qA, qOut);
	}
}

void quaternion_math::cosc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	if(qA->x == 0)// && qA->y == 0 && qA->z == 0 && qA->w == 0)
	{
		qOut->x = 1;
		qOut->y = 0;
		qOut->z = 0;
		qOut->w = 0;
	}
	else
	{
		quaternion cos_quat;

		cos(qA, 0, &cos_quat);
		div(&cos_quat, qA, qOut);
	}
}

void quaternion_math::coshc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	if(qA->x == 0)// && qA->y == 0 && qA->z == 0 && qA->w == 0)
	{
		qOut->x = 1;
		qOut->y = 0;
		qOut->z = 0;
		qOut->w = 0;
	}
	else
	{
		quaternion cosh_quat;

		cosh(qA, 0, &cosh_quat);
		div(&cosh_quat, qA, qOut);
	}
}

void quaternion_math::sec(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion cos_quat;

	cos(qA, 0, &cos_quat);
	div(&one_quat, &cos_quat, qOut);
}

void quaternion_math::sech(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion cosh_quat;

	cosh(qA, 0, &cosh_quat);
	div(&one_quat, &cosh_quat, qOut);
}

void quaternion_math::exsec(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion sec_quat;

	sec(qA, 0, &sec_quat);
	sub(&sec_quat, &one_quat, qOut);
}

void quaternion_math::exsech(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion sech_quat;

	sech(qA, 0, &sech_quat);
	sub(&sech_quat, &one_quat, qOut);
}

void quaternion_math::vercsc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion sec_quat;

	sec(qA, 0, &sec_quat);
	sub(&one_quat, &sec_quat, qOut);
}

void quaternion_math::vercsch(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion sech_quat;

	sech(qA, 0, &sech_quat);
	sub(&one_quat, &sech_quat, qOut);
}

void quaternion_math::secc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	if(qA->x == 0)// && qA->y == 0 && qA->z == 0 && qA->w == 0)
	{
		qOut->x = 1;
		qOut->y = 0;
		qOut->z = 0;
		qOut->w = 0;
	}
	else
	{
		quaternion sec_quat;

		sec(qA, 0, &sec_quat);
		div(&sec_quat, qA, qOut);
	}
}

void quaternion_math::sechc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	if(qA->x == 0)// && qA->y == 0 && qA->z == 0 && qA->w == 0)
	{
		qOut->x = 1;
		qOut->y = 0;
		qOut->z = 0;
		qOut->w = 0;
	}
	else
	{
		quaternion sech_quat;

		sech(qA, 0, &sech_quat);
		div(&sech_quat, qA, qOut);
	}
}

void quaternion_math::tan(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	quaternion sin_quat;
	quaternion cos_quat;

	sin(qA, 0, &sin_quat);
	cos(qA, 0, &cos_quat);

	div(&sin_quat, &cos_quat, qOut);
}

void quaternion_math::tanh(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	quaternion sinh_quat;
	quaternion cosh_quat;

	sinh(qA, 0, &sinh_quat);
	cosh(qA, 0, &cosh_quat);

	div(&sinh_quat, &cosh_quat, qOut);
}

void quaternion_math::extan(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion tan_quat;

	tan(qA, 0, &tan_quat);
	sub(&tan_quat, &one_quat, qOut);
}

void quaternion_math::extanh(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion tanh_quat;

	tanh(qA, 0, &tanh_quat);
	sub(&tanh_quat, &one_quat, qOut);
}

void quaternion_math::covertan(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion tan_quat;

	tan(qA, 0, &tan_quat);
	sub(&one_quat, &tan_quat, qOut);
}

void quaternion_math::covertanh(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion tanh_quat;

	tanh(qA, 0, &tanh_quat);
	sub(&one_quat, &tanh_quat, qOut);
}

void quaternion_math::tanc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	if(qA->x == 0)// && qA->y == 0 && qA->z == 0 && qA->w == 0)
	{
		qOut->x = 1;
		qOut->y = 0;
		qOut->z = 0;
		qOut->w = 0;
	}
	else
	{
		quaternion tan_quat;

		tan(qA, 0, &tan_quat);
		div(&tan_quat, qA, qOut);
	}
}

void quaternion_math::tanhc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	if(qA->x == 0)// && qA->y == 0 && qA->z == 0 && qA->w == 0)
	{
		qOut->x = 1;
		qOut->y = 0;
		qOut->z = 0;
		qOut->w = 0;
	}
	else
	{
		quaternion tanh_quat;

		tanh(qA, 0, &tanh_quat);
		div(&tanh_quat, qA, qOut);
	}
}

void quaternion_math::cot(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion tan_quat;

	tan(qA, 0, &tan_quat);
	div(&one_quat, &tan_quat, qOut);
}

void quaternion_math::coth(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion tanh_quat;

	tanh(qA, 0, &tanh_quat);
	div(&one_quat, &tanh_quat, qOut);
}

void quaternion_math::excot(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion cot_quat;

	cot(qA, 0, &cot_quat);
	sub(&cot_quat, &one_quat, qOut);
}

void quaternion_math::excoth(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion coth_quat;

	coth(qA, 0, &coth_quat);
	sub(&coth_quat, &one_quat, qOut);
}

void quaternion_math::covercot(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion cot_quat;

	cot(qA, 0, &cot_quat);
	sub(&one_quat, &cot_quat, qOut);
}

void quaternion_math::covercoth(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	static quaternion one_quat(1, 0, 0, 0);
	quaternion coth_quat;

	coth(qA, 0, &coth_quat);
	sub(&one_quat, &coth_quat, qOut);
}

void quaternion_math::cotc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	if(qA->x == 0)// && qA->y == 0 && qA->z == 0 && qA->w == 0)
	{
		qOut->x = 1;
		qOut->y = 0;
		qOut->z = 0;
		qOut->w = 0;
	}
	else
	{
		quaternion cot_quat;

		cot(qA, 0, &cot_quat);
		div(&cot_quat, qA, qOut);
	}
}

void quaternion_math::cothc(const quaternion *const qA, const quaternion *const qB, quaternion *const qOut)
{
	if(qA->x == 0)// && qA->y == 0 && qA->z == 0 && qA->w == 0)
	{
		qOut->x = 1;
		qOut->y = 0;
		qOut->z = 0;
		qOut->w = 0;
	}
	else
	{
		quaternion coth_quat;

		coth(qA, 0, &coth_quat);
		div(&coth_quat, qA, qOut);
	}
}
