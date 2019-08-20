#include <JWin/jwin_x11.h>
#ifdef JUTIL_LINUX


namespace jwin {

	JWIN_CREATE_PROC(CreateContextAttribs, glXCreateContextAttribsARB);
	JWIN_CREATE_PROC(SwapIntervalEXT, glXSwapIntervalEXT);
	JWIN_CREATE_PROC(SwapIntervalMESA, glXSwapIntervalMESA);
	JWIN_CREATE_PROC(SwapIntervalSGI, glXSwapIntervalSGI);

	Atom
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


	void init() {
		XInitThreads();
		if (display_manager::createDisplay()) {
			input_manager::init();
			window_manager::init();
		}
	}

	void terminate() {
		window_manager::terminate();
		input_manager::terminate();
		display_manager::destroyDisplay();
	}
}

#endif
