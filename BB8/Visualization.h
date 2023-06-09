#pragma once

#include <array>

#include "framework.h"
#include "RenderDevice.h"
#include "Camera.h"
#include "Lighting.h"

class Visualization {
public:
    Visualization();

    void OnKeyDown(WPARAM wParam, LPARAM lParam);
    void Update(double elapsed_time, Vector3 sphere_location, Quaternion sphere_rotation);
    void Render(RenderDevice& render_device);

private:
    const double camera_pan_speed = 0.045;
    const double camera_zoom_speed = 0.96;

    Camera camera;
    Lighting lighting;

    Vector3 sphere_location;
    Quaternion sphere_rotation;

    bool wireframe_mode;

    Mesh sphere;
    Mesh ground;

    void reset_view();
};
