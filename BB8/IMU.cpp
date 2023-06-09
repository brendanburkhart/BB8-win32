#include "IMU.h"

#include <cmath>

constexpr double PI = 3.1415926535;

IMU::IMU(Simulation* simulation) : simulation(simulation) {}

double IMU::heading() const {
    auto reference_axis = Vector3(1.0, 0.0, 0.0);
    auto rotated_axis = simulation->get_rotation().Rotate(reference_axis);
    double theta = std::atan2(rotated_axis.Y, rotated_axis.X);

    if (theta < 0.0) {
        theta += 2 * PI;
    }

    return theta;
}

double IMU::tilt() const {
    return 0.0;
}

double IMU::tilt_velocity() const {
    return 0.0;
}

double IMU::angular_velocity() const {
    return 0.0;
}
