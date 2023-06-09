#include "Lighting.h"

#include <cmath>

void Lighting::AddLight(Light light) {
    lights.push_back(light);
}

Color Lighting::Model(Vector3 position, Vector3 normal, Color material) const {
    Color result(0.0, 0.0, 0.0, 1.0);

    for (auto& light : lights) {
        Vector3 direction = Vector3::Subtract(light.position, position);

        double dot = Vector3::Dot(Vector3::Normalize(normal), Vector3::Normalize(direction));
        double diffuse = std::min(1.0, std::max(0.0, dot));
        result.Red += material.Red * (diffuse * light.diffuse.Red + light.ambient.Red);
        result.Green += material.Green * (diffuse * light.diffuse.Green + light.ambient.Green);
        result.Blue += material.Blue * (diffuse * light.diffuse.Blue + light.ambient.Blue);
    }

    result.Red = std::min(result.Red, 1.0);
    result.Green = std::min(result.Green, 1.0);
    result.Blue = std::min(result.Blue, 1.0);

    return result;
}
