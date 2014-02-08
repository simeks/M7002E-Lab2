#ifndef __PRIMITIVEFACTORY_H__
#define __PRIMITIVEFACTORY_H__

#include "Material.h"

#include <framework/RenderDevice.h>

/// @brief Struct representing a primitive that can be rendered.
struct Primitive
{
	DrawCall draw_call;
};


class RenderDevice;

/// @brief Factory used for creating primitives that can be rendered onto the scene.
class PrimitiveFactory
{
public:
	PrimitiveFactory(RenderDevice* _render_device);
	~PrimitiveFactory();

	/// @brief Creates a square based 3d pyramid.
	/// @param size The size of the pyramid.
	Primitive CreatePyramid(const Vec3& size);

	/// @brief Creates a cube.
	/// @param size The size of the cube.
	Primitive CreateCube(const Vec3& size);

	/// @brief Creates a sphere.
	/// @param radius The radius of the sphere.
	Primitive CreateSphere(float radius);
	
	/// @brief Creates a plane.
	/// @param size Size of the plane.
	Primitive CreatePlane(const Vec2& size);

	/// @brief Destroys the specified primitive, releasing any resources it haves.
	void DestroyPrimitive(Primitive& primitive);


private:
	RenderDevice* _render_device;

};

#endif // __PRIMITIVEFACTORY_H__
