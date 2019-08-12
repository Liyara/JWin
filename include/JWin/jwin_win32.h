#ifndef JWIN_WIN32_H
#define JWIN_WIN32_H
#include <JWin/jwin_platform.h>
#ifdef JUTIL_WINDOWS

#define JWIN_CREATE_PROC_FORWARD(_n) JWinType_##_n JWin##_n;
#define JWIN_CREATE_PROC_EXTERN(_n) extern JWIN_CREATE_PROC_FORWARD(_n)
#define JWIN_CREATE_PROC(_n, _p) JWin##_n = (JWinType_##_n) wglGetProcAddress((LPCSTR)#_p);

namespace jwin {

	typedef BOOL (WINAPI *JWinType_SwapIntervalEXT)(int);
	typedef BOOL (WINAPI *JWinType_GetPixelFormatAttribiv)(HDC, int, int, UINT, const int*, int*);
	typedef const char * (WINAPI *JWinType_GetExtensionsString)(HDC);
	typedef HGLRC (WINAPI *JWinType_CreateContextAttribs)(HDC, HGLRC, const int*);

	JWIN_CREATE_PROC_EXTERN(CreateContextAttribs);
	JWIN_CREATE_PROC_EXTERN(SwapIntervalEXT);
	JWIN_CREATE_PROC_EXTERN(GetPixelFormatAttribiv);
	JWIN_CREATE_PROC_EXTERN(GetExtensionsString);

	namespace window_manager {

		extern WNDCLASSEX windowClass;
		bool registerClass();
	}

	namespace input_manager {

		struct InternalEvent {
			Event event;
			Handle window;
			UINT msg;
			WPARAM wp;
			LPARAM lp;
			InternalEvent() :
				window(nullptr),
				msg(0),
				wp(0),
				lp(0)
			{
				event.type = Event::Type::NO_TYPE;
			}
		};

		LRESULT CALLBACK eventProc(HWND, UINT, WPARAM, LPARAM);
		extern jutil::Queue<InternalEvent> eventQueue;
		bool hasEvents(Handle);
		InternalEvent popEvent(Handle);
		void internalEventHandler(Handle);
	}

	namespace display_manager {

		extern const jutil::Queue<int> ATTRIB_NAMES;

		int CALLBACK cbMonitorInfo(HMONITOR__*, HDC__*, tagRECT*, LPARAM);
	}
}

#endif
#endif