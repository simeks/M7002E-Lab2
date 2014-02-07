#ifndef __MATRIXSTACK_H__
#define __MATRIXSTACK_H__

#include <stack>

class RenderDevice;
class MatrixStack
{
public:
	MatrixStack();
	~MatrixStack();

	/// Pushes 
	void Push();
	void Pop();
	
	void SetViewMatrix(const Mat4x4& view_matrix);
	void SetProjectionMatrix(const Mat4x4& projection_matrix);

	void Translate3f(const Vec3& translation);
	void Rotate3f(float head, float pitch, float roll);
	void Scale3f(const Vec3& scale);

	/// Applies the current matrices to the pipeline, this assumes that a shader with 
	///		the appropriate uniforms is bound.
	void Apply(RenderDevice& render_device);

private:
	struct State
	{
		Mat4x4 model_matrix;
		Mat4x4 view_matrix;
		Mat4x4 projection_matrix;
	};

	std::stack<State> _states;
	bool _state_dirty;
};

#endif // __MATRIXSTACK_H__
