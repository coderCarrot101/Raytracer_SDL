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
    std::vector<std::array<float, 3>> vertex_normals;
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
            object.vertex_normals.resize(size++);
            fscanf(file, "%f %f %f\n", &object.vertex_normals[size][0], &object.vertex_normals[size][1], &object.vertex_normals[size][2]);
            printf("v %f %f %f\n", object.vertex_normals[size][0], object.vertex_normals[size][0], object.vertex_normals[size][0]);

        } else {
            // Ignore other line types (vn, vt, #, etc.)
            char skipBuffer[1000];
            fgets(skipBuffer, 1000, file); // Skip the rest of the line
            
        }
    }
    fclose(file);
    return object;
}

#endif
