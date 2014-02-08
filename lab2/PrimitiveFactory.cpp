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
	primitive.draw_call.draw_mode = GL_TRIANGLES;
	primitive.draw_call.vertex_count = 18; 

	float vertex_data[18*6]; // 18 vertices, 6 floats each (Px, Py, Pz, Nx, Ny, Nz)
	int i = 0;

	Vec3 half_size;
	half_size.x = size.x * 0.5f;
	half_size.y = size.y * 0.5f;
	half_size.z = size.z * 0.5f;

	Vec3 normal = Vec3(0.0f, -1.0f, 0.0f); // Bottom

	// The bottom
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // bottom 4
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // bottom 1
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // bottom 2
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;

	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // bottom 4
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // bottom 2
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // bottom 3
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;

	// Side 1
	normal = vector::Cross(Vec3(0.0f, 0.0f, 1.0f), vector::Subtract(Vec3(0.0f, half_size.y, 0.0f), Vec3(-half_size.x, -half_size.y, 0.0f)));

	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // bottom 1
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // bottom 4
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = 0.0f;			vertex_data[i++] = half_size.y;		vertex_data[i++] = 0.0f; // Top
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;

	// Side 2
	normal = vector::Cross(Vec3(-1.0f, 0.0f, 0.0f), vector::Subtract(Vec3(0.0f, half_size.y, 0.0f), Vec3(0.0f, -half_size.y, -half_size.z)));

	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // bottom 2
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // bottom 1
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = 0.0f;			vertex_data[i++] = half_size.y;		vertex_data[i++] = 0.0f; // Top
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;

	// Side 3
	normal = vector::Cross(Vec3(0.0f, 0.0f, -1.0f), vector::Subtract(Vec3(0.0f, half_size.y, 0.0f), Vec3(half_size.x, -half_size.y, 0.0f)));

	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // bottom 3
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // bottom 2
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = 0.0f;			vertex_data[i++] = half_size.y;		vertex_data[i++] = 0.0f; // Top
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	
	// Side 4
	normal = vector::Cross(Vec3(1.0f, 0.0f, 0.0f), vector::Subtract(Vec3(0.0f, half_size.y, 0.0f), Vec3(0.0f, -half_size.y, half_size.z)));

	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // bottom 4
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // bottom 3
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = 0.0f;			vertex_data[i++] = half_size.y;		vertex_data[i++] = 0.0f; // Top
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;

	// Create the vertex buffer, possible optimization would be to create a shared vertex buffer for the rectangles
	//	rather than creating a new one for each rectangle.
	primitive.draw_call.vertex_buffer = _render_device->CreateVertexBuffer(6*primitive.draw_call.vertex_count*sizeof(float), vertex_data);
	primitive.draw_call.vertex_offset = 0;
	primitive.draw_call.vertex_format = vertex_format::VF_POSITION3F_NORMAL3F;
	primitive.draw_call.index_buffer = -1; // Specify that we don't want to use an index buffer
	
	primitive.bounding_radius = sqrtf(half_size.x * half_size.x + half_size.y * half_size.y + half_size.z * half_size.z);

	return primitive;
}

