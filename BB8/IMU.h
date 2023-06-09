#pragma once

#include "Simulation.h"

class IMU
{
public:
    IMU(Simulation* simulation);

    double heading() const;
    double tilt() const;
    double tilt_velocity() const;
    double angular_velocity() const;

private:
    Simulation* simulation;
};

