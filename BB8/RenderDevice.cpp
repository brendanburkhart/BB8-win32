#include "RenderDevice.h"

#include "framework.h"

#include <algorithm>
#include <limits>
#include <sstream>

constexpr double epsilon = 0.5;

RenderDevice::RenderDevice() : RenderDevice(0, 0) {}

RenderDevice::RenderDevice(UINT32 width, UINT32 height)
    : width(width), height(height) {
    color_buffer.resize(width * height * 4);
    depth_buffer.resize(width * height);
    shadow_buffer.resize(width * height);
}

void RenderDevice::Clear(Color fillColor) {
    // Buffer data is in 4 byte-per-pixel format, iterates from 0 to end of buffer
    for (auto index = 0; index < color_buffer.size(); index += 4) {
        // BGRA is the color system used by Windows.
        color_buffer[index] = (char)(fillColor.Blue * 255);
        color_buffer[index + 1] = (char)(fillColor.Green * 255);
        color_buffer[index + 2] = (char)(fillColor.Red * 255);
        color_buffer[index + 3] = (char)(fillColor.Alpha * 255);
    }

    // Clear the depth buffer
    for (auto index = 0; index < depth_buffer.size(); index++) {
        depth_buffer[index] = std::numeric_limits<double>::max();
    }

    // Clear the shadow buffer
    for (auto index = 0; index < shadow_buffer.size(); index++) {
        shadow_buffer[index] = std::numeric_limits<double>::max();
    }
}

HRESULT RenderDevice::PresentTo(ID2D1HwndRenderTarget* render_target) const {
    ID2D1Bitmap* bitmap;

    // Create bitmap
    HRESULT result = render_target->CreateBitmap(
        D2D1::SizeU(width, height),
        color_buffer.data(),
        4 * width,
        D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
        &bitmap);

    if (FAILED(result)) {
        return result;
    }

    // Present to render target
    D2D1_SIZE_F target_size = render_target->GetSize();
    D2D1_SIZE_F source_size = bitmap->GetSize();

    render_target->DrawBitmap(
        bitmap,
        D2D1::RectF(0, 0, target_size.width, target_size.height),
        1.0f,
        D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
        D2D1::RectF(0, 0, source_size.width, source_size.height)
    );

    COMSafeRelease(&bitmap);

    return S_OK;
}

void RenderDevice::RenderSurface(const Camera& camera, const Lighting& lighting, const Mesh& mesh, const Quaternion& rotation, const Vector3& translation) {
    Matrix camera_transform = camera.ViewTransform(double(width)/height);
    Matrix model_transform = Matrix::Transformation(rotation, translation);

    Matrix transform = camera_transform * model_transform;

    for (const auto& face : mesh.Faces) {
        // Get each vertex for this face
        const auto& vertex_a = mesh.Vertices[face.A];
        const auto& vertex_b = mesh.Vertices[face.B];
        const auto& vertex_c = mesh.Vertices[face.C];

        // Transform to get the pixel
        auto [clip_a, pixel_a] = Project(vertex_a, transform);
        auto [clip_b, pixel_b] = Project(vertex_b, transform);
        auto [clip_c, pixel_c] = Project(vertex_c, transform);

        if (clip_a || clip_b || clip_c) {
            continue;
        }

        Vector3 normal = rotation.Rotate(face.normal);
        Vector3 position = Vector3::Add(rotation.Rotate(face.position), translation);

        // Rasterize face as a triangles
        auto color = lighting.Model(position, normal, face.color);
        RasterizeTriangle(pixel_a, pixel_b, pixel_c, color);
    }
}

void RenderDevice::RenderWireframe(const Camera& camera, const Mesh& mesh, const Quaternion& rotation, const Vector3& translation, const Color& color, int thickness) {
    Matrix camera_transform = camera.ViewTransform(double(width) / height);
    Matrix model_transform = Matrix::Transformation(rotation, translation);

    Matrix transform = camera_transform * model_transform;

    for (const auto& edge : mesh.Edges) {
        const auto& vertex_a = mesh.Vertices[edge.first];
        const auto& vertex_b = mesh.Vertices[edge.second];

        // Transform to get the pixel
        auto [clip_a, pixel_a] = Project(vertex_a, transform);
        auto [clip_b, pixel_b] = Project(vertex_b, transform);

        DrawLine(pixel_a, pixel_b, color, thickness);
    }
}

