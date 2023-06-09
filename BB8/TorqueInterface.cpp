#include "TorqueInterface.h"

TorqueInterface::TorqueInterface(std::function<double(double)> acceleration_fn, std::function<double(double)> inertia_fn)
    : acceleration_fn(acceleration_fn), inertia_fn(inertia_fn) { }

double TorqueInterface::acceleration(double torque) const {
    return acceleration_fn(torque);
}

double TorqueInterface::inertia(double torque) const {
    return inertia_fn(torque);
}
