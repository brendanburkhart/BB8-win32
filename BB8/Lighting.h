#pragma once

#include <vector>

#include "Camera.h"
#include "Color.h"
#include "Vector3.h"

class Lighting
{
public:
    class Light {
    public:
        Vector3 position;
        Color diffuse;
        Color ambient;
    };

    void AddLight(Light light);

    Color Model(Vector3 position, Vector3 normal, Color material) const;

private:
    std::vector<Light> lights;
};

