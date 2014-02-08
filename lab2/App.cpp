#include <framework/Common.h>
#include "App.h"
#include "Scene.h"

#include <framework/RenderDevice.h>


static const char* vertex_shader_src = " \
	uniform mat4 model_view_projection_matrix; \
	uniform mat4 model_view_matrix; \
	\
	attribute vec3 vertex_position; \
	attribute vec3 vertex_normal; \
	\
	varying vec3 normal_view; /* Normal in view-space */ \
	varying vec3 position_view; /* Vertex position in view-space */ \
	\
	void main() \
	{ \
		gl_Position = model_view_projection_matrix * vec4(vertex_position, 1.0); \
		\
		/* Transform normals into view-space */ \
		normal_view = transpose(inverse(model_view_matrix)) * vec4(normalize(vertex_normal), 0.0); \
		normal_view = normalize(normal_view); \
		position_view = model_view_matrix * vec4(vertex_position, 1.0); \
	}";

static const char* fragment_shader_src = " \
	varying vec3 normal_view; /* Normal in view-space */ \
	varying vec3 position_view; /* Vertex position in view space */ \
	uniform mat4 model_view_matrix; \
	uniform mat4 view_matrix; \
	\
	/* Light uniforms */ \
	uniform vec3 light_direction;\
	uniform vec3 light_ambient;\
	uniform vec3 light_diffuse;\
	uniform vec3 light_specular;\
	\
	/* Material uniforms */ \
	uniform vec3 material_ambient;\
	uniform vec3 material_diffuse;\
	uniform vec3 material_specular;\
	\
	\
	void main() \
	{ \
		float specular_power = 16.0; \
		/* Transform light direction into eye-space as all light calculations are done in view-space */ \
		vec3 L = (view_matrix * vec4(light_direction, 0.0)).xyz; \
		vec3 v = normalize(-position_view); /* Direction to the camera (The camera is at (0,0,0) as we calculate in view-space) */ \
		\
		/* Diffuse and specular lighting, Real-Time Rendering, 5.5, page 110 */ \
		\
		vec3 h = normalize(v + L); \
		float cosTh = max(dot(normal_view, h), 0.0); \
		float cosTi = max(dot(normal_view, L), 0.0); \
		\
		vec3 ambient_term = material_ambient + light_ambient; \
		vec3 diffuse_term = material_diffuse * light_diffuse; \
		vec3 specular_term = material_specular * light_specular; \
		gl_FragColor = vec4((diffuse_term + specular_term * pow(cosTh, specular_power)) * cosTi + ambient_term, 1.0); \
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
	_camera.projection_matrix = matrix::CreatePerspective(30.0f*(float)MATH_PI/180.0f, (float)win_width/(float)win_height, 1.0f, 1000.0f);
	_camera.position = Vec3(0.0f, 0.0f, 0.0f);
	_camera.direction = Vec3(0.0f, 0.0f, -1.0f);

	_primitive_factory = new PrimitiveFactory(_render_device);
	
	_default_shader = _render_device->CreateShader(vertex_shader_src, fragment_shader_src);

	Material default_material;
	default_material.shader = _default_shader;
	default_material.diffuse = Color(0.0f, 0.0f, 1.0f, 1.0f);
	default_material.specular = Color(0.5f, 0.5f, 0.5f, 1.0f);
	_scene = new Scene(default_material, _primitive_factory);

	_scene->CreateEntity(Entity::ET_PYRAMID)->position = Vec3(1.0f, 0.0f, 0.5f);
	_scene->CreateEntity(Entity::ET_CUBE)->position = Vec3(-1.0f, 0.0f, 0.5f);
	_scene->CreateEntity(Entity::ET_SPHERE)->position = Vec3(0.0f, 0.0f, -0.5f);

	_scene->GetLight().ambient = Color(0.01f, 0.01f, 0.01f, 1.0f);
	_scene->GetLight().specular = Color(1.0f, 1.0f, 1.0f, 1.0f);
	_scene->GetLight().direction = Vec3(0.0f, 1.0f, 0.5f);

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
void Lab2App::Render(float a)
{
	static float camera_angle = 0.0f;
	camera_angle = camera_angle + 0.0f * a;
	
	_camera.position = Vec3(20.0f*sinf(camera_angle), 10.0f, 20.0f*cosf(camera_angle));
	_camera.position = Vec3(0.0f, 15.0f, 35.0f);
	_camera.direction = vector::Subtract(Vec3(0.0f, 0.0f, 0.0f), _camera.position);
	vector::Normalize(_camera.direction);

	_matrix_stack.Push();

	// Setup camera transforms
	_matrix_stack.SetProjectionMatrix(_camera.projection_matrix);
	_matrix_stack.SetViewMatrix(matrix::LookAt(_camera.position, vector::Add(_camera.position, _camera.direction), Vec3(0.0f, 1.0f, 0.0f)));

	_scene->Render(*_render_device, _matrix_stack);
	
	_matrix_stack.Pop();
}

