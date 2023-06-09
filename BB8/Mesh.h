#pragma once

#include <vector>

#include "Color.h"
#include "Vector3.h"

class Mesh {
public:
    class Face {
    public:
        size_t A, B, C;
        Vector3 normal;
        Vector3 position;

        Color color;

        Face();
        Face(size_t a, size_t b, size_t c);
    };

    std::vector<Vector3> Vertices;
    std::vector<Mesh::Face> Faces;
    std::vector<std::pair<size_t, size_t>> Edges;

    Mesh();

    void addFace(size_t a, size_t b, size_t c, Color color);
    void addFace(size_t a, size_t b, size_t c, Color color, Vector3 normal);
};
