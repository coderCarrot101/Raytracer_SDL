#include <SDL3/SDL.h>
#include <iostream>
#include <cstdint>
#include <array>
#include <cmath>
#include <string.h>
#include "io-utils.h"
#include "vector-utils.h"

#define WINDOW_WIDTH 1920/2
#define WINDOW_HEIGHT 1080/2

int is_running = false;        
int color[] = {255, 255, 255};
bool pressed = false;

void pixel_out(const SDL_Color&, const std::array<int, 2>&, SDL_Texture*, SDL_Renderer*, void*, int);
float get_magnitude(std::array<float, 3>);
SDL_Color rays_raytracer(int, int, int, int);
std::array<float, 3> normalize_vector(std::array<float, 3>);
float dot_product(std::array<float, 3>, std::array<float, 3>);
std::array<float, 3> cross_product(std::array<float, 3>, std::array<float, 3>);

ObjectData Carrot;
ObjectData Cube;

//all code should be ran from the MAIN CODE section - Elijah
int main(int argc, char* argv[]){
   //TODO: add rasterization - Elijah

   SDL_Init(SDL_INIT_VIDEO);

   SDL_Window* window = SDL_CreateWindow("PHREEEEEEEEEEET", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
   SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

   const int W = WINDOW_WIDTH;
   const int H = WINDOW_HEIGHT;

   SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, W, H);

   bool running = true;
   SDL_Event e;
   int frame = 0;
   void* pixels;
   int pitch;

   SDL_Color color{225, 0, 0, 255};
   std::array<int, 2> position2D = {100, 100};

   Carrot = load_object("carrot.obj", 0, 100); // add angle? texture? lightSource?
   Cube = load_object("testcube.obj", 100, 0);




   while (running) {
      while (SDL_PollEvent(&e)) {
         if (e.type == SDL_EVENT_QUIT)
            running = false;
      }

      void* pixels;
      int pitch;

      SDL_LockTexture(texture, nullptr, &pixels, &pitch);
      //if (SDL_LockTexture(texture, nullptr, &pixels, &pitch) != 0) {
      //   printf("Lock Texture failed: %s\n", SDL_GetError()); 
      //}

      ///////////////////////// MAIN CODE /////////////////////////

      for (int y = 0; y < H; y++) {
         for (int x = 0; x < W; x++) {
               std::array<int, 2> position2D = {x, y};
               color = rays_raytracer(x, y, W, H);
               pixel_out(color, position2D, texture, renderer, pixels, pitch);
         }
      }
      ////////////////////////////////////////////////////////////

      SDL_UnlockTexture(texture);
      SDL_RenderClear(renderer);
      //if (SDL_RenderClear(renderer) != 0) {
       //  printf("Clear Render failed: %s\n", SDL_GetError()); 
      //}
      SDL_RenderTexture(renderer, texture, nullptr, nullptr);
      //if (SDL_RenderTexture(renderer, texture, nullptr, nullptr) != 0) {
      //   printf("Render Texture failed: %s\n", SDL_GetError()); 
      //}
      SDL_RenderPresent(renderer);
      //if (SDL_RenderPresent(renderer) != 0) {
      //   printf("Present Render failed: %s\n", SDL_GetError()); 
      //}
      SDL_Delay(16);
   }


   SDL_DestroyTexture(texture);
   SDL_DestroyRenderer(renderer);
   SDL_DestroyWindow(window);
   SDL_Quit();

   return 0;
}

std::array<float, 3> get_intersection_point(std::array<float, 3> rayOrigin, std::array<float, 3> rayDirection, std::array<float, 3> triangleNormal, std::array<std::array<float, 3>, 3> trianglePoints){
   return std::array<float, 3>{multiply_vector_by_scalar(rayDirection, dot_product(trianglePoints[1], triangleNormal)/dot_product(rayDirection, triangleNormal))};
}

bool inside_triangle(std::array<float, 3> rayOrigin, std::array<float, 3> rayDirection, std::array<float, 3> triangleNormal, std::array<std::array<float, 3>, 3> trianglePoints){
   std::array<float, 3> intersectionPoint = get_intersection_point(rayOrigin, rayDirection, triangleNormal, trianglePoints);

   std::array<float, 3> e0 = subtract_vectors(trianglePoints[1], trianglePoints[0]);
   std::array<float, 3> e1 = subtract_vectors(trianglePoints[2], trianglePoints[0]);
   std::array<float, 3> v = subtract_vectors(intersectionPoint, trianglePoints[0]);

   float d00 = dot_product(e0, e0);
   float d01 = dot_product(e0, e1);
   float d11 = dot_product(e1, e1);
   float d20 = dot_product(v, e0);
   float d21 = dot_product(v, e1);

   float denom = (d00 * d11) - powf(d01, 2);
   float beta = ((d11 * d20) - (d01 * d21))/denom;
   float gamma = ((d00 * d21) - (d01 * d20))/denom;
   float alpha = 1 - beta - gamma;

   if (alpha >= 0 && beta >= 0 && gamma >= 0){
      return true;
   } else {
      return false;
   }
}

