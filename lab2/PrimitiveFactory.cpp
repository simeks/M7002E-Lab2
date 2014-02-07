#include <framework/Common.h>

#include "PrimitiveFactory.h"

#include <framework/RenderDevice.h>

PrimitiveFactory::PrimitiveFactory(RenderDevice* render_device) : _render_device(render_device)
{
}
PrimitiveFactory::~PrimitiveFactory()
{
}

Primitive PrimitiveFactory::CreatePyramid(const Vec3& size)
{
	Primitive primitive;
	primitive.draw_call.draw_mode = GL_LINE_STRIP;

	// We can draw the outlines of the 3d pyramid with 11 vertices using GL_LINE_STRIP
	primitive.draw_call.vertex_count = 11; 

	float vertex_data[11*3]; // 6 vertices, 3 floats each (x, y, z)
	int i = 0;

	Vec3 half_size;
	half_size.x = size.x * 0.5f;
	half_size.y = size.y * 0.5f;
	half_size.z = size.z * 0.5f;

	// The bottom
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // bottom 1
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // bottom 2
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // bottom 3
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // bottom 4
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // bottom 1

	// Side 4 (bottom 1 -> Top)
	vertex_data[i++] = 0.0f;			vertex_data[i++] = half_size.y;		vertex_data[i++] = 0.0f; // Top

	// Side 3 (Top -> bottom 2 -> Top)
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // bottom 2
	vertex_data[i++] = 0.0f;			vertex_data[i++] = half_size.y;		vertex_data[i++] = 0.0f; // Top

	// Side 2 (Top -> bottom 3 -> Top)
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // bottom 3
	vertex_data[i++] = 0.0f;			vertex_data[i++] = half_size.y;		vertex_data[i++] = 0.0f; // Top
	
	// Side 1 (Top -> bottom 4)
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // bottom 4

	// Create the vertex buffer, possible optimization would be to create a shared vertex buffer for the rectangles
	//	rather than creating a new one for each rectangle.
	primitive.draw_call.vertex_buffer = _render_device->CreateVertexBuffer(3*primitive.draw_call.vertex_count*sizeof(float), vertex_data);
	primitive.draw_call.vertex_offset = 0;
	primitive.draw_call.vertex_format = vertex_format::VF_POSITION3F;

	return primitive;
}
Primitive PrimitiveFactory::CreateFilledRectangle(const Vec2& size)
{
	Primitive primitive;
	primitive.draw_call.draw_mode = GL_TRIANGLES;

	// The rectangle consists of two triangles, which means we have 6 vertices.
	primitive.draw_call.vertex_count = 6; 

	float vertex_data[6*3]; // 6 vertices, 3 floats each (x, y, z)
	int i = 0;

	Vec2 half_size;
	half_size.x = size.x * 0.5f;
	half_size.y = size.y * 0.5f;

	// Triangle 1
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = 0.0f; // Bottom left
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = 0.0f; // Bottom right
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = 0.0f; // Top right

	// Triangle 2
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = 0.0f; // Bottom left
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = 0.0f; // Top right
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = half_size.y;		vertex_data[i++] = 0.0f; // Top left

	// Create the vertex buffer, possible optimization would be to create a shared vertex buffer for the rectangles
	//	rather than creating a new one for each rectangle.
	primitive.draw_call.vertex_buffer = _render_device->CreateVertexBuffer(3*primitive.draw_call.vertex_count*sizeof(float), vertex_data);
	primitive.draw_call.vertex_offset = 0;
	primitive.draw_call.vertex_format = vertex_format::VF_POSITION3F;

	return primitive;
}
Primitive PrimitiveFactory::CreateFilledStar(const Vec2& size)
{
	Primitive primitive;
	primitive.draw_call.draw_mode = GL_TRIANGLES;

	// The star consists of two triangles, which means we have 6 vertices.
	primitive.draw_call.vertex_count = 6; 

	float vertex_data[6*3]; // 6 vertices, 3 floats each (x, y, z)
	int i = 0;

	Vec2 half_size;
	half_size.x = size.x * 0.5f;
	half_size.y = size.y * 0.5f;
	
	
	// Triangle 1
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = half_size.y*0.5f;	vertex_data[i++] = 0.0f;
	vertex_data[i++] = 0.0f;			vertex_data[i++] = -half_size.y;		vertex_data[i++] = 0.0f; 
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y*0.5f;	vertex_data[i++] = 0.0f; 

	// Triangle 2
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y*0.5f;	vertex_data[i++] = 0.0f;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y*0.5f;	vertex_data[i++] = 0.0f; 
	vertex_data[i++] = 0.0f;			vertex_data[i++] = half_size.y;			vertex_data[i++] = 0.0f; 

	// Create the vertex buffer, possible optimization would be to create a shared vertex buffer for the stars
	//	rather than creating a new one for each star.
	primitive.draw_call.vertex_buffer = _render_device->CreateVertexBuffer(3*primitive.draw_call.vertex_count*sizeof(float), vertex_data);
	primitive.draw_call.vertex_offset = 0;
	primitive.draw_call.vertex_format = vertex_format::VF_POSITION3F;

	return primitive;
}

