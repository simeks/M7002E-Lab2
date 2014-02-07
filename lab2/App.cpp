#include <framework/Common.h>
#include "App.h"
#include "Scene.h"

#include <framework/RenderDevice.h>


static const char* vertex_shader_src = " \
	uniform mat4 model_view_projection_matrix; \
	in vec3 position; \
	void main() \
	{ \
		gl_Position = model_view_projection_matrix * vec4(position, 1.0f); \
	}";

static const char* fragment_shader_src = " \
	void main() \
	{ \
		gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); \
	}";


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

	// Camera setup
	// Set the perspective, 45 degrees FOV, aspect ratio to match viewport, z range: [1.0, 1000.0]
	_camera.projection_matrix = matrix::CreatePerspective(45.0f*(float)MATH_PI/180.0f, (float)win_width/(float)win_height, 1.0f, 1000.0f);
	
	// Set the camera position to (0, 0, 400) and make it look at the center of the scene (0, 0, 0)
	_camera.view_matrix = matrix::LookAt(Vec3(5.0f, 5.0f, 5.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));

	_primitive_factory = new PrimitiveFactory(_render_device);
	_scene = new Scene(_primitive_factory);

	_default_shader = _render_device->CreateShader(vertex_shader_src, fragment_shader_src);

	Material default_material;
	default_material.shader = _default_shader;
	_scene->SetMaterialTemplate(default_material);
	
	_scene->CreateEntity(Entity::ET_PYRAMID);

	return true;
}
void Lab2App::Shutdown()
{
	_render_device->ReleaseShader(_default_shader);
	_default_shader = -1;

	// Cleanup
	delete _scene;
	_scene = NULL;
	delete _primitive_factory;
	_primitive_factory = NULL;

	ShutdownSDL();
}
void Lab2App::Render(float)
{
	_matrix_stack.Push();

	// Setup camera transforms
	_matrix_stack.SetProjectionMatrix(_camera.projection_matrix);
	_matrix_stack.SetViewMatrix(_camera.view_matrix);

	_scene->Render(*_render_device, _matrix_stack);
	
	_matrix_stack.Pop();
}
