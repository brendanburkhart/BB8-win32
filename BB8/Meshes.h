#pragma once

#include "Mesh.h"

Mesh Robot(double radius, int density);

Mesh Platform(double width, double thickness);
Mesh Pendulum(double thickness, double length);

Mesh Plane(double square_size, int extent);
