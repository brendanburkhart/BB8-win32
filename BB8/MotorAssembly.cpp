#include "MotorAssembly.h"

MotorAssembly::MotorAssembly(Motor motor, Gearbox gearbox)
    : motor(motor), gearbox(gearbox) {}

void MotorAssembly::update(double voltage, double output_torque, double dt) {
    double motor_torque = gearbox.inputTorque(output_torque);

    motor.update(voltage, motor_torque, dt);
    gearbox.update(motor.velocity());
}

double MotorAssembly::velocity() const {
    return gearbox.outputSpeed(motor.velocity());
}

double MotorAssembly::acceleration(double output_torque) const {
    double motor_torque = gearbox.inputTorque(output_torque);
    double motor_acceleration = motor.couplingAcceleration(motor_torque);
    double output_acceleration = gearbox.outputSpeed(motor_acceleration);

    return output_acceleration;
}

double MotorAssembly::inertia(double output_torque) const {
    double motor_torque = gearbox.inputTorque(output_torque);
    double motor_inertia = motor.couplingInertia(motor_torque) / gearbox.reductionRatio();
    double output_inertia = gearbox.outputSpeed(motor_inertia);

    return output_inertia;
}
