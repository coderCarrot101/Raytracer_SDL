#version 460

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec4 a_color;

layout(location = 0) out vec4 v_color;
layout(location = 1) out vec2 v_pos;

void main() {
    gl_Position = vec4(a_position, 1.0);
    v_color = a_color;
    v_pos = a_position.xy; // pass NDC position
}

//https://vulkan.lunarg.com/sdk/home#windows for compiler