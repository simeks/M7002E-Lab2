#ifndef __APP_H__
#define __APP_H__


struct SDL_Window;
class App
{
public:
	App();
	virtual ~App();

	/// Runs the application, this will not return until the application stops.
	void Run();

	/// @brief Specifies the clear color for when clearing the back buffer. 
	void SetClearColor(float r, float g, float b, float a);

	/// @brief Specifies the window title.
	void SetWindowTitle(const char* title);

protected:

	/// @return True if initialization was successful, false if not.
	virtual bool Initialize() = 0;
	virtual void Shutdown() = 0;

	/// @brief Callback invoked once every frame to let the application perform rendering.
	virtual void Render(float dtime) = 0;

	/// @brief Initializes SDL and creates a new primary window for rendering.
	/// @return True if initialization was successful, false if not.
	bool InitializeSDL(uint32_t window_width, uint32_t window_height);

	/// @brief SDL shutdown.
	void ShutdownSDL();

	SDL_Window* _window;
	bool _running; // Specifies whether the framework is currently running, setting this to false will exit the application.

	uint32_t _last_tick;
};


#endif // __APP_H__
