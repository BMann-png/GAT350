#include "Framebuffer.h"
#include "PostProcess.h"
#include <iostream>
#include <SDL.h>

int main(int, char**)
{
	const int WIDTH = 800;
	const int HEIGHT = 600;

	std::unique_ptr<Renderer> renderer = std::make_unique<Renderer>();
	renderer->Initialize(WIDTH, HEIGHT);

	std::unique_ptr<Framebuffer> framebuffer = std::make_unique<Framebuffer>(renderer.get(), renderer.get()->width, renderer.get()->height);
	
	std::unique_ptr<Image> image = std::make_unique<Image>();
	image->Load("../resources/flower.bmp", 190);
	image->Flip();


	bool quit = false;
	SDL_Event event;
	while (!quit)
	{
		SDL_PollEvent(&event);
		switch (event.type)
		{
		case SDL_QUIT:
			quit = true;
			break;
		}

		framebuffer->Clear({ 0, 0, 0, 255 });
		for (int i = 0; i < 40; i++)
		{
			framebuffer->DrawRect(rand() % renderer->width, rand() % renderer->height, 50, 50, { (uint8_t)((rand() % 2) * 255), (uint8_t)((rand() % 2) * 255), (uint8_t)((rand() % 2) * 255), (uint8_t)(rand() % 256) });
		}
		/*for (int i = 0; i < 100; i++)
		{
			framebuffer->DrawPoint(rand() % renderer->width, rand() % renderer->height, { (uint8_t)((rand() % 2) * 255), (uint8_t)((rand() % 2) * 255), (uint8_t)((rand() % 2) * 255), (uint8_t)(rand() % 256) });
		}
		for (int i = 0; i < 5; i++)
		{
			framebuffer->DrawRect(rand() % renderer->width, rand() % renderer->height, 50, 50, { 0, 255, 0, 255 });
		}    
		for (int i = 0; i < 10; i++)
		{
			framebuffer->DrawLine(renderer->width/2, renderer->height/2, rand() % renderer->width, rand() % renderer->height, { 255, 255, 0, 255 });
		}
		framebuffer->DrawTriangle(400, 250, 450, 350, 350, 350, { 255, 255, 255, 255 });

		framebuffer->DrawSimpleCurve(200, 200, 300, 100, 3, { 255, 255, 255, 255 });
		framebuffer->DrawQuadraticCurve(200, 200, 300, 100, 400, 200, 15, { 255, 255, 255, 255 });
		framebuffer->DrawCubicCurve(200, 200, 300, 500, 400, 70, 600, 50, 30, { 255, 255, 255, 255 });

		framebuffer->DrawCircle(400, 300, 100, { 255, 255, 255, 255 });*/

		framebuffer->DrawImage(300, 50, image.get());

		std::unique_ptr<Image> image1 = std::make_unique < Image>(*image.get());
		PostProcess::BoxBlur(image1->colorbuffer);
		framebuffer->DrawImage(0, 300, image1.get());

		std::unique_ptr<Image> image2 = std::make_unique < Image>(*image.get());
		PostProcess::GaussianBlur(image2->colorbuffer);
		framebuffer->DrawImage(200, 300, image2.get());

		std::unique_ptr<Image> image3 = std::make_unique < Image>(*image.get());
		PostProcess::Sharpen(image3->colorbuffer);
		framebuffer->DrawImage(400, 300, image3.get());

		std::unique_ptr<Image> image4 = std::make_unique < Image>(*image.get());
		PostProcess::Edge(image4->colorbuffer, -255);
		framebuffer->DrawImage(600, 300, image4.get());

		//PostProcess::Invert(framebuffer->colorbuffer);
		//PostProcess::Monochrome(framebuffer->colorbuffer);
		//PostProcess::Noise(framebuffer->colorbuffer, 100);
		//PostProcess::Brightness(framebuffer->colorbuffer, 100);
		//PostProcess::Brightness(framebuffer->colorbuffer, -100);
		//PostProcess::ColorBalance(framebuffer->colorbuffer, 0, 0, 100);
		//PostProcess::Threshold(framebuffer->colorbuffer, 200);

		framebuffer->Update();

		renderer->CopyBuffer(framebuffer.get());
		renderer->Present();
	}

	SDL_Quit();

	return 0;
}
