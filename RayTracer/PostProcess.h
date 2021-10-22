#pragma once
#include "ColorBuffer.h"

namespace PostProcess
{
	void Invert(const ColorBuffer& colorbuffer);
	void Monochrome(const ColorBuffer& colorbuffer);
	void ColorBalance(const ColorBuffer& colorbuffer, int16_t ro, int16_t go, int16_t bo);
	void Brightness(const ColorBuffer& colorbuffer, int16_t brightness);
	void Noise(const ColorBuffer& colorbuffer, uint8_t noise);
	void Threshold(const ColorBuffer& colorbuffer, uint8_t threshold);
	void BoxBlur(const ColorBuffer& colorbuffer);
	void GaussianBlur(const ColorBuffer& colorbuffer);
	void Sharpen(const ColorBuffer& colorbuffer);
	void Edge(const ColorBuffer& colorbuffer, uint8_t threshold);

}
