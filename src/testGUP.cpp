#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

int main(int argc, char *argv[]) {
    // 1. Initialize Video subsystem
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    // 2. Create a window
    SDL_Window* window = SDL_CreateWindow("SDL3 GPU Clear Example", 800, 600, 0);
    
    // 3. Create the GPU Device (Auto-selects Vulkan, Metal, or D3D12)
    // We specify SPIR-V as a supported shader format for cross-platform compatibility
    SDL_GPUDevice* gpu_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);
    
    // 4. Link the window to the GPU device
    SDL_ClaimWindowForGPUDevice(gpu_device, window);

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
        }

        // 5. Acquire a Command Buffer to send instructions to the GPU
        SDL_GPUCommandBuffer* cmd_buffer = SDL_AcquireGPUCommandBuffer(gpu_device);
        if (cmd_buffer) {
            uint32_t w, h;
            // Get the texture for the current frame
            SDL_GPUTexture* swapchain_tex;
            SDL_AcquireGPUSwapchainTexture(cmd_buffer, window, &swapchain_tex, &w, &h);

            if (swapchain_tex) {
                SDL_GPUColorTargetInfo color_target = { 0 };
                color_target.texture = swapchain_tex;
                color_target.load_op = SDL_GPU_LOADOP_CLEAR;
                color_target.store_op = SDL_GPU_STOREOP_STORE;

                // Set colors individually
                color_target.clear_color.r = 0.1f;
                color_target.clear_color.g = 0.6f;
                color_target.clear_color.b = 0.1f;
                color_target.clear_color.a = 1.0f;
                color_target.load_op = SDL_GPU_LOADOP_CLEAR;
                color_target.store_op = SDL_GPU_STOREOP_STORE;

                // 6. Start a Render Pass to clear the screen
                SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(cmd_buffer, &color_target, 1, NULL);
                SDL_EndGPURenderPass(render_pass);
            }

            // 7. Submit commands to the GPU
            SDL_SubmitGPUCommandBuffer(cmd_buffer);
        }
    }

    // Cleanup
    SDL_ReleaseWindowFromGPUDevice(gpu_device, window);
    SDL_DestroyGPUDevice(gpu_device);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}