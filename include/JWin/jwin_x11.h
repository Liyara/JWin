#ifndef JWIN_X11_H
#define JWIN_X11_H

#include <JUtil/Core/version.h>
#ifdef JUTIL_LINUX
#include <JWin/jwin_platform.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>
#include <X11/XKBlib.h>
#include <GL/glew.h>
#include <GL/glx.h>
#include <GL/glu.h>

#define JWIN_CREATE_PROC_EXTERN(_n, _p) extern JWinType_##_n JWin##_n;
#define JWIN_CREATE_PROC(_n, _p) JWinType_##_n JWin##_n = (JWinType_##_n) glXGetProcAddressARB((const GLubyte*)#_p);
#define JWIN_RGBA_32 0x08080808
#define JWIN_DS_32 0x00180008
#define JWIN_ACCUM_64 0x10101010

typedef Window XWindow;

namespace jwin {

	typedef XID ScreenID;
	typedef unsigned long Pixel;

	typedef GLXContext (*JWinType_CreateContextAttribs)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
	typedef void (*JWinType_SwapIntervalEXT)(Display*, GLXDrawable, int);
	typedef int (*JWinType_SwapIntervalMESA)(int);
	typedef int (*JWinType_SwapIntervalSGI)(int);

	JWIN_CREATE_PROC_EXTERN(CreateContextAttribs, glXCreateContextAttribsARB);
	JWIN_CREATE_PROC_EXTERN(SwapIntervalEXT, glXSwapIntervalEXT);
	JWIN_CREATE_PROC_EXTERN(SwapIntervalMESA, glXSwapIntervalMESA);
	JWIN_CREATE_PROC_EXTERN(SwapIntervalSGI, glXSwapIntervalSGI);

	extern Atom
		windowState,
		windowMaximizeX,
		windowMaximizeY,
		windowMinimize,
		windowIgnoreTaskbar,
		windowIgnorePager,
		windowFullscreen,
		windowAlert,
		windowOnTop,
		windowOnBottom,

		windowType,
		windowTypeDesktop,
		windowTypeDock,
		windowTypeToolbar,
		windowTypeMenu,
		windowTypeUtility,
		windowTypeSplash,
		windowTypeDialog,
		windowTypeNormal,
		windowTypeUndecorated,

		windowPermissions,
		windowAllowMove,
		windowAllowResize,
		windowAllowMinimize,
		windowAllowMaximizeX,
		windowAllowMaximizeY,
		windowAllowFullscreen,

		windowDestroy,
		windowFocus,
		windowOpacity
	;

	extern jutil::Queue<jutil::String> supportedGLXExtensions;

	namespace window_manager {

		//Events accepted by JWin windows
		extern const long WINDOW_EVENT_MASK;

		//Window command categories, handled by seperate internal properties
		extern const unsigned STATE_COMMANDS;
		extern const unsigned MOTIF_COMMANDS;

		//Internal helper functions needed when handling some commands
		jutil::Queue<Atom> getWindowState(Handle, Atom, Atom);
		void remapWindow(Handle, jutil::Queue<Atom>* = nullptr);
		void switchToggle(WindowAction*, jutil::Queue<Atom>*, Atom);
		void createMessage(Handle, Atom, Atom, Atom);

		//Handle commands sent by issueCommands()
		void handleMaximizeCommand(Handle, WindowAction, jutil::Queue<Atom>*);
		void handleMinimizeCommand(Handle, WindowAction, jutil::Queue<Atom>*);
		void handleFullscreenCommand(Handle, WindowAction, jutil::Queue<Atom>*);
		void handleTaskbarCommand(Handle, WindowAction, jutil::Queue<Atom>*, bool*);
		void handleAlertCommand(Handle, WindowAction, jutil::Queue<Atom>*, bool*);
		void handleTopCommand(Handle, WindowAction, jutil::Queue<Atom>*);
		void handleMotifFunctionCommand(jutil::Queue<Atom>*, WindowAction action, unsigned flags);
		void handleMotifDecorationCommand(jutil::Queue<Atom>*, WindowAction);

		//Helper functions for issueCommands()
		void issueStateCommands(Handle, unsigned, WindowAction);
		void issueMotifCommands(Handle, unsigned, WindowAction);

		void terminate();
	}

	namespace display_manager {

		//Attributes describing visual information for use by new windows.
		struct DisplayAttribs {
			GLboolean renderable;
			GLboolean doubleBuffered;
			GLint drawableType;
			GLint renderType;
			GLint visualType;
			GLuint rgbaMask;
			GLuint dsMask;
			DisplayAttribs() :
				renderable(True),
				doubleBuffered(True),
				drawableType(GLX_WINDOW_BIT),
				renderType(GLX_RGBA_BIT),
				visualType(GLX_TRUE_COLOR),
				rgbaMask(JWIN_RGBA_32),
				dsMask(JWIN_DS_32)
			{}
		};

		extern DisplayAttribs displayAttribs;

		typedef jutil::Queue<int> DisplayAttribArray;

		//Data structure representation of a GLXFBConfig
		struct FBConfig {
			XID id;
			XID vid;
			XRenderPictFormat *format;
			XVisualInfo *visualInfo;
			DisplayAttribs attribs;
			GLXFBConfig glxConfig;
			GLint bufferLevel;
			GLboolean stereoColor;
			GLint nAuxBuffers;
			GLuint accumMask;
			GLboolean transparent;
			GLint samples;
			FBConfig() :
				id(0),
				vid(0),
				format(nullptr),
				visualInfo(nullptr),
				bufferLevel(0),
				stereoColor(False),
				nAuxBuffers(0),
				accumMask(JWIN_ACCUM_64),
				transparent(False),
				samples(0)
			{}
		};

		extern FBConfig idealConfig;
		extern FBConfig currentConfig;

		struct ContextData {
			GLXContext context;
			FBConfig config;
		};

		//Data structure for internal data controlling the program's visuals
		struct DisplayData {
			Display *display;
			ContextData contextData;
			ScreenID currentScreenID;
			XWindow rootWindow;
			int glVersionMajor, glVersionMinor;
			DisplayData() :
				display(nullptr),
				currentScreenID(0),
				glVersionMajor(0),
				glVersionMinor(0)
			{}
		};

		extern DisplayData displayData;

		typedef jutil::Queue<FBConfig> FBConfigList;

		FBConfig generateConfig(const GLXFBConfig&);
		FBConfigList getAllConfigs();
		FBConfig getNearestConfig(const FBConfig&, const FBConfigList&);
		bool validConfig(const FBConfig&);
		ContextData createContext(const FBConfig&);
		void setGLVersionMajor(int);
		void setGLVersionMinor(int);
		void makeContextCurrent(Handle, const ContextData&);
	}

	namespace input_manager {
		extern jutil::Queue<Event::Action> keyboardState;
		extern jutil::Queue<Event::Key> keycodeTranslation;
		extern InputMode inputMode;

		void buildKeycodeTranslator();
		Event::Key translateKeycode(int);
		void buildKeycodeTranslator();
		Event::Key resolveKeycode(int);
		void keyEvent(const XEvent&, int, Event::Action, Event*);
		void buttonEvent(const XEvent&, Event*);
		void moveEvent(Event*, int, int);
		void resizeEvent(Event*, int, int);
	}
}

#endif
#endif