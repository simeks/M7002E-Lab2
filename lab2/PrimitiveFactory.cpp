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

	float vertex_data[18*3]; // 18 vertices, 3 floats each (x, y, z)
	int i = 0;

	Vec3 half_size;
	half_size.x = size.x * 0.5f;
	half_size.y = size.y * 0.5f;
	half_size.z = size.z * 0.5f;

	// The bottom
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // bottom 4
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // bottom 1
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // bottom 2
	
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // bottom 4
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // bottom 2
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // bottom 3

	// Side 1
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // bottom 1
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // bottom 4
	vertex_data[i++] = 0.0f;			vertex_data[i++] = half_size.y;		vertex_data[i++] = 0.0f; // Top

	// Side 2
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // bottom 2
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // bottom 1
	vertex_data[i++] = 0.0f;			vertex_data[i++] = half_size.y;		vertex_data[i++] = 0.0f; // Top

	// Side 3
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // bottom 3
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // bottom 2
	vertex_data[i++] = 0.0f;			vertex_data[i++] = half_size.y;		vertex_data[i++] = 0.0f; // Top
	
	// Side 4
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // bottom 4
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // bottom 3
	vertex_data[i++] = 0.0f;			vertex_data[i++] = half_size.y;		vertex_data[i++] = 0.0f; // Top

	// Create the vertex buffer, possible optimization would be to create a shared vertex buffer for the rectangles
	//	rather than creating a new one for each rectangle.
	primitive.draw_call.vertex_buffer = _render_device->CreateVertexBuffer(3*primitive.draw_call.vertex_count*sizeof(float), vertex_data);
	primitive.draw_call.vertex_offset = 0;
	primitive.draw_call.vertex_format = vertex_format::VF_POSITION3F;
	primitive.draw_call.index_buffer = -1; // Specify that we don't want to use an index buffer

	return primitive;
}

Primitive PrimitiveFactory::CreateCube(const Vec3& size)
{
	Primitive primitive;
	primitive.draw_call.draw_mode = GL_TRIANGLES;

	// The cube consists of 6 faces => 36 vertices.
	primitive.draw_call.vertex_count = 36; 

	float vertex_data[36*3]; // 24 vertices, 3 floats each (x, y, z)
	int i = 0;

	Vec3 half_size;
	half_size.x = size.x * 0.5f;
	half_size.y = size.y * 0.5f;
	half_size.z = size.z * 0.5f;

	// Top
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = half_size.y;		vertex_data[i++] = -half_size.z; // Bottom left
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = -half_size.z; // Bottom right
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top right
	
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = half_size.y;		vertex_data[i++] = -half_size.z; // Bottom left
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top right
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top left

	// Bottom
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom left
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // Top right
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom right
	
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom left
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // Top left
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // Top right

	// Front
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // Bottom left
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top right
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // Bottom right
	
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // Bottom left
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top left
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top right
	
	// Back
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom left
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom right
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = -half_size.z; // Top right
	
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom left
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = -half_size.z; // Top right
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = half_size.y;		vertex_data[i++] = -half_size.z; // Top left
	
	// Left
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom left
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top right
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // Bottom right
	
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom left
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = half_size.y;		vertex_data[i++] = -half_size.z; // Top left
	vertex_data[i++] = -half_size.x;	vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top right
	
	// Right
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom left
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = half_size.z; // Bottom right
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top right
	
	vertex_data[i++] = half_size.x;		vertex_data[i++] = -half_size.y;	vertex_data[i++] = -half_size.z; // Bottom left
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = half_size.z; // Top right
	vertex_data[i++] = half_size.x;		vertex_data[i++] = half_size.y;		vertex_data[i++] = -half_size.z; // Top left


	// Create the vertex buffer, possible optimization would be to create a shared vertex buffer for the rectangles
	//	rather than creating a new one for each rectangle.
	primitive.draw_call.vertex_buffer = _render_device->CreateVertexBuffer(3*primitive.draw_call.vertex_count*sizeof(float), vertex_data);
	primitive.draw_call.vertex_offset = 0;
	primitive.draw_call.vertex_format = vertex_format::VF_POSITION3F;
	primitive.draw_call.index_buffer = -1; // Specify that we don't want to use an index buffer

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

	float vertex_data[ring_count*sector_count*3];
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
/*
			*n++ = x;
			*n++ = y;
			*n++ = z;*/
		}
	}
	primitive.draw_call.vertex_buffer = _render_device->CreateVertexBuffer(3*primitive.draw_call.vertex_count*sizeof(float), vertex_data);

	// Index data
	primitive.draw_call.index_count = ring_count * sector_count * 6;
	uint16_t index_data[ring_count * sector_count * 6];
	int index_idx = 0;

	for(int r = 0; r < ring_count; r++)
	{
		for(int s = 0; s < sector_count; s++) 
		{
			index_data[index_idx++] = (uint16_t)(r * sector_count + s);
			index_data[index_idx++] = (uint16_t)((r + 1) * sector_count + s + 1);
			index_data[index_idx++] = (uint16_t)((r + 1) * sector_count + s);
			
			index_data[index_idx++] = (uint16_t)(r * sector_count + s);
			index_data[index_idx++] = (uint16_t)(r * sector_count + s + 1);
			index_data[index_idx++] = (uint16_t)((r + 1) * sector_count + s + 1);
		}
	}
	primitive.draw_call.index_buffer = _render_device->CreateIndexBuffer(primitive.draw_call.index_count, index_data);
	
	primitive.draw_call.vertex_format = vertex_format::VF_POSITION3F;

	return primitive;
}
void PrimitiveFactory::DestroyPrimitive(Primitive& primitive)
{
	// Delete the buffers that the primitive holds

	_render_device->ReleaseHardwareBuffer(primitive.draw_call.vertex_buffer);
}
