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

	Entity() : rotation(0.0f, 0.0f, 0.0f), position(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f) {}
};

struct Light
{
	Color ambient;
	Color diffuse;
	Color specular;

	Vec3 direction;
};


struct Camera;
class RenderDevice;
class MatrixStack;

class Scene
{
public:
	/// @param material Material template that will be used by all new entities.
	Scene(const Material& material, PrimitiveFactory* factory);
	~Scene();

	/// @brief Tries to select an entity at the specified mouse position.
	/// @return The entity selected or NULL if no entity was found.
	Entity* SelectEntity(const Vec2& mouse_position, const Camera& camera);

	/// @brief Converts the specified mouse position to world coordinates.
	Vec3 ToWorld(const Vec2& mouse_position, const Camera& camera);

	/// @brief Creates an entity of the specified type and adds it to the scene.
	Entity* CreateEntity(Entity::EntityType type);

	/// @brief Destroys the specified entity.
	void DestroyEntity(Entity* entity);

	/// @brief Destroys all entities in the scene.
	void DestroyAllEntities();

	/// @brief Renders the scene with the specified device.
	void Render(RenderDevice& device, MatrixStack& matrix_stack);

	/// @brief Returns the active light for this scene.
	Light& GetLight();

private:
	/// Binds material specific shader uniforms.
	void BindMaterialUniforms(RenderDevice& device, const Material& material);
	/// Binds light specific shader uniforms.
	void BindLightUniforms(RenderDevice& device);

	std::vector<Entity*> _entities;

	PrimitiveFactory* _primitive_factory;
	Material _material_template; // Template material which will be used for all new entities.

	Light _active_light;
};


#endif // __SCENE_H__
