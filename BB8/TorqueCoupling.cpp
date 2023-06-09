#include "TorqueCoupling.h"

#include "framework.h"

#include <cmath>
#include <sstream>
#include <stdexcept>

TorqueCoupling::TorqueCoupling(TorqueInterface input, TorqueInterface output)
    : input(input), output(output), last_input_torque(0.0) {}

std::pair<double, double> TorqueCoupling::solve() {
    if (std::isnan(last_input_torque)) {
        last_input_torque = 0.0;
    }

    double torque = newton(last_input_torque, 6);
    last_input_torque = torque;

    double acceleration = input.acceleration(torque);
    return { torque, acceleration };
}

double TorqueCoupling::newton(double initial_value, size_t iterations) const {
    // apply Newton's method
    double x = initial_value;

    auto f = [=](double torque) { return input.acceleration(torque) - output.acceleration(torque); };
    auto df = [=](double torque) { return input.inertia(torque) - output.inertia(torque); };

    for (size_t i = 0; i < iterations; i++) {
        if (std::fabs(df(x)) < 1e-5) {
            throw std::domain_error("Newton's method encountered stationary point");
        }

        x = x - f(x) / df(x);
    }

    return x;
}
