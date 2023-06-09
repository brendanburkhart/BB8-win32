#pragma once

#include <tuple>
#include <vector>

#include "framework.h"

#include "Camera.h"
#include "Color.h"
#include "Lighting.h"
#include "Mesh.h"
#include "Quaternion.h"
#include "Vector3.h"

class RenderDevice {
public:
    RenderDevice();
    RenderDevice(UINT32 pixelWidth, UINT32 pixelHeight);

    // This method is called to clear the back buffer with a specific color
    void Clear(Color fillColor);

    // Once scene is rendered, use to present scene to render target
    HRESULT PresentTo(ID2D1HwndRenderTarget* render_target) const;

    void RenderSurface(const Camera& camera, const Lighting& lighting, const Mesh& mesh, const Quaternion& rotation, const Vector3& translation);
    void RenderWireframe(const Camera& camera, const Mesh& mesh, const Quaternion& rotation, const Vector3& translation, const Color& color, int thickness);

private:
    std::vector<uint8_t> color_buffer;
    std::vector<double> depth_buffer;
    std::vector<double> shadow_buffer;

    UINT32 width, height;

    // Project transform coordinate and projects to screen-space
    std::tuple<bool, Vector3> Project(const Vector3& coord, const Matrix& camera_transform) const;

    void RasterizeTriangle(Vector3 p1, Vector3 p2, Vector3 p3, Color color);

    // Draw scan line at y in triangle formed by pa, pb, and pc
    void ProcessScanLine(int y, Vector3 pa, Vector3 pb, Vector3 pc, Vector3 pd, Color color);

    // Clamps value between min and max
    double Clamp(double value, double min = 0, double max = 1);

    // Interpolates gradient between min and max
    double Interpolate(double min, double max, double gradient);

    // Draws a line between point A and point B using modified Bresenham's algorithm
    void DrawLine(Vector3 pointA, Vector3 pointB, Color color, int width);

    // DrawPoint calls PutPixel but does the clipping operation before
    void DrawPoint(Vector3 point, Color color);

    // Called to put a pixel on screen at a specific X,Y coordinates
    void PutPixel(int x, int y, Color color);
};
