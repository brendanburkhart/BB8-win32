#include "Simulation.h"

#include <cmath>

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
      rotation(Quaternion::Identity()),
      drive_assembly(Vex775(), Gearbox(5.0, 0.0)),
      drive_coupling(
      TorqueInterface(
          [=](double t) { return drive_acceleration(t); },
          [=](double t) { return drive_d_acceleration(t); }),
      TorqueInterface(
              [=](double t) { return drive_assembly.acceleration(t); },
              [=](double t) { return drive_assembly.inertia(t); })),
      tilt_motor(Vex775()),
      roll(0.0),
      angular_velocity(0.0),
      heading(0.0),
      tilt(0.43 * PI),
      tilt_velocity(0.0),
      platform_angle(0.0),
      platform_velocity(0.0),
      pendulum_angle(0.0),
      pendulum_velocity(0.0) {
    // initial tilt
    Vector3 axis(-1.0, 0.0, 0.0);
    rotation = Quaternion::EulerAngle(tilt, axis).Multiply(rotation);
}

Vector3 Simulation::get_position() const {
    return position;
}

Quaternion Simulation::get_rotation() const {
    return rotation;
}

double Simulation::drive_acceleration(double torque) const {
    double numerator = torque - radius * std::cos(tilt) * angular_velocity * tilt_velocity;
    double denominator = (sphere_mass + pendulum_mass) * radius * radius * (2.0 / 3.0 + std::sin(tilt) * std::sin(tilt));

    return numerator / denominator;
}

double Simulation::drive_d_acceleration(double torque) const {
    double denominator = (sphere_mass + pendulum_mass) * radius * radius * (2.0 / 3.0 + std::sin(tilt) * std::sin(tilt));

    return 1.0 / denominator;
}

double Simulation::get_heading() const {
    return std::fmod(heading, 2 * PI);
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

    // negotiate drive torque

    auto [torque_m, drive_shaft_acceleration] = drive_coupling.solve();
    double torque_p = 0.0;

    drive_assembly.update(2.0, torque_m, dt);
    tilt_motor.update(0.0, torque_p, dt);

    double m = sphere_mass + pendulum_mass;

    double domega = drive_acceleration(torque_m);
    angular_velocity += dt * domega;
    roll += dt * angular_velocity;

    tilt += dt * tilt_velocity;
    tilt_velocity += dt * 3.0 * torque_p / (2 * m * radius * radius);

    platform_angle += dt * platform_velocity;
    platform_velocity += dt * (torque_m - pendulum_length * std::cos(pendulum_angle) * std::sin(platform_angle) * pendulum_mass * g) / (pendulum_mass * pendulum_length * pendulum_length);

    pendulum_angle += dt * pendulum_velocity;
    pendulum_velocity += dt * (torque_p - pendulum_length * std::cos(platform_angle) * std::sin(pendulum_angle) * pendulum_mass * g) / (pendulum_mass * pendulum_length * pendulum_length);

    Vector3 axis1 = Vector3(0, 0, -1);
    Quaternion dr1 = Quaternion::EulerAngle(roll, axis1);
    Vector3 axis2 = Vector3(1, 0, 0);
    Quaternion dr2 = Quaternion::EulerAngle(tilt, axis2);
    Vector3 axis3 = Vector3(0, 0, 1);
    Quaternion dr3 = Quaternion::EulerAngle(heading, axis3);

    rotation = dr3.Multiply(dr2.Multiply(dr1));
}
