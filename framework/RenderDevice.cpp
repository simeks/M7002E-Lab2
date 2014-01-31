#include "Common.h"

#include "RenderDevice.h"


RenderDevice::RenderDevice()
{
}
RenderDevice::~RenderDevice()
{
}
bool RenderDevice::Initialize()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set the clear color to black

#ifndef PLATFORM_MACOSX // There's no real use for GLEW on OSX so we skip it.
	// Initialize glew, which handles opengl extensions
	GLenum err = glewInit(); 
	if(err != GLEW_OK)
	{
		debug::Printf("[Error] glewInit failed: %s\n", glewGetErrorString(err));
		return false;
	}
#endif

	const GLubyte *version = glGetString(GL_VERSION);
	debug::Printf("OpenGL Version: %s\n", version);

	return true;
}
void RenderDevice::Shutdown()
{
	// Release any buffers that are still allocated
	for(std::vector<GLuint>::iterator it = _vertex_buffers.begin();
		it != _vertex_buffers.end(); ++it)
	{
		glDeleteBuffers(1, &(*it));
	}
	_vertex_buffers.clear();

}

void RenderDevice::Draw(int )
{

}

void RenderDevice::SetClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

int RenderDevice::CreateVertexBuffer(uint32_t size, void* vertex_data)
{
	// Vertex buffer objects in opengl are objects that allows us to upload data directly to the GPU.
	//	This means that opengl doesn't need to upload the data everytime we render something. As with 
	//	display lists or glBegin()...glEnd().

	GLuint buffer; // The resulting buffer name will be stored here.

	// Generate a name for our new buffer.
	glGenBuffers(1, &buffer);

	// Bind the buffer, this will also perform the actual creation of the buffer.
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// Upload the data to the buffer.
	glBufferData(GL_ARRAY_BUFFER, 
				size, // The total size of the buffer
				vertex_data, // The data that should be uploaded
				GL_STATIC_DRAW // Specifies that the buffer should be static and it should be used for drawing.
				);

	_vertex_buffers.push_back(buffer);

	return _vertex_buffers.size() - 1;
}
void RenderDevice::ReleaseVertexBuffer(int vertex_buffer)
{
	assert(vertex_buffer >= 0 && (uint32_t)vertex_buffer < _vertex_buffers.size());

	// Delete the buffer
	glDeleteBuffers(1, &_vertex_buffers[vertex_buffer]);

	_vertex_buffers.erase(_vertex_buffers.begin() + vertex_buffer);

}
