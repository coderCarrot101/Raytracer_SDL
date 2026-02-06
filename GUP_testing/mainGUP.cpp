#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL.h>
#include <iostream>
#include <cstdint>
#include <array>
#include <cmath>
#include <string.h>

/*
1. Create a Device, which is requesting access to a compatible GPU.
2. Create Buffers, which are containers of data on the GPU.
3. Create a Graphics Pipeline which tells the GPU how to use these buffers.
4. Acquire a Command Buffer to start issuing tasks to the GPU.
5. Fill the buffers with data using Transfer Buffer in a Copy Pass.
6. Acquire the Swapchain texture, or simply the window to draw onto.
7. Finally, issue the Draw Call in a Render Pass.
*/

SDL_Window* window;
SDL_GPUDevice* device;
SDL_GPUBuffer* vertexBuffer;
//A texture, much like a buffer, is just a container that contains the image pixel data in a certain format. A Swapchain is simply a collection of textures that are “swapped” on the window surface one after the other.
SDL_GPUTexture* texture;


Uint32 width, height;

/*
So, how do we draw anything?
How does floats and integers end up as something on the screen? This is done by making a GPU pipeline. The steps needed can be summarized in the following points:

You first create GPU buffers that hold data and upload the data to these GPU buffers. You can create Vertex Buffers that contain positions, colors, textures UV, etc. You can upload Textures that you want to be available to use in shaders. You can also create other types of buffers like Uniform Buffers, Storage Buffers, and Instance Data Buffers. You will need to update all these as the state of your app changes.
You also need Vertex shaders, Fragment shaders, and finally a Graphics pipeline that uses these shaders. The graphics pipeline tells the GPU how to interpret the data provided in buffers and textures. It also allows you to create some awesome effects.
You finally make your draw call after setting the correct state. You can choose which pipeline, which textures, which buffers, what offset, and how many vertices you want the GPU to draw.
*/

struct Vertex {
    float x, y, z;
    float r, g, b, a;
};

// three points that make up the triangle
//Normalized Device Coordinates.
static Vertex vertices[] {
    {0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f},
    {0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
    {0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f}
};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    // create a window
    window = SDL_CreateWindow("Lo, and behold!", 960, 540, SDL_WINDOW_RESIZABLE);

    //Vulkan SDK is needed for deubug
    //Shader format is Vulkan, cause that's pretty platform independent.
    device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, false, NULL);
    SDL_ClaimWindowForGPUDevice(device, window);

    //resizable array of commands to the GPU, can multithread
    SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(device);

    SDL_GPUTransferBufferCreateInfo transferInfo{};
    transferInfo.size = sizeof(vertices); 
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);

    SDL_GPUBufferCreateInfo bufferInfo{};
    bufferInfo.size = sizeof(vertices); 
    bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    vertexBuffer = SDL_CreateGPUBuffer(device, &bufferInfo);

    Vertex* data = (Vertex*)SDL_MapGPUTransferBuffer(device, transferBuffer, false);

    SDL_memcpy(data, vertices, sizeof(vertices));

    SDL_UnmapGPUTransferBuffer(device, transferBuffer);


    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(commandBuffer);
    SDL_GPUTransferBufferLocation location{};
    location.transfer_buffer = transferBuffer;
    location.offset = 0;

    SDL_GPUBufferRegion region{};
    




    SDL_GPUColorTargetInfo Color_Info {};
    SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, window, &texture, &width, &height);

    Color_Info.clear_color = {255/255.0f, 219/255.0f, 187/255.0f, 255/255.0f};
    Color_Info.load_op = SDL_GPU_LOADOP_CLEAR; 

    // store the content to the texture
    Color_Info.store_op = SDL_GPU_STOREOP_STORE;

    // where are we going to store the result?
    Color_Info.texture = texture;



    SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(commandBuffer, &Color_Info, 1, NULL);

    // draw something

    SDL_EndGPURenderPass(renderPass);

    //sends buffer to memory
    SDL_SubmitGPUCommandBuffer(commandBuffer);


    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    // close the window on request
    if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    //release buffers
    SDL_ReleaseGPUBuffer(device, vertexBuffer);

    SDL_DestroyGPUDevice(device);
    // destroy the window
    SDL_DestroyWindow(window);
}

