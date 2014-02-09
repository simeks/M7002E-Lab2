#include "Common.h"

#include "RenderDevice.h"


RenderDevice::RenderDevice()
	: _next_buffer_id(0),
	_next_shader_id(0),
	_current_shader(-1)
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
	for(std::map<int, GLuint>::iterator it = _hardware_buffers.begin();
		it != _hardware_buffers.end(); ++it)
	{
		glDeleteBuffers(1, &it->second);
	}
	_hardware_buffers.clear();

	// Release any remaining shaders
	for(std::map<int, Shader>::iterator it = _shaders.begin(); 
		it != _shaders.end(); ++it)
	{
		if(it->second.vertex_shader != 0)
			glDeleteShader(it->second.vertex_shader);
		if(it->second.fragment_shader != 0)
			glDeleteShader(it->second.fragment_shader);
	
		glDeleteProgram(it->second.program);
	}
	_shaders.clear();
}
void RenderDevice::BindShader(int shader_handle)
{
	if(shader_handle >= 0)
	{
		std::map<int, Shader>::iterator it = _shaders.find(shader_handle);
		assert(it != _shaders.end());

		glUseProgram(it->second.program);

		_current_shader = shader_handle;
	}
	else
	{
		// Unbind current program
		glUseProgram(0);

		_current_shader = -1; // Setting the current shader to -1 indicates that no shader is bound.
	}
}

void RenderDevice::SetUniform4f(const char* name, const Vec4& value)
{
	if(_current_shader == -1) // Nothing to do if no shader is bound.
	{
		debug::Printf("RenderDevice: Failed setting uniform value; no shader bound.\n");
		return;
	}

	std::map<int, Shader>::iterator it = _shaders.find(_current_shader);
	assert(it != _shaders.end());

	// Find the location of the variable with the specified name
	GLint location = glGetUniformLocation(it->second.program, name);
	if(location == -1)
	{
		debug::Printf("RenderDevice: No uniform variable with the name '%s' found.\n", name);
		return;
	}

	// Set the value at the found location
	glUniform4f(location, value.x, value.y, value.z, value.w);
}
void RenderDevice::SetUniform3f(const char* name, const Vec3& value)
{
	if(_current_shader == -1) // Nothing to do if no shader is bound.
	{
		debug::Printf("RenderDevice: Failed setting uniform value; no shader bound.\n");
		return;
	}

	std::map<int, Shader>::iterator it = _shaders.find(_current_shader);
	assert(it != _shaders.end());

	// Find the location of the variable with the specified name
	GLint location = glGetUniformLocation(it->second.program, name);
	if(location == -1)
	{
		debug::Printf("RenderDevice: No uniform variable with the name '%s' found.\n", name);
		return;
	}

	// Set the value at the found location
	glUniform3f(location, value.x, value.y, value.z);
}
void RenderDevice::SetUniform1f(const char* name, float value)
{
	if(_current_shader == -1) // Nothing to do if no shader is bound.
	{
		debug::Printf("RenderDevice: Failed setting uniform value; no shader bound.\n");
		return;
	}

	std::map<int, Shader>::iterator it = _shaders.find(_current_shader);
	assert(it != _shaders.end());

	// Find the location of the variable with the specified name
	GLint location = glGetUniformLocation(it->second.program, name);
	if(location == -1)
	{
		debug::Printf("RenderDevice: No uniform variable with the name '%s' found.\n", name);
		return;
	}

	// Set the value at the found location
	glUniform1f(location, value);
}
void RenderDevice::SetUniformMatrix4f(const char* name, const Mat4x4& value)
{
	if(_current_shader == -1) // Nothing to do if no shader is bound.
	{
		debug::Printf("RenderDevice: Failed setting uniform value; no shader bound.\n");
		return;
	}

	std::map<int, Shader>::iterator it = _shaders.find(_current_shader);
	assert(it != _shaders.end());

	// Find the location of the variable with the specified name
	GLint location = glGetUniformLocation(it->second.program, name);
	if(location == -1)
	{
		debug::Printf("RenderDevice: No uniform variable with the name '%s' found.\n", name);
		return;
	}

	// Set the value at the found location
	glUniformMatrix4fv(location, 1, false, (float*)&value);
}

