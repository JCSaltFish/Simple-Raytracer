#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "scene.h"

Scene::Scene()
{
	backgroundColor = glm::vec3(0.0f);
	traceDepth = 1;
	antialiasLevel = 0;
	resolution = glm::ivec2(800, 800);
}

Scene::~Scene()
{
	for (Shape* s : shapes)
		delete s;
	shapes.swap(std::vector<Shape*>());
}

bool Scene::LoadScene(std::string file)
{
	for (Shape* s : shapes)
		delete s;
	shapes.swap(std::vector<Shape*>());

	std::ifstream in;
	in.open(file, std::ios::in);
	if (!in.is_open())
	{
		std::cout << "Failed to open scene file: " << file << std::endl;
		return false;
	}
	std::string str;
	ShapeType currentType = ShapeType::NONE;
	int currentPosCount = 0;
	float x, y, z;
	int i, j;
	while (std::getline(in, str))
	{
		std::stringstream ss(str);
		std::string key;
		while (1)
		{
			ss >> key;
			if (ss.fail()) break;
			if (key.length() >= 2)
			{
				if (key.substr(0, 2) == "//")
					break;
			}
			
			if (key == "LIGHT")
			{
				Shape* light = new Light;
				shapes.push_back(light);
				currentType = ShapeType::LIGHT;
				currentPosCount = 0;
			}
			else if (key == "SPHERE")
			{
				Shape* shpere = new Sphere;
				shapes.push_back(shpere);
				currentType = ShapeType::SPHERE;
				currentPosCount = 0;
			}
			else if (key == "QUAD")
			{
				Shape* quad = new Quad;
				shapes.push_back(quad);
				currentType = ShapeType::QUAD;
				currentPosCount = 0;
			}
			else if (key == "POS")
			{
				ss >> x >> y >> z;
				if (ss.fail()) break;
				if (currentType == ShapeType::LIGHT)
				{
					Light* l = (Light*)shapes.back();
					l->SetCenter(glm::vec3(x, y, z));
				}
				else if (currentType == ShapeType::SPHERE)
				{
					Sphere* s = (Sphere*)shapes.back();
					s->SetCenter(glm::vec3(x, y, z));
				}
				else if (currentType == ShapeType::QUAD)
				{
					Quad* q = (Quad*)shapes.back();
					if (currentPosCount == 0)
						q->SetV1(glm::vec3(x, y, z));
					else if (currentPosCount == 1)
						q->SetV2(glm::vec3(x, y, z));
					else if (currentPosCount == 2)
						q->SetV3(glm::vec3(x, y, z));
				}
				currentPosCount++;
			}
			else if (key == "RADIUS")
			{
				if (currentType == ShapeType::SPHERE)
				{
					ss >> x;
					if (ss.fail()) break;
					Sphere* s = (Sphere*)shapes.back();
					s->SetRadius(x);
				}
			}
			else if (key == "DIFF")
			{
				ss >> x >> y >> z;
				if (ss.fail()) break;
				shapes.back()->SetDiff(glm::vec3(x, y, z));
			}
			else if (key == "SPEC")
			{
				ss >> x >> y >> z;
				if (ss.fail()) break;
				shapes.back()->SetSpec(glm::vec3(x, y, z));
			}
			else if (key == "SHININESS")
			{
				ss >> x;
				if (ss.fail()) break;
				shapes.back()->SetShininess(x);
			}
			else if (key == "REFLECTIVITY")
			{
				ss >> x;
				if (ss.fail()) break;
				shapes.back()->SetReflectivity(x);
			}
			else if (key == "MOVEDIR")
			{
				ss >> x >> y >> z;
				if (ss.fail()) break;
				shapes.back()->SetMoveDirection(glm::normalize(glm::vec3(x, y, z)));
			}
			else if (key == "MOVEDISTANCE")
			{
				ss >> x;
				if (ss.fail()) break;
				shapes.back()->SetMoveDistance(x);
			}
			else if (key == "MOVESPEED")
			{
				ss >> x;
				if (ss.fail()) break;
				shapes.back()->SetMoveSpeed(x);
			}
			else if (key == "BACKGROUND")
			{
				ss >> x >> y >> z;
				if (ss.fail()) break;
				backgroundColor = glm::vec3(x, y, z);
			}
			else if (key == "RESOLUTION")
			{
				ss >> i >> j;
				if (ss.fail()) break;
				resolution = glm::ivec2(i, j);
			}
			else if (key == "MAXDEPTH")
			{
				ss >> i;
				if (ss.fail()) break;
				traceDepth = i;
			}
			else if (key == "ANTIALIAS")
			{
				ss >> i;
				if (ss.fail()) break;
				antialiasLevel = i;
				if (antialiasLevel <= 0)
					antialiasLevel = 1;
			}
		}
	}
	return true;
}

void Scene::UpdateScene()
{
	for (auto s : shapes)
		s->Move();
}
