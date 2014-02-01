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

int RenderDevice::CreateShader(const char* vertex_shader_src, const char* fragment_shader_src)
{
	Shader shader;
	shader.vertex_shader = 0;
	shader.fragment_shader = 0;

	shader.program = glCreateProgram();
	
	// Vertex shader
	{
		shader.vertex_shader = glCreateShader(GL_VERTEX_SHADER);

		// Load shader source into shader
		glShaderSource(shader.vertex_shader, 1, &vertex_shader_src, NULL); 
		// Compile shader
		glCompileShader(shader.vertex_shader);

		// Check if the shader compiled successfuly
		int param = -1;
		glGetShaderiv(shader.vertex_shader, GL_COMPILE_STATUS, &param);
		if(param != GL_TRUE)
		{
			debug::Printf("RenderDevice: Failed to compile vertex shader %u.\n", shader.vertex_shader);
			PrintShaderInfoLog(shader.vertex_shader);
			return -1;
		}

		// Attach the vertex shader to the shader program
		glAttachShader(shader.program, shader.vertex_shader);
	}
	// Fragment shader
	{
		shader.fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

		// Load shader source into shader
		glShaderSource(shader.fragment_shader, 1, &fragment_shader_src, NULL); 
		// Compile shader
		glCompileShader(shader.fragment_shader);

		// Check if the shader compiled successfuly
		int param = -1;
		glGetShaderiv(shader.fragment_shader, GL_COMPILE_STATUS, &param);
		if(param != GL_TRUE)
		{
			debug::Printf("RenderDevice: Failed to compile vertex shader %u.\n", shader.fragment_shader);
			PrintShaderInfoLog(shader.fragment_shader);
			return -1;
		}

		// Attach the vertex shader to the shader program
		glAttachShader(shader.program, shader.fragment_shader);
	}

	glLinkProgram(shader.program);

	_shaders.push_back(shader);
	return _shaders.size() - 1;
}
void RenderDevice::ReleaseShader(int shader_handle)
{
	assert(shader_handle >= 0 && shader_handle < _shaders.size());

	Shader& shader = _shaders[shader_handle];
	
	if(shader.vertex_shader != 0)
		glDeleteShader(shader.vertex_shader);
	if(shader.fragment_shader != 0)
		glDeleteShader(shader.fragment_shader);
	
	glDeleteProgram(shader.program);

	_shaders.erase(_shaders.begin() + shader_handle);
}
void RenderDevice::PrintShaderInfoLog(GLuint shader)
{
	char info_log[2048];
	int length = 0;

	// Get the info log for the specified shader
	glGetShaderInfoLog(shader, 2048, &length, info_log);

	debug::Printf("%s\n", info_log);
}

