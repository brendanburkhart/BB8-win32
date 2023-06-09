#include "Quaternion.h"

#include <cmath>

Quaternion::Quaternion(double r, double a, double b, double c)
    : r(r), a(a), b(b), c(c) { }

Quaternion Quaternion::EulerAngle(double theta, Vector3 axis) {
    double r = cos(0.5 * theta);
    double s = sin(0.5 * theta);

    axis = Vector3::Normalize(axis);

    return Quaternion(r, axis.X * s, axis.Y * s, axis.Z * s);
}

Quaternion Quaternion::Identity() {
    return Quaternion(1.0, 0.0, 0.0, 0.0);
}

Quaternion Quaternion::Scale(double scale) const {
    return Quaternion(
        scale * r,
        scale * a,
        scale * b,
        scale * c
    );
}

double Quaternion::Dot(const Quaternion& q) const {
    return r * q.r + a * q.a + b * q.b + c * q.c;
}

Quaternion Quaternion::Multiply(const Quaternion& q) const {
    return Quaternion(
        r * q.r - a * q.a - b * q.b - c * q.c,
        r * q.a + a * q.r + b * q.c - c * q.b,
        r * q.b + b * q.r + c * q.a - a * q.c,
        r * q.c + c * q.r + a * q.b - b * q.a
    );
}

Quaternion Quaternion::Inverse() const {
    return Quaternion(r, -a, -b, -c);
}

Vector3 Quaternion::Rotate(const Vector3& coordinate) const {
    Quaternion p = Quaternion(0, coordinate.X, coordinate.Y, coordinate.Z);

    // q*p*q^(-1)
    Quaternion result = Multiply(p.Multiply(Inverse()));
    return Vector3(result.a, result.b, result.c);
}
