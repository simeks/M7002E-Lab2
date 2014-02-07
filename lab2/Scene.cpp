#include <framework/Common.h>

#include "Scene.h"
#include "MatrixStack.h"

#include <framework/RenderDevice.h>

Scene::Scene(PrimitiveFactory* factory) : _primitive_factory(factory)
{

}
Scene::~Scene()
{
	// Destroy remaning entities
	DestroyAllEntities();
}
void Scene::SetMaterialTemplate(const Material& material)
{
	_material_template = material;
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
	default:
		assert(false);
	};

	entity->position = Vec3(0.0f, 0.0f, 0.0f);
	entity->rotation = Vec3(0.0f, 0.0f, 0.0f);
	entity->scale = Vec3(1.0f, 1.0f, 1.0f);
	entity->material = _material_template;

	_entities.push_back(entity);

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
	for(std::vector<Entity*>::iterator it = _entities.begin(); 
		it != _entities.end(); ++it)
	{
		// Make sure the material have a shader, otherwise we have nothing to render
		if((*it)->material.shader != -1)
		{
			// Bind shader and set material parameters
			device.BindShader((*it)->material.shader);
		
			matrix_stack.Push();

			// Transform object
			matrix_stack.Scale3f((*it)->scale);
			matrix_stack.Translate3f((*it)->position);
			matrix_stack.Rotate3f((*it)->rotation.x, (*it)->rotation.y, (*it)->rotation.z);

			// Apply transformations to pipeline
			matrix_stack.Apply(device);

			// Perform the actual draw call
			device.Draw((*it)->primitive.draw_call);

			matrix_stack.Pop();
		}
	}

}
