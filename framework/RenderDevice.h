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

private:
	std::vector<GLuint> _vertex_buffers;

};

#endif // __RENDERDEVICE_H__
