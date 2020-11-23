#include <Tools/Texture.h>

TextureMapping2D::~TextureMapping2D() { }

UVMapping2D::UVMapping2D(Float su, Float sv, Float du, Float dv)
    : su(su), sv(sv), du(du), dv(dv) {}
Point2f UVMapping2D::Map(const SurfaceInteraction& si, Vector2f* dstdx,
    Vector2f* dstdy) const {
    // Compute texture differentials for 2D identity mapping
    *dstdx = Vector2f(su * si.dudx, sv * si.dvdx);
    *dstdy = Vector2f(su * si.dudy, sv * si.dvdy);
    return Point2f(su * si.uv[0] + du, sv * si.uv[1] + dv);
}


Point2f SphericalMapping2D::Map(const SurfaceInteraction& si, Vector2f* dstdx,
    Vector2f* dstdy) const {
    Point2f st = sphere(si.p);
    // Compute texture coordinate differentials for sphere $(u,v)$ mapping
    const Float delta = .1f;
    Point2f stDeltaX = sphere(si.p + delta * si.dpdx);
    *dstdx = (stDeltaX - st) / delta;
    Point2f stDeltaY = sphere(si.p + delta * si.dpdy);
    *dstdy = (stDeltaY - st) / delta;

    // Handle sphere mapping discontinuity for coordinate differentials
    if ((*dstdx)[1] > .5)
        (*dstdx)[1] = 1 - (*dstdx)[1];
    else if ((*dstdx)[1] < -.5f)
        (*dstdx)[1] = -((*dstdx)[1] + 1);
    if ((*dstdy)[1] > .5)
        (*dstdy)[1] = 1 - (*dstdy)[1];
    else if ((*dstdy)[1] < -.5f)
        (*dstdy)[1] = -((*dstdy)[1] + 1);
    return st;
}


Point2f SphericalMapping2D::sphere(const Point3f& p) const {
    Vector3f vec = Normalize(WorldToTexture(p) - Point3f(0, 0, 0));
    Float theta = SphericalTheta(vec), phi = SphericalPhi(vec);
    return Point2f(theta * InvPi, phi * Inv2Pi);
}
