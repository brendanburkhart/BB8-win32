#include "Vector3.h"

#include <cmath>

Vector3::Vector3(double x, double y, double z) : X(x), Y(y), Z(z) {};

Vector3::Vector3() : Vector3(0.0, 0.0, 0.0) {};

Vector3 Vector3::UnitY() {
    return Vector3(0.0, 1.0, 0.0);
}

Vector3 Vector3::Origin() {
    return Vector3(0.0, 0.0, 0.0);
}

Vector3 Vector3::Subtract(Vector3 minuend, Vector3 subtrahend) {
    return Vector3(minuend.X - subtrahend.X, minuend.Y - subtrahend.Y, minuend.Z - subtrahend.Z);
}

Vector3 Vector3::Add(Vector3 summand1, Vector3 summand2) {
    return Vector3(summand1.X + summand2.X, summand1.Y + summand2.Y, summand1.Z + summand2.Z);
}

Vector3 Vector3::Multiply(Vector3 multiplicand, double multiplier) {
    return Vector3(multiplicand.X * multiplier, multiplicand.Y * multiplier, multiplicand.Z * multiplier);
}

Vector3 Vector3::Divide(Vector3 dividend, double divisor) {
    return Vector3(dividend.X / divisor, dividend.Y / divisor, dividend.Z / divisor);
}

Vector3 Vector3::Negate(Vector3 vector) {
    return Vector3(-vector.X, -vector.Y, -vector.Z);
}

Vector3 Vector3::Normalize(Vector3 vector) {
    double length = Length(vector);

    if (!(length < 1e-6)) {
        double inverse = 1.0 / length;

        return Vector3(vector.X * inverse, vector.Y * inverse, vector.Z * inverse);
    }
    return vector;
}

double Vector3::Length(Vector3 vector) {
    return std::sqrt((vector.X * vector.X) + (vector.Y * vector.Y) + (vector.Z * vector.Z));
}

double Vector3::Dot(Vector3 left, Vector3 right) {
    return (left.X * right.X) + (left.Y * right.Y) + (left.Z * right.Z);
}

Vector3 Vector3::Cross(Vector3 left, Vector3 right) {
    return Vector3((left.Y * right.Z) - (left.Z * right.Y),

        (left.Z * right.X) - (left.X * right.Z),

        (left.X * right.Y) - (left.Y * right.X));
}
