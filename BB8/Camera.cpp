#include "Camera.h"

#include <cmath>

constexpr double PI = 3.1415926535;

Camera::Camera() : distance(10.0), theta(0.0), phi(PI/4.0) {}

std::tuple<double, double, double> Camera::State() const {
    return { distance, theta, phi };
}

void Camera::MoveTo(double distance, double theta, double phi) {
    this->distance = distance;
    this->theta = theta;
    this->phi = phi;
}

void Camera::PointAt(Vector3 target) {
    this->target = target;
}

Matrix Camera::ViewTransform(double aspect_ratio) const {
    Matrix view_matrix = LookAtLH();
    Matrix projection_matrix = PerspectiveFovLH(aspect_ratio);

    return projection_matrix * view_matrix;
}

Matrix Camera::LookAtLH() const {
    Vector3 x_axis, y_axis, z_axis;

    double rho = distance * std::cos(phi);
    Vector3 relative_position(rho * std::cos(theta), rho * std::sin(theta), distance * std::sin(phi));
    Vector3 absolute_position = Vector3::Add(target, relative_position);

    Vector3 up = Vector3(0.0, 0.0, 1.0);

    if (std::cos(phi) < 1e-5) {
        up = Vector3(-std::cos(theta), -std::sin(theta), 0.0);
        if (phi < 0.0) {
            up = Vector3::Negate(up);
        }
    }

    // forward
    z_axis = Vector3::Normalize(Vector3::Negate(relative_position));
    // right
    x_axis = Vector3::Normalize(Vector3::Cross(up, z_axis));
    // up
    y_axis = Vector3::Cross(z_axis, x_axis);

    // translation represented in new coordinate system
    Vector3 translation = Vector3(
        Vector3::Dot(x_axis, absolute_position),
        Vector3::Dot(y_axis, absolute_position),
        Vector3::Dot(z_axis, absolute_position)
    );

    Matrix transform = Matrix(
        x_axis.X, x_axis.Y, x_axis.Z, -translation.X,
        y_axis.X, y_axis.Y, y_axis.Z, -translation.Y,
        z_axis.X, z_axis.Y, z_axis.Z, -translation.Z,
        0.0, 0.0, 0.0, 1.0);

    return transform;
}

Matrix Camera::PerspectiveFovLH(double aspect_ratio) const {
    double y_scale = 1.0 / std::tan(fov * 0.5);
    double x_scale = y_scale / aspect_ratio;
    double q = z_far / (z_far - z_near);

    Matrix transform = Matrix(
        x_scale, 0.0, 0.0, 0.0,
        0.0, y_scale, 0.0, 0.0,
        0.0, 0.0, q, -2*q*z_near,
        0.0, 0.0, 1.0, 0.0);

    return transform;
}
