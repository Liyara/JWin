#ifndef JWIN_PLATFORM
#define JWIN_PLATFORM

#include <JWin/x11_definitions.h>
#include <JWin/win32_definitions.h>

#include <GL/glu.h>

namespace jwin {

	template <typename T>
	uint32_t createMask(jutil::Queue<T> values) {
		if (sizeof(T) * values.size() != sizeof(uint32_t)) return 0;
		uint32_t r = 0;
		for (size_t i = 0; i < values.size(); ++i) r |= ((uint32_t)(values[i]) << ((sizeof(T) << 3) * i));
		return r;
	}

	namespace display_manager {
		//Data structure describing monitors connected to the system
		struct MonitorData {
			jutil::Queue<Monitor*> monitors;
			Monitor *primaryMonitor;
			MonitorData() :
				primaryMonitor(nullptr)
			{}
		};

		extern MonitorData monitorData;

		bool createDisplay();
		void destroyDisplay();

		struct PixelFormat {
			JWIN_ID id;
			JWIN_PLATFORM_FORMAT platformConfig;
			GLboolean stereoColor;
			GLint nAuxBuffers;
			GLuint accumMask;
			GLboolean transparent;
			GLint samples;
			GLboolean renderable;
			GLboolean doubleBuffered;
			GLint drawableType;
			GLint renderType;
			GLuint rgbaMask;
			GLuint dsMask;
			PixelFormat() :
				id(0),
				stereoColor(false),
				nAuxBuffers(0),
				accumMask(JWIN_ACCUM_64),
				transparent(false),
				samples(0),
				renderable(true),
				doubleBuffered(false),
				drawableType(JWIN_TARGET_WINDOW),
				renderType(JWIN_RENDER_RGBA),
				rgbaMask(JWIN_RGBA),
				dsMask(JWIN_DS_32)
			{}
		};

		typedef jutil::Queue<PixelFormat> PixelFormatList;

		struct ContextSettings {
			PixelFormat pixelFormat;
			GLint glVersionMajor;
			GLint glVersionMinor;
			GLboolean debug;
			GLboolean forwardCompatible;
			GLint profile;
			ContextSettings() :
				glVersionMajor(4),
				glVersionMinor(0),
				debug(false),
				forwardCompatible(true),
				profile(JWIN_CORE_PROFILE)
			{}
		};

		struct ContextData {
			JWIN_CONTEXT renderContext;
			JWIN_DISPLAY_CONTEXT displayContext;
			ContextSettings settings;
			Handle drawableObject;
		};

		extern ContextSettings desiredContextSettings;
		extern jutil::Queue<ContextData> contexts;
		extern ContextData *currentContext;
		extern jutil::Queue<jutil::String> supportedExtensions;

		//Take a raw platform-specific Frame Buffer Configuration and use it to construct a PixelFormat object
		PixelFormat generateConfig(JWIN_DISPLAY_CONTEXT, const JWIN_RAW_FBCONFIG&);

		//Retrieve all posiible Frame Buffer Configurations using a platform-specific display context
		jutil::Queue<PixelFormat> getAllConfigs(JWIN_DISPLAY_CONTEXT);

		//Find the nearest PixelFormat to the one given from the list given
		PixelFormat getNearestConfig(const PixelFormat&, const jutil::Queue<PixelFormat>&);

		//Test if the given config is actually usable
		bool validConfig(const PixelFormat&);

		//Use pixel format to construct an OpenGL context, represented by a ContextData object
		ContextData createContext(Handle, JWIN_DISPLAY_CONTEXT, const ContextSettings&);

		void setGLVersionMajor(int);
		void setGLVersionMinor(int);
		void makeContextCurrent(ContextData*);

		void populateSupportedExtensions(JWIN_DISPLAY_CONTEXT);
	}

	namespace window_manager {
		/*
			List of Handles referncing currently active windows.

			Attempting to pass unregistered Handles to JWin functions
			will result in errors.
		*/
		extern jutil::Queue<Handle> registeredWindows;

		bool init();
		void terminate();
	}

	namespace input_manager {

		extern jutil::Queue<Event::Action> keyboardState;
		extern jutil::Queue<Event::Key> keycodeTranslation;
		extern InputMode inputMode;



		void buildKeycodeTranslator();
		Event::Key translateKeycode(int);

		bool init();
		void terminate();
	}

	namespace _dmg = display_manager;
	namespace _wmg = window_manager;
	namespace _img = input_manager;
}

#endif
