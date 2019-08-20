#include <JWin/jwin_win32.h>
#ifdef JUTIL_WINDOWS

namespace jwin {

	JWIN_CREATE_PROC_FORWARD(CreateContextAttribs);
	JWIN_CREATE_PROC_FORWARD(SwapIntervalEXT);
	JWIN_CREATE_PROC_FORWARD(GetPixelFormatAttribiv);
	JWIN_CREATE_PROC_FORWARD(GetExtensionsString);

	void init() {
		if (_dmg::createDisplay()) {
			_img::init();
			_wmg::init();
		}
	}

	bool fwininit = false;

	void win32init(Handle handle) {
	    if (fwininit) return;
		//Grab processes

		PIXELFORMATDESCRIPTOR pfd;
	    HGLRC tempContext;
	    HDC tempDC;

	    ZeroMemory(&pfd, sizeof(pfd));
	    pfd.nSize = sizeof(pfd);
	    pfd.nVersion = 1;
	    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	    pfd.iPixelType = PFD_TYPE_RGBA;
	    pfd.cColorBits = 32;

	    tempDC = GetDC((HWND)handle);

	    SetPixelFormat(tempDC, ChoosePixelFormat(tempDC, &pfd), &pfd);

	    tempContext = wglCreateContext(tempDC);
	    wglMakeCurrent(tempDC, tempContext);

	    JWIN_CREATE_PROC(CreateContextAttribs, wglCreateContextAttribsARB);
		JWIN_CREATE_PROC(SwapIntervalEXT, wglSwapIntervalEXT);
		JWIN_CREATE_PROC(GetPixelFormatAttribiv, wglGetPixelFormatAttribivARB);
		JWIN_CREATE_PROC(GetExtensionsString, wglGetExtensionsStringARB);

	    wglDeleteContext(tempContext);

	    fwininit = true;
	}

	void terminate() {
		_wmg::terminate();
		_img::terminate();
		_dmg::destroyDisplay();
	}

	void setContextHintPriority(ContextHint, float) {}

}

#endif
