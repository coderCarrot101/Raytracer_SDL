#ifndef SHADERS_H
#define SHADERS_H

#include <SDL3/SDL.h>
#include <iostream>
#include <array>
#include <cmath>
#include <algorithm>
#include "vector-utils.h"

std::array<float, 3> diffuse_shader(std::array<float, 3> albedo, std::array<float, 3> normal, std::array<float, 3> lightVector, std::array<float, 3> lightColor){
    return multiply_vector_by_scalar(multiply_vectors(lightColor, albedo), std::clamp(dot_product(normal, lightVector), (float)0, (float)1));
}
std::array<float, 3> specular_shader(){

}

//Emission + Ambient + sum of all lights(Shadow * Attenuation * (Diffuse + Specular))
std::array<float, 3> final_shader_addition(std::array<float, 3> ambient, std::array<float, 3> diffuse, std::array<float, 3> specular, float lightAttenuation){
    return sum_multiple_vectors({ambient, multiply_multiple_vectors({lightAttenuation, add_vectors(diffuse, specular)})});
}

#endif