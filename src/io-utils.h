
#ifndef IO_UTILS_H
#define IO_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

#include <SDL3/SDL.h>

#include <iostream>
#include <iterator>
#include <cstdint>
#include <array>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>



const int CAPACITY = 100;

/*
    ObjectName.vertices (x, y, z, [w]), w is optional and defaults to 1.0.

    ObjectName.vertex_normals (x,y,z) normals might not be unit vectors.

    ObjectName.faces (f 3/1 4/2 5/3) Polygonal face element

    ObjectName.texture_coordinates (u, [v, w]) these will vary between 0 and 1. v, w are optional and default to 0.

    ObjectName.space_vertices (u, [v, w]) free form geometry statement
*/
struct ObjectData {
    /*
        List of geometric vertices, with (x, y, z, [w]) coordinates, w is optional and defaults to 1.0.
        v 0.123 0.234 0.345 1.0
    */
    std::vector<std::array<float, 3>> vertices;//Wikepedia says there's a fourth element but I don't see one. - Elijah
    int verticesSize = 0; 

    /*
        List of vertex normals in (x,y,z) form; normals might not be unit vectors.
        vn 0.707 0.000 0.707
    */
    std::vector<std::array<float, 3>> vertex_normals;
    int vertexNormalsSize = 0;

    /*
        Polygonal face element
        f 1 2 3
        f 3/1 4/2 5/3

        f v1/vt1/vn1 
        obj.faces[changes vertically][changes chunks horizontally][changes between slashes]
    */
    std::vector<std::array<std::array<int, 3>, 3>> faces;
    int facesSize = 0;

    /*
        List of texture coordinates, in (u, [v, w]) coordinates, these will vary between 0 and 1. v, w are optional and default to 0.
        vt 0.500 1 [0]
    */
    std::vector<std::array<float, 3>> texture_coordinates;
    int textureCoordinateSize = 0;

    /*
    Parameter space vertices in (u, [v, w]) form; free form geometry statement (see below)
    vp 0.310000 3.210000 2.100000
    */
    std::vector<std::array<float, 3>> space_vertices;
    int spaceVerticesSize = 0;

    int xPosition = 0;
    int yPosition = 0;
};


/*
loads object data into an ObjectData struct
ex: ObjectData cube = load_object("cube.obj");
*/
ObjectData load_object(const char* path, int xPos, int yPos) {

    clock_t start = clock();
    
    ObjectData object;
    object.vertices.reserve(CAPACITY);
    object.vertex_normals.reserve(CAPACITY);
    object.faces.reserve(CAPACITY);
    object.texture_coordinates.reserve(CAPACITY);
    object.space_vertices.reserve(CAPACITY);

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("cannot open file\n");
        return object;
    }

    //printf("opened %s\n", path);
    
    char lineHeader[128];
    while (fscanf(file, "%s", lineHeader) != EOF) {
        if (strcmp(lineHeader, "v") == 0) {

            object.vertices.resize(object.verticesSize + 1);
            fscanf(file, "%f %f %f\n", &object.vertices[object.verticesSize][0], &object.vertices[object.verticesSize][1], &object.vertices[object.verticesSize][2]);
            //printf("v %f %f %f\n", object.vertices[object.verticesSize][0], object.vertices[object.verticesSize][0], object.vertices[object.verticesSize][0]);
            object.verticesSize++;

        } else if (strcmp(lineHeader, "vn") == 0) {

            object.vertex_normals.resize(object.vertexNormalsSize + 1);
            fscanf(file, "%f %f %f\n", &object.vertex_normals[object.vertexNormalsSize][0], &object.vertex_normals[object.vertexNormalsSize][1], &object.vertex_normals[object.vertexNormalsSize][2]);
            //printf("vn %f %f %f\n", object.vertex_normals[object.vertexNormalsSize][0], object.vertex_normals[object.vertexNormalsSize][0], object.vertex_normals[object.vertexNormalsSize][0]);
            object.vertexNormalsSize++;

        }else if (strcmp(lineHeader, "vt") == 0) {

            object.texture_coordinates.resize(object.textureCoordinateSize + 1);
            fscanf(file, "%f %f %f\n", &object.texture_coordinates[object.textureCoordinateSize][0], &object.texture_coordinates[object.textureCoordinateSize][1], &object.texture_coordinates[object.textureCoordinateSize][2]);
            //printf("vt %f %f %f\n", object.texture_coordinates[object.textureCoordinateSize][0], object.texture_coordinates[object.textureCoordinateSize][0], object.texture_coordinates[object.textureCoordinateSize][0]);
            object.textureCoordinateSize++;

        }else if (strcmp(lineHeader, "f") == 0) {

            object.faces.resize(object.facesSize + 1);
            fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &object.faces[object.facesSize][0][0], &object.faces[object.facesSize][0][1], &object.faces[object.facesSize][0][2], &object.faces[object.facesSize][1][0], &object.faces[object.facesSize][1][1], &object.faces[object.facesSize][1][2], &object.faces[object.facesSize][2][0], &object.faces[object.facesSize][2][1], &object.faces[object.facesSize][2][2]);
            object.faces[object.facesSize][0][0] -= 1;
            object.faces[object.facesSize][0][1] -= 1;
            object.faces[object.facesSize][0][2] -= 1;
            object.faces[object.facesSize][1][0] -= 1;
            object.faces[object.facesSize][1][1] -= 1;
            object.faces[object.facesSize][1][2] -= 1;
            object.faces[object.facesSize][2][0] -= 1;
            object.faces[object.facesSize][2][1] -= 1;
            object.faces[object.facesSize][2][2] -= 1;
            object.facesSize++;

        }else if (strcmp(lineHeader, "vp") == 0) {

            object.space_vertices.resize(object.spaceVerticesSize + 1);
            fscanf(file, "%f %f %f\n", &object.space_vertices[object.spaceVerticesSize][0], &object.space_vertices[object.spaceVerticesSize][1], &object.space_vertices[object.spaceVerticesSize][2]);
            //printf("vp %f %f %f\n", object.space_vertices[object.spaceVerticesSize][0], object.space_vertices[object.spaceVerticesSize][0], object.space_vertices[object.spaceVerticesSize][0]);
            object.spaceVerticesSize++;

        } else {
            char skipBuffer[1000];
            fgets(skipBuffer, 1000, file); // Skip the rest of the line
            
        }
    }

    object.xPosition = xPos;
    object.yPosition = yPos;

    clock_t end = clock();
    //printf("saved and closed %s\n", path);
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Spent %f seconds loading %s\n", elapsed, path);
    fclose(file);
    return object;
}



std::tuple<int, std::vector<std::string>> io(void) {

    std::string input_line;
    std::string temp_word;
    std::vector<std::string> words_array;

    std::cout << "user> ";

    std::getline(std::cin, input_line);
    std::istringstream ss(input_line);

    while (ss >> temp_word) {
        words_array.push_back(temp_word); // Add each word to the vector
    }
    int length = std::size(words_array);

    return std::make_tuple(length, words_array);
    
}

/*
    
   int argLen = 0;
   std::vector<std::string> arge;

   std::tie(argLen, arge) = io();
   std::cout << "\nYou entered " << arge.size() << " words:\n";

   for (const std::string& element : arge) {
      std::cout << element << std::endl;
   }
*/

#endif
