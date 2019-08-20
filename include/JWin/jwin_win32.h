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

	void win32init(Handle);

	namespace window_manager {

		extern WNDCLASSEX windowClass;
		bool registerClass();

        typedef jutil::Pair<Handle, uint64_t> WindowStateData;
		extern jutil::Queue<WindowStateData> stateTable;

		enum WindowState : uint64_t {
            STATE_RESIZING = 0x01,
            STATE_RESIZING_LEFT = 0x02,
            STATE_RESIZING_RIGHT = 0x04,
            STATE_RESIZING_TOP = 0x08,
            STATE_RESIZING_BOTTOM = 0x10,
            STATE_RESIZING_BORDER = 0x02 | 0x04 | 0x08 | 0x10,
            STATE_MOVING = 0x20,
            STATE_DISALLOW_RESIZE = 0x40,
            STATE_DISALLOW_MOVE = 0x80,
            STATE_FLASHING = 0x100
		};

		enum WindowCommandState {
            COMMAND_STATE_ENABLE,
            COMMAND_STATE_DISABLE,
            COMMAND_STATE_KEEP
		};

		typedef void (WindowCommandHandler)(Handle, WindowCommandState);

		WindowCommandState switchWindowAction(Handle, WindowProperty, WindowAction);

		WindowCommandHandler
            commandMinimize,
            commandMaximize,
            commandResizable,
            commandMovable,
            commandHideTaskbar,
            commandAlert,
            commandBorder,
            commandTop,
            commandFullscreen
        ;

		void addToStateTable(Handle);
		void setWindowState(Handle, uint64_t);
		uint64_t getWindowState(Handle);
	}

	namespace input_manager {

		struct InternalEvent {
			Event event;
			Handle window;
			Geometry *geometry;
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

		enum CodeType {
            KEY_CODE,
            SCAN_CODE
		};

		Event::Key resolveKeycode(int, CodeType);

		extern jutil::Queue<InternalEvent> eventQueue;

		LRESULT CALLBACK eventProc(HWND, UINT, WPARAM, LPARAM);
		extern jutil::Queue<InternalEvent> eventQueue;
		bool hasEvents(Handle);
		InternalEvent popEvent(Handle);
		void internalEventHandler(InternalEvent);
		int getMods();
	}

	namespace display_manager {

		extern const jutil::Queue<int> ATTRIB_NAMES;

		int CALLBACK cbMonitorInfo(HMONITOR__*, HDC__*, tagRECT*, LPARAM);
	}
}

#endif
#endif
