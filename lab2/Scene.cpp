#include <framework/Common.h>

#include "Scene.h"
#include "App.h"
#include "MatrixStack.h"

#include <framework/RenderDevice.h>
#include <framework/Ray.h>
#include <framework/Json.h>
#include <framework/ConfigValue.h>

#include <algorithm>
#include <sstream>
#include <fstream>

Scene::Scene(const Material& material, PrimitiveFactory* factory) 
	: _primitive_factory(factory),
	_material_template(material)
{
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
	Entity* entity = NULL;
	if(type == Entity::ET_LIGHT)
	{
		assert(_lights.size() < MAX_LIGHT_COUNT);
		entity = new Light;
	}
	else
	{
		entity = new Entity;
	}
	entity->type = type;

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
	case Entity::ET_LIGHT:
		{
			entity->primitive = _primitive_factory->CreateSphere(0.25f);
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

	if(type == Entity::ET_LIGHT)
	{
		// Light nodes are all white
		entity->material.ambient = Color(1.0f, 1.0f, 1.0f, 1.0f);
		entity->material.diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);
		
		Light* light = (Light*)entity;
		light->ambient = Color(0.0f, 0.0f, 0.0f);
		light->diffuse = Color(1.0f, 1.0f, 1.0f);
		light->specular = Color(0.25f, 0.25f, 0.25f);
		light->radius = 2.5f;

		_lights.push_back(light);
	}
	else
	{
		// Randomize colors for the new object
		entity->material.diffuse.r = (rand() % 255) / 255.0f;
		entity->material.diffuse.g = (rand() % 255) / 255.0f;
		entity->material.diffuse.b = (rand() % 255) / 255.0f;

		entity->material.specular = entity->material.diffuse;
	}

	return entity;
}
void Scene::DestroyEntity(Entity* entity)
{
	if(entity->type == Entity::ET_LIGHT)
	{
		std::vector<Light*>::iterator it = std::find(_lights.begin(), _lights.end(), entity);
		if(it != _lights.end())
			_lights.erase(it);
	}

	std::vector<Entity*>::iterator it = std::find(_entities.begin(), _entities.end(), entity);
	if(it != _entities.end())
	{
		delete (*it);
		_entities.erase(it);
	}

}

