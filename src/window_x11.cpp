#include <JWin/jwin_x11.h>
#ifdef JUTIL_LINUX

#include <JUtil/Core/Thread.h>
#include <stdlib.h>

namespace jwin {
	namespace window_manager {

		jutil::Queue<Handle> registeredWindows;

		const long WINDOW_EVENT_MASK = 
			ExposureMask 			| 
			ButtonPressMask 		| 
			KeyPressMask 			| 
			KeyReleaseMask 			| 
			ButtonReleaseMask 		| 
			EnterWindowMask 		| 
			LeaveWindowMask 		| 
			PointerMotionMask 		| 
			VisibilityChangeMask 	| 
			StructureNotifyMask 	| 
			SubstructureNotifyMask 	|  
			FocusChangeMask 		| 
			PropertyChangeMask
		;

		const unsigned STATE_COMMANDS = MAXIMIZE | MINIMIZE | FULLSCREEN | HIDE_TASKBAR | ALERT | TOP;
		const unsigned MOTIF_COMMANDS = MOVABLE | RESIZABLE | BORDER;

		bool init() {return true;}

		Handle registerWindow(const Monitor *monitor, const jutil::String &title, const Dimensions &size, const Position &position) {

			jutil::out << "Creating window..." << jutil::endl;

			XWindow *win;

			char *ctitle = new char[title.size() + 1];
			title.array(ctitle);

			jml::Vector<int, 2> realSize = static_cast<jml::Vector<int, 2> >(size);
			jml::Vector<int, 2> realPosition = 
				(monitor->getPosition() + (static_cast<jml::Vector<int, 2> >(monitor->getSize()) / 2) + position) - (realSize / 2)
			;

			auto _vinf = _dmg::displayData.contextData->settings.pixelFormat.platformConfig.visualInfo;

			XSetWindowAttributes wattr;
			wattr.background_pixel = 0;
			wattr.border_pixel = 0;
			wattr.override_redirect = True;

			wattr.colormap = XCreateColormap(
				_dmg::displayData.display, 
				_dmg::displayData.rootWindow, 
				_vinf->visual, 
				AllocNone
			);

			wattr.event_mask = ExposureMask;

		   	win = new XWindow(XCreateWindow(
		   		_dmg::displayData.display,
		   		_dmg::displayData.rootWindow,
		   		realPosition.x(),
		   		realPosition.y(),
				realSize.x(),
				realSize.y(),
				1,
				_vinf->depth,
				InputOutput,
				_vinf->visual,
				CWColormap | CWBorderPixel | CWEventMask,
				&wattr
			));

			if (win) {

				jutil::out << "Window built." << jutil::endl;

				_dmg::makeContextCurrent((Handle)win, _dmg::displayData.contextData);

			   	XSizeHints hints = {0};

				hints.flags  = PPosition | PSize;
				hints.x      = realPosition.x();
				hints.y      = realPosition.y();
				hints.width  = realSize.x();
				hints.height = realSize.y();

				XSetNormalHints(_dmg::displayData.display, *win, &hints);
				XSetStandardProperties(_dmg::displayData.display, *win, ctitle, "JWin Window", None, NULL, 0, NULL);

				XSelectInput(
					_dmg::displayData.display, 
					*win, 
					WINDOW_EVENT_MASK
				);

				XClearWindow(_dmg::displayData.display, *win);

				unsigned char *motifData = (unsigned char*)calloc(5, sizeof(long));
				unsigned long *udFlags = (unsigned long*)(motifData);
				unsigned long *udFunctions = udFlags + 1;
				unsigned long *udDecoratrions = udFunctions + 1;
				*udFunctions = 1;
				*udDecoratrions = 1;
				*udFlags = 0x01 | 0x02;

				XChangeProperty(
					_dmg::displayData.display,
					*((XWindow*)win),
					windowTypeUndecorated,
					windowTypeUndecorated,
					32,
					PropModeReplace,
					motifData,
					5
				);

				free(motifData);

				XMapRaised(_dmg::displayData.display, *win);

				XFlush(_dmg::displayData.display);

				XSetWMProtocols(_dmg::displayData.display, *win, &windowDestroy, 1);

				jutil::Thread::requestGroupWait();
				registeredWindows.insert(win);
				jutil::Thread::requestGroupResume();

				jutil::out << "Windows: " << registeredWindows.size() << jutil::endl;
			}

			delete[] ctitle;

			return (Handle)win;
		}

		bool unregisterWindow(Handle hwin) {
			if (hwin) {
				size_t wpos;
				XWindow *win = (XWindow*)hwin;
				if (registeredWindows.find(hwin, &wpos)) {
					jutil::Thread::requestGroupWait();
					registeredWindows.erase(wpos);
					if (!XDestroyWindow(_dmg::displayData.display, *win)) return false;
					jutil::Thread::requestGroupResume();
					delete win;
					hwin = nullptr;
					win = nullptr;
				} else return false;
			} else return false;
			return true;
		}

