#pragma once

class Gearbox
{
public:
    Gearbox(double ratio, double damping);

    double outputTorque(double input_torque) const;
    double outputSpeed(double input_speed) const;

    double inputTorque(double output_torque) const;
    double inputSpeed(double output_speed) const;

    double reductionRatio() const;

    void update(double input_velocity);

private:
    const double ratio;
    const double damping;

    double velocity;
};
