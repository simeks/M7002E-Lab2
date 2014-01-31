#ifndef __LAB2_APP_H__
#define __LAB2_APP_H__

#include <framework/App.h>

#include "PrimitiveFactory.h"

struct Viewport
{
	int x, y;
	int width, height;
};

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

	/// @brief Renders the scene, this does not include any viewport setup, etc, that may be needed.
	void RenderScene();
	
private:
	PrimitiveFactory* _primitive_factory;

	Viewport _viewport;
	
};


#endif // __LAB2_APP_H__
