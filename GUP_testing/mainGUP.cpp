#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <iostream>
#include <cstdint>
#include <array>
#include <cmath>
#include <string.h>
#include "io-utils.h"
#include "vector-utils.h"
#include "shaders.h"

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

// the vertex input layout
struct Vertex
{
    float x, y, z;      //vec3 position
    float r, g, b, a;   //vec4 color
};

// a list of vertices
static Vertex vertices[] = {
    // position (x, y, z, w)     // color (ignored or debug)
    { -1.0f, -1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f },
    {  3.0f, -1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f },
    { -1.0f,  3.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f },
};
// a list of vertices
// static Vertex vertices[]
// {
//     { 0.0f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f, 1.0f },
//     { -0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f },
//     { 0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f, 1.0f }
// };
SDL_Window* window;
SDL_GPUDevice* device;
SDL_GPUBuffer* vertexBuffer;
SDL_GPUTransferBuffer* transferBuffer;
SDL_GPUGraphicsPipeline* graphicsPipeline;

const int W = WINDOW_WIDTH;
const int H = WINDOW_HEIGHT;

std::vector<uint32_t> pixels(W * H);

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {


    // for (int y = 0; y < H; y++) {
    //     for (int x = 0; x < W; x++) {
    //         SDL_Color c = rays_raytracer(x, y, W, H);
    //         pixels[y * W + x] = (c.a << 24) | (c.r << 16) | (c.g << 8) | (c.b);
    //     }
    // }
    // create a window
    window = SDL_CreateWindow("Lo, and behold! A Triangle!", W, H, SDL_WINDOW_RESIZABLE);
    
    // create the device
    device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);
    SDL_ClaimWindowForGPUDevice(device, window);
    
    // load the vertex shader code
    size_t vertexCodeSize; 
    void* vertexCode = SDL_LoadFile("shaders/vertex.spv", &vertexCodeSize);

    // create the vertex shader
    SDL_GPUShaderCreateInfo vertexInfo{};
    vertexInfo.code = (Uint8*)vertexCode;
    vertexInfo.code_size = vertexCodeSize;
    vertexInfo.entrypoint = "main";
    vertexInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
    vertexInfo.stage = SDL_GPU_SHADERSTAGE_VERTEX;
    vertexInfo.num_samplers = 0;
    vertexInfo.num_storage_buffers = 0;
    vertexInfo.num_storage_textures = 0;
    vertexInfo.num_uniform_buffers = 0;

    SDL_GPUShader* vertexShader = SDL_CreateGPUShader(device, &vertexInfo);

    // free the file
    SDL_free(vertexCode);

    // load the fragment shader code
    size_t fragmentCodeSize; 
    void* fragmentCode = SDL_LoadFile("shaders/fragment.spv", &fragmentCodeSize);

    // create the fragment shader
    SDL_GPUShaderCreateInfo fragmentInfo{};
    fragmentInfo.code = (Uint8*)fragmentCode;
    fragmentInfo.code_size = fragmentCodeSize;
    fragmentInfo.entrypoint = "main";
    fragmentInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
    fragmentInfo.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    fragmentInfo.num_samplers = 0;
    fragmentInfo.num_storage_buffers = 0;
    fragmentInfo.num_storage_textures = 0;
    fragmentInfo.num_uniform_buffers = 0;

    SDL_GPUShader* fragmentShader = SDL_CreateGPUShader(device, &fragmentInfo);

    // free the file
    SDL_free(fragmentCode);

    // create the graphics pipeline
    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.vertex_shader = vertexShader;
    pipelineInfo.fragment_shader = fragmentShader;
    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    
    // describe the vertex buffers
    SDL_GPUVertexBufferDescription vertexBufferDesctiptions[1];
    vertexBufferDesctiptions[0].slot = 0;
    vertexBufferDesctiptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    vertexBufferDesctiptions[0].instance_step_rate = 0;
    vertexBufferDesctiptions[0].pitch = sizeof(Vertex);

    pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = vertexBufferDesctiptions;

    // describe the vertex attribute
    SDL_GPUVertexAttribute vertexAttributes[2];

    // a_position
    vertexAttributes[0].buffer_slot = 0;
    vertexAttributes[0].location = 0;
    vertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
    vertexAttributes[0].offset = 0;

    // a_color
    vertexAttributes[1].buffer_slot = 0;
    vertexAttributes[1].location = 1;
    vertexAttributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
    vertexAttributes[1].offset = sizeof(float) * 3;

    pipelineInfo.vertex_input_state.num_vertex_attributes = 2;
    pipelineInfo.vertex_input_state.vertex_attributes = vertexAttributes;
    // describe the color target
    SDL_GPUColorTargetDescription colorTargetDescriptions[1];
    colorTargetDescriptions[0] = {};
    colorTargetDescriptions[0].blend_state.enable_blend = true;
    colorTargetDescriptions[0].blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTargetDescriptions[0].blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTargetDescriptions[0].blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTargetDescriptions[0].blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTargetDescriptions[0].blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTargetDescriptions[0].blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTargetDescriptions[0].format = SDL_GetGPUSwapchainTextureFormat(device, window);

    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.color_target_descriptions = colorTargetDescriptions;

    // create the pipeline
    graphicsPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);

    // we don't need to store the shaders after creating the pipeline
    SDL_ReleaseGPUShader(device, vertexShader);
    SDL_ReleaseGPUShader(device, fragmentShader);

    // create the vertex buffer
    SDL_GPUBufferCreateInfo bufferInfo{};
    bufferInfo.size = sizeof(vertices);
    bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    vertexBuffer = SDL_CreateGPUBuffer(device, &bufferInfo);

    // create a transfer buffer to upload to the vertex buffer
    SDL_GPUTransferBufferCreateInfo transferInfo{};
    transferInfo.size = sizeof(vertices);
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);

    // fill the transfer buffer
    Vertex* data = (Vertex*)SDL_MapGPUTransferBuffer(device, transferBuffer, false);
    
    SDL_memcpy(data, (void*)vertices, sizeof(vertices));

    SDL_UnmapGPUTransferBuffer(device, transferBuffer);

    // start a copy pass
    SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(device);
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(commandBuffer);

    // where is the data
    SDL_GPUTransferBufferLocation location{};
    location.transfer_buffer = transferBuffer;
    location.offset = 0;
    
    // where to upload the data
    SDL_GPUBufferRegion region{};
    region.buffer = vertexBuffer;
    region.size = sizeof(vertices);
    region.offset = 0;

    // upload the data
    SDL_UploadToGPUBuffer(copyPass, &location, &region, true);

    // end the copy pass
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    // acquire the command buffer
    SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(device);

    // get the swapchain texture
    SDL_GPUTexture* swapchainTexture;
    Uint32 width, height;
    SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, window, &swapchainTexture, &width, &height);

    // end the frame early if a swapchain texture is not available
    if (swapchainTexture == NULL)
    {
        SDL_SubmitGPUCommandBuffer(commandBuffer);
        return SDL_APP_CONTINUE;
    }

    // create the color target
    SDL_GPUColorTargetInfo colorTargetInfo{};
    colorTargetInfo.clear_color = {240/255.0f, 240/255.0f, 240/255.0f, 255/255.0f};
    colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.texture = swapchainTexture;

    // begin a render pass
    SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, NULL);

    SDL_BindGPUGraphicsPipeline(renderPass, graphicsPipeline);
    SDL_GPUBufferBinding bufferBindings[1];
    bufferBindings[0].buffer = vertexBuffer;
    bufferBindings[0].offset = 0;

    SDL_BindGPUVertexBuffers(renderPass, 0, bufferBindings, 1);


    //DRAWWWW
    SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);

    // end the render pass
    SDL_EndGPURenderPass(renderPass);

    // submit the command buffer
    SDL_SubmitGPUCommandBuffer(commandBuffer);

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
    // release buffers
    SDL_ReleaseGPUBuffer(device, vertexBuffer);
    SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
    
    // release graphics pipeline
    SDL_ReleaseGPUGraphicsPipeline(device, graphicsPipeline);

    SDL_DestroyGPUDevice(device);
    SDL_DestroyWindow(window);
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
      return false;
   }

   std::array<float, 3> tVect = subtract_vectors(rayOrigin, trianglePoints[0]);
   
   float u = dot_product(tVect, p)/det;
   if(u < 0 || u > 1){
      return false;
   }

   std::array<float, 3> q = cross_product(tVect, e1);

   float v = dot_product(rayDirection, q)/det;
   if(v < 0 || (u + v) > 1){
      return false;
   }
   
   float t = dot_product(e2, q)/det;
   if(t <= epsilon){
      return false;
   }

   return t;
}

