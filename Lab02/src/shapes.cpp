#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shapes.h"

Shape::Shape()
{
	type = ShapeType::NONE;
	center = glm::vec3(0.0f);
	diff_color = glm::vec3(0.0f);
	spec_color = glm::vec3(0.0f);
	shininess = 0.0f;
	reflectivity = 0.0f;
	moveDirection = glm::vec3(0.0f);
	moveDistance = 0.0f;
	moveSpeed = 0.0f;
	offset = 0.0f;
	moveForward = true;
}

void Shape::SetCenter(glm::vec3 pos)
{
	center = pos;
}

void Shape::SetDiff(glm::vec3 diff)
{
	diff_color = diff;
}

void Shape::SetSpec(glm::vec3 spec)
{
	spec_color = spec;
}

void Shape::SetShininess(float s)
{
	shininess = s;
}

void Shape::SetReflectivity(float r)
{
	reflectivity = r;
}

void Shape::SetMoveDirection(glm::vec3 dir)
{
	moveDirection = glm::normalize(dir);
}

void Shape::SetMoveDistance(float dist)
{
	moveDistance = dist;
}

void Shape::SetMoveSpeed(float speed)
{
	moveSpeed = speed;
}

bool Shape::Hit(glm::vec3 rayOrg, glm::vec3 rayDir, float& hitDepth)
{
	return false;
}

void Shape::Move()
{
	if (moveDistance == 0.0f || moveSpeed == 0.0f || moveDirection == glm::vec3(0.0f))
		return;
	if (offset > moveDistance)
		moveForward = false;
	else if (offset < 0.0f)
		moveForward = true;
	if (moveForward)
	{
		offset += moveSpeed;
		center += moveDirection * moveSpeed;
	}
	else
	{
		offset -= moveSpeed;
		center -= moveDirection * moveSpeed;
	}
}

Light::Light()
{
	type = ShapeType::LIGHT;
	center = glm::vec3(0.0f);
	diff_color = glm::vec3(0.0f);
	spec_color = glm::vec3(0.0f);
	shininess = 0.0f;
	reflectivity = 0.0f;
}

Sphere::Sphere()
{
	type = ShapeType::SPHERE;
	center = glm::vec3(0.0f);
	radius = 0.0f;
	diff_color = glm::vec3(0.0f);
	spec_color = glm::vec3(0.0f);
	shininess = 0.0f;
	reflectivity = 0.0f;
}

void Sphere::SetRadius(float r)
{
	radius = r;
}

bool Sphere::Hit(glm::vec3 rayOrg, glm::vec3 rayDir, float& hitDepth)
{
	glm::vec3 oc = center - rayOrg;
	float op = glm::dot(rayDir, oc);
	if (op < 0.0f)
		return false;
	float oc2 = glm::dot(oc, oc);
	float d2 = oc2 - op * op;
	if (d2 > radius * radius)
		return false;
	float discriminant = radius * radius - d2;
	if (discriminant < EPSILON)
		hitDepth = op;
	else
	{
		discriminant = sqrt(discriminant);
		hitDepth = op - discriminant;
		if (hitDepth < 0.0f)
			hitDepth = op + discriminant;
	}
	return true;
}

Quad::Quad()
{
	type = ShapeType::QUAD;
	center = glm::vec3(0.0f);
	vertex1 = glm::vec3(0.0f);
	vertex2 = glm::vec3(0.0f);
	vertex3 = glm::vec3(0.0f);
	vertex4 = glm::vec3(0.0f);
	normal = glm::vec3(1.0f, 0.0f, 0.0f);
	diff_color = glm::vec3(0.0f);
	spec_color = glm::vec3(0.0f);
	shininess = 0.0f;
	reflectivity = 0.0f;
}

void Quad::SetV1(glm::vec3 v1)
{
	vertex1 = v1;
}

void Quad::SetV2(glm::vec3 v2)
{
	vertex2 = v2;
}

void Quad::SetV3(glm::vec3 v3)
{
	vertex3 = v3;
	vertex4 = vertex3 + (vertex2 - vertex1);
	center = (vertex2 + vertex3) * 0.5f;
	normal = glm::normalize(glm::cross((vertex2 - vertex1), (vertex3 - vertex1)));
}

bool Quad::Hit(glm::vec3 rayOrg, glm::vec3 rayDir, float& hitDepth)
{
	if (glm::dot(rayDir, normal) == 0.0f)
		return false;
	float d = glm::dot((vertex1 - rayOrg), normal) / glm::dot(rayDir, normal);
	if (d < EPSILON)
		return false;
	glm::vec3 p = d * rayDir + rayOrg;

	float r = 0.0f;
	glm::vec3 v1 = glm::normalize(vertex1 - p);
	glm::vec3 v2 = glm::normalize(vertex2 - p);
	r += glm::acos(glm::dot(v1, v2));
	v1 = glm::normalize(vertex2 - p);
	v2 = glm::normalize(vertex4 - p);
	r += glm::acos(glm::dot(v1, v2));
	v1 = glm::normalize(vertex4 - p);
	v2 = glm::normalize(vertex3 - p);
	r += glm::acos(glm::dot(v1, v2));
	v1 = glm::normalize(vertex3 - p);
	v2 = glm::normalize(vertex1 - p);
	r += glm::acos(glm::dot(v1, v2));
	if (abs(r - 2.0f * M_PI) < EPSILON)
	{
		hitDepth = d;
		return true;
	}
	return false;
}

void Quad::Move()
{
	if (moveDistance == 0.0f || moveSpeed == 0.0f || moveDirection == glm::vec3(0.0f))
		return;
	if (offset > moveDistance)
		moveForward = false;
	else if (offset < 0.0f)
		moveForward = true;
	if (moveForward)
	{
		offset += moveSpeed;
		center += moveDirection * moveSpeed;
		vertex1 += moveDirection * moveSpeed;
		vertex2 += moveDirection * moveSpeed;
		vertex3 += moveDirection * moveSpeed;
		vertex4 += moveDirection * moveSpeed;
	}
	else
	{
		offset -= moveSpeed;
		center -= moveDirection * moveSpeed;
		vertex1 -= moveDirection * moveSpeed;
		vertex2 -= moveDirection * moveSpeed;
		vertex3 -= moveDirection * moveSpeed;
		vertex4 -= moveDirection * moveSpeed;
	}
}
