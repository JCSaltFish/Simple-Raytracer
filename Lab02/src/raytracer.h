#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

#include <iostream>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "scene.h"

const float INF = 0XFFFF;

class RayTracer
{
private:
	Scene scene;
	glm::ivec2 nativeResolution;
	GLubyte* nativeImg;
	GLubyte* outImg;

	glm::vec3 camPos;
	glm::vec3 camDir;
	glm::vec3 camUp;
	float camFocal;
	float camFovy;

	std::vector<Shape*> objects;
	std::vector<Light*> lights;

public:
	RayTracer();
	~RayTracer();

private:
	float IntersectionDistance(glm::vec3 rayOrg, glm::vec3 rayDir, Shape* self, Shape*& hitObj);
	std::vector<Light*> ShadowRays(glm::vec3 p, Shape* self);
	glm::vec3 Phong(glm::vec3 n, glm::vec3 v, glm::vec3 p, Light light, Shape object);
	void SSAADownScale();
	glm::vec3 Trace(glm::vec3 rayOrg, glm::vec3 rayDir, Shape* self, int depth);

public:
	void SetOutImage(GLubyte* out);
	glm::ivec2 GetResolution();
	bool LoadScene(std::string file);
	void SetCamera(glm::vec3 pos, glm::vec3 dir, glm::vec3 up);
	void SetProjection(float f, float fovy);
	void RenderFrame();
};

#endif