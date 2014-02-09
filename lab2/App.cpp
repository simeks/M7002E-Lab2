#include <framework/Common.h>
#include "App.h"
#include "Scene.h"
#include "ColorPicker.h"

#include <framework/RenderDevice.h>
#include <framework/Ray.h>

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


Lab2App::Lab2App() : _primitive_factory(NULL), _color_picker(NULL), _camera_angle(0.0f)
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
	_render_device->SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);

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
		_scene->CreateEntity(Entity::ET_PYRAMID)->position = Vec3(2.5f, 0.0f, -2.5f);
		_scene->CreateEntity(Entity::ET_CUBE)->position = Vec3(-2.5f, 0.0f, -2.5f);
		_scene->CreateEntity(Entity::ET_SPHERE)->position = Vec3(0.0f, 0.0f, -3.0f);

		Light* light = (Light*)_scene->CreateEntity(Entity::ET_LIGHT);

		light->ambient = Color(0.0f, 0.0f, 0.0f, 1.0f);
		light->specular = Color(0.25f, 0.25f, 0.25f, 1.0f);
		light->diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);
		light->position = Vec3(0.0f, 0.0f, 2.0f);
		light->radius = 25.0f;

		_scene->SaveScene(SCENE_FILE_NAME);
	}

	_color_picker = new ColorPicker(_render_device, _viewport);

	return true;
}
void Lab2App::Shutdown()
{
	delete _color_picker;
	_color_picker = NULL;

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
	glViewport(_viewport.x, _viewport.y, _viewport.width, _viewport.height);

	_camera.position = Vec3(35.0f*sinf(_camera_angle), 15.0f, 35.0f*cosf(_camera_angle));
	_camera.direction = vector::Subtract(Vec3(0.0f, 0.0f, 0.0f), _camera.position);
	vector::Normalize(_camera.direction);

	_matrix_stack.Push();

	// Setup camera transforms
	_matrix_stack.SetProjectionMatrix(_camera.projection_matrix);
	_matrix_stack.SetViewMatrix(matrix::LookAt(_camera.position, vector::Add(_camera.position, _camera.direction), Vec3(0.0f, 1.0f, 0.0f)));

	_scene->Render(*_render_device, _matrix_stack);
	
	_matrix_stack.Pop();

	// Render UI
	_color_picker->Render();
}