float moller_trumbore(std::array<float, 3> rayOrigin, std::array<float, 3> rayDirection, std::array<std::array<float, 3>, 3> trianglePoints){
   const float epsilon = 1e-6f;

   std::array<float, 3> e1 = subtract_vectors(trianglePoints[1], trianglePoints[0]);
   std::array<float, 3> e2 = subtract_vectors(trianglePoints[2], trianglePoints[0]);

   std::array<float, 3> p = cross_product(rayDirection, e2);

   float det = dot_product(e1, p);
   if (det < epsilon){ //backface culling included
      return 999;
   }

   std::array<float, 3> tVect = subtract_vectors(rayOrigin, trianglePoints[0]);
   
   float u = dot_product(tVect, p)/det;
   if(u < 0 || u > 1){
      return 999;
   }

   std::array<float, 3> q = cross_product(tVect, e1);

   float v = dot_product(rayDirection, q)/det;
   if(v < 0 || (u + v) > 1){
      return 999;
   }
   
   float t = dot_product(e2, q)/det;
   if(t <= 0){
      return 999;
   }

   return t;
}

 std::vector<float> list_distances(std::array<float, 3> rayOrigin, std::array<float, 3> rayDirection, ObjectData object){
   std::vector<float> intersectDistanceList;
   std::array<float, 3> intersectPoint;
   float intersectDistance;
   for (int i = 0; i < object.facesSize; i++){
      intersectDistanceList.push_back(moller_trumbore(rayOrigin, rayDirection, {object.vertices[object.faces[i][0][0]], object.vertices[object.faces[i][1][0]], object.vertices[object.faces[i][2][0]]}));
   }
   return intersectDistanceList;
 }

 float get_smallest(std::vector<float> list, ObjectData object, std::array<float, 3> lightPosition, std::array<float, 3> rayOrigin, std::array<float, 3> rayDirection){
   float smallest = 999;
   int smallestIndex;
   
   for (int i = 0; i < list.size(); i++){
      if (list[i] < smallest){
         smallest = list[i];
         smallestIndex = 1;
      }
    }
    return get_magnitude(subtract_vectors(add_vectors(rayOrigin, multiply_vector_by_scalar(rayDirection, smallest)), lightPosition));
 }

SDL_Color rays_raytracer(int pixelX, int pixelY, int screenWidth, int screenHeight) {
    // Placeholder for the GOAT's raytracer function - Ray
    std::array<float, 3> cameraPosition = {1.5f, -4.0f, 1.5f}; 
    std::array<float, 3> cameraDirectionVector;
    std::array<float, 3> rayPosition;
    std::array<float, 3> rayDirectionVector;
    std::array<float, 3> imagePlanePointPreRotation;
    std::array<float, 3> colorRGB;
    std::vector<float> intersectDistanceList;
    float intersectDistance;

    std::array<float, 3> lightPosition = {-2, -3, 2}; /*TEMPORARY, will add better lighting later - Ray*/ 
    float lightIntensity = 4; /*TEMPORARY, will add better lighting later - Ray*/

    imagePlanePointPreRotation = {(float)pixelX-(screenWidth/2), (float)screenWidth/2, -((float)pixelY-(screenHeight/2))};
    rayPosition = cameraPosition;
    rayDirectionVector = normalize_vector(imagePlanePointPreRotation);

    float smallest = get_smallest(list_distances(rayPosition, rayDirectionVector, Cube), Cube, lightPosition, rayPosition, rayDirectionVector);

    colorRGB = multiply_vector_by_scalar({255,255,255}, (lightIntensity/powf(smallest, 2)));

    return SDL_Color {(unsigned char)colorRGB[0], (unsigned char)colorRGB[1], (unsigned char)colorRGB[2], 255};
}

//takes the position and color of a pixel and outputs to the screen - Elijah
void pixel_out(const SDL_Color& color, const std::array<int, 2>& location2D, SDL_Texture* texture, SDL_Renderer* renderer, void* pixels, int pitch) {
   
   const int x = location2D[0];
   const int y = location2D[1];

   uint32_t* buffer = (uint32_t*)pixels;

   buffer[y * (pitch / 4) + x] = (color.a << 24) | (color.r << 16) | (color.g << 8) | (color.b);;



}



//to compile: g++ src/main.cpp -IC:/SDL3-3.4.0/x86_64-w64-mingw32/include -LC:/SDL3-3.4.0/x86_64-w64-mingw32/lib -lSDL3 -o sdl3test

/*
recomended standards:
function names: my_function
variable names: myVariable
Constant names: MYCONSTANT
Struct names: Vertex

Always check return values:
example:

if (SDL_LockTexture(texture, nullptr, &pixels, &pitch) != 0) {
   printf("Lock failed: %s", SDL_GetError());
}
*/

/*
brainstorming:

to make it easier to develop we could add commands like
move camera x 50
set camera x 50
set cube y 50
rotate camera y 180
rotate cube x 200

*/