std::tuple<bool, Vector3> RenderDevice::Project(const Vector3& coord, const Matrix& transform) const {
    Vector4 inclusion(coord.X, coord.Y, coord.Z, 1.0);
    Vector4 product = transform * inclusion;

    if (product.W < 0.0) {
        return { true, Vector3() };
    }

    Vector3 camera_point = Vector3(product.X / product.W, product.Y / product.W, product.Z / product.W);

    if (camera_point.X < -2.0 || camera_point.X > 2.0 || camera_point.Y < -2.0 || camera_point.Y > 2.0) {
        return { true, Vector3() };
    }

    // The transformed coordinates will be based on coordinate system
    // starting on the center of the screen. But drawing on screen normally starts
    // from top left. We then need to transform them again to have x:0, y:0 on top left.
    // Includes Z pos for the Z-Buffer, un-transformed
    double x = width * (camera_point.X + 0.5);
    double y = height * (-camera_point.Y + 0.5);
    Vector3 screen_point(x, y, camera_point.Z);
    return { false, screen_point };
}

void RenderDevice::RasterizeTriangle(Vector3 p1, Vector3 p2, Vector3 p3, Color color) {
    // Sort points
    if (p1.Y > p2.Y) {
        auto temp = p2;
        p2 = p1;
        p1 = temp;
    }
    if (p2.Y > p3.Y) {
        auto temp = p2;
        p2 = p3;
        p3 = temp;
    }
    if (p1.Y > p2.Y) {
        auto temp = p2;
        p2 = p1;
        p1 = temp;
    }

    if (p3.Y - p1.Y < 1.0) {
        return;
    }

    if ((p2.Y - p1.Y < epsilon) && p1.X > p2.X) {
        auto temp = p2;
        p2 = p1;
        p1 = temp;
    }

    // Calculate inverse slopes
    double dP1P2, dP1P3;
    bool horizontal = false;

    if (p2.Y - p1.Y > epsilon) {
        dP1P2 = (p2.X - p1.X) / (p2.Y - p1.Y);
    } else {
        horizontal = true;
        dP1P2 = 0;
    }

    if (p3.Y - p1.Y > epsilon) {
        dP1P3 = (p3.X - p1.X) / (p3.Y - p1.Y);
    } else {
        dP1P3 = 0;
    }

    // Two cases for triangle shape once points are sorted

    int y_start = std::max(0, (int)p1.Y);
    int y_end = std::min((int)height, (int)p3.Y);

    // p1-p2 line is above p1-p3
    if (horizontal || dP1P2 > dP1P3) {
        // Iterate over height of triangle
        for (auto y = y_start; y <= y_end; y++) {
            // Reverse once second point is reached
            if (y < p2.Y) {
                // Draw line across triangle's width
                ProcessScanLine(y, p1, p3, p1, p2, color);
            } else {
                ProcessScanLine(y, p1, p3, p2, p3, color);
            }
        }
    } // p1-p3 is above p1-p2
    else {
        for (auto y = y_start; y <= y_end; y++) {
            if (y < p2.Y) {
                ProcessScanLine(y, p1, p2, p1, p3, color);
            } else {
                ProcessScanLine(y, p2, p3, p1, p3, color);
            }
        }
    }
}

double RenderDevice::Clamp(double value, double min, double max) {
    return std::max(min, std::min(value, max));
}

double RenderDevice::Interpolate(double min, double max, double gradient) {
    return min + ((max - min) * gradient);
}

