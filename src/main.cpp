#include <SDL3/SDL.h>
#include <iostream>
#include <cstdint>
#include <array>


void output(const SDL_Color& color, const std::array<int, 2>& location2D, SDL_Texture* texture, SDL_Renderer* renderer) {
   const int W = 320;
   const int H = 240;
   const int x = static_cast<uint8_t>(location2D[0]);
   const int y = static_cast<uint8_t>(location2D[1]);

   void* pixels;
   int pitch;

   SDL_LockTexture(texture, nullptr, &pixels, &pitch);
   uint32_t* buffer = (uint32_t*)pixels;

   buffer[y * (pitch / 4) + x] = (color.a << 24) | (color.r << 16) | (color.g << 8) | (color.b);;


   SDL_UnlockTexture(texture);

   SDL_RenderClear(renderer);
   SDL_RenderTexture(renderer, texture, nullptr, nullptr);
   SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Direct Pixel Manipulation", 800, 600, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    
    const int W = 320;
    const int H = 240;

    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        W,
        H
    );

    bool running = true;
    SDL_Event e;
    int frame = 0;

    while (running)
    {
        while (SDL_PollEvent(&e)) {
         if (e.type == SDL_EVENT_QUIT)
            running = false;
        }

         SDL_Color color{225, 0, 0, 255};
         std::array<int, 2> position2D = {100, 100};

         for (int y = 0; y < H; y++) {
            for (int x = 0; x < W - 20; x++) {
               position2D[0] = x;
               position2D[1] = y;
               output(color, position2D, texture, renderer);
            }
         }

         output(color, position2D, texture, renderer);

         frame++;
         SDL_Delay(16);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void raysRaytracer(int pixelX, int pixelY) {
    // Placeholder for the GOAT's raytracer function
    std::array<float, 3> cameraPosition; /*conceptualized by the GOAT*/
    std::array<float, 3> cameraDirectionVector; /*conceptualized by the GOAT*/
    std::array<float, 3> rayPosition; /*conceptualized by the GOAT*/
    std::array<float, 3> rayDirectionVector; /*conceptualized by the GOAT*/
    std::array<float, 3> imagePlanePointPreRotation; /*conceptualized by the GOAT*/

    std::array<float, 3> lightPosition; /*conceptualized by the GOAT -- TEMPORARY, will add better lighting later*/ 
    float lightIntensity; /*conceptualized by the GOAT -- TEMPORARY, will add better lighting later*/

}


//to compile: g++ src/main.cpp -IC:/SDL3-3.4.0/x86_64-w64-mingw32/include -LC:/SDL3-3.4.0/x86_64-w64-mingw32/lib -lSDL3 -o sdl3test