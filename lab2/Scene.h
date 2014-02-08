#ifndef __SCENE_H__
#define __SCENE_H__

#include "PrimitiveFactory.h"

/// @brief Represents an object in the scene.
struct Entity
{
	enum EntityType
	{
		ET_PYRAMID,
		ET_CUBE,
		ET_SPHERE
	};

	Primitive primitive;
	Material material;

	// Transform

	Vec3 rotation; // Head, pitch, roll
	Vec3 position;
	Vec3 scale;
};

struct Light
{
	Color ambient;
	Color diffuse;
	Color specular;

	Vec3 direction;
};



class RenderDevice;
class MatrixStack;

class Scene
{
public:
	Scene(PrimitiveFactory* factory);
	~Scene();

	void SetMaterialTemplate(const Material& material);

	Entity* CreateEntity(Entity::EntityType type);
	void DestroyEntity(Entity* entity);

	void DestroyAllEntities();


	void Render(RenderDevice& device, MatrixStack& matrix_stack);

	/// @brief Returns the active light for this scene.
	Light& GetLight();

private:
	void BindMaterialUniforms(RenderDevice& device, const Material& material);
	void BindLightUniforms(RenderDevice& device);

	std::vector<Entity*> _entities;

	PrimitiveFactory* _primitive_factory;
	Material _material_template; // Template material which will be used for all new entities.

	Light _active_light;
};


#endif // __SCENE_H__
