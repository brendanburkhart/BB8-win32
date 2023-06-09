#pragma once

#include "Gearbox.h"
#include "Motor.h"

class MotorAssembly
{
public:
    MotorAssembly(Motor motor, Gearbox gearbox);

    void update(double voltage, double output_torque, double dt);

    double velocity() const;
    double acceleration(double output_torque) const;
    double inertia(double output_torque) const;

private:
    Motor motor;
    Gearbox gearbox;
};
