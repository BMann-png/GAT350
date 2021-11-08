#include "Tracer.h"
#include "Scene.h"

void Tracer::Trace(const ColorBuffer& colorbuffer, Scene* scene, Camera* camera)
{
	float aspectRatio = colorbuffer.width / (float)colorbuffer.height;



	for (int y = 0; y < colorbuffer.height; y++)
	{
		for (int x = 0; x < colorbuffer.width; x++)
		{
			glm::vec3 color = { 0, 0, 0 };

			for (int sample = 0; sample < samples; sample++)
			{
				glm::vec2 viewport = camera->ScreenToViewport({ x + random01(), y + random01() });
				viewport.y = 1 - (viewport.y);
				ray_t ray = camera->ViewportToRay(viewport);

				//glm::vec2 point;
				//point.x = x / (float)colorbuffer.width;
				//point.y = 1 - (y / (float)colorbuffer.height);

				//glm::vec3 direction = glm::vec3{ (point * 2.0f) - 1.0f, -1 };
				//direction.y /= aspectRatio;
				//direction = glm::normalize(direction);

				//ray_t ray{ {0,0,0}, direction };

				raycastHit_t hit;
				color += scene->Trace(ray, 0.001f, FLT_MAX, hit, depth);
			}
			//color /= (float)samples;
			color.r = sqrt(color.r / samples);
			color.g = sqrt(color.g / samples);
			color.b = sqrt(color.b / samples);

			colorbuffer.SetColor(x, y, Vec3ToColor(color));
		}
	}
}
