#pragma once
#include "Renderer.h"
#include "ColorBuffer.h"
#include <string>

class Image
{
public:
	friend class Framebuffer;
	Image() = default;
	Image(const std::string& filename, uint8_t alpha = 255);

	~Image();
	bool Load(const std::string& filename, uint8_t alpha = 255);
	void Flip();

public:
	ColorBuffer colorbuffer;
};
