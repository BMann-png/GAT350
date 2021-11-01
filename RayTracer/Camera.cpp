#include "Camera.h"

void Camera::Set(const glm::vec3& eye, const glm::vec3& lookAt, const glm::vec3& up, float fov)
{
	this->eye = eye;

	forward = glm::normalize(lookAt - this->eye);
	right = glm::normalize(glm::cross(forward, up));
	this->up = glm::cross(right, forward);

	horizontal = focalLength * viewportWidth * right;
	vertical = focalLength * viewportHeight * this->up;
	lowerLeft = this->eye - (horizontal * 0.5f) - (vertical * 0.5f) + forward * focalLength;

}

glm::vec2 Camera::ScreenToViewport(const glm::vec2& screen)
{
	glm::vec2 viewport{ 0, 0 };

	viewport.x = screen.x / (float)screenSize.x;
	viewport.y = screen.y / (float)screenSize.y;

	return viewport;
}

ray_t Camera::ViewportToRay(const glm::vec2& viewport)
{
	glm::vec3 random = lensRadius * randomInUnitDisk();
	glm::vec3 offset = right * random.x + up * random.y;

	ray_t ray;
	ray.origin = eye + offset;
	ray.direction = lowerLeft + (viewport.x * horizontal) + (viewport.y * vertical) - eye - offset;

	return ray;
}
