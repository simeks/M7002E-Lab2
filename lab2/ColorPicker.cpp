#include <framework/Common.h>

#include "ColorPicker.h"
#include "Scene.h"
#include "App.h"

static const char* vertex_shader_src = " \
	varying float x;\
	\
	attribute vec3 vertex_position; \
	\
	uniform vec3 offset; \
	uniform vec3 scale; \
	\
	void main() \
	{ \
		x = vertex_position.x; \
		gl_Position = vec4(scale*vertex_position + offset, 1.0); \
	}";

static const char* fragment_shader_src = " \
	varying float x; \
	uniform float current_x; \
	uniform vec4 color; \
	void main() \
	{ \
		if(abs(x - current_x) > 0.005 )\
			gl_FragColor = max(color * x, 0);  \
		else \
			gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0); \
	}";



ColorPicker::ColorPicker(RenderDevice* device, const Viewport& viewport) 
	: _device(device),
	_visible(false),
	_shader(-1),
	_viewport_size((float)viewport.width, (float)viewport.height)
{
	Setup();
}
ColorPicker::~ColorPicker()
{
}
void ColorPicker::Render()
{
	if(!_visible)
		return;

	_device->BindShader(_shader);

	RenderGradients(_ambient_gradient, *_ambient_gradient.target);
	RenderGradients(_specular_gradient, *_specular_gradient.target);
	RenderGradients(_diffuse_gradient, *_diffuse_gradient.target);

	_device->BindShader(-1);
}

void ColorPicker::SetTarget(Entity* target)
{
	if(target == NULL)
	{
		// Unset target
		_ambient_gradient.target = NULL;
		_specular_gradient.target = NULL;
		_diffuse_gradient.target = NULL;
	}
	else
	{
		if(target->type == Entity::ET_LIGHT)
		{
			// If the entity is a light we want to modify the light parameters rather than the material parameters.
			Light* light = (Light*)target;
		
			_ambient_gradient.target = &light->ambient;
			_specular_gradient.target = &light->specular;
			_diffuse_gradient.target = &light->diffuse;
		}
		else
		{
			_ambient_gradient.target = &target->material.ambient;
			_specular_gradient.target = &target->material.specular;
			_diffuse_gradient.target = &target->material.diffuse;
		}
	}
}

bool ColorPicker::OnMouseEvent(const Vec2& mouse_position)
{
	// We can't do changes if the sliders are hidden.
	if(!_visible)
		return false;

	// Check each gradient set. We will not need to check any further if one
	//	is changed as you can only change one at a time.

	if(HandleInput(mouse_position, _ambient_gradient))
		return true;
	if(HandleInput(mouse_position, _specular_gradient))
		return true;
	if(HandleInput(mouse_position, _diffuse_gradient))
		return true;

	return false;
}

bool ColorPicker::HandleInput(const Vec2& mouse_position, Gradients& gradients)
{
	if(mouse_position.x >= gradients.position.x && 
		mouse_position.x < (gradients.position.x + _gradient_size.x) &&
		mouse_position.y >= gradients.position.y && 
		mouse_position.y < (gradients.position.y + _gradient_size.y))
	{
		float component_height = _gradient_size.y / 3.0f;

		// Check which color component, we only need to check y-axis
		// Blue
		if(mouse_position.y >= gradients.position.y &&
			mouse_position.y < (gradients.position.y + component_height))
		{
			gradients.target->b = (mouse_position.x - gradients.position.x) / _gradient_size.x;
			return true;
		}
		// Green
		if(mouse_position.y >= (gradients.position.y + component_height) &&
			mouse_position.y < (gradients.position.y + component_height*2.0f))
		{
			gradients.target->g = (mouse_position.x - gradients.position.x) / _gradient_size.x;
			return true;
		}
		// Red
		if(mouse_position.y >= (gradients.position.y + component_height*2.0f) &&
			mouse_position.y < (gradients.position.y + component_height*3.0f))
		{
			gradients.target->r = (mouse_position.x - gradients.position.x) / _gradient_size.x;
			return true;
		}

	}
	return false;
}