Primitive PrimitiveFactory::CreateCube(const Vec3& size)
{
	Primitive primitive;
	primitive.draw_call.draw_mode = GL_LINES;

	// The cube consists of 12 lines => 24 vertices.
	primitive.draw_call.vertex_count = 24; 

	float vertex_data[24*3]; // 24 vertices, 3 floats each (x, y, z)
	int i = 0;

	Vec3 half_size;
	half_size.x = size.x * 0.5f;
	half_size.y = size.y * 0.5f;
	half_size.z = size.z * 0.5f;

	// Face 3 (Top)
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = half_size.y;		vertex_data[i++] = -half_size.z; // Bottom left
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = -half_size.z; // Bottom right
	
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = -half_size.z; // Bottom right
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top right
	
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top right
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top left
	
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top left
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = half_size.y;		vertex_data[i++] = -half_size.z; // Bottom left

	// Face 4 (Bottom)
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom left
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom right
	
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom right
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // Top right
	
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // Top right
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // Top left
	
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // Top left
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom left

	// Connect top and bottom
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = half_size.y;		vertex_data[i++] = -half_size.z; // Bottom left (Top)
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom left (Bottom)
	
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = -half_size.z; // Bottom right (Top)
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom right (Bottom)

	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top right (Top)
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // Top right (Bottom)

	vertex_data[i++] = -half_size.x;	vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top left (Top)
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // Top left (Bottom)


	// Create the vertex buffer, possible optimization would be to create a shared vertex buffer for the rectangles
	//	rather than creating a new one for each rectangle.
	primitive.draw_call.vertex_buffer = _render_device->CreateVertexBuffer(3*primitive.draw_call.vertex_count*sizeof(float), vertex_data);
	primitive.draw_call.vertex_offset = 0;
	primitive.draw_call.vertex_format = vertex_format::VF_POSITION3F;

	return primitive;
}
Primitive PrimitiveFactory::CreateFilledCircle(float radius)
{
	static const uint32_t line_count = 32; // Number of segments per circle

	Primitive primitive;
	primitive.draw_call.draw_mode = GL_TRIANGLE_FAN;
	primitive.draw_call.vertex_count = (line_count+1); // +1 for the center vertex 

	float vertex_data[(line_count+1) * 3];
	int vertex_idx = 0;

	vertex_data[vertex_idx++] = 0.0f; // Center x
	vertex_data[vertex_idx++] = 0.0f; // Center y
	vertex_data[vertex_idx++] = 0.0f; // Center z

	for(int i = 0; i < line_count; ++i)
	{
		float rad = i * (2 * (float)MATH_PI) / (line_count-1);
		vertex_data[vertex_idx++] = cosf(rad) * radius;
		vertex_data[vertex_idx++] = sinf(rad) * radius;
		vertex_data[vertex_idx++] = 0.0f;
	}
	

	// Create the vertex buffer, possible optimization would be to create a shared vertex buffer for the rectangles
	//	rather than creating a new one for each rectangle.
	primitive.draw_call.vertex_buffer = _render_device->CreateVertexBuffer(3*primitive.draw_call.vertex_count*sizeof(float), vertex_data);
	primitive.draw_call.vertex_offset = 0;
	primitive.draw_call.vertex_format = vertex_format::VF_POSITION3F;
	

	return primitive;
}
void PrimitiveFactory::DestroyPrimitive(Primitive& primitive)
{
	// Delete the buffers that the primitive holds

	_render_device->ReleaseVertexBuffer(primitive.draw_call.vertex_buffer);
}
