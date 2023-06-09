#pragma once

#include <functional>

class TorqueInterface
{
public:
    TorqueInterface(std::function<double(double)> acceleration_fn, std::function<double(double)> inertia_fn);

    // resultant angular acceleration, given torque
    double acceleration(double torque) const;

    // derivative of angular acceleration with respect to torque
    double inertia(double torque) const;

private:
    const std::function<double(double)> acceleration_fn;
    const std::function<double(double)> inertia_fn;
};
