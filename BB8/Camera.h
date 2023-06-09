#pragma once

#include <tuple>

#include "Color.h"
#include "Matrix.h"
#include "Vector3.h"

class Camera {
private:
    double distance;
    double theta;
    double phi;

    double fov = 0.78;
    double z_near = 0.01;
    double z_far = 1.0;

    Vector3 target;

public:
    Camera();

    std::tuple<double, double, double> State() const;
    void MoveTo(double distance, double theta, double phi);
    void PointAt(Vector3 target);

    Matrix ViewTransform(double aspect_ratio) const;

private:
    Matrix LookAtLH() const;
    Matrix PerspectiveFovLH(double aspect_ratio) const;
};
