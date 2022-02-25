#define _USE_MATH_DEFINES
#include <math.h>

#include "raytracer.h"
#include "omp.h"

RayTracer::RayTracer()
{
	nativeResolution = glm::ivec2(0);
	nativeImg = 0;
	outImg = 0;

	camPos = glm::vec3(0.0f, 0.0f, -250.0f);
	camDir = glm::vec3(0.0f, 0.0f, 1.0f);
	camUp = glm::vec3(0.0f, 1.0f, 0.0f);
	camFocal = 0.1f;
	camFovy = 90;
}

RayTracer::~RayTracer()
{
	if (nativeImg)
	{
		delete nativeImg;
		nativeImg = 0;
	}
}

void RayTracer::SetOutImage(GLubyte* out)
{
	outImg = out;
}

glm::ivec2 RayTracer::GetResolution()
{
	return scene.resolution;
}

bool RayTracer::LoadScene(std::string file)
{
	objects.swap(std::vector<Shape*>());
	lights.swap(std::vector<Light*>());
	bool res = scene.LoadScene(file);
	if (!res)
		return res;
	nativeResolution.x = scene.resolution.x * scene.antialiasLevel;
	nativeResolution.y = scene.resolution.y * scene.antialiasLevel;
	nativeImg = new GLubyte[nativeResolution.x * nativeResolution.y * 3];
	for (auto s : scene.shapes)
	{
		if (s->type == ShapeType::LIGHT)
			lights.push_back((Light*)s);
		else
			objects.push_back(s);
	}
	return res;
}

void RayTracer::SetCamera(glm::vec3 pos, glm::vec3 dir, glm::vec3 up)
{
	camPos = pos;
	camDir = glm::normalize(dir);
	camUp = glm::normalize(up);
}

void RayTracer::SetProjection(float f, float fovy)
{
	camFocal = f;
	if (camFocal <= 0.0f)
		camFocal = 0.1f;
	camFovy = fovy;
	if (camFovy <= 0.0f)
		camFovy = 0.1f;
	else if (camFovy >= 180.0f)
		camFovy = 179.5;
}

float RayTracer::IntersectionDistance(glm::vec3 rayOrg, glm::vec3 rayDir, Shape* self, Shape*& hitObj)
{
	float currDepth = INF;
	int hitIndex = -1;
	int currIndex = -1;
	for (auto s : objects)
	{
		currIndex++;
		if (s == self)
			continue;
		float hitDepth = 0.0f;
		if (s->Hit(rayOrg, rayDir, hitDepth))
		{
			if (hitDepth < currDepth)
			{
				currDepth = hitDepth;
				hitIndex = currIndex;
			}
		}
	}
	if (hitIndex != -1)
		hitObj = objects[hitIndex];
	return currDepth;
}

std::vector<Light*> RayTracer::ShadowRays(glm::vec3 p, Shape* self)
{
	std::vector<Light*> res;
	for (auto l : lights)
	{
		glm::vec3 ray = glm::normalize(l->center - p);
		float lightDist = glm::distance(p, l->center);

		float currDepth = lightDist;
		bool isHit = false;
		for (auto s : objects)
		{
			if (s == self)
				continue;
			float hitDepth = 0.0f;
			if (s->Hit(p, ray, hitDepth))
			{
				if (hitDepth < currDepth)
				{
					isHit = true;
					break;
				}
			}
		}
		if (!isHit)
			res.push_back(l);
	}
	return res;
}

glm::vec3 RayTracer::Phong(glm::vec3 n, glm::vec3 v, glm::vec3 p, Light light, Shape object)
{
	glm::vec3 l = glm::normalize(glm::vec3(light.center - p));
	glm::vec3 r = glm::normalize(glm::reflect(-l, n));
	float sDot = glm::max(glm::dot(l, n), 0.0f);
	glm::vec3 diffuse = light.diff_color * object.diff_color * sDot;
	glm::vec3 specular = glm::vec3(0.0f);
	if (sDot > 0.0f)
		specular = light.spec_color * object.spec_color * glm::pow(glm::max(glm::dot(r, v), 0.0f), object.shininess);
	return diffuse + specular;
}

