#ifndef PARSER_H
#define PARSER_H

#include <SDL3/SDL.h>
#include <iostream>
#include <cstdint>
#include <array>
#include <vector>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct ObjectData {
    std::vector<std::array<float, 4>> vertices;
    std::vector<std::array<float, 3>> vertex_normals;
    //std::vector<std::array<std::array<float, 3>, 3>> faces;
    std::vector<std::array<float, 3>> texture_coordinates;

    std::vector<std::array<float, 3>> space_vertices;
    //std::vector<int, CAPACITY> Line_element;
};



/*need vertices, vertex normals, and faces  -- texture coordinets.*/

ObjectData load_object(const char* path) {
    const int CAPACITY = 100;
    int size = 0;
    ObjectData object;
    object.vertex_normals.reserve(CAPACITY);

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("cannot open file\n");
        return object;
    }

    printf("opened %s\n", path);
    
    char lineHeader[128];
    while (fscanf(file, "%s", lineHeader) != EOF) {
        if (strcmp(lineHeader, "v") == 0) {
            object.vertices.resize(size++);
            fscanf(file, "%f %f %f\n", &object.vertices[size][0], &object.vertices[size][1], &object.vertices[size][2]);
            printf("v %f %f %f\n", object.vertices[size][0], object.vertices[size][0], object.vertices[size][0]);

        }/*if (strcmp(lineHeader, "vn") == 0) {
            //BAD BAD BAD WILL CRASH       V
            object.vertex_normals.resize(size++);
            fscanf(file, "%f %f %f\n", &object.vertex_normals[size][0], &object.vertex_normals[size][1], &object.vertex_normals[size][2]);
            printf("v %f %f %f\n", object.vertex_normals[size][0], object.vertex_normals[size][0], object.vertex_normals[size][0]);

        }*/ else {
            // Ignore other line types (vn, vt, #, etc.)
            char skipBuffer[1000];
            fgets(skipBuffer, 1000, file); // Skip the rest of the line
            
        }
    }
    fclose(file);
    return object;
}

#endif
