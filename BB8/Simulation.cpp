#include "Simulation.h"

#include <cmath>
#include <sstream>

#include "framework.h"

constexpr double PI = 3.1415926535;
constexpr double g = 9.81;

Simulation::Simulation(double radius, double sphere_mass, double pendulum_mass, double pendulum_length, double time_step, Vector3 position)
    : radius(radius),
      sphere_mass(sphere_mass),
      pendulum_mass(pendulum_mass),
      pendulum_length(pendulum_length),
      time_step(time_step),
      rolling_friction(0.01),
      position(position),
      drive_assembly(Vex775(), Gearbox(50.0, 0.0)),
      drive_coupling(
      TorqueInterface(
          [=](double t) { return drive_acceleration(t) + platform_acceleration(t); },
          [=](double t) { return drive_d_acceleration(t) + platform_d_acceleration(t); }),
      TorqueInterface(
              [=](double t) { return drive_assembly.acceleration(t); },
              [=](double t) { return drive_assembly.inertia(t); })),
      tilt_assembly(Vex775(), Gearbox(30.0, 0.0)),
      tilt_coupling(TorqueInterface(
          [=](double t) { return tilt_acceleration(t) + pendulum_acceleration(t); },
          [=](double t) { return tilt_d_acceleration(t) + pendulum_d_acceleration(t); }),
          TorqueInterface(
              [=](double t) { return tilt_assembly.acceleration(t); },
              [=](double t) { return tilt_assembly.inertia(t); })),
      roll(0.0),
      angular_velocity(0.0),
      heading(0.0),
      tilt(0.43 * PI),
      tilt_velocity(0.0),
      platform_angle(0.0),
      platform_velocity(0.0),
      pendulum_angle(0.25 * PI),
      pendulum_velocity(0.0) {
    // initial tilt
    Vector3 axis(-1.0, 0.0, 0.0);
}

Vector3 Simulation::get_position() const {
    return position;
}

Quaternion Simulation::get_rotation() const {
    Vector3 axis1 = Vector3(0, 0, -1);
    Quaternion dr1 = Quaternion::EulerAngle(roll, axis1);
    Vector3 axis2 = Vector3(1, 0, 0);
    Quaternion dr2 = Quaternion::EulerAngle(tilt, axis2);
    Vector3 axis3 = Vector3(0, 0, 1);
    Quaternion dr3 = Quaternion::EulerAngle(heading, axis3);

    return dr3.Multiply(dr2.Multiply(dr1));
}

double Simulation::get_heading() const {
    return std::fmod(heading, 2 * PI);
}

Quaternion Simulation::get_platform_rotation() const {
    Vector3 axis1 = Vector3(0, 0, -1);
    Quaternion dr1 = Quaternion::EulerAngle(platform_angle, axis1);
    Vector3 axis2 = Vector3(1, 0, 0);
    Quaternion dr2 = Quaternion::EulerAngle(tilt, axis2);
    Vector3 axis3 = Vector3(0, 0, 1);
    Quaternion dr3 = Quaternion::EulerAngle(heading, axis3);

    return dr3.Multiply(dr2.Multiply(dr1));
}

Quaternion Simulation::get_pendulum_rotation() const {
    Vector3 axis1 = Vector3(0, 0, -1);
    Quaternion dr1 = Quaternion::EulerAngle(platform_angle, axis1);
    Vector3 axis2 = Vector3(1, 0, 0);
    Quaternion dr2 = Quaternion::EulerAngle(pendulum_angle + 0.5*PI, axis2);
    Vector3 axis3 = Vector3(0, 0, 1);
    Quaternion dr3 = Quaternion::EulerAngle(heading, axis3);

    return dr3.Multiply(dr2.Multiply(dr1));
}

double Simulation::drive_acceleration(double torque) const {
    double numerator = torque - radius * std::cos(tilt) * angular_velocity * tilt_velocity;
    double denominator = radius * radius * (2.0 / 3.0 * sphere_mass + (sphere_mass + pendulum_mass) * std::sin(tilt) * std::sin(tilt));

    return numerator / denominator;
}

double Simulation::drive_d_acceleration(double torque) const {
    double denominator = radius * radius * (2.0 / 3.0 * sphere_mass + (sphere_mass + pendulum_mass) * std::sin(tilt) * std::sin(tilt));

    return 1.0 / denominator;
}

double Simulation::platform_acceleration(double torque) const {
    double numerator = torque - pendulum_length * std::cos(pendulum_angle) * std::sin(platform_angle) * pendulum_mass * g;
    double denominator = pendulum_mass * pendulum_length * pendulum_length;

    return numerator / denominator;
}

double Simulation::platform_d_acceleration(double torque) const {
    double denominator = pendulum_mass * pendulum_length * pendulum_length;

    return 1.0 / denominator;
}

double Simulation::tilt_acceleration(double torque) const {
    return 3.0 * torque / (2 * sphere_mass * radius * radius);
}

double Simulation::tilt_d_acceleration(double torque) const {
    return 3.0 / (2 * sphere_mass * radius * radius);
}

double Simulation::pendulum_acceleration(double torque) const {
    double numerator = torque - pendulum_length * std::cos(platform_angle) * std::sin(pendulum_angle) * pendulum_mass * g;
    double denominator = pendulum_mass * pendulum_length * pendulum_length;

    return numerator / denominator;
}

double Simulation::pendulum_d_acceleration(double torque) const {
    double denominator = pendulum_mass * pendulum_length * pendulum_length;

    return 1.0 / denominator;
}

void Simulation::update(double elapsed_time) {
    while (elapsed_time > time_step) {
        elapsed_time -= time_step;
        fixed_update(time_step);
    }

    fixed_update(elapsed_time);
}

void Simulation::fixed_update(double dt) {
    double ground_speed = angular_velocity * std::sin(tilt) * radius;
    double dx = dt * ground_speed * std::cos(heading);
    double dy = dt * ground_speed * std::sin(heading);

    position = Vector3::Add(position, Vector3(dx, dy, 0.0));

    double dtheta = dt * angular_velocity * std::cos(tilt);
    heading += dtheta;

    constexpr double tilt_setpoint = 0.4 * PI;

    // negotiate torque
    auto [torque_m, drive_shaft_acceleration] = drive_coupling.solve();
    auto [torque_p, tilt_shaft_acceleration] = tilt_coupling.solve();

    double drive_voltage = 1 * PI - angular_velocity;
    drive_assembly.update(drive_voltage, torque_m, dt);

    double tilt_voltage = 5*(0.35 * PI - tilt) - 10*tilt_velocity;
    tilt_assembly.update(tilt_voltage, torque_p, dt);

    angular_velocity += dt * drive_acceleration(torque_m);
    roll += dt * angular_velocity;

    tilt_velocity += dt * tilt_acceleration(torque_p);
    tilt += dt * tilt_velocity;

    platform_velocity += dt * platform_acceleration(torque_m);
    platform_angle += dt * platform_velocity;

    pendulum_velocity += dt * pendulum_acceleration(torque_p);
    pendulum_angle += dt * pendulum_velocity;
}