glm::vec3 RayTracer::Trace(glm::vec3 rayOrg, glm::vec3 rayDir, Shape* self, int depth)
{
	glm::vec3 color = glm::vec3(0.0f);
	
	Shape* hitObj = 0;
	float t = IntersectionDistance(rayOrg, rayDir, self, hitObj);
	if (t == INF)
		return scene.backgroundColor;

	glm::vec3 p = rayOrg + rayDir * t;
	glm::vec3 v = glm::normalize(rayOrg - p);
	glm::vec3 n = glm::vec3(0.0f);
	if (hitObj->type == ShapeType::SPHERE)
		n = glm::normalize(p - hitObj->center);
	else if (hitObj->type == ShapeType::QUAD)
	{
		n = ((Quad*)hitObj)->normal;
		if (glm::dot(n, v) < 0.0f)
			n = -n;
	}
	std::vector<Light*> contributedLights = ShadowRays(p, hitObj);
	for (auto l : contributedLights)
		color += Phong(n, v, p, *l, *hitObj);

	float reflectivity = hitObj->reflectivity;
	if (depth <= 0 || reflectivity == 0.0f)
		return color;

	glm::vec3 reflected = glm::normalize(glm::reflect(rayDir, n));
	glm::vec3 refColor = Trace(p, reflected, hitObj, depth - 1);
	color = (1.0f - reflectivity) * color + reflectivity * refColor;

	return color;
}

void RayTracer::SSAADownScale()
{
	glm::ivec2 res = scene.resolution;
	int numThreads = omp_get_max_threads();
	if (numThreads > 0)
		numThreads--;
	else if (numThreads > 1)
		numThreads -= 2;
	else if (numThreads > 2)
		numThreads -= 3;
	#pragma omp parallel for num_threads(numThreads)
	for (int i = 0; i < res.y; i++)
	{
		for (int j = 0; j < res.x; j++)
		{
			int colorR = 0;
			int colorG = 0;
			int colorB = 0;
			for (int k = 0; k < scene.antialiasLevel; k++)
			{
				colorR += nativeImg[((nativeResolution.y - 1 - i * scene.antialiasLevel - k) * nativeResolution.x + j * scene.antialiasLevel + k) * 3];
				colorG += nativeImg[((nativeResolution.y - 1 - i * scene.antialiasLevel - k) * nativeResolution.x + j * scene.antialiasLevel + k) * 3 + 1];
				colorB += nativeImg[((nativeResolution.y - 1 - i * scene.antialiasLevel - k) * nativeResolution.x + j * scene.antialiasLevel + k) * 3 + 2];
			}
			colorR /= scene.antialiasLevel;
			colorG /= scene.antialiasLevel;
			colorB /= scene.antialiasLevel;

			// Draw
			outImg[((res.y - 1 - i) * res.x + j) * 3] = colorR;
			outImg[((res.y - 1 - i) * res.x + j) * 3 + 1] = colorG;
			outImg[((res.y - 1 - i) * res.x + j) * 3 + 2] = colorB;
		}
	}
}

void RayTracer::RenderFrame()
{
	// Update scene for animations
	scene.UpdateScene();

	// Position world space image plane
	glm::vec3 imgCenter = camPos + camDir * camFocal;
	float imgHeight = 2.0f * camFocal * tan((camFovy / 2.0f) * M_PI / 180.0f);
	float aspect = (float)nativeResolution.x / (float)nativeResolution.y;
	float imgWidth = imgHeight * aspect;
	float deltaX = imgWidth / (float)nativeResolution.x;
	float deltaY = imgHeight / (float)nativeResolution.y;
	glm::vec3 camRight = glm::normalize(glm::cross(camUp, camDir));

	// Starting at top left
	glm::vec3 topLeft = imgCenter - camRight * (imgWidth * 0.5f);
	topLeft += camUp * (imgHeight * 0.5f);
	// Loop through each pixel
	int numThreads = omp_get_max_threads();
	if (numThreads > 0)
		numThreads--;
	else if (numThreads > 1)
		numThreads -= 2;
	else if (numThreads > 2)
		numThreads -= 3;
	#pragma omp parallel for num_threads(numThreads)
	for (int i = 0; i < nativeResolution.y; i++)
	{
		glm::vec3 pixel = topLeft - camUp * ((float)i * deltaY);
		for (int j = 0; j < nativeResolution.x; j++)
		{
			glm::vec3 rayDir = glm::normalize(pixel - camPos);
			// Trace
			glm::vec3 color = Trace(camPos, rayDir, 0, scene.traceDepth);
			if (color.r > 1.0f)
				color.r = 1.0f;
			if (color.g > 1.0f)
				color.g = 1.0f;
			if (color.b > 1.0f)
				color.b = 1.0f;
			// Draw
			nativeImg[((nativeResolution.y - 1 - i) * nativeResolution.x + j) * 3] = color.r * 255;
			nativeImg[((nativeResolution.y - 1 - i) * nativeResolution.x + j) * 3 + 1] = color.g * 255;
			nativeImg[((nativeResolution.y - 1 - i) * nativeResolution.x + j) * 3 + 2] = color.b * 255;

			pixel += camRight * deltaX;
		}
	}

	SSAADownScale();
}