void RenderDevice::ProcessScanLine(int y, Vector3 pa, Vector3 pb, Vector3 pc, Vector3 pd, Color color) {
    auto gradient1 = (abs(pa.Y - pb.Y) > epsilon) ? (y - pa.Y) / (pb.Y - pa.Y) : 1;
    auto gradient2 = (abs(pc.Y - pd.Y) > epsilon) ? (y - pc.Y) / (pd.Y - pc.Y) : 1;

    int sx = (int)Interpolate(pa.X, pb.X, gradient1);
    int ex = (int)Interpolate(pc.X, pd.X, gradient2);

    double z1 = Interpolate(pa.Z, pb.Z, gradient1);
    double z2 = Interpolate(pc.Z, pd.Z, gradient2);

    // drawing a line from left (sx) to right (ex)
    for (auto x = sx; x < ex; x++) {
        double gradient = (x - sx) / (double)(ex - sx);

        auto z = Interpolate(z1, z2, gradient);
        DrawPoint(Vector3(x, y, z), color);
    }
}

double delta(int x, int y) {
    double x2 = x * x;
    double y2 = y * y;

    return sqrt(x2 + y2);
}

void RenderDevice::DrawLine(Vector3 pointA, Vector3 pointB, Color color, int width) {
    int x0 = (int)pointA.X;
    int y0 = (int)pointA.Y;
    int x1 = (int)pointB.X;
    int y1 = (int)pointB.Y;
    int x2;
    int y2;

    int wd = (width + 1) / 2;

    auto dx = abs(x1 - x0);
    auto dy = abs(y1 - y0);
    auto sx = (x0 < x1) ? 1 : -1;
    auto sy = (y0 < y1) ? 1 : -1;
    auto err = dx - dy;
    double ed = dx + dy == 0 ? 1 : delta(dx, dy);

    int x = x0;
    int y = y0;

    while (true) {
        double t = delta(x - x0, y - y0) / delta(dx, dy);
        double z = Interpolate(pointA.Z, pointB.Z, t);
        DrawPoint(Vector3(x, y, z), color);

        double e2 = err;
        x2 = x;
        if (2 * e2 >= -dx) {
            for (e2 += dy, y2 = y; e2 < ed * wd && (y1 != y2 || dx > dy); e2 += dx) {
                y2 += sy;
                DrawPoint(Vector3(x, y2, z), color);
            }
            if (x == x1) break;
            e2 = err; err -= dy; x += sx;
        }

        if (2 * e2 <= dy) {
            for (e2 = dx - e2; e2 < ed * wd && (x1 != x2 || dx < dy); e2 += dy) {
                x2 += sx;
                DrawPoint(Vector3(x2, y, z), color);
            }
            if (y == y1) break;
            err += dx; y += sy;
        }
    }
}

void RenderDevice::DrawPoint(Vector3 point, Color color) {
    // Clip to device size
    if (point.X < 0 || point.Y < 0 || point.X >= width || point.Y >= height) {
        return;
    }

    auto index = (int)point.X + ((int)point.Y * width);

    if (point.Z > depth_buffer[index]) {
        return;
    }

    depth_buffer[index] = point.Z;

    PutPixel((int)point.X, (int)point.Y, color);
}

void RenderDevice::PutPixel(int x, int y, Color color) {
    // As we have a 1D Array for our back buffer
    // we need to know the equivalent cell in 1D based
    // on the 2D coordinates on screen
    auto index = 4 * (x + y * width);

    color_buffer[index + 0] = (char)((color.Alpha)*(color.Blue * 255)) + (char)((1.0 - color.Alpha) * (double)color_buffer[index]);
    color_buffer[index + 1] = (char)((color.Alpha)*(color.Green * 255)) + (char)((1.0 - color.Alpha) * (double)color_buffer[index + 1]);
    color_buffer[index + 2] = (char)((color.Alpha)*(color.Red * 255)) + (char)((1.0 - color.Alpha) * (double)color_buffer[index + 2]);
    color_buffer[index + 3] = (char)((color.Alpha)*(color.Alpha * 255)) + (char)((1.0 - color.Alpha) * (double)color_buffer[index + 3]);
}
