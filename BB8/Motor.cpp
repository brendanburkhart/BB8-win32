#include "Motor.h"

Motor::Motor(double Kt, double Kv,
    double damping, double inertia,
    double resistance, double inductance)
    : Kt(Kt), Kv(Kv), damping(damping), inertia(inertia),
    resistance(resistance), inductance(inductance),
    angular_velocity(0.0), current(0.0) {}

void Motor::update(double voltage, double torque, double dt) {
    double dw = couplingAcceleration(torque);
    double dI = (voltage - Kv*angular_velocity - resistance * current) / inductance;

    angular_velocity += dw * dt;
    current += dI * dt;
}

double Motor::couplingAcceleration(double torque) const {
    return (Kt * current - damping * angular_velocity - torque) / inertia;
}

double Motor::couplingInertia(double torque) const {
    return -1.0 / inertia;
}

double Motor::velocity() const {
    return angular_velocity;
}

Motor CIM() {
    return Motor(1.84e-2, 2.11e-2, 8.91e-2, 7.65e-5, 9.16e-2, 5.90e-5);
}

Motor Vex775() {
    return Motor(5.30e-3, 6.37e-4, 1.98e-7, 3e-6, 8.98e-2, 4.00e-5);
}
