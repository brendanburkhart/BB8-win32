#include "Matrix.h"

#include <vector>
#include <cmath>

#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"

Matrix Matrix::Multiply(const Matrix& a, const Matrix& b) {
    Matrix product = Matrix(0);

    for (int row = 0; row < 4; row++) {
        for (int i = 0; i < 4; i++) {
            const double a_row_i = a.data[row * 4 + i];

            for (int col = 0; col < 4; col++) {
                product.data[row * 4 + col] += a_row_i * b.data[i * 4 + col];
            }
        }
    }

    return product;
}

Matrix Matrix::RotationYawPitchRoll(double yaw, double pitch, double roll) {
    double halfRoll = roll * 0.5f;
    double halfPitch = pitch * 0.5f;
    double halfYaw = yaw * 0.5f;

    double sinRoll = (double)sin(halfRoll);
    double cosRoll = (double)cos(halfRoll);
    double sinPitch = (double)sin(halfPitch);
    double cosPitch = (double)cos(halfPitch);
    double sinYaw = (double)sin(halfYaw);
    double cosYaw = (double)cos(halfYaw);

    Vector4 rotation = Vector4(
        (cosYaw * sinPitch * cosRoll) + (sinYaw * cosPitch * sinRoll),
        (sinYaw * cosPitch * cosRoll) - (cosYaw * sinPitch * sinRoll),
        (cosYaw * cosPitch * sinRoll) - (sinYaw * sinPitch * cosRoll),
        (cosYaw * cosPitch * cosRoll) + (sinYaw * sinPitch * sinRoll));

    double xx = rotation.X * rotation.X;
    double yy = rotation.Y * rotation.Y;
    double zz = rotation.Z * rotation.Z;
    double xy = rotation.X * rotation.Y;
    double zw = rotation.Z * rotation.W;
    double zx = rotation.Z * rotation.X;
    double yw = rotation.Y * rotation.W;
    double yz = rotation.Y * rotation.Z;
    double xw = rotation.X * rotation.W;

    Matrix returnMatrix = Matrix(
        1.0f - (2.0f * (yy + zz)), 2.0f * (xy + zw), 2.0f * (zx - yw), 0.0,
        2.0f * (xy - zw), 1.0f - (2.0f * (zz + xx)), 2.0f * (yz + xw), 0.0,
        2.0f * (zx + yw), 2.0f * (yz - xw), 1.0f - (2.0f * (yy + xx)), 0.0,
        0.0, 0.0, 0.0, 1.0);

    return returnMatrix;
}

Matrix Matrix::Translation(Vector3 translation) {
    return Matrix(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        translation.X, translation.Y, translation.Z, 1
    );
}

Matrix Matrix::Transformation(Quaternion rotation, Vector3 translation) {
    Vector3 x_axis = rotation.Rotate(Vector3(1.0, 0.0, 0.0));
    Vector3 y_axis = rotation.Rotate(Vector3(0.0, 1.0, 0.0));
    Vector3 z_axis = rotation.Rotate(Vector3(0.0, 0.0, 1.0));

    return Matrix(
        x_axis.X, y_axis.X, z_axis.X, translation.X,
        x_axis.Y, y_axis.Y, z_axis.Y, translation.Y,
        x_axis.Z, y_axis.Z, z_axis.Z, translation.Z,
        0.0, 0.0, 0.0, 1.0
    );
}

Matrix Matrix::operator * (const Matrix& multiplier) const {
    return Matrix::Multiply(*this, multiplier);
}

Vector4 Matrix::operator* (const Vector4& vector) const {
    return Vector4(
        (vector.X * data[0]) + (vector.Y * data[1]) + (vector.Z * data[2]) + (vector.W * data[3]),
        (vector.X * data[4]) + (vector.Y * data[5]) + (vector.Z * data[6]) + (vector.W * data[7]),
        (vector.X * data[8]) + (vector.Y * data[9]) + (vector.Z * data[10]) + (vector.W * data[11]),
        (vector.X * data[12]) + (vector.Y * data[13]) + (vector.Z * data[14]) + (vector.W * data[15])
    );
}

Vector3 Matrix::operator* (const Vector3& vector) const {
    Vector4 inclusion(vector.X, vector.Y, vector.Z, 1.0);
    Vector4 product = operator*(inclusion);

    return Vector3(product.X/product.W, product.Y/product.W, product.Z/product.W);
}
