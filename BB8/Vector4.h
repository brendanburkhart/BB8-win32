#pragma once

class Vector4 {
public:
    double X, Y, Z, W;

    Vector4(double x, double y, double z, double w) : X(x), Y(y), Z(z), W(w) {};

    static Vector4 Subtract(Vector4 minuend, Vector4 subtrahend);
    static Vector4 Add(Vector4 summand1, Vector4 summand2);
    static Vector4 Multiply(Vector4 multiplicand, double multiplier);
    static Vector4 Divide(Vector4 dividend, double divisor);

    static Vector4 Negate(Vector4 vector);
    static double Dot(Vector4 left, Vector4 right);
};