void RenderDevice::Draw(const DrawCall& draw_call)
{
	// Binds the vertex/index buffers for use.
	{
		assert(draw_call.vertex_buffer != -1); // User needs to specify a vertex buffer.

		std::map<int, GLuint>::iterator it = _hardware_buffers.find(draw_call.vertex_buffer);
		assert(it != _hardware_buffers.end());

		glBindBuffer(GL_ARRAY_BUFFER, it->second); 

		// Bind index buffer if specified
		if(draw_call.index_buffer != -1)
		{
			std::map<int, GLuint>::iterator ib_it = _hardware_buffers.find(draw_call.index_buffer);
			assert(ib_it != _hardware_buffers.end());

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib_it->second);
		}
	}

	// Bind vertex attributes depending on the specified vertex format.
	switch(draw_call.vertex_format)
	{
	case vertex_format::VF_POSITION3F:
		{
			// Specifies the location and format of the position data.
			glVertexAttribPointer(	0, // Attribute index 0
									3, // 3 floats (x, y, z)
									GL_FLOAT, // Format,
									GL_FALSE, // Data should not be normalized
									0, // Buffer only contains positions so no need to specify stride.
									0 
								); 

			glEnableVertexAttribArray(0);
		}
		break;
	case vertex_format::VF_POSITION3F_NORMAL3F:
		{
			// Specifies the location and format of the position data.
			glVertexAttribPointer(	0, // Attribute index 0
									3, // 3 floats (Px, Py, Pz)
									GL_FLOAT, // Format,
									GL_FALSE, // Data should not be normalized
									sizeof(float)*6, 
									0 
								); 
			glEnableVertexAttribArray(0);

			// Specifies the location and format of the normal data.
			glVertexAttribPointer(	1, // Attribute index 1
									3, // 3 floats (Nx, Ny, Nz)
									GL_FLOAT, // Format,
									GL_FALSE, // Data should not be normalized
									sizeof(float)*6, 
									(void*)(sizeof(float)*3)
								); 
			glEnableVertexAttribArray(1);
		}
		break;
	};
	
	// Perform the actual draw call.
	if(draw_call.index_buffer != -1)
	{
		// Draw with index buffer
		glDrawElements(draw_call.draw_mode, draw_call.index_count, GL_UNSIGNED_SHORT, 0);
	}
	else
	{
		// Draw without index buffer
		glDrawArrays(draw_call.draw_mode, draw_call.vertex_offset, draw_call.vertex_count);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
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

	_hardware_buffers[_next_buffer_id++] = buffer;

	return _next_buffer_id - 1;
}
int RenderDevice::CreateIndexBuffer(uint32_t index_count, uint16_t* index_data)
{
	GLuint buffer; // The resulting buffer name will be stored here.

	// Generate a name for our new buffer.
	glGenBuffers(1, &buffer);

	// Bind the buffer, this will also perform the actual creation of the buffer.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);

	// Upload the data to the buffer.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
				index_count * sizeof(uint16_t), // The total size of the buffer
				index_data, // The data that should be uploaded
				GL_STATIC_DRAW // Specifies that the buffer should be static and it should be used for drawing.
				);

	_hardware_buffers[_next_buffer_id++] = buffer;

	return _next_buffer_id - 1;
}
void RenderDevice::ReleaseHardwareBuffer(int buffer)
{
	std::map<int, GLuint>::iterator it = _hardware_buffers.find(buffer);
	assert(it != _hardware_buffers.end());
	
	// Delete the buffer
	glDeleteBuffers(1, &it->second);

	_hardware_buffers.erase(it);
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
			debug::Printf("RenderDevice: Failed to compile pixel shader %u.\n", shader.fragment_shader);
			PrintShaderInfoLog(shader.fragment_shader);
			return -1;
		}

		// Attach the vertex shader to the shader program
		glAttachShader(shader.program, shader.fragment_shader);
	}

	// Link shaders
	glLinkProgram(shader.program);
	
	// Check if linking was sucessful
	int param = -1;
	glGetProgramiv(shader.program, GL_LINK_STATUS, &param);
	if(param != GL_TRUE)
	{
		debug::Printf("RenderDevice: Failed to link program %u.\n", shader.program);
			
		char info_log[2048];
		int length = 0;

		// Get the info log for the program
		glGetProgramInfoLog(shader.program, 2048, &length, info_log);

		debug::Printf("%s\n", info_log);

		return -1;
	}
	
	_shaders[_next_shader_id++] = shader;

	return _next_shader_id - 1;
}
void RenderDevice::ReleaseShader(int shader_handle)
{
	std::map<int, Shader>::iterator it = _shaders.find(shader_handle);
	assert(it != _shaders.end());
	
	if(it->second.vertex_shader != 0)
		glDeleteShader(it->second.vertex_shader);
	if(it->second.fragment_shader != 0)
		glDeleteShader(it->second.fragment_shader);
	
	glDeleteProgram(it->second.program);

	_shaders.erase(it);
}
void RenderDevice::PrintShaderInfoLog(GLuint shader)
{
	char info_log[2048];
	int length = 0;

	// Get the info log for the specified shader
	glGetShaderInfoLog(shader, 2048, &length, info_log);

	debug::Printf("%s\n", info_log);
}