void Scene::DestroyAllEntities()
{
	_lights.clear();

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

void Scene::BindMaterialUniforms(RenderDevice& device, const Material& material)
{
	device.SetUniform4f("material.ambient",  Vec4(material.ambient.r, material.ambient.g, material.ambient.b, material.ambient.a));
	device.SetUniform4f("material.diffuse",  Vec4(material.diffuse.r, material.diffuse.g, material.diffuse.b, material.diffuse.a));
	device.SetUniform4f("material.specular",  Vec4(material.specular.r, material.specular.g, material.specular.b, material.specular.a));
}
void Scene::BindLightUniforms(RenderDevice& device)
{
	// OpenGL seems to have a weird way working with uniforms (arrays of structures) so we cannot set all our data at once, 
	//	we need to set each variable separately.
	for(uint32_t i = 0; i < MAX_LIGHT_COUNT; ++i)
	{
		if(_lights.size() > i)
		{
			Light* l = _lights[i];

			std::stringstream ss;
			ss << "lights[" << i << "].ambient";
			device.SetUniform4f(ss.str().c_str(), Vec4(l->ambient.r, l->ambient.g, l->ambient.b, l->ambient.a));
			
			ss.str("");
			ss << "lights[" << i << "].diffuse";
			device.SetUniform4f(ss.str().c_str(), Vec4(l->diffuse.r, l->diffuse.g, l->diffuse.b, l->diffuse.a));
			
			ss.str("");
			ss << "lights[" << i << "].specular";
			device.SetUniform4f(ss.str().c_str(), Vec4(l->specular.r, l->specular.g, l->specular.b, l->specular.a));

			ss.str("");
			ss << "lights[" << i << "].position";
			device.SetUniform3f(ss.str().c_str(), l->position);
			
			ss.str("");
			ss << "lights[" << i << "].radius";
			device.SetUniform1f(ss.str().c_str(), l->radius);
		}
		else
		{
			// Just set the radius to 0 for any "non-existing" lights in the array.
			std::stringstream ss;
			ss << "lights[" << i << "].radius";
			device.SetUniform1f(ss.str().c_str(), 0.0f);

		}

	}
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
bool Scene::LoadScene(const char* filename)
{
	std::ifstream ifs;
	ifs.open(filename, std::ifstream::in);

	if(ifs.is_open())
	{
		// Read complete file into memory
		ifs.seekg(0, ifs.end);
		int length = (int)ifs.tellg();
		ifs.seekg(0, ifs.beg);

		std::string buffer;
		buffer.resize(length);
		ifs.read(&buffer[0], length);
		ifs.close();

		ConfigValue scene;

		// Read scene from json
		json::Reader reader;
		reader.Read(buffer.c_str(), length, scene);

		// Parse scene
		ConfigValue& entities = scene["entities"];
		if(entities.IsArray())
		{
			for(uint32_t i = 0; i < entities.Size(); ++i)
			{
				ConfigValue& entity_node = entities[i];
				Entity::EntityType type = (Entity::EntityType)entity_node["type"].AsInt();

				Entity* entity = CreateEntity(type);

				// Transform
				
				entity->rotation.x = entity_node["rotation"][0].AsFloat();
				entity->rotation.y = entity_node["rotation"][1].AsFloat();
				entity->rotation.z = entity_node["rotation"][2].AsFloat();
		
				entity->position.x = entity_node["position"][0].AsFloat();
				entity->position.y = entity_node["position"][1].AsFloat();
				entity->position.z = entity_node["position"][2].AsFloat();
		
				entity->scale.x = entity_node["scale"][0].AsFloat();
				entity->scale.y = entity_node["scale"][1].AsFloat();
				entity->scale.z = entity_node["scale"][2].AsFloat();

				// Material
				entity->material = _material_template;

				ConfigValue& material = entity_node["material"];
				if(material.IsObject())
				{
					entity->material.ambient.r = material["ambient"][0].AsFloat();
					entity->material.ambient.g = material["ambient"][1].AsFloat();
					entity->material.ambient.b = material["ambient"][2].AsFloat();
					entity->material.ambient.a = material["ambient"][3].AsFloat();
		
					entity->material.specular.r = material["specular"][0].AsFloat();
					entity->material.specular.g = material["specular"][1].AsFloat();
					entity->material.specular.b = material["specular"][2].AsFloat();
					entity->material.specular.a = material["specular"][3].AsFloat();
					
					entity->material.diffuse.r = material["diffuse"][0].AsFloat();
					entity->material.diffuse.g = material["diffuse"][1].AsFloat();
					entity->material.diffuse.b = material["diffuse"][2].AsFloat();
					entity->material.diffuse.a = material["diffuse"][3].AsFloat();
				}

				if(type == Entity::ET_LIGHT)
				{
					Light* light = (Light*)entity;

					// Light paramters
					ConfigValue& light_node = entity_node["light"];
					if(light_node.IsObject())
					{
						light->ambient.r = light_node["ambient"][0].AsFloat();
						light->ambient.g = light_node["ambient"][1].AsFloat();
						light->ambient.b = light_node["ambient"][2].AsFloat();
						light->ambient.a = light_node["ambient"][3].AsFloat();
		
						light->specular.r = light_node["specular"][0].AsFloat();
						light->specular.g = light_node["specular"][1].AsFloat();
						light->specular.b = light_node["specular"][2].AsFloat();
						light->specular.a = light_node["specular"][3].AsFloat();
					
						light->diffuse.r = light_node["diffuse"][0].AsFloat();
						light->diffuse.g = light_node["diffuse"][1].AsFloat();
						light->diffuse.b = light_node["diffuse"][2].AsFloat();
						light->diffuse.a = light_node["diffuse"][3].AsFloat();
					
						light->radius = light_node["radius"].AsFloat();
					}

				}
				
			}
		}

		return true;
	}
	else
	{
		debug::Printf("Scene: No file with the name '%s' found.\n", filename);
		return false;
	}
}
void Scene::SaveScene(const char* filename)
{
	ConfigValue scene;
	scene.SetEmptyObject();

	ConfigValue& entities = scene["entities"];
	entities.SetEmptyArray();

	// Fill a ConfigValue with a representation of our scene
	for(std::vector<Entity*>::iterator it = _entities.begin(); 
		it != _entities.end(); ++it)
	{
		ConfigValue& entity = entities.Append();
		entity.SetEmptyObject();
		entity["type"].SetInt((*it)->type);

		entity["rotation"].SetEmptyArray();
		entity["rotation"].Append().SetFloat((*it)->rotation.x);
		entity["rotation"].Append().SetFloat((*it)->rotation.y);
		entity["rotation"].Append().SetFloat((*it)->rotation.z);
		
		entity["position"].SetEmptyArray();
		entity["position"].Append().SetFloat((*it)->position.x);
		entity["position"].Append().SetFloat((*it)->position.y);
		entity["position"].Append().SetFloat((*it)->position.z);
		
		entity["scale"].SetEmptyArray();
		entity["scale"].Append().SetFloat((*it)->scale.x);
		entity["scale"].Append().SetFloat((*it)->scale.y);
		entity["scale"].Append().SetFloat((*it)->scale.z);

		entity["material"].SetEmptyObject();
		entity["material"]["ambient"].SetEmptyArray();
		entity["material"]["ambient"].Append().SetFloat((*it)->material.ambient.r);
		entity["material"]["ambient"].Append().SetFloat((*it)->material.ambient.g);
		entity["material"]["ambient"].Append().SetFloat((*it)->material.ambient.b);
		entity["material"]["ambient"].Append().SetFloat((*it)->material.ambient.a);
		
		entity["material"]["specular"].SetEmptyArray();
		entity["material"]["specular"].Append().SetFloat((*it)->material.specular.r);
		entity["material"]["specular"].Append().SetFloat((*it)->material.specular.g);
		entity["material"]["specular"].Append().SetFloat((*it)->material.specular.b);
		entity["material"]["specular"].Append().SetFloat((*it)->material.specular.a);

		entity["material"]["diffuse"].SetEmptyArray();
		entity["material"]["diffuse"].Append().SetFloat((*it)->material.diffuse.r);
		entity["material"]["diffuse"].Append().SetFloat((*it)->material.diffuse.g);
		entity["material"]["diffuse"].Append().SetFloat((*it)->material.diffuse.b);
		entity["material"]["diffuse"].Append().SetFloat((*it)->material.diffuse.a);

		if((*it)->type == Entity::ET_LIGHT)
		{
			Light* light = (Light*)(*it);

			// Additional light parameters
			entity["light"].SetEmptyObject();
			entity["light"]["ambient"].SetEmptyArray();
			entity["light"]["ambient"].Append().SetFloat(light->ambient.r);
			entity["light"]["ambient"].Append().SetFloat(light->ambient.g);
			entity["light"]["ambient"].Append().SetFloat(light->ambient.b);
			entity["light"]["ambient"].Append().SetFloat(light->ambient.a);
		
			entity["light"]["specular"].SetEmptyArray();
			entity["light"]["specular"].Append().SetFloat(light->specular.r);
			entity["light"]["specular"].Append().SetFloat(light->specular.g);
			entity["light"]["specular"].Append().SetFloat(light->specular.b);
			entity["light"]["specular"].Append().SetFloat(light->specular.a);

			entity["light"]["diffuse"].SetEmptyArray();
			entity["light"]["diffuse"].Append().SetFloat(light->diffuse.r);
			entity["light"]["diffuse"].Append().SetFloat(light->diffuse.g);
			entity["light"]["diffuse"].Append().SetFloat(light->diffuse.b);
			entity["light"]["diffuse"].Append().SetFloat(light->diffuse.a);

			entity["light"]["radius"].SetFloat(light->radius);

		}
	}

	std::stringstream ss;

	// Convert to JSON
	json::Writer writer;
	writer.Write(scene, ss, true);

	// Write to file
	std::ofstream ofs;
	ofs.open(filename, std::ofstream::out);
	ofs << ss.str();
	ofs.close();
}
