#include <JWin/jwin.h>

#ifdef JUTIL_WINDOWS
    #include <JWin/jwin_win32.h>
#elif defined JUTIL_LINUX
    #include <JWin/jwin_x11.h>
#endif // JUTIL_WINDOWS

namespace jwin {
    bool setContextHint(int setting, int value) {
		switch(setting) {
			case JWIN_SAMPLES:
				_dmg::desiredContextSettings.pixelFormat.samples = value;
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
				_dmg::desiredContextSettings.pixelFormat.transparent = value;
				break;

			case JWIN_STEREO:
				_dmg::desiredContextSettings.pixelFormat.stereoColor = value;
				break;

			default: return false;
		}
		return true;
	}

	bool setContextHint(int setting, unsigned value) {
		switch(setting) {
			case JWIN_MASK_ACCUM:
				_dmg::desiredContextSettings.pixelFormat.accumMask = value;
				break;

			case JWIN_MASK_DS:
				_dmg::desiredContextSettings.pixelFormat.dsMask = value;
				break;

			default: return false;
		}
		return true;
	}
}
