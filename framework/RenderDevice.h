#ifndef __RENDERDEVICE_H__
#define __RENDERDEVICE_H__

namespace vertex_format
{
	enum VertexFormat
	{
		VF_POSITION3F // Each vertex holds only a position: x, y, z
	};
};

struct DrawCall
{
	GLenum draw_mode; // Specifies draw mode, e.g. GL_POINTS, GL_TRIANGLES, etc.

	int vertex_buffer;
	int vertex_offset;
	int vertex_count;
	
	vertex_format::VertexFormat vertex_format; // Specifies the vertex format in the vertex buffer. See enum VertexFormat

};

/// @brief Render device handling low-level opengl calls.
class RenderDevice
{
public:
	RenderDevice();
	~RenderDevice();

	/// @brief Initializes the render device.
	/// @return True if the initialization was successful, false if it failed.
	bool Initialize();

	/// @brief Shuts down the render device, performing any necessary clean up.
	void Shutdown();
	
	/// @brief Binds the specified shader program to the pipeline.
	/// @param shader_handle Specify shader to bind, setting this to -1 will unbind any currently bound shader.
	void BindShader(int shader_handle);

	/// @brief Specifies the value of a uniform variable.
	/// @param name Name of the uniform variable.
	/// @param value Specifies the new value.
	void SetUniform4f(const char* name, const Vec4& value);

	/// @brief Specifies the value of a uniform variable.
	/// @param name Name of the uniform variable.
	/// @param value Specifies the new value.
	void SetUniformMatrix4f(const char* name, const Mat4x4& value);

	/// @param draw_mode Specifies what kind of primitives to render.
	void Draw(const DrawCall& draw_call);

	/// @brief Specifies the clear color for when clearing the back buffer. 
	void SetClearColor(float r, float g, float b, float a);

	/// @brief Creates a new vertex buffer.
	/// @param size The total size of the buffer in bytes.
	/// @param vertex_data A pointer to the data that should be copied to the buffer.
	///						NULL means the buffer will be empty.
	/// @return Handle to the new vertex buffer.
	/// @sa ReleaseVertexBuffer
	int CreateVertexBuffer(uint32_t size, void* vertex_data);

	/// @brief Releases the specified vertex buffer.
	/// @param vertex_buffer Handle to the vertex buffer.
	/// @sa CreateVertexBuffer
	void ReleaseVertexBuffer(int vertex_buffer);

	/// @brief Creates a new shader program consisting of a vertex shader and a fragment shader.
	/// @param vertex_shader_src String containing the GLSL source code for the vertex shader.
	/// @param fragment_shader_src String containing the GLSL source code for the fragment shader.
	/// @return Returns a handle to the shader if shader was created successful, returns -1 if it failed.
	/// @sa ReleaseShader
	int CreateShader(const char* vertex_shader_src, const char* fragment_shader_src);

	/// @brief Releases a shader that have been created with CreateShader.
	/// @sa CreateShader
	void ReleaseShader(int shader_handle);

private:
	/// @brief Prints the shader info log for the specified shader.
	void PrintShaderInfoLog(GLuint shader);
	
private:
	struct Shader
	{
		GLuint vertex_shader;
		GLuint fragment_shader;

		GLuint program; // Shader program that combines all our shaders above (vertex shader, fragment shader)

	};

	std::map<int, GLuint> _vertex_buffers;
	int _next_buffer_id;

	std::map<int, Shader> _shaders;
	int _next_shader_id;

	int _current_shader; // Id of the currently bound shader, -1 means no shader is bound.
};

#endif // __RENDERDEVICE_H__
