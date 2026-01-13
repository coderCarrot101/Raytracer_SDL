#include <SDL3/SDL.h>
#include <iostream>
#include <cstdint>
/*
// Window width and height
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

// Global variables
int is_running = false;        
SDL_Window *window = NULL;     
SDL_Renderer *renderer = NULL;
int color[] = {255, 255, 255};
bool pressed = false;

//use SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_STREAMING, width, height) ??

// Function prototypes
int init_window(void);         // Create window and renderer
void init_vars(void);          // Initialize variables
void process_event(void);      // Process events
void update_screen();          // Update values
void draw_screen(void);        // Draw screen
void destroy_window(void);     // Destroy window



int main(int argc, char* argv[])
{
    printf("SUCCESS?");
    // Create window and renderer
    is_running = init_window();

    // Initialize variables
    init_vars();

    // Main loop
    while (is_running) {
       process_event();// Processing SDL events (Here keyboard inputs)
       update_screen(); // Updating variables
       draw_screen();   // Drawing objects on the window (Rendering)
    }

    // Destroy renderer and SDL window
    destroy_window();

    return 0;
}

// Create window and renderer
int init_window(void)
{
    // Initialize the SDL library.
    if(SDL_Init(SDL_INIT_VIDEO) == false) {
       SDL_Log("SDL init error: %s\n", SDL_GetError());
       return false;
    }

    // Create a window with the specified dimensions and flags.
    window = SDL_CreateWindow("SDL3 window", 640, 480, SDL_WINDOW_OPENGL);

    // Check window
    if(window == NULL) {
       SDL_Log("Window creation error: %s\n", SDL_GetError());
       return false;
    }

    // Create a 2D rendering context for the window.
    renderer = SDL_CreateRenderer(window, NULL);

    // Check renderer
    if(renderer == NULL) {
       SDL_Log("Renderer creation error: %s\n", SDL_GetError());
       return false;
    }

    return true;
}

// Initialization function that runs only once at the beginning of the program
void init_vars(void)
{

}

// Function to control SDL events and process keyboard inputs
void process_event(void)
{

    SDL_Event event;

    // Creating a loop to process user inputs
    while (SDL_PollEvent(&event)) {
         switch (event.type) {
            case SDL_EVENT_QUIT: // Logout action by the user (x button at the top right of the window)
               is_running = false; // Terminates the execution of the program main loop.
               break;
            case SDL_EVENT_KEY_DOWN: // Indicate that a key was pressed.
               switch(event.key.key) {
                  case SDLK_ESCAPE:
                     is_running = false; // Terminates the execution of the program main loop.
                     break;
                  case SDLK_A:
                     pressed = true; 
                     printf("\npressed\n");
                     break;
               }
               break;
            case SDL_EVENT_KEY_UP:
               pressed = false;
               break;
         }
    }
}

// Updates objects in the main window
void update_screen(void)
{

   if (pressed == true) {
      color[0] = 0;
   } else {
      color[0] = 225;
   }
   printf("\n%d", color[0]);
}

// Render function used to draw game objects in the main window
void draw_screen(void)
{
   
   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Set the color used for drawing operations.
   SDL_RenderClear(renderer); // Clear the current rendering target with the drawing color.

   // Setting rectangle dimensions
   SDL_FRect frect = { (WINDOW_WIDTH-160)/2, (WINDOW_HEIGHT-120)/2, 160, 120 };
   SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], 255); // Set the color to fill rectangle.
   SDL_RenderFillRect(renderer, &frect); // Fill a rectangle on the current rendering target with the drawing color at subpixel precision.

   // Loading all objects drawn one by one to the back buffer to the front buffer at once and loading them onto the screen
   // This process prevents each drawn object from being displayed on the screen one by one.
   SDL_RenderPresent(renderer); // Update on screen all operations performed since the previous call
}

// Destroy Renderer and SDL window, exit from SDL3
void destroy_window(void)
{

   SDL_DestroyRenderer(renderer);
   SDL_DestroyWindow(window);
   SDL_Quit();
}
*/

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow(
        "Direct Pixel Manipulation",
        800,
        600,
        0
    );

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
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
                running = false;
        }

        void* pixels;
        int pitch;

        SDL_LockTexture(texture, nullptr, &pixels, &pitch);
        uint32_t* buffer = (uint32_t*)pixels;

        for (int y = 0; y < H; y++) {
            for (int x = 0; x < W; x++) {
                uint8_t r = (x + frame) % 256;
                uint8_t g = (y + frame) % 256;
                uint8_t b = 128;

                buffer[y * (pitch / 4) + x] = (255 << 24) | (r   << 16) | (g   << 8 ) | (b);
            }
        }

        SDL_UnlockTexture(texture);

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        frame++;
        SDL_Delay(16);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


//to compile: g++ src/main.cpp  -IC:/SDL3-3.4.0/x86_64-w64-mingw32/include  -LC:/SDL3-3.4.0/x86_64-w64-mingw32/lib  -lSDL3  -o sdl3test