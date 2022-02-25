#ifndef __SCENE_H__
#define __SCENE_H__

#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "shapes.h"

class Scene
{
public:
	glm::vec3 backgroundColor;
	int traceDepth;
	int antialiasLevel;
	glm::ivec2 resolution;

	std::vector<Shape*> shapes;

	Scene();
	~Scene();
	bool LoadScene(std::string file);
	void UpdateScene();
};

#endif
