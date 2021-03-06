#ifndef __LAB2_APP_H__
#define __LAB2_APP_H__

#include <framework/App.h>

#include "MatrixStack.h"
#include "Material.h"

struct Viewport
{
	int x, y;
	int width, height;
};

struct Camera
{
	Mat4x4 projection_matrix;
	Vec3 position;
	Vec3 direction; // Which direction the camera is looking
};

class Scene;
class PrimitiveFactory;
class ColorPicker;
struct Entity;

class Lab2App : public App
{
public:
	Lab2App();
	~Lab2App();


protected:
	bool Initialize();
	void Shutdown();

	/// @brief Callback invoked once every frame to let the application perform rendering.
	void Render(float dtime);

	/// @brief Callback for when receiving SDL events.
	void OnEvent(SDL_Event* evt);
	
	/// @brief Move the specified entity
	/// @param y_axis Specifies if the object should be moved in the y-axis.
	/// @param offset Offset from the object origin and the mouse position (In world space).
	void MoveEntity(Entity* entity, const Vec2& mouse_position, bool y_axis, const Vec3& offset);

	/// @brief Scales the specified entity
	/// @param y_axis Specifies if the object should be scaled in the y-axis.
	void ScaleEntity(Entity* entity, const Vec2& mouse_position, bool y_axis);
	
	/// @brief Rotates the specified entity
	void RotateEntity(Entity* entity, const Vec2& mouse_position);

	/// @brief Called when the user wants to select an entity.
	void SelectEntity(Entity* entity, const Vec2& mouse_position);
	/// @brief Called when the user wants to unselect the current entity.
	void UnselectEntity();

private:
	struct Selection
	{
		enum Mode
		{
			IDLE,
			MOVE,
			SCALE,
			ROTATE
		};

		Entity* entity; // Selected entity, NULL if none is selected
		Mode mode;

		Vec2 position; // Mouse position when the entity was selected
		Vec3 offset; // Offset from entity center to mouse position

		Selection() : entity(NULL), mode(IDLE) {}
	};

	MatrixStack _matrix_stack;
	Viewport _viewport;
	Camera _camera;
	float _camera_angle;

	PrimitiveFactory* _primitive_factory;
	Scene* _scene;

	int _default_shader;

	Selection _selection;
	ColorPicker* _color_picker;
};


#endif // __LAB2_APP_H__
