#include "Visualization.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <tuple>

#include "Meshes.h"

constexpr double PI = 3.14159265358979323846;

Visualization::Visualization()
  : sphere_rotation(Quaternion::Identity()),
    platform_rotation(Quaternion::Identity()),
    pendulum_rotation(Quaternion::Identity()),
    sphere_location(0, 0, 1.0),
    theta(0.0),
    heading(0.0),
    relative_camera_orientation(true),
    wireframe_mode(false)
{
    Lighting::Light primary;
    primary.diffuse = Color(0.8, 0.8, 0.8, 1.0);
    primary.ambient = Color(0.3, 0.3, 0.3, 1.0);
    primary.position = Vector3(25, 35, 15);
    lighting.AddLight(primary);

    sphere = Robot(1.0, 12);
    platform = Platform(0.9, 0.1);
    pendulum = Pendulum(0.1, 0.7);
    ground = Plane(2.0, 10);
    
    reset_view();
}

void Visualization::reset_view() {
    theta = 0.0;
    camera.MoveTo(10.0, 0.0, PI/4.0);
}

void Visualization::OnKeyDown(WPARAM wParam, LPARAM lParam) {
    switch (wParam) {
    case 'M':
        wireframe_mode = !wireframe_mode;
        break;
    case 'C':
        if (relative_camera_orientation) {
            theta = theta + heading;
            relative_camera_orientation = false;
        } else {
            theta = theta - heading;
            relative_camera_orientation = true;
        }
        break;
    case 'R':
        reset_view();
        break;
    }
}

void Visualization::Update(double elapsed_seconds, Vector3 sphere_location, Quaternion sphere_rotation,
    Quaternion platform_rotation, Quaternion pendulum_rotation, double sphere_heading) {
    auto [distance, ctheta, phi] = camera.State();

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

    if (relative_camera_orientation) {
        camera.MoveTo(distance, theta + sphere_heading, phi);
    } else {
        camera.MoveTo(distance, theta, phi);
    }

    heading = sphere_heading;
    camera.PointAt(sphere_location);

    this->sphere_location = sphere_location;
    this->sphere_rotation = sphere_rotation;
    this->platform_rotation = platform_rotation;
    this->pendulum_rotation = pendulum_rotation;
}

void Visualization::Render(RenderDevice& renderDevice) {
    renderDevice.Clear(Color(1.0, 1.0, 1.0, 1.0));

    renderDevice.RenderSurface(camera, lighting, ground, Quaternion::Identity(), Vector3());

    if (wireframe_mode) {
        renderDevice.RenderSurface(camera, lighting, platform, platform_rotation, sphere_location);
        renderDevice.RenderSurface(camera, lighting, pendulum, pendulum_rotation, sphere_location);
        renderDevice.RenderWireframe(camera, sphere, sphere_rotation, sphere_location, Color(0.0, 0.0, 0.0, 0.4), 5);
    } else {
        renderDevice.RenderSurface(camera, lighting, sphere, sphere_rotation, sphere_location);
    }
}