void Lab2App::OnEvent(SDL_Event* evt)
{
	const Uint8* key_states = SDL_GetKeyboardState(NULL);

	switch(evt->type)
	{
	case SDL_MOUSEBUTTONDOWN:
		{
			// Normalize mouse position ([-1.0, 1.0])
			Vec2 mouse_position = Vec2(	2.0f * (float)evt->button.x / (float)_viewport.width - 1.0f,
										1.0f - (2.0f * (float)evt->button.y / (float)_viewport.height)); // Flip y-axis as OpenGL has y=0 at the bottom.

			Entity* entity = _scene->SelectEntity(mouse_position, _camera);
			if(entity)
			{
				// Unselect any previous selection first
				if(_selection.entity)
				{
					_selection.entity->material.ambient = Color(0.0f, 0.0f, 0.0f, 1.0f);
					_selection.entity = NULL;
				}

				_selection.entity = entity;
				if(key_states[SDL_SCANCODE_S]) // 's' => Scaling
				{
					_selection.mode = Selection::SCALE;
				}
				else if(key_states[SDL_SCANCODE_R]) // 'r' => Rotate
				{
					_selection.mode = Selection::ROTATE;
				}
				else
				{
					_selection.mode = Selection::MOVE;
				}
				_selection.entity->material.ambient = Color(1.0f, 0.0f, 0.0f, 1.0f);

				Vec3 world_position = _scene->ToWorld(mouse_position, _camera);
				_selection.position = world_position;
				_selection.offset = vector::Subtract(_selection.entity->position, world_position);
			}
			else if(_selection.entity)
			{
				_selection.mode = Selection::IDLE;
				_selection.entity->material.ambient = Color(0.0f, 0.0f, 0.0f, 1.0f);
				_selection.entity = NULL;
			}
		}
		break;
	case SDL_MOUSEBUTTONUP:
		{
			if(_selection.entity)
			{
				_selection.mode = Selection::IDLE;
			}
		}
		break;
	case SDL_MOUSEMOTION:
		{
			if(_selection.entity)
			{
				if(_selection.mode == Selection::MOVE)
				{
					// Normalize mouse position ([-1.0, 1.0])
					Vec2 mouse_position = Vec2(	2.0f * (float)evt->button.x / (float)_viewport.width - 1.0f,
												1.0f - (2.0f * (float)evt->button.y / (float)_viewport.height)); // Flip y-axis as OpenGL has y=0 at the bottom.

					Vec3 world_position = _scene->ToWorld(mouse_position, _camera);
					if(key_states[SDL_SCANCODE_LCTRL])
					{
						_selection.entity->position.y = -vector::Subtract(world_position, _selection.position).z;
					}
					else
					{
						_selection.entity->position = vector::Add(world_position, _selection.offset);
					}
				}
				else if(_selection.mode == Selection::SCALE)
				{
					// Normalize mouse position ([-1.0, 1.0])
					Vec2 mouse_position = Vec2(	2.0f * (float)evt->button.x / (float)_viewport.width - 1.0f,
												1.0f - (2.0f * (float)evt->button.y / (float)_viewport.height)); // Flip y-axis as OpenGL has y=0 at the bottom.

					Vec3 world_position = _scene->ToWorld(mouse_position, _camera);

					float distance = vector::Length(vector::Subtract(world_position, _selection.entity->position)) / vector::Length(_selection.offset);
					_selection.entity->scale = Vec3(distance, distance, distance);
				}
				else if(_selection.mode == Selection::ROTATE)
				{
					// Normalize mouse position ([-1.0, 1.0])
					Vec2 mouse_position = Vec2(	2.0f * (float)evt->button.x / (float)_viewport.width - 1.0f,
												1.0f - (2.0f * (float)evt->button.y / (float)_viewport.height)); // Flip y-axis as OpenGL has y=0 at the bottom.

					Vec3 world_position = _scene->ToWorld(mouse_position, _camera);
					Vec3 delta = vector::Subtract(world_position, _selection.entity->position);
					_selection.entity->rotation.x = delta.x;
					_selection.entity->rotation.y = delta.z;

				}
			}
		}
		break;
	case SDL_KEYDOWN:
		{
			// Normalize mouse position ([-1.0, 1.0])
			int mouse_x, mouse_y;
			SDL_GetMouseState(&mouse_x, &mouse_y);
			Vec2 mouse_position = Vec2(	2.0f * (float)mouse_x / (float)_viewport.width - 1.0f,
										1.0f - (2.0f * (float)mouse_y / (float)_viewport.height)); // Flip y-axis as OpenGL has y=0 at the bottom.

			Vec3 world_position = _scene->ToWorld(mouse_position, _camera);

			switch(evt->key.keysym.scancode)
			{
			case SDL_SCANCODE_ESCAPE:
				{
					Stop();
				}
				break;
			case SDL_SCANCODE_1:
				{
					Entity* entity = _scene->CreateEntity(Entity::ET_PYRAMID);
					entity->position = world_position;
				}
				break;
			case SDL_SCANCODE_2:
				{
					Entity* entity = _scene->CreateEntity(Entity::ET_CUBE);
					entity->position = world_position;
				}
				break;
			case SDL_SCANCODE_3:
				{
					Entity* entity = _scene->CreateEntity(Entity::ET_SPHERE);
					entity->position = world_position;
				}
				break;
			case SDL_SCANCODE_DELETE:
				{
					// [Ctrl] + [Delete] => Delete all entities
					if(key_states[SDL_SCANCODE_LCTRL])
					{
						_scene->DestroyAllEntities();
					}
					if(_selection.entity)
					{
						_scene->DestroyEntity(_selection.entity);

						_selection.mode = Selection::IDLE;
						_selection.entity = NULL;
					}
				}
				break;
			};
		}
		break;
	};
}