		bool windowIsRegistered(Handle handle) {
			if (handle) {
				return registeredWindows.find(handle);
			} else return false;
		}


		jutil::Queue<Atom> getWindowState(Handle win, Atom cat, Atom type) {
			Atom ua0;
			int ui0;
			unsigned long ul0;

			unsigned long numItems;
			unsigned char *items;

			XGetWindowProperty(
				_dmg::displayData.display,
				*((XWindow*)win),
				cat,
				0,
				0xff,
				False,
				type,
				&ua0,
				&ui0,
				&numItems,
				&ul0,
				&items
			);

			jutil::Queue<Atom> r;

			r.reserve(numItems);

			for (size_t i = 0; i < numItems; ++i) {
				r.insert(*(((Atom*)items) + i));
			}

			return r;
		}

		void remapWindow(Handle win, jutil::Queue<Atom> *state) {
			XSync(_dmg::displayData.display, True);
			XUnmapWindow(_dmg::displayData.display, *((XWindow*)win));
			XMapRaised(_dmg::displayData.display, *((XWindow*)win));
			XFlush(_dmg::displayData.display);
		}

		void switchToggle(WindowAction *action, jutil::Queue<Atom> *state, Atom atom) {
			if (*action == TOGGLE) {
				size_t index;
				if (state->find(atom, &index)) {
					*action = UNSET;
					state->erase(index);
				}
				else {
					*action = SET;
					state->insert(atom);
				}
			}
		}

		void createMessage(Handle win, Atom t, Atom a, Atom b) {
			XClientMessageEvent message = {0};
			message.type = ClientMessage;
			message.format = 32;
			message.window = *((XWindow*)win);
			message.message_type = t;
			message.data.l[0] = 1;
			message.data.l[1] = a;
			message.data.l[2] = b;
			message.data.l[3] = 1;
			XSendEvent(
				_dmg::displayData.display,
				_dmg::displayData.rootWindow,
				False,
				SubstructureNotifyMask,
				(XEvent*)&message
			);
		}

		void handleMaximizeCommand(Handle win, WindowAction action, jutil::Queue<Atom> *state) {
			if (action == SET) {
				bool send = false;
				if (!state->find(windowMaximizeX)) {
					state->insert(windowMaximizeX);
					send = true;
				}
				if (!state->find(windowAllowMaximizeY)) {
					state->insert(windowMaximizeY);
					send = true;
				}
				if (send) createMessage(win, windowState, windowMaximizeX, windowMaximizeY);
			} 
		}

		void handleMinimizeCommand(Handle win, WindowAction action, jutil::Queue<Atom> *state) {
			switchToggle(&action, state, windowMinimize);
			if (action == SET) XIconifyWindow(_dmg::displayData.display, *((XWindow*)win), _dmg::displayData.currentScreenID);
			else if (action == UNSET) remapWindow(win, state);
		}

		void handleFullscreenCommand(Handle win, WindowAction action, jutil::Queue<Atom> *state) {
			switchToggle(&action, state, windowFullscreen);
			if (action == SET) {
				createMessage(win, windowState, windowFullscreen, 0);
			} else if (action == UNSET) remapWindow(win, state);
		}

		void handleTaskbarCommand(Handle win, WindowAction action, jutil::Queue<Atom> *state, bool *changes) {
			if (action == TOGGLE) {
				switchToggle(&action, state, windowIgnoreTaskbar);
				if (!(*changes)) *changes = true;
			} else if (action == SET) {
				if (!state->find(windowIgnoreTaskbar)) {
					state->insert(windowIgnoreTaskbar);
					if (!(*changes)) *changes = true;
				}
			} else if (action == UNSET) {
				size_t index;
				if (state->find(windowIgnoreTaskbar, &index)) {
					state->erase(index);
					if (!(*changes)) *changes = true;
				}
			}
		}

		void handleAlertCommand(Handle win, WindowAction action, jutil::Queue<Atom> *state, bool *changes) {
			if (action == TOGGLE) {
				switchToggle(&action, state, windowAlert);
				if (!(*changes)) *changes = true;
			} else if (action == SET) {
				if (!state->find(windowAlert)) {
					state->insert(windowAlert);
					if (!(*changes)) *changes = true;
				}
			} else if (action == UNSET) {
				size_t index;
				if (state->find(windowAlert, &index)) {
					state->erase(index);
					if (!(*changes)) *changes = true;
				}
			}
		}

		void handleTopCommand(Handle win, WindowAction action, jutil::Queue<Atom> *state) {
			switchToggle(&action, state, windowOnTop);
			if (action == SET) {
				createMessage(win, windowState, windowOnTop, 0);
			} else if (action == UNSET) {
				remapWindow(win, state);
			}
		}

