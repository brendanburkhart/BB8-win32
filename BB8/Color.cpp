#include "Color.h"

#include <algorithm>

Color::Color(double blue, double green, double red, double alpha)
    : Blue(blue), Green(green), Red(red), Alpha(alpha) {}

Color::Color() : Color(0.0, 0.0, 0.0, 1.0) {}
