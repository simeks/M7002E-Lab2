#include <framework/Common.h>
#include "App.h"
#include "Scene.h"

#include <framework/RenderDevice.h>

#define SCENE_FILE_NAME "scene.json"

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
	#define MAX_LIGHT_COUNT 16 \n\
	varying vec3 normal_view; /* Normal in view-space */ \
	varying vec3 position_view; /* Vertex position in view space */ \
	uniform mat4 model_view_matrix; \
	uniform mat4 view_matrix; \
	\
	/* Light uniforms */ \
	struct Light \
	{ \
		vec4 ambient; \
		vec4 diffuse; \
		vec4 specular; \
		vec3 position; \
		float radius; \
	}; \
	\
	uniform Light lights[MAX_LIGHT_COUNT]; \
	\
	/* Material uniforms */ \
	uniform struct \
	{ \
		vec4 ambient;\
		vec4 diffuse;\
		vec4 specular;\
		\
	} material; \
	\
	void main() \
	{ \
		float specular_power = 16.0; \
		vec3 v = normalize(-position_view); /* Direction to the camera (The camera is at (0,0,0) as we calculate in view-space) */ \
		\
		vec4 light_accumulation = material.ambient; \
		for(int i = 0; i < MAX_LIGHT_COUNT; ++i) \
		{ \
			vec4 ambient_term = lights[i].ambient; \
			vec4 diffuse_term = material.diffuse * lights[i].diffuse; \
			vec4 specular_term = material.specular * lights[i].specular; \
			\
			/* Calculate and transform light direction into eye-space as all light calculations are done in view-space */ \
			vec3 light_dir = (view_matrix * vec4(lights[i].position, 1.0)).xyz - position_view; \
			\
			/* Distance to the light */ \
			float distance = length(light_dir); \
			\
			/* Omni-light attenuation [Real-Time Rendering, 7.4.1, page 218]  */\
			float att = max(1.0 - (distance / lights[i].radius), 0.0); \
			\
			/* Diffuse and specular lighting [Real-Time Rendering, 5.5, page 110] */ \
			\
			vec3 h = normalize(v + light_dir); \
			float cosTh = max(dot(normal_view, h), 0.0); \
			float cosTi = max(dot(normal_view, light_dir), 0.0); \
			\
			light_accumulation += att * ((diffuse_term + specular_term * pow(cosTh, specular_power)) * cosTi + ambient_term);  \
		} \
		\
		gl_FragColor = light_accumulation;  \
	}";


Lab2App::Lab2App() : _primitive_factory(NULL), _camera_angle(0.0f)
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
	default_material.ambient = Color(0.0f, 0.0f, 0.0f, 1.0f);
	_scene = new Scene(default_material, _primitive_factory);

	// Try loading a scene
	if(!_scene->LoadScene(SCENE_FILE_NAME))
	{
		// If no scene was loaded setup a small test scene
		_scene->CreateEntity(Entity::ET_PYRAMID)->position = Vec3(1.0f, 0.0f, 0.5f);
		_scene->CreateEntity(Entity::ET_CUBE)->position = Vec3(-1.0f, 0.0f, 0.5f);
		_scene->CreateEntity(Entity::ET_SPHERE)->position = Vec3(0.0f, 0.0f, -0.5f);

		Light* light = (Light*)_scene->CreateEntity(Entity::ET_LIGHT);

		light->ambient = Color(0.0f, 0.0f, 0.0f, 1.0f);
		light->specular = Color(0.25f, 0.25f, 0.25f, 1.0f);
		light->diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);
		light->position = Vec3(0.0f, 0.0f, 0.0f);
		light->radius = 15.0f;

		_scene->SaveScene(SCENE_FILE_NAME);
	}

	return true;
}
void Lab2App::Shutdown()
{
	// Save scene before shutting down.
	_scene->SaveScene(SCENE_FILE_NAME);

	_render_device->ReleaseShader(_default_shader);
	_default_shader = -1;

	// Cleanup
	delete _scene;
	_scene = NULL;
	delete _primitive_factory;
	_primitive_factory = NULL;

	ShutdownSDL();
}
void Lab2App::Render(float )
{
	_camera.position = Vec3(35.0f*sinf(_camera_angle), 15.0f, 35.0f*cosf(_camera_angle));
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
					_selection.entity->material.ambient = _selection.previous_color;
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
				_selection.previous_color = _selection.entity->material.ambient;
				// Change the color of the entity to mark it as selected.
				_selection.entity->material.ambient = Color(0.75f, 0.0f, 0.0f, 1.0f); 

				Vec3 world_position = _scene->ToWorld(mouse_position, _camera);
				_selection.position = world_position;
				_selection.offset = vector::Subtract(_selection.entity->position, world_position);
			}
			else if(_selection.entity)
			{
				_selection.mode = Selection::IDLE;
					_selection.entity->material.ambient = _selection.previous_color;
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
			if(key_states[SDL_SCANCODE_C])
			{
				_camera_angle += evt->motion.xrel * 0.01f;
			}
			else if(_selection.entity)
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

					if(_selection.entity->type == Entity::ET_LIGHT)
					{
						// If the scaled entity is a light, scale its light radius rather than the object size
						Light* light = (Light*)_selection.entity;
						light->radius = distance;
					}
					else
					{
						_selection.entity->scale = Vec3(distance, distance, distance);
					}
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
			case SDL_SCANCODE_4:
				{
					Entity* entity = _scene->CreateEntity(Entity::ET_LIGHT);
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
