#include "Gearbox.h"

Gearbox::Gearbox(double ratio, double damping)
    : ratio(ratio), damping(damping), velocity(0.0) {}

double Gearbox::outputTorque(double input_torque) const {
    return input_torque * ratio - damping * velocity;
}

double Gearbox::outputSpeed(double input_speed) const {
    return input_speed / ratio;
}

double Gearbox::inputTorque(double output_torque) const {
    return (output_torque + damping * velocity) / ratio;
}

double Gearbox::inputSpeed(double output_speed) const {
    return output_speed * ratio;
}

double Gearbox::reductionRatio() const {
    return ratio;
}

void Gearbox::update(double input_velocity) {
    velocity = input_velocity;
}
