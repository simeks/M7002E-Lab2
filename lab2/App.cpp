#include <framework/Common.h>
#include "App.h"

#include <framework/RenderDevice.h>

Lab2App::Lab2App() : _primitive_factory(NULL)
{
}
Lab2App::~Lab2App()
{
}

bool Lab2App::Initialize()
{
	uint32_t win_width = 1024, win_height = 768;

	// Initialize SDL and create a 1024x768 winodw for rendering.
	if(!InitializeSDL(win_width, win_height))
		return false;

	SetWindowTitle("OpenGL - Lab 2");
	_render_device->SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_CULL_FACE); // Enable face culling
	glEnable(GL_DEPTH_TEST); // Enable depth testing

	_viewport.x = _viewport.y = 0;
	_viewport.width = win_width;
	_viewport.height = win_height;
	
	_primitive_factory = new PrimitiveFactory(_render_device);

	return true;
}
void Lab2App::Shutdown()
{
	// Cleanup
	delete _primitive_factory;
	_primitive_factory = NULL;

	ShutdownSDL();
}
void Lab2App::Render(float)
{
	// Draw the scene
	RenderScene();


}
void Lab2App::RenderScene()
{
	// Render all the primitives in the scene.

}
