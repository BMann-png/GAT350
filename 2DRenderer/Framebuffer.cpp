#include "Framebuffer.h"

//define SLOPE
#define DDA

Framebuffer::Framebuffer(Renderer* renderer, int width, int height)
{
    colorbuffer.width = width;
    colorbuffer.height = height;

    texture = SDL_CreateTexture(renderer->renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, width, height);

    colorbuffer.pitch = colorbuffer.width * sizeof(color_t);
    colorbuffer.data = new uint8_t[colorbuffer.pitch * colorbuffer.height];
}

Framebuffer::~Framebuffer()
{
    SDL_DestroyTexture(texture);
}

void Framebuffer::Update()
{
    SDL_UpdateTexture(texture, nullptr, colorbuffer.data, colorbuffer.pitch);
}

void Framebuffer::Clear(const color_t& color)
{
    for (int i = 0; i < colorbuffer.width * colorbuffer.height; i++)
    {
        ((color_t*)colorbuffer.data)[i] = color;

        //buffer[i * sizeof(color_t)] = color.r;
        //buffer[i * sizeof(color_t) + 1] = color.g;
        //buffer[i * sizeof(color_t) + 2] = color.b;
        //buffer[i * sizeof(color_t) + 3] = color.a;
    }
}

void Framebuffer::DrawPoint(int x, int y, const color_t& color)
{
    if (x < 0 || x >= colorbuffer.width || y < 0 || y >= colorbuffer.height) return;
    
    //((color_t*)colorbuffer.data)[x + y * colorbuffer.width] = color;
    
    //alpha blending
    uint8_t alpha = color.a;
    uint8_t invAlpha = 255 - alpha;

    color_t& destColor = ((color_t*)(colorbuffer.data))[x + y * colorbuffer.width];

    destColor.r = ((color.r * alpha) + (destColor.r * invAlpha)) >> 8;
    destColor.g = ((color.g * alpha) + (destColor.r * invAlpha)) >> 8;
    destColor.b = ((color.b * alpha) + (destColor.r * invAlpha)) >> 8;

    //buffer[index] = color.r;
    //buffer[index + 1] = color.g;
    //buffer[index + 2] = color.b;
    //buffer[index + 3] = color.a;
}

void Framebuffer::DrawRect(int x, int y, int width, int height, const color_t& color)
{
    for (int sy = y; sy < y + height; sy++)
    {
        for (int sx = x; sx < x + width; sx++)
        {
            DrawPoint(sx, sy, color);
        }
    }
}

