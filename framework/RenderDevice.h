#ifndef __RENDERDEVICE_H__
#define __RENDERDEVICE_H__


/// @brief Render device handling low-level opengl calls.
class RenderDevice
{
public:
	RenderDevice();
	~RenderDevice();

	bool Initialize();
	void Shutdown();
	
	void Draw(int vertex_buffer);

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

	std::vector<GLuint> _vertex_buffers;
	std::vector<Shader> _shaders;

};

#endif // __RENDERDEVICE_H__
