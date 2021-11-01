#pragma once
#include "Camera.h"
#include "ColorBuffer.h"

class Scene;

class Tracer
{
public:
	void Trace(const ColorBuffer& colorbuffer, Scene* scene, Camera* camera);

public:
	int samples = 30;
};