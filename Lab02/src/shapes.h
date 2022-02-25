#ifndef __SHAPES_H__
#define __SHAPES_H__

#include <glm/glm.hpp>

const float EPSILON = 0.001f;

enum class ShapeType
{
	NONE,
	LIGHT,
	SPHERE,
	QUAD,
};

class Shape
{
public:
	ShapeType type;
	glm::vec3 center;
	glm::vec3 diff_color;
	glm::vec3 spec_color;
	float shininess;
	float reflectivity;
	glm::vec3 moveDirection;
	float moveDistance;
	float moveSpeed;

protected:
	float offset;
	bool moveForward;

public:
	Shape();
	void SetCenter(glm::vec3 pos);
	void SetDiff(glm::vec3 diff);
	void SetSpec(glm::vec3 spec);
	void SetShininess(float s);
	void SetReflectivity(float r);
	void SetMoveDirection(glm::vec3 dir);
	void SetMoveDistance(float dist);
	void SetMoveSpeed(float speed);

	virtual bool Hit(glm::vec3 rayOrg, glm::vec3 rayDir, float& hitDepth);
	virtual void Move();
};

class Light : public Shape
{
public:
	Light();
};

class Sphere : public Shape
{
public:
	float radius;
	Sphere();
	void SetRadius(float r);

	bool Hit(glm::vec3 rayOrg, glm::vec3 rayDir, float& hitDepth);
};

class Quad : public Shape
{
public:
	glm::vec3 vertex1;
	glm::vec3 vertex2;
	glm::vec3 vertex3;
	glm::vec3 vertex4;
	glm::vec3 normal;
	Quad();
	void SetV1(glm::vec3 v1);
	void SetV2(glm::vec3 v2);
	void SetV3(glm::vec3 v3);

	bool Hit(glm::vec3 rayOrg, glm::vec3 rayDir, float& hitDepth);
	void Move();
};

#endif
