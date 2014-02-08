#include <framework/Common.h>

#include "Scene.h"
#include "App.h"
#include "MatrixStack.h"

#include <framework/RenderDevice.h>
#include <framework/Ray.h>

#include <algorithm>

Scene::Scene(const Material& material, PrimitiveFactory* factory) 
	: _primitive_factory(factory),
	_material_template(material)
{
	_active_light.ambient = Color(0.0f, 0.0f, 0.0f, 1.0f);
	_active_light.diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);
	_active_light.specular = Color(0.0f, 0.0f, 0.0f, 1.0f);
	_active_light.direction = Vec3(0.0f, 1.0f, 0.0f);

	// Create a floor
	_floor_entity = new Entity;
	_floor_entity->primitive = _primitive_factory->CreatePlane(Vec2(25.0f, 25.0f));
	_floor_entity->scale = Vec3(1.0f, 1.0f, 1.0f);
	_floor_entity->position = Vec3(0.0f, -0.5f, 0.0f);
	_floor_entity->material = _material_template;
	_floor_entity->material.diffuse = Color(0.40f, 0.40f, 0.40f);
	_floor_entity->material.specular = Color(0.40f, 0.40f, 0.40f);
}
Scene::~Scene()
{
	// Destroy remaning entities
	DestroyAllEntities();
	// Destroy the floor
	delete _floor_entity;
	_floor_entity = NULL;
}

struct EntityDepthSort
{
	const Camera& _camera;

	EntityDepthSort(const Camera& camera) : _camera(camera) {}

	bool operator() (Entity* ent1, Entity* ent2) 
	{
		float d1 = vector::Length(vector::Subtract(ent1->position, _camera.position));
		float d2 = vector::Length(vector::Subtract(ent2->position, _camera.position));
		
		return (d1 < d2);
	}
};

Entity* Scene::SelectEntity(const Vec2& mouse_position, const Camera& camera)
{
	// Make sure entities are sorted by depth, this way the select will always chose the closest possible entity.
	std::sort(_entities.begin(), _entities.end(), EntityDepthSort(camera));

	// Ray in clip-space
	Vec4 ray_clip = Vec4(mouse_position.x, mouse_position.y, -1.0f, 1.0f); 
	
	// Transform ray into view-space
	Vec4 ray_view = matrix::Multiply(matrix::Inverse(camera.projection_matrix), ray_clip);
	ray_view = Vec4(ray_view.x, ray_view.y, -1.0f, 0.0f);

	// Transform ray into world-space
	Mat4x4 view = matrix::LookAt(camera.position, vector::Add(camera.position, camera.direction), Vec3(0.0f, 1.0f, 0.0f)); // Build the view matrix
	
	Vec4 ray_world = matrix::Multiply(matrix::Inverse(view), ray_view);
	vector::Normalize(ray_world);
	
	for(std::vector<Entity*>::iterator it = _entities.begin(); 
		it != _entities.end(); ++it)
	{
		float radius = max(max((*it)->scale.x, (*it)->scale.y), (*it)->scale.z) * (*it)->primitive.bounding_radius; // Scale bounding radius
		if(RaySphereIntersect(camera.position, Vec3(ray_world.x, ray_world.y, ray_world.z), (*it)->position, radius))
		{
			return (*it);
		}
	}
	return NULL;
}
Vec3 Scene::ToWorld(const Vec2& mouse_position, const Camera& camera)
{
	// Ray in clip-space
	Vec4 ray_clip = Vec4(mouse_position.x, mouse_position.y, -1.0f, 1.0f); 
	
	// Transform ray into view-space
	Vec4 ray_view = matrix::Multiply(matrix::Inverse(camera.projection_matrix), ray_clip);
	ray_view = Vec4(ray_view.x, ray_view.y, -1.0f, 0.0f);

	// Transform ray into world-space
	Mat4x4 view = matrix::LookAt(camera.position, vector::Add(camera.position, camera.direction), Vec3(0.0f, 1.0f, 0.0f)); // Build the view matrix
	
	Vec4 ray_world = matrix::Multiply(matrix::Inverse(view), ray_view);
	vector::Normalize(ray_world);

	return RayPlaneIntersect(camera.position, Vec3(ray_world.x, ray_world.y, ray_world.z), Vec3(0.0f, 1.0f, 0.0f));
}