Primitive PrimitiveFactory::CreateCube(const Vec3& size)
{
	Primitive primitive;
	primitive.draw_call.draw_mode = GL_TRIANGLES;

	// The cube consists of 6 faces => 36 vertices.
	primitive.draw_call.vertex_count = 36; 

	float vertex_data[36*6]; // 36 vertices, 6 floats each (Px, Py, Pz, Nx, Ny, Nz)
	int i = 0;

	Vec3 half_size;
	half_size.x = size.x * 0.5f;
	half_size.y = size.y * 0.5f;
	half_size.z = size.z * 0.5f;
	
	// Top
	Vec3 normal = Vec3(0.0f, 1.0f, 0.0f); // Up

	vertex_data[i++] = -half_size.x;	vertex_data[i++] = half_size.y;		vertex_data[i++] = -half_size.z; // Bottom left
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top right
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = -half_size.z; // Bottom right
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = half_size.y;		vertex_data[i++] = -half_size.z; // Bottom left
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top left
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top right
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	

	// Bottom
	normal = Vec3(0.0f, -1.0f, 0.0f); // Bottom

	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom left
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom right
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // Top right
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom left
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // Top right
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // Top left
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;

	// Front
	normal = Vec3(0.0f, 0.0f, 1.0f); // Front

	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // Bottom left
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // Bottom right
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top right
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // Bottom left
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top right
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top left
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	
	// Back
	normal = Vec3(0.0f, 0.0f, -1.0f); // Back

	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom left
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = -half_size.z; // Top right
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom right
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom left
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = half_size.y;		vertex_data[i++] = -half_size.z; // Top left
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = -half_size.z; // Top right
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	
	// Left
	normal = Vec3(-1.0f, 0.0f, 0.0f); // Left

	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom left
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // Bottom right
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top right
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom left
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top right
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = half_size.y;		vertex_data[i++] = -half_size.z; // Top left
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	
	// Right
	normal = Vec3(1.0f, 0.0f, 0.0f); // Right

	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom left
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top right
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // Bottom right
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom left
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = -half_size.z; // Top left
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top right
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;		vertex_data[i++] = normal.z;


	primitive.draw_call.vertex_buffer = _render_device->CreateVertexBuffer(6*primitive.draw_call.vertex_count*sizeof(float), vertex_data);
	primitive.draw_call.vertex_offset = 0;
	primitive.draw_call.vertex_format = vertex_format::VF_POSITION3F_NORMAL3F;
	primitive.draw_call.index_buffer = -1; // Specify that we don't want to use an index buffer
	
	primitive.bounding_radius = sqrtf(half_size.x * half_size.x + half_size.y * half_size.y + half_size.z * half_size.z);

	return primitive;
}
Primitive PrimitiveFactory::CreateSphere(float radius)
{
	const int ring_count = 32;
	const int sector_count = 32;

	Primitive primitive;
	primitive.draw_call.draw_mode = GL_TRIANGLES;
	primitive.draw_call.vertex_count = ring_count * sector_count; // +1 for the center vertex 
	primitive.draw_call.vertex_offset = 0;

	float vertex_data[ring_count*sector_count*3*2];
	int vertex_idx = 0;

    float const inv_rings = 1.0f/(float)(ring_count-1);
    float const inv_sectors = 1.0f/(float)(sector_count-1);
	for(int r = 0; r < ring_count; r++)
	{
		float y = sin(-(float)MATH_HALF_PI + (float)MATH_PI * r * inv_rings);
		for(int s = 0; s < sector_count; s++) 
		{
			float x = cos((float)MATH_TWO_PI * s * inv_sectors) * sin((float)MATH_PI * r * inv_rings);
			float z = sin((float)MATH_TWO_PI * s * inv_sectors) * sin((float)MATH_PI * r * inv_rings);

			vertex_data[vertex_idx++] = x * radius;
			vertex_data[vertex_idx++] = y * radius;
			vertex_data[vertex_idx++] = z * radius;

			// Normals
			vertex_data[vertex_idx++] = x;
			vertex_data[vertex_idx++] = y;
			vertex_data[vertex_idx++] = z;
		}
	}
	primitive.draw_call.vertex_buffer = _render_device->CreateVertexBuffer(3*2*primitive.draw_call.vertex_count*sizeof(float), vertex_data);

	// Index data
	primitive.draw_call.index_count = (ring_count-1) * (sector_count-1) * 6;
	uint16_t index_data[(ring_count-1) * (sector_count-1) * 6];
	int index_idx = 0;

	for(int r = 0; r < ring_count-1; r++)
	{
		for(int s = 0; s < sector_count-1; s++) 
		{
			index_data[index_idx++] = (uint16_t)(r * sector_count + s);
			index_data[index_idx++] = (uint16_t)((r + 1) * sector_count + s);
			index_data[index_idx++] = (uint16_t)((r + 1) * sector_count + s + 1);
			
			index_data[index_idx++] = (uint16_t)(r * sector_count + s);
			index_data[index_idx++] = (uint16_t)((r + 1) * sector_count + s + 1);
			index_data[index_idx++] = (uint16_t)(r * sector_count + s + 1);
		}
	}
	primitive.draw_call.index_buffer = _render_device->CreateIndexBuffer(primitive.draw_call.index_count, index_data);
	
	primitive.draw_call.vertex_format = vertex_format::VF_POSITION3F_NORMAL3F;
	
	primitive.bounding_radius = radius;

	return primitive;
}
Primitive PrimitiveFactory::CreatePlane(const Vec2& size)
{
	Primitive primitive;
	primitive.draw_call.draw_mode = GL_TRIANGLES;

	primitive.draw_call.vertex_count = 6; 

	float vertex_data[6*6]; // 6 vertices, 6 floats each (Px, Py, Pz, Nx, Ny, Nz)
	int i = 0;

	Vec2 half_size;
	half_size.x = size.x * 0.5f;
	half_size.y = size.y * 0.5f;
	
	// Top
	Vec3 normal = Vec3(0.0f, 1.0f, 0.0f); // Up

	vertex_data[i++] = -half_size.x;	vertex_data[i++] = 0.0f;		vertex_data[i++] = -half_size.y; // Bottom left
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;	vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = 0.0f;		vertex_data[i++] = half_size.y; // Top right
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;	vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = 0.0f;		vertex_data[i++] = -half_size.y; // Bottom right
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;	vertex_data[i++] = normal.z;
	
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = 0.0f;		vertex_data[i++] = -half_size.y; // Bottom left
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;	vertex_data[i++] = normal.z;
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = 0.0f;		vertex_data[i++] = half_size.y; // Top left
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;	vertex_data[i++] = normal.z;
	vertex_data[i++] = half_size.x;		vertex_data[i++] = 0.0f;		vertex_data[i++] = half_size.y; // Top right
	vertex_data[i++] = normal.x;		vertex_data[i++] = normal.y;	vertex_data[i++] = normal.z;
	

	primitive.draw_call.vertex_buffer = _render_device->CreateVertexBuffer(6*primitive.draw_call.vertex_count*sizeof(float), vertex_data);
	primitive.draw_call.vertex_offset = 0;
	primitive.draw_call.vertex_format = vertex_format::VF_POSITION3F_NORMAL3F;
	primitive.draw_call.index_buffer = -1; // Specify that we don't want to use an index buffer

	primitive.bounding_radius = sqrtf(half_size.x * half_size.x + half_size.y * half_size.y);

	return primitive;
}
void PrimitiveFactory::DestroyPrimitive(Primitive& primitive)
{
	// Delete the buffers that the primitive holds

	_render_device->ReleaseHardwareBuffer(primitive.draw_call.vertex_buffer);
}
