#ifndef __RAY_H__
#define __RAY_H__

/// Checks if a ray intersects with a sphere.
/// @param origin The origin of the ray.
/// @param ray Vector specifying the direction of the ray.
/// @param center Center of the sphere.
/// @param radius Sphere radius.
/// @return True if the ray intersects, false if not.
bool RaySphereIntersect(const Vec3& origin, const Vec3& ray, const Vec3& center, float radius);

/// Calculates the intersection point (if any) between a ray and a plane.
/// @param origin The origin of the ray.
/// @param ray Vector specifying the direction of the ray.
/// @param normal The plane normal.
/// @return The intersection point, this may not be valid as the ray may not intersect.
Vec3 RayPlaneIntersect(const Vec3& origin, const Vec3& ray, const Vec3& normal, float d);

#endif // __RAY_H__
