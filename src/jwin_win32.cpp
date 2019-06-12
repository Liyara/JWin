#include <JWin/jwin_win32.h>
#ifdef JUTIL_WINDOWS

#include <JUtil/IO/IO.h>

namespace jwin {

	void init() {
		if (_dmg::createDisplay()) {
			_img::init();
			_wmg::init();
		}
	}

	void terminate() {
		_wmg::terminate();
		_img::terminate();
		_dmg::destroyDisplay();
	}

	void setContextHintPriority(ContextHint, float) {}

	bool setContextHint(int, int) {}
	bool setContextHint(int, bool) {}
	bool setContextHint(int, unsigned) {}

	namespace window_manager {

		WNDCLASSEX windowClass;
		jutil::Queue<Handle> registeredWindows;

		bool init() {
			return registerClass();
		}
		void terminate() {
			for (auto &i: registeredWindows) unregisterWindow(i);
		}

		bool registerClass() {

			ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

			windowClass.cbSize = sizeof(WNDCLASSEX);
			windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
			windowClass.lpfnWndProc = _img::eventProc;
			windowClass.hInstance = GetModuleHandle(NULL);
		    windowClass.lpszClassName = "JWin WIndow Class";

			windowClass.hIcon 	= LoadIcon (NULL, IDI_APPLICATION);
		    windowClass.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
		    windowClass.hCursor = LoadCursor (NULL, IDC_ARROW);
		    
		    return RegisterClassEx(&windowClass);
		}


		Handle registerWindow(const Monitor *monitor, const jutil::String &title, const Dimensions &size, const Position &position) {

			jml::Vector<int, 2> realSize = static_cast<jml::Vector<int, 2> >(size);
			jml::Vector<int, 2> realPosition = 
				(monitor->getPosition() + (static_cast<jml::Vector<int, 2> >(monitor->getSize()) / 2) + position) - (realSize / 2)
			;

			char *ctitle = new char[title.size() + 1];
			title.array(ctitle);

			HWND win;

		    win = CreateWindowEx(
		       	WS_EX_CLIENTEDGE,
		        windowClass.lpszClassName,
		        ctitle,
		        WS_OVERLAPPEDWINDOW,
		        realPosition.x(),
		        realPosition.y(),
		       	realSize.x(),
		        realSize.y(),
		        HWND_DESKTOP,
		        NULL,
		       	windowClass.hInstance,
		        NULL
		    );

		    delete[] ctitle;

		    if (win) {

		    	jutil::out << "Window registered!" << jutil::endl;

			    ShowWindow(win, SW_SHOW);

			    registeredWindows.insert(win);
			} else jutil::out << "Window failed to register!" << jutil::endl;

		    return win;
		}


		bool unregisterWindow(Handle handle) {
			for (auto &i: registeredWindows) {
				if (i == handle) {
					registeredWindows.erase(&i);
					DestroyWindow((HWND)handle);
					return true;
				}
			}
			return false;
		}

		bool windowIsRegistered(Handle handle) {
			return registeredWindows.find(handle);
		}

		bool issueCommands(Handle, unsigned, WindowAction) {}
		bool moveWindow(Handle, Position, const Geometry&) {}
		bool resizeWindow(Handle, const Dimensions&) {}
		bool toggleStatus(Handle, unsigned) {}
		bool isSet(Handle, WindowProperty) {}
	}

	namespace input_manager {
		jutil::Queue<InternalEvent> eventQueue;

		bool init() {return true;}
		void terminate() {
			eventQueue.clear();
		}

		const Position &getCursorPosition(Handle) {}

		const Event::Action &keyState(const Event::Key&) {}

		bool hasEvents(Handle handle) {
			for (auto &i: eventQueue) {
				if (i.window == handle) {
					return true;
				}
			}
			return false;
		}

		InternalEvent popEvent(Handle handle) {
			InternalEvent *r = nullptr;
			for (InternalEvent *it = eventQueue.begin(); it != eventQueue.end();) {
				if (it->window == handle) {
					if (it->event.type == Event::Type::NO_TYPE) {
						eventQueue.erase(it);
					} else {
						if (!r) {
							r = it;
							eventQueue.erase(it);
						} else ++it;
					}	
				} else ++it;
			}
			if (!r) {
				return InternalEvent();
			} else return *r;
		}

		void internalEventHandler(InternalEvent e) {
			//DefWindowProc(e.window, e.msg, e.wp, e.lp)
		}

		Event pollEvent(Handle handle, Geometry* geo) {
			if (!hasEvents(handle)) {
				MSG msg;
				if (PeekMessage(&msg, (HWND)handle, 0, 0, PM_REMOVE)) {
			        DispatchMessage(&msg);
			        return pollEvent(handle, geo);
				} else {
					return InternalEvent().event;
				}
			} else {
				InternalEvent e = popEvent(handle);
				internalEventHandler(e);
				return e.event;
			}
		}

		void setInputMode(InputMode) {}

		LRESULT CALLBACK eventProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

