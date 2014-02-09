#include "Common.h"

#include "Ray.h"

bool RaySphereIntersect(const Vec3& origin, const Vec3& ray, const Vec3& center, float radius)
{
	// Ray/Sphere intersection [Real-Time Rendering 16.6]

	Vec3 origin_center = vector::Subtract(origin, center);
	float b = vector::Dot(ray, origin_center);
	float c = vector::Dot(origin_center, origin_center) - radius*radius;

	float bb_c = b*b-c; 

	return (bb_c >= 0);
}

Vec3 RayPlaneIntersect(const Vec3& origin, const Vec3& ray, const Vec3& normal, float d)
{
	float t = -((vector::Dot(origin, normal) + d) / vector::Dot(ray, normal));

	return vector::Add(origin, Vec3(ray.x * t, ray.y * t, ray.z * t));
}
