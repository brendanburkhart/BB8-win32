#pragma once

#include "Vector3.h"

class Quaternion
{
private:
    double r;
    double a, b, c;

    Quaternion(double r, double a, double b, double c);

public:
    static Quaternion EulerAngle(double theta, Vector3 axis);
    static Quaternion Identity();

    Quaternion Scale(double scale) const;
    double Dot(const Quaternion& q) const;
    Quaternion Multiply(const Quaternion& q) const;
    Quaternion Inverse() const;

    Vector3 Rotate(const Vector3& coordinate) const;

    friend class Slerp;
};
