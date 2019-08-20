#ifndef JWIN_X11_H
#define JWIN_X11_H

#include <JWin/jwin_platform.h>
#ifdef JUTIL_LINUX

#define JWIN_CREATE_PROC_EXTERN(_n, _p) extern JWinType_##_n JWin##_n;
#define JWIN_CREATE_PROC(_n, _p) JWinType_##_n JWin##_n = (JWinType_##_n) glXGetProcAddressARB((const GLubyte*)#_p);

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

		//Data structure for internal data controlling the program's visuals
		struct DisplayData {
			Display *display;
			ContextData *contextData;
			ScreenID currentScreenID;
			XWindow rootWindow;
			DisplayData() :
				display(nullptr),
				contextData(nullptr),
				currentScreenID(0)
			{}
		};

		extern DisplayData displayData;
	}

	namespace input_manager {


		void keyEvent(const XEvent&, int, Event::Action, Event*);
		void buttonEvent(const XEvent&, Event*);
		void moveEvent(Event*, int, int);
		void resizeEvent(Event*, int, int);
		Event::Key resolveKeycode(int);
	}
}

#endif
#endif