			InternalEvent e;
			e.window = hwnd;
			e.msg = msg;
			e.wp = wp;
			e.lp = lp;
			LRESULT r = 0;

			switch(msg) {
				case WM_CLOSE:
					e.event.type = Event::Type::CLOSE;
					break;

				case WM_DESTROY:
					PostQuitMessage(0);
					break;

				case WM_LBUTTONDOWN:
					e.event.type = Event::Type::MOUSE;
					e.event.code = Event::Button::LEFT;
					e.event.action = Event::Action::DOWN;
					break;

				case WM_LBUTTONUP:
					e.event.type = Event::Type::MOUSE;
					e.event.code = Event::Button::LEFT;
					e.event.action = Event::Action::UP;
					break;

				case WM_RBUTTONDOWN:
					e.event.type = Event::Type::MOUSE;
					e.event.code = Event::Button::RIGHT;
					e.event.action = Event::Action::DOWN;
					break;

				case WM_RBUTTONUP:
					e.event.type = Event::Type::MOUSE;
					e.event.code = Event::Button::RIGHT;
					e.event.action = Event::Action::UP;
					break;

				case WM_MBUTTONDOWN:
					e.event.type = Event::Type::MOUSE;
					e.event.code = Event::Button::MIDDLE;
					e.event.action = Event::Action::DOWN;
					break;

				case WM_MBUTTONUP:
					e.event.type = Event::Type::MOUSE;
					e.event.code = Event::Button::MIDDLE;
					e.event.action = Event::Action::UP;
					break;

				case WM_KEYDOWN:
					e.event.type = Event::Type::KEY;
					e.event.code = 0;
					e.event.action = Event::Action::DOWN;
					break;

				case WM_KEYUP:
					e.event.type = Event::Type::KEY;
					e.event.code = 0;
					e.event.action = Event::Action::UP;
					break;

				case WM_SIZE:
					InvalidateRect(hwnd, 0, TRUE);
					break;

				default: return DefWindowProc(hwnd, msg, wp, lp);
			}

			eventQueue.insert(e);

			return r;
		}
	}

	namespace display_manager {
		int CALLBACK cbMonitorInfo(HMONITOR__* monitor, HDC__*, tagRECT *rect, long int lpid) {
			size_t *id = reinterpret_cast<size_t*>(lpid);

			MONITORINFOEX info;
			info.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(monitor, &info);

			if (!(info.dwFlags & DISPLAY_DEVICE_MIRRORING_DRIVER)) {

				monitorData.monitors.insert(new Monitor(
					jutil::String(info.szDevice),
					static_cast<unsigned long>(*id),
					{static_cast<size_t>(info.rcMonitor.right - info.rcMonitor.left), static_cast<size_t>(info.rcMonitor.bottom - info.rcMonitor.top)},
					{info.rcMonitor.left, info.rcMonitor.top}
				));

				if (info.dwFlags & 1) monitorData.primaryMonitor = monitorData.monitors.last();
			}

			++(*id);

			return true;
		}

		bool createDisplay() {
			size_t id = 0;
			EnumDisplayMonitors(NULL, NULL, &cbMonitorInfo, reinterpret_cast<LPARAM>(&id));
			if (monitorData.monitors.empty()) {
				jutil::err << "JWin: Unable to find any connected monitors." << jutil::endl;
				return false;
			}
			else if (!monitorData.primaryMonitor) monitorData.primaryMonitor = monitorData.monitors.first();
			return true;
		}
		void destroyDisplay() {
			monitorData.primaryMonitor = nullptr;
			monitorData.monitors.clear();
		}
		const jutil::Queue<Monitor*> &getMonitors() {
			return monitorData.monitors;
		}
		const Monitor *getMonitor(size_t i) {
			return monitorData.monitors[i];
		}
		const Monitor *getPrimaryMonitor() {
			return monitorData.primaryMonitor;
		}
		const Monitor *pointInMonitor(const Position &pos) {
			for (auto &i: monitorData.monitors) {
				int 
					leftSide = i->getPosition().x(),
					rightSide = leftSide + i->getSize().x(),
					top = i->getPosition().y(),
					bottom = top + i->getSize().y()
				;

				if (pos.x() >= leftSide && pos.x() < rightSide && pos.y() >= top && pos.y() < bottom) return i;
			}

			return nullptr;
		}

		Position monitorToDisplay(const Monitor *m, const Position &pos) {
			return (m->getPosition() + (static_cast<jml::Vector<int, 2> >(m->getSize()) / 2) + pos);
		}

		Position displayToMonitor(const Position &pos, const Monitor **m) {
			const Monitor *nm = pointInMonitor(pos);
			if (nm) {
				if (m) *m = nm;
				return (pos - (static_cast<jml::Vector<int, 2> >(nm->getSize()) / 2) - nm->getPosition());
			} else {
				if (m) *m = nullptr;
				return 0;
			}
		}
		void setVSync(Handle, bool) {}
		void swapBuffers(Handle) {}
	}
}

#endif