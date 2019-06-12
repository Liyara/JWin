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

	jutil::Queue<jutil::String> supportedGLXExtensions;

	bool setContextHint(int setting, int value) {
		switch(setting) {
			case JWIN_SAMPLES:
				_dmg::idealConfig.samples = value;
				break;

			case JWIN_GL_VERSION_MAJOR:
				_dmg::setGLVersionMajor(value);
				break;

			case JWIN_GL_VERSION_MINOR:
				_dmg::setGLVersionMinor(value);
				break;

			default: return false;
		}
		return true;
	}

	bool setContextHint(int setting, bool value) {
		switch(setting) {
			case JWIN_TRANSPARENT:
				_dmg::idealConfig.transparent = value;
				break;

			case JWIN_STEREO:
				_dmg::idealConfig.stereoColor = value;
				break;

			default: return false;;
		}
		return true;
	} 

	bool setContextHint(int setting, unsigned value) {
		switch(setting) {
			case JWIN_MASK_ACCUM:
				_dmg::idealConfig.accumMask = value;
				break;

			case JWIN_MASK_DS:
				_dmg::idealConfig.attribs.dsMask = value;
				break;

			default: return false;
		}
		return true;
	}


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