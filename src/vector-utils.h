#ifndef VECTOR_UTILS_H
#define VECTOR_UTILS_H

#include <SDL3/SDL.h>
#include <iostream>
#include <array>
#include <vector>
#include <cmath>
#include <algorithm>

std::array<float, 3> add_vectors(std::array<float, 3> vector1, std::array<float, 3> vector2){
   return std::array<float, 3>{vector1[0] + vector2[0], vector1[1] + vector2[1], vector1[2] + vector2[2]};
}

std::array<float, 3> subtract_vectors(std::array<float, 3> vector1, std::array<float, 3> vector2){
   return std::array<float, 3>{vector1[0] - vector2[0], vector1[1] - vector2[1], vector1[2] - vector2[2]};
}

std::array<float, 3> multiply_vectors(std::array<float, 3> vector1, std::array<float, 3> vector2){
   return std::array<float, 3>{vector1[0]*vector2[0], vector1[1]*vector2[1], vector1[2]*vector2[2]};
}

std::array<float, 3> divide_vectors(std::array<float, 3> vector1, std::array<float, 3> vector2){
   return std::array<float, 3>{vector1[0]/vector2[0], vector1[1]/vector2[1], vector1[2]/vector2[2]};
}

std::array<float, 3> multiply_vector_by_scalar(std::array<float, 3> vector, float scalar){
   return std::array<float, 3>{vector[0]*scalar, vector[1]*scalar, vector[2]*scalar};
}

std::array<float, 3> divide_vector_by_scalar(std::array<float, 3> vector, float scalar){
   return std::array<float, 3>{vector[0]/scalar, vector[1]/scalar, vector[2]/scalar};
}


std::array<float, 3> sum_multiple_vectors(std::vector<std::array<float, 3>> vectors){
    std::array<float, 3> sum = {0, 0, 0};
    for (int i = 0; i < (vectors.size() - 1); i++){
        sum = add_vectors(vectors[i], vectors[i+1]);
    }
    return sum;
}

std::array<float, 3> multiply_multiple_vectors(std::vector<std::array<float, 3>> vectors){
    std::array<float, 3> product = {0, 0, 0};
    for (int i = 0; i < (vectors.size() - 1); i++){
        product = multiply_vectors(vectors[i], vectors[i+1]);
    }
    return product;
}


float get_magnitude(std::array<float, 3> vector){
   return sqrtf(powf(vector[0], 2) + powf(vector[1], 2) + powf(vector[2], 2));
}

std::array<float, 3> normalize_vector(std::array<float, 3> vector){
   float magnitude = get_magnitude(vector);
   return std::array<float, 3>{vector[0]/magnitude, vector[1]/magnitude, vector[2]/magnitude};
}


std::array<float, 3> cross_product(std::array<float, 3> vector1, std::array<float, 3> vector2){
   return {(vector1[1]*vector2[2])-(vector1[2]*vector2[1]), (vector1[2]*vector2[0])-(vector1[0]*vector2[2]), (vector1[0]*vector2[1])-(vector1[1]*vector2[0])};
}

float dot_product(std::array<float, 3> vector1, std::array<float, 3> vector2){
   float output = 0;
   for (int i = 0; i < 3; i++ ){
      output += vector1[i] * vector2[i];
   }
   return output;
}

#endif