		void issueStateCommands(Handle win, unsigned commands, WindowAction action) {
			jutil::Queue<Atom> newState = getWindowState(win, windowState, XA_ATOM);
			bool changes = false;
			if (commands & MAXIMIZE) handleMaximizeCommand(win, action, &newState);
			if (commands & MINIMIZE) handleMinimizeCommand(win, action, &newState);
			if (commands & FULLSCREEN) handleFullscreenCommand(win, action, &newState);
			if (commands & TOP) handleTopCommand(win, action, &newState);
			if (commands & HIDE_TASKBAR) handleTaskbarCommand(win, action, &newState, &changes);
			if (commands & ALERT) handleAlertCommand(win, action, &newState, &changes);
			if (changes) {
				XChangeProperty(
					_dmg::displayData.display,
					*((XWindow*)win),
					windowState,
					XA_ATOM,
					32,
					PropModeReplace,
					reinterpret_cast<unsigned char*>(newState.getArray()),
					newState.size()
				);
			}
		}

		void handleMotifFunctionCommand(jutil::Queue<Atom> *state, WindowAction action, unsigned flags) {
			Atom &affected = (*state)[1];
			if (affected == 1) affected = 0x02 | 0x04 | 0x08 | 0x10 | 0x20;
			if (action == TOGGLE) {
				if (affected & flags) action = UNSET;
				else action = SET;
			}
			if (action == SET) affected |= flags;
			else if (action == UNSET) affected &= ~flags;
		}

		void handleMotifDecorationCommand(jutil::Queue<Atom> *state, WindowAction action) {
			Atom &affected = (*state)[2];
			if (action == TOGGLE) {
				if (affected) action = UNSET;
				else action = SET;
			}
			if (action == SET) affected = 1;
			else if (action == UNSET) affected = 0;
		}

		void issueMotifCommands(Handle win, unsigned commands, WindowAction action) {
			jutil::Queue<Atom> newState = getWindowState(win, windowTypeUndecorated, windowTypeUndecorated);
			if (commands & RESIZABLE) handleMotifFunctionCommand(&newState, action, 0x02);
			if (commands & MOVABLE) handleMotifFunctionCommand(&newState, action, 0x04);
			if (commands & BORDER) handleMotifDecorationCommand(&newState, action);
			XChangeProperty(
				_dmg::displayData.display,
				*((XWindow*)win),
				windowTypeUndecorated,
				windowTypeUndecorated,
				32,
				PropModeReplace,
				reinterpret_cast<unsigned char*>(newState.getArray()),
				5
			);
		}

		bool issueCommands(Handle win, unsigned commands, WindowAction action) {
			if (win && commands) {
				jutil::Thread::requestGroupWait();
				if (commands & STATE_COMMANDS) {
					issueStateCommands(win, commands & ~MOTIF_COMMANDS, action);
				}
				if (commands & MOTIF_COMMANDS) {
					issueMotifCommands(win, commands & ~STATE_COMMANDS, action);
				}
				jutil::Thread::requestGroupResume();
				return true;
			} else return false;
		}

		bool isSet(Handle win, WindowProperty prop) {
			jutil::Queue<Atom> propState;
			if (STATE_COMMANDS & (unsigned)prop) {
				propState = getWindowState(win, windowState, XA_ATOM);
				if (prop == FULLSCREEN) return propState.find(windowFullscreen);
				if (prop == MAXIMIZE) return (propState.find(windowMaximizeX) && propState.find(windowMaximizeY));
				if (prop == MINIMIZE) return propState.find(windowMinimize);
				if (prop == ALERT) return propState.find(windowAlert);
				if (prop == TOP) return propState.find(windowOnTop);
				if (prop == HIDE_TASKBAR) return propState.find(windowIgnoreTaskbar);
				return false;
			} else if (MOTIF_COMMANDS & (unsigned)prop) {
				propState = getWindowState(win, windowTypeUndecorated, windowTypeUndecorated);
				if (prop == MOVABLE) return (propState[1] == 1 || propState[1] & 0x04);
				if (prop == RESIZABLE) return (propState[1] == 1 || propState[1] & 0x02);
				if (prop == BORDER) return propState[2];
				return false;
			} else return false;
		}

		bool resizeWindow(Handle win, const Dimensions &d) {
			return XResizeWindow(_dmg::displayData.display, *((XWindow*)win), d.x(), d.y());
		}

		bool moveWindow(Handle win, Position p, const Geometry &geo) {
			p.x() -= geo.size.x() / 2;
			p.y() -= geo.size.y() / 2;
			return XMoveWindow(_dmg::displayData.display, *((XWindow*)win), p.x(), p.y());
		}

		void terminate() {
			jutil::Thread::requestGroupWait();
			for (auto &i: registeredWindows) unregisterWindow(i);
			registeredWindows.clear();
			jutil::Thread::requestGroupResume();
		}

	}
}

#endif