void Framebuffer::DrawLine(int x1, int y1, int x2, int y2, const color_t& color)
{
    int dx = x2 - x1; //run
    int dy = y2 - y1; //rise
    
    
#if defined(SLOPE)
    //float m = 0;

    //if (dx == 0)
    //{
    //    m = dy;
    //}
    //else
    //{
    //    m = dy / (float)dx; //slope (m)
    //}
    float m = (dx == 0) dy : dy / (float)dx;

    float b = y1 - (m * x1); //y intercept

    if (dx == 0)
    {
        if (y1 > y2) std::swap(y1, y2);
        for (int y = y1; y <= y2; y++)
        {

            DrawPoint(x1, y, color);

        }
    }
    else if (std::abs(dx) > std::abs(dy))
    {
        if (x1 > x2) std::swap(x1, x2);
        for (int x = x1; x <= x2; x++)
        {
            int y = (int)round((m * x) + b);
            DrawPoint(x, y, color);
        }
    }
    else
    {
        if (y1 > y2) std::swap(y1, y2);
        for (int y = y1; y <= y2; y++)
        {
            int x = (int)round((y-b)/m);
            DrawPoint(x, y, color);
        }
    }
#elif defined(DDA)
    int steps = std::max(std::abs(dx), std::abs(dy));
    float ddx = dx / (float)steps;
    float ddy = dy / (float)steps;

    float x = (float)x1;
    float y = (float)y1;

    for (int i = 0; i < steps; i++)
    {
        DrawPoint((int)x, (int)y, color);
        x += ddx;
        y += ddy;
    }
#elif defined(BresenhamsLineAlgorithm)
    bool steep = std::abs(dx) < std::abs(dy);
    if (steep)
    {
        // steep slope
        std::swap(x1, y1);
        std::swap(x2, y2);
}

    if (x1 > x2)
    {
        // left -> right
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    dx = x2 - x1;
    dy = std::abs(y2 - y1);

    int error = dx / 2;
    int ystep = (y1 < y2) ? 1 : -1;

    for (int x = x1, y = y1; x <= x2; x++)
    {
        (steep) ? DrawPoint(y, x, color) : DrawPoint(x, y, color);

        error -= dy;
        if (error < 0)
        {
            y += ystep;
            error += dx;
        }
    }
#endif
}

void Framebuffer::DrawCircle(int cx, int cy, int radius, const color_t& color)
{
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    DrawCircleOctants(cx, cy, x, y, color);
    while (y >= x)
    {
        x++;
        if (d > 0) // east - south
        {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        else // east
        {
            d = d + 4 * x + 6;
        }
        DrawCircleOctants(cx, cy, x, y, color);
    }

}

void Framebuffer::DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, const color_t& color)
{
    // connect the points to form a triangle
    // draw line from x1,y1 -> x2,y2
    DrawLine(x1, y1, x2, y2, color);
    // draw line
    DrawLine(x2, y2, x3, y3, color);
    // draw line
    DrawLine(x3, y3, x1, y1, color);
}

void Framebuffer::DrawImage(int x1, int y1, class Image* image)
{
    for (int y = 0; y < image->colorbuffer.height; y++)
    {
        int sy = y1 + y;
        for (int x = 0; x < image->colorbuffer.width; x++)
        {
            int sx = x1 + x;
            if (sx > colorbuffer.width || sy > colorbuffer.height || sx < 0 || sy < 0) continue;

            color_t color = ((color_t*)image->colorbuffer.data)[x + (y * image->colorbuffer.width)];
            DrawPoint(sx, sy, color);
            //((color_t*)colorbuffer.data)[sx + (sy * colorbuffer.width)] = ((color_t*)image->colorbuffer.data)[x + (y * image->colorbuffer.width)];
        }
    }
}


void Framebuffer::DrawSimpleCurve(int x1, int y1, int x2, int y2, int steps, const color_t& color)
{
    //Bezier Curve

    float dt = 1.0f / steps;
    for (int i = 0; i < steps; i++)
    {
        float t1 = i * dt;
        float t2 = (i + 1) * dt;

        int sx1 = Lerp(x1, x2, t1);
        int sy1 = Lerp(y1, y2, t1);
        int sx2 = Lerp(x1, x2, t2);
        int sy2 = Lerp(y1, y2, t2);

        DrawLine(sx1, sy1, sx2, sy2, color);
    }
}

void Framebuffer::DrawQuadraticCurve(int x1, int y1, int x2, int y2, int x3, int y3, int steps, const color_t& color)
{
    float dt = 1.0f / steps;
    for (int i = 0; i < steps; i++)
    {
        float t1 = i * dt;
        float t2 = (i + 1) * dt;

        float a1 = (float)pow((1.0 - t1), 2.0f);
        float b1 = 2.0f * (1.0f - t1) * t1;
        float c1 = (float)pow(t1, 2.0f);
        
        float a2 = (float)pow((1.0 - t2), 2.0f);
        float b2 = 2.0f * (1.0f - t2) * t2;
        float c2 = (float)pow(t2, 2.0f);

        int sx1 = (int)(a1 * x1 + b1 * x2 + c1 * x3);
        int sy1 = (int)(a1 * y1 + b1 * y2 + c1 * y3);
        int sx2 = (int)(a2 * x1 + b2 * x2 + c2 * x3);
        int sy2 = (int)(a2 * y1 + b2 * y2 + c2 * y3);

        DrawLine(sx1, sy1, sx2, sy2, color);
    }
}

void Framebuffer::DrawCubicCurve(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int steps, const color_t& color)
{
    float dt = 1.0f / steps;
    for (int i = 0; i < steps; i++)
    {
        float t1 = i * dt;
        float t2 = (i + 1) * dt;

        float a1 = float(pow((1.0 - t1), 3.0));
        float b1 = 3.0f * float(pow((1.0 - t1), 2.0)) * t1;
        float c1 = 3.0f * float(pow((t1), 2.0)) * (1.0f - t1);
        float d1 = float(pow(t1, 3.0));

        float a2 = float(pow((1.0 - t2), 3.0));
        float b2 = 3.0f * float(pow((1.0 - t2), 2.0)) * t2;
        float c2 = 3.0f * float(pow((t2), 2.0)) * (1.0f - t2);
        float d2 = float(pow(t2, 3.0));

        int sx1 = (int)(a1 * x1 + b1 * x2 + c1 * x3 + d1 * x4);
        int sy1 = (int)(a1 * y1 + b1 * y2 + c1 * y3 + d1 * y4);

        int sx2 = (int)(a2 * x1 + b2 * x2 + c2 * x3 + d2 * x4);
        int sy2 = (int)(a2 * y1 + b2 * y2 + c2 * y3 + d2 * y4);

        DrawLine(sx1, sy1, sx2, sy2, color);
    }

}

void Framebuffer::DrawCircleOctants(int cx, int cy, int x, int y, const color_t& color)
{
    DrawPoint(cx + x, cy + y, color);
    DrawPoint(cx + x, cy - y, color);
    DrawPoint(cx - x, cy + y, color);
    DrawPoint(cx - x, cy - y, color);

    DrawPoint(cx + y, cy + x, color);
    DrawPoint(cx + y, cy - x, color);
    DrawPoint(cx - y, cy + x, color);
    DrawPoint(cx - y, cy - x, color);
}


int Framebuffer::Lerp(int a, int b, float t)
{
    return (int)(a + ((b - a) * t));
}
