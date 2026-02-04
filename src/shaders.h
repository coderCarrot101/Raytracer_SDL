#ifndef SHADERS_H
#define SHADERS_H

#include <SDL3/SDL.h>
#include <iostream>
#include <array>
#include <cmath>
#include <algorithm>
#include "vector-utils.h"

std::array<float, 3> diffuse_shader(std::array<float, 3> albedo, std::array<float, 3> normal, std::array<float, 3> lightVector, std::array<float, 3> lightColor){
    return multiply_vector_by_scalar(multiply_vector_by_scalar(multiply_vectors(divide_vector_by_scalar(lightColor, 255), divide_vector_by_scalar(albedo, 255)), std::clamp(dot_product(normalize_vector(normal), normalize_vector(lightVector)), (float)0, (float)1)), 255);
}
//(reflection vector DOT view vector)^shininess   *    light color
std::array<float, 3> specular_shader(std::array<float, 3> lightColor, std::array<float, 3> lightVector, std::array<float, 3> normal, std::array<float, 3> viewVector, float shininess){
    std::array<float, 3> R = normalize_vector(reflect_vector(subtract_vectors({0, 0, 0}, normalize_vector(lightVector)), normalize_vector(normal)));
    if (dot_product(normalize_vector(normal), normalize_vector(lightVector)) <= 0){
        return {0,0,0};
    }
    return multiply_vector_by_scalar(lightColor, powf(std::max(0.0f, dot_product(R, normalize_vector(viewVector))), shininess));
}

float light_attenuation(float lightDistance, float lightIntensity){
    if(fabs(lightDistance) < 0.001){
        return 999;
    }
    return lightIntensity/powf(lightDistance, 2);
}

//Emission + Ambient + sum of all lights(Shadow * Attenuation * (Diffuse + Specular))
std::array<float, 3> final_shader_addition(std::array<float, 3> emission, std::array<float, 3> ambient, std::array<float, 3> diffuse, std::array<float, 3> specular, float lightAttenuation){
    return clamp_vector(sum_multiple_vectors({emission, ambient, multiply_vector_by_scalar(add_vectors(diffuse, specular), lightAttenuation)}), {0, 0, 0}, {255, 255, 255});
}

#endif 