void ColorPicker::Show()
{
	_visible = true;
}
void ColorPicker::Hide()
{
	_visible = false;
}
bool ColorPicker::Visible() const
{
	return _visible;
}

void ColorPicker::Setup()
{
	_draw_call.draw_mode = GL_TRIANGLES;
	_draw_call.index_buffer = -1;
	_draw_call.index_count = 0;
	_draw_call.vertex_count = 6; // Rectangle => 6 vertices
	_draw_call.vertex_offset = 0;
	_draw_call.vertex_format = vertex_format::VF_POSITION3F;
	
	float vertex_data[6*3];
	int vertex_idx = 0;

	vertex_data[vertex_idx++] = 0.0f; vertex_data[vertex_idx++] = 0.0f; vertex_data[vertex_idx++] = 0.0f; // Bottom-left
	vertex_data[vertex_idx++] = 1.0f; vertex_data[vertex_idx++] = 0.0f; vertex_data[vertex_idx++] = 0.0f; // Bottom-right
	vertex_data[vertex_idx++] = 1.0f; vertex_data[vertex_idx++] = 1.0f; vertex_data[vertex_idx++] = 0.0f; // Top-right

	vertex_data[vertex_idx++] = 0.0f; vertex_data[vertex_idx++] = 0.0f; vertex_data[vertex_idx++] = 0.0f; // Bottom-left
	vertex_data[vertex_idx++] = 1.0f; vertex_data[vertex_idx++] = 1.0f; vertex_data[vertex_idx++] = 0.0f; // Top-right
	vertex_data[vertex_idx++] = 0.0f; vertex_data[vertex_idx++] = 1.0f; vertex_data[vertex_idx++] = 0.0f; // Top-left

	_draw_call.vertex_buffer = _device->CreateVertexBuffer(6*3*sizeof(float), vertex_data);

	_shader = _device->CreateShader(vertex_shader_src, fragment_shader_src);
	
	_gradient_size.x = 300;
	_gradient_size.y = 90;

	float x = _viewport_size.x - _gradient_size.x - 50;
	_ambient_gradient.position = Vec2(x, 210);
	_specular_gradient.position = Vec2(x, 110);
	_diffuse_gradient.position = Vec2(x, 10);

}
void ColorPicker::RenderGradients(const Gradients& gradients, const Color& current_value)
{
	// Transform pixel coordinates to [-1, 1]
	Vec3 offset = Vec3(	2.0f * (gradients.position.x / _viewport_size.x) - 1.0f, 
						2.0f * (gradients.position.y / _viewport_size.y)  - 1.0f, 0);

	Vec3 scale = Vec3(	2.0f * (_gradient_size.x / _viewport_size.x), 
						2.0f * (_gradient_size.y / _viewport_size.y) / 3.0f, 0);


	_device->SetUniform3f("scale", Vec3(scale.x, scale.y, 1.0f));
	// Blue
	{
		_device->SetUniform4f("color", Vec4(0.0f, 0.0f, 1.0f, 1.0f));
		_device->SetUniform3f("offset", offset);
		_device->SetUniform1f("current_x", current_value.b);

		_device->Draw(_draw_call);

		offset.y = offset.y + scale.y;
	}
	// Green
	{
		_device->SetUniform4f("color", Vec4(0.0f, 1.0f, 0.0f, 1.0f));
		_device->SetUniform3f("offset", offset);
		_device->SetUniform1f("current_x", current_value.g);

		_device->Draw(_draw_call);

		offset.y = offset.y + scale.y;
	}
	// Red
	{
		_device->SetUniform4f("color", Vec4(1.0f, 0.0f, 0.0f, 1.0f));
		_device->SetUniform3f("offset", offset);
		_device->SetUniform1f("current_x", current_value.r);

		_device->Draw(_draw_call);
	}

}
