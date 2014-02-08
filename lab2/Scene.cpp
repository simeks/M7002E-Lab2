#include <framework/Common.h>

#include "Scene.h"
#include "MatrixStack.h"

#include <framework/RenderDevice.h>

Scene::Scene(PrimitiveFactory* factory) : _primitive_factory(factory)
{
	_active_light.ambient = Color(0.0f, 0.0f, 0.0f, 1.0f);
	_active_light.diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);
	_active_light.specular = Color(0.0f, 0.0f, 0.0f, 1.0f);
	_active_light.direction = Vec3(0.0f, 1.0f, 0.0f);
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
			
			BindLightUniforms(device);
			BindMaterialUniforms(device, (*it)->material);
		
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

