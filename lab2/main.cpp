#include <framework/Common.h>
#include "App.h"


#ifdef PLATFORM_WIN32
int WINAPI WinMain(HINSTANCE , HINSTANCE , LPSTR , int )
#else
int main(int argc, char* argv[])
#endif
{
	Lab2App app;
	app.Run();
}

