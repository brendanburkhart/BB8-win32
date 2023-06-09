#pragma once

#include "Gearbox.h"
#include "Motor.h"
#include "MotorAssembly.h"
#include "Quaternion.h"
#include "TorqueCoupling.h"
#include "Vector3.h"

class Simulation
{
public:
    Simulation(double radius, double sphere_mass, double pendulum_mass, double pendulum_length, double time_step, Vector3 position);

    Vector3 get_position() const;
    Quaternion get_rotation() const;

    double get_heading() const;

    void update(double elapsed_time);

private:
    const double radius;
    const double sphere_mass;
    const double pendulum_mass;
    const double pendulum_length;
    const double time_step;

    const double rolling_friction;

    Vector3 position;
    Quaternion rotation;

    MotorAssembly drive_assembly;
    TorqueCoupling drive_coupling;

    Motor tilt_motor;

    double roll;
    double angular_velocity;
    double heading;
    double tilt;
    double tilt_velocity;
    double platform_angle;
    double platform_velocity;
    double pendulum_angle;
    double pendulum_velocity;

    double drive_acceleration(double drive_torque) const;
    double drive_d_acceleration(double drive_torque) const;

    void fixed_update(double dt);
};
