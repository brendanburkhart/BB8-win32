#include "Visualization.h"

#include <cmath>
#include <iostream>
#include <tuple>

#include "Meshes.h"

constexpr double PI = 3.14159265358979323846;


Visualization::Visualization()
    : sphere_rotation(Quaternion::Identity()),
    sphere_location(0, 0, 1.0),
    wireframe_mode(false)
{
    Lighting::Light primary;
    primary.diffuse = Color(0.8, 0.8, 0.8, 1.0);
    primary.ambient = Color(0.3, 0.3, 0.3, 1.0);
    primary.position = Vector3(25, 35, 15);
    lighting.AddLight(primary);

    sphere = Robot(1.0, 16);
    ground = Plane(2.0, 10);
    
    reset_view();
}

void Visualization::reset_view() {
    camera.MoveTo(10.0, 0.0, PI/4.0);
}

void Visualization::OnKeyDown(WPARAM wParam, LPARAM lParam) {
    switch (wParam) {
    case 'M':
        wireframe_mode = !wireframe_mode;
        break;
    case 'R':
        reset_view();
        break;
    }
}

void Visualization::Update(double elapsed_seconds, Vector3 sphere_location, Quaternion sphere_rotation) {
    auto [distance, theta, phi] = camera.State();

    SHORT high_bit_mask = 0x8000;

    if (GetKeyState(VK_LEFT) & high_bit_mask) {
        theta += camera_pan_speed;
    }

    if (GetKeyState(VK_RIGHT) & high_bit_mask) {
        theta -= camera_pan_speed;
    }

    if (GetKeyState(VK_UP) & high_bit_mask) {
        phi += camera_pan_speed;
    }

    if (GetKeyState(VK_DOWN) & high_bit_mask) {
        phi -= camera_pan_speed;
    }

    phi = std::max(-0.5 * PI, std::min(0.5 * PI, phi));

    if (GetKeyState('Z') & high_bit_mask) {
        if (GetKeyState(VK_SHIFT) & high_bit_mask) {
            distance = distance / camera_zoom_speed;
        } else {
            distance = distance * camera_zoom_speed;
        }

        distance = std::min(std::max(2.0, distance), 100.0);
    }

    camera.MoveTo(distance, theta, phi);
    camera.PointAt(sphere_location);

    this->sphere_location = sphere_location;
    this->sphere_rotation = sphere_rotation;
}

void Visualization::Render(RenderDevice& renderDevice) {
    renderDevice.Clear(Color(1.0, 1.0, 1.0, 1.0));

    renderDevice.RenderSurface(camera, lighting, ground, Quaternion::Identity(), Vector3());

    if (wireframe_mode) {
        renderDevice.RenderWireframe(camera, sphere, sphere_rotation, sphere_location, Color(0.0, 0.0, 0.0, 0.4), 5);
    } else {
        renderDevice.RenderSurface(camera, lighting, sphere, sphere_rotation, sphere_location);
    }
}
