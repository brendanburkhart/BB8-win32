#include "Mesh.h"

#include <stack>

Mesh::Face::Face() : A(0), B(0), C(0), color(0.0, 0.0, 0.0, 1.0) { }

Mesh::Face::Face(size_t a, size_t b, size_t c) : A(a), B(b), C(c), color(0.0, 0.0, 0.0, 1.0) { }

Mesh::Mesh()
    : Vertices(std::vector<Vector3>()),
    Faces(std::vector<Mesh::Face>()),
    Edges(std::vector<std::pair<size_t, size_t>>()) {}

void Mesh::addFace(size_t a, size_t b, size_t c, Color color) {
    Vector3 ab = Vector3::Subtract(Vertices[b], Vertices[a]);
    Vector3 ac = Vector3::Subtract(Vertices[c], Vertices[a]);
    Vector3 normal = Vector3::Normalize(Vector3::Cross(ab, ac));

    addFace(a, b, c, color, normal);
}

void Mesh::addFace(size_t a, size_t b, size_t c, Color color, Vector3 normal) {
    Face face = Face(a, b, c);

    Vector3 sum = Vector3::Add(Vertices[a], Vector3::Add(Vertices[b], Vertices[c]));
    face.position = Vector3::Divide(sum, 3.0);
    face.normal = normal;
    face.color = color;

    Faces.push_back(face);
}
