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
		ET_SPHERE,
		ET_LIGHT
	};
	EntityType type;

	Primitive primitive;
	Material material;

	// Transform

	Vec3 rotation; // Head, pitch, roll
	Vec3 position;
	Vec3 scale;

	bool selected; // Specifies if this entity is currently selected.

	Entity() : rotation(0.0f, 0.0f, 0.0f), position(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), selected(false) {}
};

/// Point-light
struct Light : public Entity
{
	Color ambient;
	Color diffuse;
	Color specular;

	float radius;
};


struct Camera;
class RenderDevice;
class MatrixStack;

class Scene
{
public:
	enum { MAX_LIGHT_COUNT = 16 };

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
	
	/// Loads the scene from a file.
	/// @return True if a scene was loaded, false if not.
	bool LoadScene(const char* filename);
	/// Saves the scene to a file.
	void SaveScene(const char* filename);

private:
	/// Binds material specific shader uniforms.
	void BindMaterialUniforms(RenderDevice& device, Entity* entity);
	/// Binds light specific shader uniforms.
	void BindLightUniforms(RenderDevice& device);

	void RenderEntity(RenderDevice& device, MatrixStack& matrix_stack, Entity* entity); 

	std::vector<Entity*> _entities;
	Entity* _floor_entity;

	std::vector<Light*> _lights;

	PrimitiveFactory* _primitive_factory;
	Material _material_template; // Template material which will be used for all new entities.

};


#endif // __SCENE_H__
