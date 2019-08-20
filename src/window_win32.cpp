#include <JWin/jwin_win32.h>
#ifdef JUTIL_WINDOWS

#include <JUtil/Core/Thread.h>

namespace jwin {
    namespace window_manager {

		WNDCLASSEX windowClass;
		jutil::Queue<Handle> registeredWindows;
		jutil::Queue<WindowStateData> stateTable;

		void addToStateTable(Handle handle) {
		    jutil::Thread::requestGroupWait();
            for (auto &i: stateTable) if (i.first() == handle) return;
            stateTable.insert(WindowStateData(handle, 0));
            jutil::Thread::requestGroupResume();
		}

		void setWindowState(Handle handle, uint64_t state) {
		    jutil::Thread::requestGroupWait();
            for (auto &i: stateTable) {
                if (i.first() == handle) {
                    i.second() = state;
                    break;
                }
            }
            jutil::Thread::requestGroupResume();
		}

		uint64_t getWindowState(Handle handle) {
             for (auto &i: stateTable) {
                if (i.first() == handle) {
                    return i.second();
                }
             }
            return 0;
		}

		bool init() {
			return registerClass();
		}
		void terminate() {
			for (auto &i: registeredWindows) unregisterWindow(i);
			stateTable.clear();
		}

		bool registerClass() {

			ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

			windowClass.cbSize = sizeof(WNDCLASSEX);
			windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
			windowClass.lpfnWndProc = _img::eventProc;
			windowClass.hInstance = GetModuleHandle(NULL);
		    windowClass.lpszClassName = "JWin Window Class";

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

			    ShowWindow(win, SW_SHOW);

                jutil::Thread::requestGroupWait();
			    registeredWindows.insert(win);
                addToStateTable(win);
			    jutil::Thread::requestGroupResume();
			}

		    return win;
		}


		bool unregisterWindow(Handle handle) {
			for (auto &i: registeredWindows) {
				if (i == handle) {
					registeredWindows.erase(&i);
					DestroyWindow((HWND)handle);
					for (auto &ii: stateTable) {
                        if (ii.first() == handle) {
                            stateTable.erase(&ii);
                            break;
                        }
					}
					return true;
				}
			}
			return false;
		}

		bool windowIsRegistered(Handle handle) {
			return registeredWindows.find(handle);
		}

		WindowCommandState switchWindowAction(Handle win, WindowProperty property, WindowAction action) {
		    bool currentlySet = isSet(win, property);
            if (action == SET) {
                if (currentlySet) return COMMAND_STATE_KEEP;
                else return COMMAND_STATE_ENABLE;
            } else if (action == UNSET) {
                if (currentlySet) return COMMAND_STATE_DISABLE;
                else return COMMAND_STATE_KEEP;
            } else { //TOGGLE
                if (currentlySet) return COMMAND_STATE_DISABLE;
                else return COMMAND_STATE_ENABLE;
            }
		}

		void commandMinimize(Handle win, WindowCommandState commandState) {
            if (commandState == COMMAND_STATE_ENABLE) ShowWindow((HWND)win, SW_MINIMIZE);
            else if (commandState == COMMAND_STATE_DISABLE) ShowWindow((HWND)win, SW_RESTORE);
		}

		void commandMaximize(Handle win, WindowCommandState commandState) {
            if (commandState == COMMAND_STATE_ENABLE) ShowWindow((HWND)win, SW_MAXIMIZE);
		}

		void commandResizable(Handle win, WindowCommandState commandState) {
            if (commandState == COMMAND_STATE_ENABLE) setWindowState(win, getWindowState(win) & ~STATE_DISALLOW_RESIZE);
            else if (commandState == COMMAND_STATE_DISABLE) setWindowState(win, getWindowState(win) | STATE_DISALLOW_RESIZE);
		}

		void commandMovable(Handle win, WindowCommandState commandState) {
            if (commandState == COMMAND_STATE_ENABLE) setWindowState(win, getWindowState(win) & ~STATE_DISALLOW_MOVE);
            else if (commandState == COMMAND_STATE_DISABLE) setWindowState(win, getWindowState(win) | STATE_DISALLOW_MOVE);
		}

		void commandHideTaskbar(Handle win, WindowCommandState commandState) {
            if (commandState == COMMAND_STATE_ENABLE) ShowWindow((HWND)win, SW_HIDE);
            else if (commandState == COMMAND_STATE_DISABLE) ShowWindow((HWND)win, SW_SHOW);
		}

		void commandAlert(Handle win, WindowCommandState commandState) {
		    FLASHWINFO flashWindow = {0};
            flashWindow.cbSize = sizeof(FLASHWINFO);
            flashWindow.hwnd = (HWND)win;
            flashWindow.uCount = 1;
            if (commandState == COMMAND_STATE_ENABLE) {
                if (GetActiveWindow() != (HWND)win) setWindowState(win, getWindowState(win) | STATE_FLASHING);
                flashWindow.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
                FlashWindowEx(&flashWindow);
            } else if (commandState == COMMAND_STATE_DISABLE) {
                setWindowState(win, getWindowState(win) & ~STATE_FLASHING);
                flashWindow.dwFlags = FLASHW_STOP;
                FlashWindow((HWND)win, FALSE);
            }
		}