SDL_Color rays_raytracer(int pixelX, int pixelY, int screenWidth, int screenHeight) {
    // Placeholder for the GOAT's raytracer function - Ray
    std::array<float, 3> cameraPosition = {1.5f, -4.0f, 1.5f}; 
    std::array<float, 3> cameraDirectionVector;
    std::array<float, 3> rayOrigin;
    std::array<float, 3> rayDirection;
    std::array<float, 3> imagePlanePointPreRotation;
    std::array<float, 3> colorRGB;
    float intersectDistance;

    ObjectData object = Cube;

    std::array<float, 3> lightPosition = {0, -2, 2}; /*TEMPORARY, will add better lighting later - Ray*/ 
    std::array<float, 3> lightColor = {255, 255, 255};
    std::array<float, 3> ambientLight = {25, 25, 50};
    float lightIntensity = 4; /*TEMPORARY, will add better lighting later - Ray*/

    imagePlanePointPreRotation = {(float)pixelX-(screenWidth/2), (float)screenWidth/2, -((float)pixelY-(screenHeight/2))};
    rayOrigin = cameraPosition;
    rayDirection = normalize_vector(imagePlanePointPreRotation);

    std::vector<float> intersectDistanceList;
    std::array<float, 3> intersectPoint;

    for (int i = 0; i < object.facesSize; i++){
       intersectDistanceList.push_back(moller_trumbore(rayOrigin, rayDirection, {object.vertices[object.faces[i][0][0]], object.vertices[object.faces[i][1][0]], object.vertices[object.faces[i][2][0]]}));
    }

    float smallest = 999;
    int smallestIndex = -1;
   
    for (int i = 0; i < intersectDistanceList.size(); i++){
       if (intersectDistanceList[i] < smallest && intersectDistanceList[i] != false){
          smallest = intersectDistanceList[i];
          smallestIndex = i;
       }
    }

    if (smallestIndex == -1) {
      return SDL_Color{0, 0, 0, 255}; // background
    }
    
    intersectPoint = add_vectors(rayOrigin, multiply_vector_by_scalar(rayDirection, smallest));
    std::array<float, 3> lightVector = subtract_vectors(lightPosition, intersectPoint);

    //colorRGB = multiply_vector_by_scalar({255,255,255}, light_attenuation(get_magnitude(lightVector), lightIntensity));
    //colorRGB = diffuse_shader({255, 255, 255}, object.vertex_normals[object.faces[smallestIndex][0][2]], lightVector, lightColor);
    //std::cout << object.vertex_normals[object.faces[smallestIndex][0][2]][0] << " " << object.vertex_normals[object.faces[smallestIndex][0][2]][1] << " " << object.vertex_normals[object.faces[smallestIndex][0][2]][2] << "\n";
    //colorRGB = specular_shader(lightColor, lightVector, object.vertex_normals[object.faces[smallestIndex][0][2]], subtract_vectors(cameraPosition, intersectPoint), 4.0f);
    colorRGB = final_shader_addition({0, 0, 0}, ambientLight, diffuse_shader({255, 255, 255}, object.vertex_normals[object.faces[smallestIndex][0][2]], lightVector, lightColor), specular_shader(lightColor, lightVector, object.vertex_normals[object.faces[smallestIndex][0][2]], subtract_vectors(cameraPosition, intersectPoint), 4.0f), light_attenuation(get_magnitude(lightVector), lightIntensity));
    //colorRGB = final_shader_addition({0, 0, 0}, {0, 0, 0}, diffuse_shader({100, 100, 100}, {1, 0, 0}, {1, 0, 0}, {100, 100, 100}), specular_shader(), light_attenuation(4, 1));

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