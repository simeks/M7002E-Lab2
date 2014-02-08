require "tundra.syntax.glob"

StaticLibrary {
	Name = "Framework",
	Env = {
		CPPPATH = { 
			"framework",
			".",
			"dependencies/SDL2-2.0.1/include",
			"dependencies/glew/include",
			{ "/Library/Frameworks/SDL2.framework/Headers"; Config = "macosx-*-*" }
		}, 
	},

	Sources = {
		FGlob {
			Dir = "framework",
			Extensions = { ".cpp", ".h", ".inl" },
			Filters = {
				{ Pattern = "win32"; Config = "win32-*-*"; },
				{ Pattern = "macosx"; Config = "macosx-*-*"; },
			},
		},
	},

	Frameworks = { "OpenGL", "SDL2" },
}

Program {
	Name = "Lab2",
	Env = {
		CPPPATH = { 
			"lab2",
			".",
			"dependencies/SDL2-2.0.1/include",
			"dependencies/glew/include",
		}, 
		LIBPATH = {
			{
				"dependencies/SDL2-2.0.1/lib/x86",
				"dependencies/glew/lib/Release/Win32";
				Config = "win32-*-*";
			}
		},
		PROGOPTS = {
			{ "/SUBSYSTEM:WINDOWS"; Config = { "win32-*-*" } },
		}
	},
	Sources = {
		FGlob {
			Dir = "lab2",
			Extensions = { ".cpp", ".h", ".inl" },
			Filters = {
				{ Pattern = "win32"; Config = "win32-*-*"; },
				{ Pattern = "macosx"; Config = "macosx-*-*"; },
			},
		},
	},
	Depends = { "Framework" },

	Libs = { 
		{ 
			"kernel32.lib", 
			"user32.lib", 
			"gdi32.lib", 
			"comdlg32.lib", 
			"advapi32.lib", 
			"SDL2.lib",
			"opengl32.lib",
			"glew32.lib",
			"glu32.lib",
			Config = { "win32-*-*", "win64-*-*" } 
		}
	},

	Frameworks = { "OpenGL", "SDL2" },
}

Default "Lab2"
