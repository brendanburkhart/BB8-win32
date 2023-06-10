#include "Meshes.h"

#include <cmath>

constexpr double PI = 3.1415926535;

Mesh Robot(double radius, int density) {
    Mesh sphere;

    auto x = [](double rho, double theta, double phi) {
        double r = rho * std::cos(phi);
        return r * std::cos(theta);
    };

    auto y = [](double rho, double theta, double phi) {
        double r = rho * std::cos(phi);
        return r * std::sin(theta);
    };

    auto z = [](double rho, double theta, double phi) {
        return rho * std::sin(phi);
    };

    for (size_t i = 0; i <= 2 * density; i++) {
        double alpha = 0.5 * PI * (double(i) - density) / density;
        for (size_t j = 0; j < density; j++) {
            double theta = 2 * PI * j / density;
            auto point = Vector3(x(radius, theta, alpha), y(radius, theta, alpha), z(radius, theta, alpha));
            sphere.Vertices.push_back(point);
        }
    }

    auto primary_color = Color(0.3, 0.3, 0.3, 1.0);
    auto polar_color = Color(1.0, 0.7, 0.3, 1.0);
    auto stripe_color = Color(0.3, 0.8, 1.0, 1.0);

    for (size_t i = 0; i < 2 * density; i++) {
        for (size_t j = 0; j < density; j++) {
            size_t a = i * density + j;
            size_t b = i * density + ((j + 1) % density);
            size_t c = (i + 1) * density + j;
            size_t d = (i + 1) * density + ((j + 1) % density);

            sphere.Edges.push_back(std::pair<size_t, size_t>(a, b));
            sphere.Edges.push_back(std::pair<size_t, size_t>(a, c));
            sphere.Edges.push_back(std::pair<size_t, size_t>(a, d));

            auto color = (i <= 3 || i >= 2 * density - 4) ? polar_color : primary_color;
            
            sphere.addFace(a, b, d, (i == density) ? stripe_color : color);
            Vector3 normal = sphere.Faces[sphere.Faces.size() - 1].position;
            sphere.Faces[sphere.Faces.size() - 1].normal = normal;

            sphere.addFace(a, d, c, (i == density - 1) ? stripe_color : color);
            normal = sphere.Faces[sphere.Faces.size() - 1].position;
            sphere.Faces[sphere.Faces.size() - 1].normal = normal;
        }
    }

    return sphere;
}

Mesh Platform(double width, double thickness) {
    Mesh platform;
    Color color(0.8, 0.6, 0.2, 1.0);

    platform.Vertices.push_back(Vector3(-0.5 * width, 0.5 * thickness, -0.5 * width));
    platform.Vertices.push_back(Vector3(-0.5 * width, -0.5 * thickness, -0.5 * width));

    platform.Vertices.push_back(Vector3(-0.5 * width, 0.5 * thickness, 0.5 * width));
    platform.Vertices.push_back(Vector3(-0.5 * width, -0.5 * thickness, 0.5 * width));

    platform.Vertices.push_back(Vector3(0.5 * width, 0.5 * thickness, 0.5 * width));
    platform.Vertices.push_back(Vector3(0.5 * width, -0.5 * thickness, 0.5 * width));

    platform.Vertices.push_back(Vector3(0.5 * width, 0.5 * thickness, -0.5 * width));
    platform.Vertices.push_back(Vector3(0.5 * width, -0.5 * thickness, -0.5 * width));

    platform.addFace(0, 2, 4, color);
    platform.addFace(4, 6, 0, color);

    platform.addFace(1, 3, 5, color);
    platform.addFace(5, 7, 1, color);

    platform.addFace(0, 2, 3, color);
    platform.addFace(3, 1, 0, color);

    platform.addFace(2, 4, 5, color);
    platform.addFace(5, 3, 2, color);

    platform.addFace(4, 6, 7, color);
    platform.addFace(7, 5, 4, color);

    platform.addFace(6, 0, 1, color);
    platform.addFace(1, 7, 6, color);

    return platform;
}

Mesh Pendulum(double thickness, double length) {
    Mesh pendulum;
    Color color(0.8, 0.6, 0.2, 1.0);

    pendulum.Vertices.push_back(Vector3(-0.5 * thickness, 0.0 * length, -0.5 * thickness));
    pendulum.Vertices.push_back(Vector3(-0.5 * thickness, -1.0 * length, -0.5 * thickness));

    pendulum.Vertices.push_back(Vector3(-0.5 * thickness, 0.0 * length, 0.5 * thickness));
    pendulum.Vertices.push_back(Vector3(-0.5 * thickness, -1.0 * length, 0.5 * thickness));

    pendulum.Vertices.push_back(Vector3(0.5 * thickness, 0.0 * length, 0.5 * thickness));
    pendulum.Vertices.push_back(Vector3(0.5 * thickness, -1.0 * length, 0.5 * thickness));

    pendulum.Vertices.push_back(Vector3(0.5 * thickness, 0.0 * length, -0.5 * thickness));
    pendulum.Vertices.push_back(Vector3(0.5 * thickness, -1.0 * length, -0.5 * thickness));

    pendulum.addFace(0, 2, 4, color);
    pendulum.addFace(4, 6, 0, color);

    pendulum.addFace(1, 3, 5, color);
    pendulum.addFace(5, 7, 1, color);

    pendulum.addFace(0, 2, 3, color);
    pendulum.addFace(3, 1, 0, color);

    pendulum.addFace(2, 4, 5, color);
    pendulum.addFace(5, 3, 2, color);

    pendulum.addFace(4, 6, 7, color);
    pendulum.addFace(7, 5, 4, color);

    pendulum.addFace(6, 0, 1, color);
    pendulum.addFace(1, 7, 6, color);

    return pendulum;
}

Mesh Plane(double square_size, int extent) {
    Mesh plane;

    for (int x = -extent; x <= extent; x++) {
        for (int y = -extent; y <= extent; y++) {
            plane.Vertices.push_back(Vector3(x * square_size, y * square_size, 0.0));
        }
    }

    Color gray(0.6, 0.6, 0.6, 1.0);
    Color red(0.0, 0.0, 1.0, 1.0);

    for (int i = 0; i < 2 * extent; i++) {
        for (int j = 0; j < 2*extent; j++) {
            size_t a = i * (2 * extent + 1) + j;
            size_t b = (i + 1) * (2 * extent + 1) + j;
            size_t c = i * (2 * extent + 1) + (j + 1);
            size_t d = (i + 1) * (2 * extent + 1) + (j + 1);

            auto color = (i + j) % 2 == 0 ? gray : red;
            plane.addFace(a, b, c, color);
            plane.addFace(c, b, d, color);
        }
    }

    return plane;
}
