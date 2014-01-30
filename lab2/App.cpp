#include <framework/Common.h>
#include "App.h"


Lab2App::Lab2App()
{
}
Lab2App::~Lab2App()
{
}

void Lab2App::DrawPrimitive(const Primitive& primitive, const Vec3& position, const Vec3& rotation)
{
	glEnableClientState(GL_VERTEX_ARRAY); // Enables the use of the vertex array specified in our vertex buffer.
	glEnableClientState(GL_COLOR_ARRAY); // Enables the use of the color array specified in our vertex color buffer.

	// Binds the vertex buffer for use.
	glBindBuffer(GL_ARRAY_BUFFER, primitive.vertex_buffer); 
	// Specifies the location and format of the vertex data to use when drawing.
	glVertexPointer(3, // Each vertex contains 3 elements (x, y, z)
					GL_FLOAT, // Our vertex data is represented as floats
					0, // Stride set to 0 as all vertices are consecutive 
					0); 

	// Binds the color buffer for use.
	glBindBuffer(GL_ARRAY_BUFFER, primitive.color_buffer); 
	// Specifies the location and format of the color data to use when drawing.
	glColorPointer(4, // Each vertex contains 4 elements (r, g, b, a)
					GL_FLOAT, // Our vertex data is represented as floats
					0, // Stride set to 0 as all vertices are consecutive 
					0); 

	// Specify the modelview matrix as the current matrix.
	glMatrixMode(GL_MODELVIEW);

	// Push a new matrix to the stack so that we easily can pop back to the previous state.
	glPushMatrix();
	
	// Translate the primitive to the specified position.
	glTranslatef(position.x, position.y, position.z);

	// Rotate the primitive
	glRotatef(rotation.x, 1.0f, 0.0f, 0.0f); // Yaw
	glRotatef(rotation.y, 0.0f, 1.0f, 0.0f); // Pitch
	glRotatef(rotation.z, 0.0f, 0.0f, 1.0f); // Roll


	// Perform the actual draw call.
	glDrawArrays(primitive.draw_mode, 0, primitive.vertex_count);

	// Return the modelview matrix to its previous state
	glPopMatrix();
	
	glDisableClientState(GL_COLOR_ARRAY); // Disables the use of the color array.
	glDisableClientState(GL_VERTEX_ARRAY); // Disables the use of the vertex array.
}

bool Lab2App::Initialize()
{
	uint32_t win_width = 1024, win_height = 768;

	// Initialize SDL and create a 1024x768 winodw for rendering.
	if(!InitializeSDL(win_width, win_height))
		return false;

	SetWindowTitle("OpenGL - Lab 2");
	SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_CULL_FACE); // Enable face culling
	glEnable(GL_DEPTH_TEST); // Enable depth testing

	_viewport.x = _viewport.y = 0;
	_viewport.width = win_width;
	_viewport.height = win_height;
	
	return true;
}
void Lab2App::Shutdown()
{
	// Cleanup

	ShutdownSDL();
}
void Lab2App::Render(float)
{
	// Reset the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Setup projection matrix
	glMatrixMode(GL_PROJECTION);

	// Save previous matrix
	glPushMatrix();
	
	glLoadIdentity();
	
	// Set the perspective, 45 degrees FOV, aspect ratio to match viewport, z range: [1.0, 1000.0]
	gluPerspective(45.0, (double)_viewport.width/(double)_viewport.height, 1.0, 1000.0);
	// Set the camera position to (0, 0, 400) and make it look at the center of the scene (0, 0, 0)
	gluLookAt(0.0, 0.0, 400.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// Draw the scene
	RenderScene();
	
	// Restore projection matrix
	glPopMatrix();

}
void Lab2App::RenderScene()
{
	// Render all the primitives in the scene.

}
