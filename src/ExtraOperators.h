#pragma once

/**
 * @file ExtraOperators.h
 *
 * @brief Define some glm vector operations.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */


#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

bool operator==(const glm::vec3& a, const glm::vec3& b)
{
	const double epsilon = 0.0000001;

	return glm::abs(a[0] - b[0]) < epsilon &&
		glm::abs(a[1] - b[0]) < epsilon &&
		glm::abs(a[2] - b[2]) < epsilon;
}

bool operator!=(const glm::vec3& a, const glm::vec3& b)
{
	const double epsilon = 0.0000001;
	return !(glm::abs(a[0] - b[0]) < epsilon) ||
		!(glm::abs(a[1] - b[0]) < epsilon) ||
		!(glm::abs(a[2] - b[2]) < epsilon);
}

glm::vec3 operator+(const glm::vec3 v, const double a)
{
	glm::vec3 sum(v);
	sum[0] += a;
	sum[1] += a;
	sum[2] += a;

	return sum;
}

glm::vec3 operator-(const glm::vec3 v, const double a)
{
	glm::vec3 subst(v);
	subst[0] -= a;
	subst[1] -= a;
	subst[2] -= a;

	return subst;
}

glm::vec3 operator*(const glm::vec3 v, const double a)
{
	glm::vec3 product(v);

	product[0] *= a;
	product[1] *= a;
	product[2] *= a;

	return product;
}

glm::vec3 operator/(const glm::vec3 v, const double a)
{
	glm::vec3 division(v);

	division[0] /= a;
	division[1] /= a;
	division[2] /= a;

	return division;
}
