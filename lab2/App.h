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
	Mat4x4 projection_matrix;
	Vec3 position;
	Vec3 direction; // Which direction the camera is looking
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

	void OnEvent(SDL_Event* evt);
	
private:
	struct Selection
	{
		enum Mode
		{
			IDLE,
			MOVE,
			SCALE,
			ROTATE
		};

		Entity* entity; // Selected entity, NULL if none is selected
		Mode mode;

		Selection() : entity(NULL), mode(IDLE) {}
	};

	MatrixStack _matrix_stack;
	Viewport _viewport;
	Camera _camera;

	PrimitiveFactory* _primitive_factory;
	Scene* _scene;

	int _default_shader;

	Selection _current_selection;
};


#endif // __LAB2_APP_H__