Entity* Scene::CreateEntity(Entity::EntityType type)
{
	Entity* entity = new Entity;

	switch(type)
	{
	case Entity::ET_PYRAMID:
		{
			entity->primitive = _primitive_factory->CreatePyramid(Vec3(1.0f, 1.0f, 1.0f));
		}
		break;
	case Entity::ET_CUBE:
		{
			entity->primitive = _primitive_factory->CreateCube(Vec3(1.0f, 1.0f, 1.0f));
		}
		break;
	case Entity::ET_SPHERE:
		{
			entity->primitive = _primitive_factory->CreateSphere(0.5f);
		}
		break;
	default:
		assert(false);
	};

	entity->position = Vec3(0.0f, 0.0f, 0.0f);
	entity->rotation = Vec3(0.0f, 0.0f, 0.0f);
	entity->scale = Vec3(1.0f, 1.0f, 1.0f);
	entity->material = _material_template;

	_entities.push_back(entity);

	// Randomize colors for the new object
	entity->material.diffuse.r = (rand() % 255) / 255.0f;
	entity->material.diffuse.g = (rand() % 255) / 255.0f;
	entity->material.diffuse.b = (rand() % 255) / 255.0f;

	entity->material.specular = entity->material.diffuse;

	return entity;
}
void Scene::DestroyEntity(Entity* entity)
{
	std::vector<Entity*>::iterator it = std::find(_entities.begin(), _entities.end(), entity);
	if(it != _entities.end())
	{
		delete (*it);
		_entities.erase(it);
	}

}

void Scene::DestroyAllEntities()
{
	for(std::vector<Entity*>::iterator it = _entities.begin(); 
		it != _entities.end(); ++it)
	{
		delete (*it);
	}
	_entities.clear();
}

void Scene::Render(RenderDevice& device, MatrixStack& matrix_stack)
{
	// Render floor
	RenderEntity(device, matrix_stack, _floor_entity);

	// Render the rest of the entities
	for(std::vector<Entity*>::iterator it = _entities.begin(); 
		it != _entities.end(); ++it)
	{
		RenderEntity(device, matrix_stack, *it);
	}

}
Light& Scene::GetLight()
{
	return _active_light;
}
void Scene::BindMaterialUniforms(RenderDevice& device, const Material& material)
{
	device.SetUniform3f("material_ambient",  Vec3(material.ambient.r, material.ambient.g, material.ambient.b));
	device.SetUniform3f("material_diffuse",  Vec3(material.diffuse.r, material.diffuse.g, material.diffuse.b));
	device.SetUniform3f("material_specular",  Vec3(material.specular.r, material.specular.g, material.specular.b));
}
void Scene::BindLightUniforms(RenderDevice& device)
{
	device.SetUniform3f("light_direction", _active_light.direction);
	device.SetUniform3f("light_ambient",  Vec3(_active_light.ambient.r, _active_light.ambient.g, _active_light.ambient.b));
	device.SetUniform3f("light_diffuse",  Vec3(_active_light.diffuse.r, _active_light.diffuse.g, _active_light.diffuse.b));
	device.SetUniform3f("light_specular",  Vec3(_active_light.specular.r, _active_light.specular.g, _active_light.specular.b));
}
void Scene::RenderEntity(RenderDevice& device, MatrixStack& matrix_stack, Entity* entity)
{
	// Make sure the material have a shader, otherwise we have nothing to render
	if(entity->material.shader != -1)
	{
		// Bind shader and set material parameters
		device.BindShader(entity->material.shader);
			
		BindLightUniforms(device);
		BindMaterialUniforms(device, entity->material);
		
		matrix_stack.Push();

		// Transform object
		matrix_stack.Translate3f(entity->position);
		matrix_stack.Scale3f(entity->scale);
		matrix_stack.Rotate3f(entity->rotation.x, entity->rotation.y, entity->rotation.z);

		// Apply transformations to pipeline
		matrix_stack.Apply(device);

		// Perform the actual draw call
		device.Draw(entity->primitive.draw_call);

		matrix_stack.Pop();
	}
}
