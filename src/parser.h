#ifndef PARSER_H
#define PARSER_H

#include <SDL3/SDL.h>
#include <iostream>
#include <cstdint>
#include <array>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

std::array<float, 3> vertex_normals;

/*need vertices, vertex normals, and faces  -- texture coordinets.*/

void load_object(const char* path) {
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("cannot open file\n");
        return;
    }

    char lineHeader[128];
    while (fscanf(file, "%s", lineHeader) != EOF) {
        if (strcmp(lineHeader, "v") == 0) {
            fscanf(file, "%f %f %f\n", vertex_normals[0], vertex_normals[1], vertex_normals[2]);
            printf("v %f %f %f\n", vertex_normals[0], vertex_normals[1], vertex_normals[2]);

        } else {
            // Ignore other line types (vn, vt, #, etc.)
            char skipBuffer[1000];
            fgets(skipBuffer, 1000, file); // Skip the rest of the line
        }
    }
    fclose(file);
}

#endif
