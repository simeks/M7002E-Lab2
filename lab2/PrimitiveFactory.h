#ifndef __PRIMITIVEFACTORY_H__
#define __PRIMITIVEFACTORY_H__

/// @brief Struct representing a primitive that can be rendered.
struct Primitive
{
	int vertex_buffer; // VBO holding the vertex positions. Expecting the element format: float x, y, z
	int color_buffer; // VBO holding the vertex colors. Expecting the element format: float r, g, b, a

	uint32_t vertex_count; // Number of vertices for this primitive.

	GLenum draw_mode; // Specifies draw mode, e.g. GL_POINTS, GL_TRIANGLES, etc.
};

/// @brief RGBA color
struct Color
{
	Color(float _r = 0.0f, float _g = 0.0f, float _b = 0.0f, float _a = 1.0f) 
		: r(_r), g(_g), b(_b), a(_a)
	{}

	float r, g, b, a;
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
	/// @param color The color of the pyramid.
	Primitive CreatePyramid(const Vec3& size, const Color& color);

	/// @brief Creates a filled rectangle primitive.
	/// @param size The size of the rectangle.
	/// @param color The color of the rectangle.
	Primitive CreateFilledRectangle(const Vec2& size, const Color& color);
	
	/// @brief Creates a filled star primitive.
	/// @param size The size of the star.
	/// @param color The color of the star.
	Primitive CreateFilledStar(const Vec2& size, const Color& color);

	/// @brief Creates a cube.
	/// @param size The size of the cube.
	/// @param color The color of the cube.
	Primitive CreateCube(const Vec3& size, const Color& color);

	/// @brief Creates a filled circle.
	/// @param radius The radius of the circle.
	/// @param color The color of the circle.
	Primitive CreateFilledCircle(float radius, const Color& color);

	/// @brief Destroys the specified primitive, releasing any resources it haves.
	void DestroyPrimitive(Primitive& primitive);


private:
	RenderDevice* _render_device;

};

#endif // __PRIMITIVEFACTORY_H__
