#include <SDL.h>
#include <cmath>
#include <cstdio>

using namespace std;

#define N_PLASMAS 2

#define RGB(r, g, b) (r << 16 | g << 8 | b)
#define GRAY(v) RGB(v, v, v)

const int kWidth = 320;
const int kHeight = 240;

float plasma1(float u, float v)
{
    Uint32 ticks = SDL_GetTicks();

    float time = ticks * 0.0025;
    float v1 = sinf(u * 10 + time);
    float v2 = sinf(25 * (u * sinf(time / 2) + v * cosf(time / 3)) + time);
    float cu = u + 0.5f * sinf(time / 5);
    float cv = v + 0.5f * cosf(time / 3);
    float v3 = sinf(sqrtf(200*(cu * cu + cv * cv) + 1) + time);
    // Uint8 gray = 128 + ((v1 + v2 + v3) / 3.0) * 127;

    return v1 + v2 + v3;
}

float plasma2(float u, float v)
{
    Uint32 ticks = SDL_GetTicks();

    float time = ticks * 0.0025;

    float v1 = sinf(16 * u + time);
    float v2 = sinf(32 * v + time / 3);
    float v3 = sinf((u + v) * 16 + time / 2);
    float v4 = sinf(32 * hypot(u, v) + time / 5);

    return (v1 + v2 + v3 + v4) / 4.0;
}

void draw(Uint32* pixels, int plasma)
{
    for (int x = 0; x < kWidth; ++x) {
        for (int y = 0; y < kHeight; ++y) {
            float u = -0.5f + float(x) / kWidth;
            float v = -0.5f + float(y) / kHeight;
            Uint32 ticks = SDL_GetTicks();

            float time = ticks * 0.0025;
            // float u = -0.5f + float(x) / kWidth;
            // float v = -0.5f + float(y) / kHeight;
            // float v1 = sin(u * 10 + time);
            // float v2 = sinf(25 * (u * sinf(time / 2) + v * cosf(time / 3)) + time);
            // float cu = u + 0.5f * sinf(time / 5);
            // float cv = v + 0.5f * cosf(time / 3);
            // float v3 = sinf(sqrtf(200*(cu * cu + cv * cv) + 1) + time);
            // Uint8 gray = 128 + ((v1 + v2 + v3) / 3.0) * 127;

            float gv = 0.0;
            switch (plasma) {
                case 0:
                    gv = plasma1(u, v); // / 3.0;
                    break;
                case 1:
                    gv = plasma2(u, v);
                    break;
            }
            Uint8 r = 255;
            Uint8 g = 255 * (0.5f + 0.5f * cosf(gv * M_PI));
            Uint8 b = 255 * (0.5f + 0.5f * sinf(gv * M_PI));
            pixels[y * kWidth + x] = RGB(r, g, b); //GRAY(gray);
        }
    }
}

int main(int argc, char ** argv)
{
    bool quit = false;
    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("PLASMA",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 3 * kWidth, 3 * kHeight, 0);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    SDL_Texture* texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, kWidth, kHeight);
    Uint32* pixels = new Uint32[kWidth * kHeight];
    memset(pixels, 0, kWidth * kHeight * sizeof(Uint32));

    Uint32 prev = SDL_GetTicks();
    int plasma = 0;
    while (!quit) {
        draw(pixels, plasma);

        SDL_UpdateTexture(texture, NULL, pixels, kWidth * sizeof(Uint32));
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            quit = true;
                            break;
                        case SDLK_p:
                            plasma += 1;
                            plasma = plasma % N_PLASMAS;
                            printf("Plasma #%d\n", plasma);
                            break;
                    }
                    break;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);


        Uint32 now = SDL_GetTicks();
        // printf("ms/frame: %d\n", now - prev);
        prev = now;
    }

    delete[] pixels;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