		void commandBorder(Handle win, WindowCommandState commandState) {
		    const LONG WINDOW_LONG = GetWindowLong((HWND)win, GWL_STYLE);
            const LONG WINDOW_LONG_EX = GetWindowLong((HWND)win, GWL_EXSTYLE);
            if (commandState == COMMAND_STATE_ENABLE) {
                SetWindowLong((HWND)win, GWL_STYLE, WINDOW_LONG | (WS_CAPTION | WS_SYSMENU | WS_THICKFRAME));
                SetWindowLong((HWND)win, GWL_EXSTYLE, WINDOW_LONG_EX | WS_EX_CLIENTEDGE);
            }
            else if (commandState == COMMAND_STATE_DISABLE) {
                SetWindowLong((HWND)win, GWL_STYLE, WINDOW_LONG & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_DLGFRAME));
                SetWindowLong((HWND)win, GWL_EXSTYLE, WINDOW_LONG_EX & ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE | WS_EX_WINDOWEDGE));
            }
		}

		void commandTop(Handle win, WindowCommandState commandState) {
            if (commandState == COMMAND_STATE_ENABLE) SetWindowPos((HWND)win, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            else if (commandState == COMMAND_STATE_DISABLE) SetWindowPos((HWND)win, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		void commandFullscreen(Handle win, WindowCommandState commandState) {
		    if (commandState == COMMAND_STATE_ENABLE) {
                RECT windowRect;
                    GetWindowRect((HWND)win, &windowRect);
                    const Monitor *monitor = _dmg::pointInMonitor({windowRect.left, windowRect.top});
                    if (!monitor) monitor = _dmg::getPrimaryMonitor();

                    RECT screenRect;
                    screenRect.left = monitor->getPosition().x();
                    screenRect.top = monitor->getPosition().y();
                    screenRect.right = screenRect.left + monitor->getSize().x();
                    screenRect.bottom = screenRect.top + monitor->getSize().y();

                    AdjustWindowRectEx(&screenRect, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, FALSE, WS_EX_APPWINDOW);

                    issueCommands(win, BORDER, UNSET);
                    SetWindowLong((HWND)win, GWL_EXSTYLE, WS_EX_APPWINDOW);

                    SetWindowPos((HWND)win, HWND_TOP, screenRect.left, screenRect.top, screenRect.right - screenRect.left, screenRect.bottom - screenRect.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		    } else if (commandState == COMMAND_STATE_DISABLE) {
                issueCommands(win, BORDER, SET);
                SetWindowLong((HWND)win, GWL_EXSTYLE, WS_EX_CLIENTEDGE);
                if (isSet(win, TOP)) {
                    SetWindowPos((HWND)win, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                } else {
                    SetWindowPos((HWND)win, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                }
            }
		}

		bool issueCommands(Handle win, unsigned commands, WindowAction action) {
		    if (commands & MINIMIZE) commandMinimize(win, switchWindowAction(win, MINIMIZE, action));
            if (commands & MAXIMIZE) commandMaximize(win, switchWindowAction(win, MAXIMIZE, action));
            if (commands & RESIZABLE) commandResizable(win, switchWindowAction(win, RESIZABLE, action));
            if (commands & MOVABLE) commandMovable(win, switchWindowAction(win, MOVABLE, action));
            if (commands & HIDE_TASKBAR) commandHideTaskbar(win, switchWindowAction(win, HIDE_TASKBAR, action));
            if (commands & ALERT) commandAlert(win, switchWindowAction(win, ALERT, action));
            if (commands & BORDER) commandBorder(win, switchWindowAction(win, BORDER, action));
            if (commands & TOP) commandTop(win, switchWindowAction(win, TOP, action));
            if (commands & FULLSCREEN) commandFullscreen(win, switchWindowAction(win, FULLSCREEN, action));
            return true;
		}
		bool moveWindow(Handle handle, Position pos) {
           if (isSet(handle, TOP)) {
                return SetWindowPos((HWND)handle, HWND_TOPMOST, pos.x(), pos.y(), 0, 0, SWP_NOSIZE);
            } else {
                return SetWindowPos((HWND)handle, HWND_NOTOPMOST, pos.x(), pos.y(), 0, 0, SWP_NOSIZE);
            }
		}
		bool resizeWindow(Handle handle, const Dimensions &size) {
            if (isSet(handle, TOP)) {
                return SetWindowPos((HWND)handle, HWND_TOPMOST, 0, 0, size.x(), size.y(), SWP_NOSIZE);
            } else {
                return SetWindowPos((HWND)handle, HWND_NOTOPMOST, 0, 0, size.x(), size.y(), SWP_NOMOVE);
            }
		}
		bool isSet(Handle win, WindowProperty property) {
            switch(property) {

            case MINIMIZE:
                return IsIconic((HWND)win);

            case MAXIMIZE: {
                WINDOWPLACEMENT wp = {0};
                wp.length = sizeof(WINDOWPLACEMENT);
                GetWindowPlacement((HWND)win, &wp);
                return wp.showCmd == SW_SHOWMAXIMIZED;
            }

            case RESIZABLE:
                return !(getWindowState(win) & STATE_DISALLOW_RESIZE);

            case MOVABLE:
                return !(getWindowState(win) & STATE_DISALLOW_MOVE);

            case HIDE_TASKBAR:
                return !(IsWindowVisible((HWND)win));

            case ALERT:
                if (getWindowState(win) & STATE_FLASHING) {
                    if (GetActiveWindow() == (HWND)win) {
                        setWindowState(win, getWindowState(win) & ~STATE_FLASHING);
                        return false;
                    } else return true;
                } else return false;

            case BORDER:
                return (
                    (GetWindowLong((HWND)win, GWL_STYLE) & (WS_THICKFRAME | WS_DLGFRAME)) ||
                    (GetWindowLong((HWND)win, GWL_EXSTYLE) & (WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE | WS_EX_WINDOWEDGE))
                );

            case TOP:
                return GetWindowLong((HWND)win, GWL_EXSTYLE) & WS_EX_TOPMOST;

            case FULLSCREEN:
                return GetWindowLong((HWND)win, GWL_EXSTYLE) == WS_EX_APPWINDOW;

            default: return false;

            }
		}
	}
}

#endif // JUTIL_WINDOWS
