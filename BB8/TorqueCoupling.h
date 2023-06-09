#pragma once

#include <functional>
#include <tuple>

#include "TorqueInterface.h"

class TorqueCoupling
{
public:
    TorqueCoupling(TorqueInterface input, TorqueInterface output);

    std::pair<double, double> solve();

private:
    const TorqueInterface input;
    const TorqueInterface output;

    double last_input_torque;

    double newton(double initial_value, size_t iterations = 4) const;
};
