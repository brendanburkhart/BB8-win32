#include "Vector4.h"

#include <cmath>

Vector4 Vector4::Subtract(Vector4 minuend, Vector4 subtrahend) {
    return Vector4(minuend.X - subtrahend.X, minuend.Y - subtrahend.Y, minuend.Z - subtrahend.Z, minuend.W);
}

Vector4 Vector4::Add(Vector4 summand1, Vector4 summand2) {
    return Vector4(summand1.X + summand2.X, summand1.Y + summand2.Y, summand1.Z + summand2.Z, summand1.W);
}

Vector4 Vector4::Multiply(Vector4 multiplicand, double multiplier) {
    return Vector4(multiplicand.X * multiplier, multiplicand.Y * multiplier, multiplicand.Z * multiplier, multiplicand.W * multiplier);
}

Vector4 Vector4::Divide(Vector4 dividend, double divisor) {
    return Vector4(dividend.X / divisor, dividend.Y / divisor, dividend.Z / divisor, dividend.W / divisor);
}

Vector4 Vector4::Negate(Vector4 vector) {
    return Vector4(-vector.X, -vector.Y, -vector.Z, -vector.W);
}

double Vector4::Dot(Vector4 left, Vector4 right) {
    return (left.X * right.X) + (left.Y * right.Y) + (left.Z * right.Z) + (left.W * right.W);
}

double Length(Vector4 vector) {
    return sqrt((vector.X * vector.X) + (vector.Y * vector.Y) + (vector.Z * vector.Z) + (vector.W * vector.W));
}

Vector4 Normalize(Vector4 vector) {

    double length = Length(vector);

    if (!(length < 1e-6)) {
        double inverse = 1.0 / length;

        return Vector4(vector.X * inverse, vector.Y * inverse, vector.Z * inverse, vector.W * inverse);
    }
    return vector;
}