void Lab2App::OnEvent(SDL_Event* evt)
{
	const Uint8* key_states = SDL_GetKeyboardState(NULL);

	switch(evt->type)
	{
	case SDL_MOUSEBUTTONDOWN:
		{
			// If the color picker is active, let it process mouse input.
			if(_color_picker->Visible() && _color_picker->OnMouseEvent(Vec2((float)evt->button.x, _viewport.height - (float)evt->button.y)))
				break;

			// Normalize mouse position ([-1.0, 1.0])
			Vec2 mouse_position = Vec2(	2.0f * (float)evt->button.x / (float)_viewport.width - 1.0f,
										1.0f - (2.0f * (float)evt->button.y / (float)_viewport.height)); // Flip y-axis as OpenGL has y=0 at the bottom.

			Entity* entity = _scene->SelectEntity(mouse_position, _camera);
			if(entity)
			{
				SelectEntity(entity, mouse_position);

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

			}
			else if(_selection.entity)
			{
				UnselectEntity();
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
			if(key_states[SDL_SCANCODE_V])
			{
				_camera_angle += evt->motion.xrel * 0.01f;
			}
			else if(_selection.entity)
			{
				// Normalize mouse position ([-1.0, 1.0])
				Vec2 mouse_position = Vec2(	2.0f * (float)evt->motion.x / (float)_viewport.width - 1.0f,
											1.0f - (2.0f * (float)evt->motion.y / (float)_viewport.height)); // Flip y-axis as OpenGL has y=0 at the bottom.

				if(_selection.mode == Selection::MOVE)
				{
					MoveEntity(_selection.entity, mouse_position, (key_states[SDL_SCANCODE_LCTRL] != 0), _selection.offset);
				}
				else if(_selection.mode == Selection::SCALE)
				{
					ScaleEntity(_selection.entity, mouse_position, (key_states[SDL_SCANCODE_LCTRL] != 0));
				}
				else if(_selection.mode == Selection::ROTATE)
				{
					RotateEntity(_selection.entity, mouse_position);
				}
				// Check if the user tries to drag the color sliders
				else if(_color_picker->Visible() && evt->motion.state & SDL_BUTTON(1))
				{
					// Flip height as SDL goes from top to bottom while the rest of our system goes bottom to top.
					_color_picker->OnMouseEvent(Vec2((float)evt->motion.x, _viewport.height - (float)evt->motion.y));
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

			Vec3 world_position = _scene->ToWorld(mouse_position, _camera, 0.0f);

			switch(evt->key.keysym.scancode)
			{
			case SDL_SCANCODE_ESCAPE:
				{
					Stop();
				}
				break;
			case SDL_SCANCODE_F1:
				{
					_scene->SaveScene(SCENE_FILE_NAME);
				}
				break;
			case SDL_SCANCODE_F2:
				{
					UnselectEntity(); // Unselect the current entity, as it may not exist in the loaded world.
					_scene->LoadScene(SCENE_FILE_NAME);
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

						// Disable the color picker if it happends to be enabled
						if(_color_picker->Visible())
						{
							_color_picker->Hide();
							_color_picker->SetTarget(NULL);
						}
					}
				}
				break;
			};
		}
		break;
	};
}
void Lab2App::MoveEntity(Entity* entity, const Vec2& mouse_position, bool y_axis, const Vec3& offset)
{
	if(y_axis)
	{
		// Move the y_axis (and any axis that is the cross product of the y_axis and the camera direction)
		//	We do this by casting a ray from the mouse position against a plane that is fixed on the y-axis.
		//	The intersection point will then be the new position for the object.

		Vec4 ray_clip = Vec4(mouse_position.x, mouse_position.y, -1.0f, 1.0f); 

		// Transform ray into view-space
		Vec4 ray_view = matrix::Multiply(matrix::Inverse(_camera.projection_matrix), ray_clip);
		ray_view = Vec4(ray_view.x, ray_view.y, -1.0f, 0.0f);

		// Transform ray into world-space
		Mat4x4 view = matrix::LookAt(_camera.position, vector::Add(_camera.position, _camera.direction), Vec3(0.0f, 1.0f, 0.0f)); // Build the view matrix
		Vec4 ray_world = matrix::Multiply(matrix::Inverse(view), ray_view);
		vector::Normalize(ray_world);

		Vec3 plane_normal = vector::Subtract(Vec3(0, 0, 0), _camera.direction);
		plane_normal.y = 0.0f; // Fix the plane on the y-axis

		// We add an offset to the plane to make sure we're actually moving the object relative to it's previous position and not relative to (0, 0, 0).
		Vec3 intersection = RayPlaneIntersect(_camera.position, Vec3(ray_world.x, ray_world.y, ray_world.z), plane_normal, -vector::Dot(entity->position, plane_normal)); 
		// We add the saved offset to avoid any popping effect caused by the user not clicking in the absolute middle of the object.
		entity->position = intersection;
	}
	else
	{
		// Move the object in the x-axis and the z-axis.

		// Calculate the position in world coordinates
		Vec3 world_position = _scene->ToWorld(mouse_position, _camera, entity->position.y);
		// We add the saved offset to avoid any popping effect caused by the user not clicking in the absolute middle of the object.
		entity->position = vector::Add(world_position, offset);
	}
}

void Lab2App::ScaleEntity(Entity* entity, const Vec2& mouse_position, bool y_axis)
{
	if(entity->type == Entity::ET_LIGHT)
	{	
		Vec3 world_position = _scene->ToWorld(mouse_position, _camera, entity->position.y);
		Vec3 d = vector::Subtract(world_position, entity->position);

		// If the scaled entity is a light, scale its light radius rather than the object size
		Light* light = (Light*)_selection.entity;
		light->radius = vector::Length(d);
	}
	else if(entity->type == Entity::ET_SPHERE)
	{
		// Only allow scaling one axis of the sphere

		Vec3 world_position = _scene->ToWorld(mouse_position, _camera, entity->position.y);
		Vec3 d = vector::Subtract(world_position, entity->position);

		float r = vector::Length(d);
		entity->scale = Vec3(r, r, r);
	}
	else
	{
		if(y_axis)
		{
			// Move the y_axis (and any axis that is the cross product of the y_axis and the camera direction)
			//	We do this by casting a ray from the mouse position against a plane that is fixed on the y-axis.
			//	The intersection point will then be the new position for the object.

			Vec4 ray_clip = Vec4(mouse_position.x, mouse_position.y, -1.0f, 1.0f); 

			// Transform ray into view-space
			Vec4 ray_view = matrix::Multiply(matrix::Inverse(_camera.projection_matrix), ray_clip);
			ray_view = Vec4(ray_view.x, ray_view.y, -1.0f, 0.0f);

			// Transform ray into world-space
			Mat4x4 view = matrix::LookAt(_camera.position, vector::Add(_camera.position, _camera.direction), Vec3(0.0f, 1.0f, 0.0f)); // Build the view matrix
			Vec4 ray_world = matrix::Multiply(matrix::Inverse(view), ray_view);
			vector::Normalize(ray_world);

			Vec3 plane_normal = vector::Subtract(Vec3(0, 0, 0), _camera.direction);
			plane_normal.y = 0.0f; // Fix the plane on the y-axis

			// We add an offset to the plane to make sure we're actually moving the object relative to it's previous position and not relative to (0, 0, 0).
			Vec3 intersection = RayPlaneIntersect(_camera.position, Vec3(ray_world.x, ray_world.y, ray_world.z), plane_normal, -vector::Dot(entity->position, plane_normal)); 
			Vec3 d = vector::Subtract(entity->position, intersection);

			entity->scale.y = fabs(d.y);
		}
		else
		{
			// Move the object in the x-axis and the z-axis.

			// Calculate the position in world coordinates
			Vec3 world_position = _scene->ToWorld(mouse_position, _camera, entity->position.y);
			Vec3 d = vector::Subtract(entity->position, world_position);

			entity->scale = Vec3(fabs(d.x), entity->scale.y, fabs(d.z));
		}
	}
}

void Lab2App::RotateEntity(Entity* entity, const Vec2& mouse_position)
{
	// Move the y_axis (and any axis that is the cross product of the y_axis and the camera direction)
	//	We do this by casting a ray from the mouse position against a plane that is fixed on the y-axis.
	//	The intersection point will then be the new position for the object.

	Vec4 ray_clip = Vec4(mouse_position.x, mouse_position.y, -1.0f, 1.0f); 

	// Transform ray into view-space
	Vec4 ray_view = matrix::Multiply(matrix::Inverse(_camera.projection_matrix), ray_clip);
	ray_view = Vec4(ray_view.x, ray_view.y, -1.0f, 0.0f);

	// Transform ray into world-space
	Mat4x4 view = matrix::LookAt(_camera.position, vector::Add(_camera.position, _camera.direction), Vec3(0.0f, 1.0f, 0.0f)); // Build the view matrix
	Vec4 ray_world = matrix::Multiply(matrix::Inverse(view), ray_view);
	vector::Normalize(ray_world);

	Vec3 plane_normal = vector::Subtract(Vec3(0, 0, 0), _camera.direction);
	plane_normal.y = 0.0f; // Fix the plane on the y-axis

	// We add an offset to the plane to make sure we're actually moving the object relative to it's previous position and not relative to (0, 0, 0).
	Vec3 intersection = RayPlaneIntersect(_camera.position, Vec3(ray_world.x, ray_world.y, ray_world.z), plane_normal, -vector::Dot(entity->position, plane_normal)); 
	Vec3 d = vector::Subtract(entity->position, intersection);

	Vec3 world_position = _scene->ToWorld(mouse_position, _camera, _selection.entity->position.y);
	Vec3 delta = vector::Subtract(world_position, _selection.entity->position);
	_selection.entity->rotation.x = delta.x;
	_selection.entity->rotation.y = delta.z;
}

void Lab2App::SelectEntity(Entity* entity, const Vec2& mouse_position)
{
	// Unselect any previous selection first
	if(_selection.entity)
	{
		UnselectEntity();
	}

	_selection.entity = entity;
	_selection.entity->selected = true;
	_selection.position = mouse_position;
				
	// Enable the color picker
	_color_picker->Show();
	_color_picker->SetTarget(entity);

	// Calculate the offset between the object origin and the mouse position, this is used later to avoid snapping.
	Vec3 world_position = _scene->ToWorld(mouse_position, _camera, _selection.entity->position.y);
	_selection.offset = vector::Subtract(_selection.entity->position, world_position);
}
void Lab2App::UnselectEntity()
{
	// Disable the color picker if it happends to be enabled
	if(_color_picker->Visible())
	{
		_color_picker->Hide();
		_color_picker->SetTarget(NULL);
	}

	_selection.mode = Selection::IDLE;
	_selection.entity->selected = false;
	_selection.entity = NULL;
}
