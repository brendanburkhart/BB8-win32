#pragma once

class Motor
{
public:
    Motor(double Kt, double Kv,
        double damping, double inertia,
        double resistance, double inductance);

    void bringToSteadyState(double velocity);
    void update(double voltage, double torque, double dt);

    // coupling behavior
    double couplingAcceleration(double torque) const;
    double couplingInertia(double torque) const;

    double velocity() const;

private:
    const double Kt;
    const double Kv;
    const double damping;
    const double inertia;
    const double resistance;
    const double inductance;

    double angular_velocity;
    double current;
};

Motor CIM();
