#ifndef __COLORPICKER_H__
#define __COLORPICKER_H__

#include <framework/RenderDevice.h>

struct Entity;
struct Viewport;
struct Color;

/// Utility-UI for picking colors
class ColorPicker
{
public:
	ColorPicker(RenderDevice* device, const Viewport& viewport);
	~ColorPicker();
	
	/// @brief Renders the color picker, this will do nothing if the picker is hidden.
	void Render();

	/// @brief Specifies the entity that should receive the color changes.
	void SetTarget(Entity* entity);

	/// @brief Processes mouse input.
	/// The picker will process the given input and check if it will affect the UI. If it will
	///		it will handle the input and return true, if not (e.g. mouse clicks outside the sliders),
	///		return false.
	/// @param mouse_position Global mouse position in pixels.
	bool OnMouseEvent(const Vec2& mouse_position);

	/// @brief Enables the color picker.
	void Show();
	
	/// @brief Disables the color picker.
	void Hide();
	
	/// @return True if the color picker is currently visible.
	bool Visible() const;

private:
	struct Gradients
	{
		Vec2 position; // Pixel coordinates
		Color* target; // Pointer to the target color

		Gradients() : target(NULL) {}
	};

	void Setup();
	void RenderGradients(const Gradients& gradients, const Color& current_value);

	/// Process the input and check it against the specified gradient set.
	/// @return True if the gradients was changed, false if not.
	bool HandleInput(const Vec2& mouse_position, Gradients& gradients);

	RenderDevice* _device;
	bool _visible;

	DrawCall _draw_call; // Shared draw call for all gradients
	int _shader;

	Vec2 _viewport_size;
	Vec2 _gradient_size; // Size of each gradient set in pixels

	Gradients _ambient_gradient;
	Gradients _specular_gradient;
	Gradients _diffuse_gradient;
};


#endif // __COLORPICKER_H__
