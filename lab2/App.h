#ifndef __LAB2_APP_H__
#define __LAB2_APP_H__

#include <framework/App.h>

#include "MatrixStack.h"

struct Viewport
{
	int x, y;
	int width, height;
};

struct Camera
{
	Mat4x4 view_matrix;
	Mat4x4 projection_matrix;
};

class Scene;
class PrimitiveFactory;
class Lab2App : public App
{
public:
	Lab2App();
	~Lab2App();


protected:
	bool Initialize();
	void Shutdown();

	/// @brief Callback invoked once every frame to let the application perform rendering.
	void Render(float dtime);
	
private:
	MatrixStack _matrix_stack;
	Viewport _viewport;
	Camera _camera;

	PrimitiveFactory* _primitive_factory;
	Scene* _scene;

	int _default_shader;
};


#endif // __LAB2_APP_H__
