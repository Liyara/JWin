#include <JWin/jwin_win32.h>
#ifdef JUTIL_WINDOWS

#include <JUtil/Core/Thread.h>

#define SIG_16 0x000000000000FFFF
#define SIG_24 0x0000000000FFFFFF

//For some reason, Microsoft forget to finish writing their macros, so I did it for them
#define VK_0    0x30
#define VK_1    0x31
#define VK_2    0x32
#define VK_3    0x33
#define VK_4    0x34
#define VK_5    0x35
#define VK_6    0x36
#define VK_7    0x37
#define VK_8    0x38
#define VK_9    0x39

#define VK_A    0x41
#define VK_B    0x42
#define VK_C    0x43
#define VK_D    0x44
#define VK_E    0x45
#define VK_F    0x46
#define VK_G    0x47
#define VK_H    0x48
#define VK_I    0x49
#define VK_J    0x4A
#define VK_K    0x4B
#define VK_L    0x4C
#define VK_M    0x4D
#define VK_N    0x4E
#define VK_O    0x4F
#define VK_P    0x50
#define VK_Q    0x51
#define VK_R    0x52
#define VK_S    0x53
#define VK_T    0x54
#define VK_U    0x55
#define VK_V    0x56
#define VK_W    0x57
#define VK_X    0x58
#define VK_Y    0x59
#define VK_Z    0x5A

#define VK_FN   0xFF

#define VSC_0   0x00B
#define VSC_1   0x002
#define VSC_2   0x003
#define VSC_3   0x004
#define VSC_4   0x005
#define VSC_5   0x006
#define VSC_6   0x007
#define VSC_7   0x008
#define VSC_8   0x009
#define VSC_9   0x00A

#define VSC_A   0x01E
#define VSC_B   0x030
#define VSC_C   0x02E
#define VSC_D   0x020
#define VSC_E   0x012
#define VSC_F   0x021
#define VSC_G   0x022
#define VSC_H   0x023
#define VSC_I   0x017
#define VSC_J   0x024
#define VSC_K   0x025
#define VSC_L   0x026
#define VSC_M   0x032
#define VSC_N   0x031
#define VSC_O   0x018
#define VSC_P   0x019
#define VSC_Q   0x010
#define VSC_R   0x013
#define VSC_S   0x01F
#define VSC_T   0x014
#define VSC_U   0x016
#define VSC_V   0x02F
#define VSC_W   0x011
#define VSC_X   0x02D
#define VSC_Y   0x015
#define VSC_Z   0x02C

#define VSC_APOSTROPHE      0x028
#define VSC_BACKSLASH       0x02B
#define VSC_COMMA           0x033
#define VSC_EQUAL           0x00D
#define VSC_GRAVE           0x029
#define VSC_BRACKET_OPEN    0x01A
#define VSC_MINUS           0x00C
#define VSC_PERIOD          0x034
#define VSC_BRACKET_CLOSE   0x01B
#define VSC_SEMICOLON       0x027
#define VSC_SLASH           0x035
#define VSC_WORLD_2         0x056
#define VSC_BACKSPACE       0x00E
#define VSC_DELETE          0x153
#define VSC_END             0x14F
#define VSC_RETURN          0x01C
#define VSC_ESCAPE          0x001
#define VSC_HOME            0x147
#define VSC_INSERT          0x152
#define VSC_MENU            0x15D
#define VSC_PAGE_DOWN       0x151
#define VSC_PAGE_UP         0x149
#define VSC_PAUSE           0x045
#define VSC_SPACE           0x039
#define VSC_TAB             0x00F
#define VSC_CAPS            0x03A
#define VSC_NUM             0x145
#define VSC_SCROLL          0x046

#define VSC_F1      0x03B
#define VSC_F2      0x03C
#define VSC_F3      0x03D
#define VSC_F4      0x03E
#define VSC_F5      0x03F
#define VSC_F6      0x040
#define VSC_F7      0x041
#define VSC_F8      0x042
#define VSC_F9      0x043
#define VSC_F10     0x044
#define VSC_F11     0x057
#define VSC_F12     0x058
#define VSC_F13     0x064
#define VSC_F14     0x065
#define VSC_F15     0x066
#define VSC_F16     0x067
#define VSC_F17     0x068
#define VSC_F18     0x069
#define VSC_F19     0x06A
#define VSC_F20     0x06B
#define VSC_F21     0x06C
#define VSC_F22     0x06D
#define VSC_F23     0x06E
#define VSC_F24     0x076

#define VSC_LALT    0x038
#define VSC_LCTRL   0x01D
#define VSC_LSHIFT  0x02A
#define VSC_LSUPER  0x15B
#define VSC_PRINT   0x137
#define VSC_RALT    0x138
#define VSC_RCTRL   0x11D
#define VSC_RSHIFT  0x036
#define VSC_RSUPER  0x15C
#define VSC_DOWN    0x150
#define VSC_LEFT    0x14B
#define VSC_RIGHT   0x14D
#define VSC_UP      0x148

#define VSC_KP_0            0x052
#define VSC_KP_1            0x04F
#define VSC_KP_2            0x050
#define VSC_KP_3            0x051
#define VSC_KP_4            0x04B
#define VSC_KP_5            0x04C
#define VSC_KP_6            0x04D
#define VSC_KP_7            0x047
#define VSC_KP_8            0x048
#define VSC_KP_9            0x049
#define VSC_KP_ADD          0x04E
#define VSC_KP_DECIMAL      0x053
#define VSC_KP_DIVIDE       0x135
#define VSC_KP_RETURN       0x11C
#define VSC_KP_EQUAL        0x059
#define VSC_KP_MULTIPLY     0x037
#define VSC_KP_SUBTRACT     0x04A

namespace jwin {
    namespace input_manager {

		jutil::Queue<InternalEvent> eventQueue;
		Position cursorPosition;

		bool init() {
		    inputMode = RAW;
		    keyboardState.reserve(Event::Key::__KEY_COUNT__);
		    keyboardState.resize(Event::Key::__KEY_COUNT__);
		    keycodeTranslation.reserve(0x200);
		    keycodeTranslation.resize(0x200);

		    for (auto &i: keyboardState) i = Event::Action::UP;
		    for (auto &i: keycodeTranslation) i = Event::Key::UNKNOWN;

		    buildKeycodeTranslator();
		    return true;
        }
		void terminate() {
			eventQueue.clear();
		}

		const Position &getCursorPosition(Handle win) {
            POINT cursor;
            GetCursorPos(&cursor);
            if (win) ScreenToClient((HWND)win, &cursor);
            jutil::Thread::requestGroupWait();
            cursorPosition.x() = cursor.x;
            cursorPosition.y() = cursor.y;
            jutil::Thread::requestGroupResume();
            return cursorPosition;
		}

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
                        jutil::Thread::requestGroupWait();
						eventQueue.erase(it);
                        jutil::Thread::requestGroupResume();
					} else {
						if (!r) {
							r = it;
							jutil::Thread::requestGroupWait();
							eventQueue.erase(it);
							jutil::Thread::requestGroupResume();
						} else ++it;
					}
				} else ++it;
			}
			if (!r) {
				return InternalEvent();
			} else return *r;
		}

		void internalEventHandler(InternalEvent e) {
			switch(e.event.type) {

            case Event::MOVE:
                jutil::Thread::requestGroupWait();
                e.geometry->position.x() = e.event.x;
                e.geometry->position.y() = e.event.y;
                jutil::Thread::requestGroupResume();
                break;

            case Event::RESIZE:
                jutil::Thread::requestGroupWait();
                e.geometry->size.x() = e.event.width;
                e.geometry->size.y() = e.event.height;
                jutil::Thread::requestGroupResume();
                break;

            default: break;

			}
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
				e.geometry = geo;
				internalEventHandler(e);
				return e.event;
			}
		}

        Event::Key translateKeycode(int ks) {
            switch(ks) {
				case VK_SPACE: 			return Event::Key::SPACE;
				case VK_OEM_7: 	        return Event::Key::APOS;
				case VK_OEM_COMMA: 		return Event::Key::COMMA;
				case VK_OEM_MINUS: 		return Event::Key::MINUS;
				case VK_OEM_PERIOD: 	return Event::Key::PERIOD;
				case VK_OEM_2: 			return Event::Key::SLASH;
				case VK_0: 				return Event::Key::ZERO;
				case VK_1: 				return Event::Key::ONE;
				case VK_2: 				return Event::Key::TWO;
				case VK_3: 				return Event::Key::THREE;
				case VK_4:		 		return Event::Key::FOUR;
				case VK_5: 				return Event::Key::FIVE;
				case VK_6: 				return Event::Key::SIX;
				case VK_7: 				return Event::Key::SEVEN;
				case VK_8: 				return Event::Key::EIGHT;
				case VK_9: 				return Event::Key::NINE;
				case VK_OEM_1: 		    return Event::Key::SEMICOLON;
				case VK_OEM_PLUS: 		return Event::Key::EQUAL;
				case VK_A: 				return Event::Key::A;
				case VK_B: 				return Event::Key::B;
				case VK_C: 				return Event::Key::C;
				case VK_D: 				return Event::Key::D;
				case VK_E: 				return Event::Key::E;
				case VK_F: 				return Event::Key::F;
				case VK_G: 				return Event::Key::G;
				case VK_H: 				return Event::Key::H;
				case VK_I: 				return Event::Key::I;
				case VK_J: 				return Event::Key::J;
				case VK_K: 				return Event::Key::K;
				case VK_L: 				return Event::Key::L;
				case VK_M:		 		return Event::Key::M;
				case VK_N: 				return Event::Key::N;
				case VK_O: 				return Event::Key::O;
				case VK_P: 				return Event::Key::P;
				case VK_Q: 				return Event::Key::Q;
				case VK_R: 				return Event::Key::R;
				case VK_S: 				return Event::Key::S;
				case VK_T: 				return Event::Key::T;
				case VK_U: 				return Event::Key::U;
				case VK_V: 				return Event::Key::V;
				case VK_W: 				return Event::Key::W;
				case VK_X: 				return Event::Key::X;
				case VK_Y: 				return Event::Key::Y;
				case VK_Z: 				return Event::Key::Z;
				case VK_OEM_4: 	        return Event::Key::BRACKET_OPEN;
				case VK_OEM_5: 		    return Event::Key::BACKSLASH;
				case VK_OEM_6: 	        return Event::Key::BRACKET_CLOSE;
				case VK_OEM_3: 			return Event::Key::GRAVE;
				case VK_ESCAPE: 		return Event::Key::ESCAPE;
				case VK_RETURN: 		return Event::Key::PERIOD;
				case VK_TAB: 			return Event::Key::TAB;
				case VK_BACK: 		    return Event::Key::BACKSPACE;
				case VK_INSERT: 		return Event::Key::INSERT;
				case VK_DELETE:         return (Event::Key)0x105;
				case VK_RIGHT: 			return Event::Key::ARROW_RIGHT;
				case VK_LEFT: 			return Event::Key::ARROW_LEFT;
				case VK_DOWN: 			return Event::Key::ARROW_DOWN;
				case VK_UP: 			return Event::Key::ARROW_UP;
				case VK_PRIOR: 		    return Event::Key::PAGE_UP;
				case VK_NEXT: 		    return Event::Key::PAGE_DOWN;
				case VK_HOME: 			return Event::Key::HOME;
				case VK_END: 			return Event::Key::END;
				case VK_CAPITAL: 		return Event::Key::CAPS;
				case VK_SCROLL: 	    return Event::Key::SCROLL;
				case VK_NUMLOCK: 		return Event::Key::NUM;
				case VK_PRINT: 			return Event::Key::PRINT;
				case VK_PAUSE: 			return Event::Key::PAUSE;
				case VK_F1: 			return Event::Key::F1;
				case VK_F2: 			return Event::Key::F2;
				case VK_F3: 			return Event::Key::F3;
				case VK_F4: 			return Event::Key::F4;
				case VK_F5: 			return Event::Key::F5;
				case VK_F6: 			return Event::Key::F6;
				case VK_F7: 			return Event::Key::F7;
				case VK_F8: 			return Event::Key::F8;
				case VK_F9: 			return Event::Key::F9;
				case VK_F10: 			return Event::Key::F10;
				case VK_F11: 			return Event::Key::F11;
				case VK_F12: 			return Event::Key::F12;
				case VK_F13: 			return Event::Key::F13;
				case VK_F14: 			return Event::Key::F14;
				case VK_F15: 			return Event::Key::F15;
				case VK_F16: 			return Event::Key::F16;
				case VK_F17: 			return Event::Key::F17;
				case VK_F18: 			return Event::Key::F18;
				case VK_F19: 			return Event::Key::F19;
				case VK_F20: 			return Event::Key::F20;
				case VK_F21: 			return Event::Key::F21;
				case VK_F22: 			return Event::Key::F22;
				case VK_F23: 			return Event::Key::F23;
				case VK_F24: 			return Event::Key::F24;
				case VK_LSHIFT: 		return Event::Key::LEFT_SHIFT;
				case VK_LCONTROL: 		return Event::Key::LEFT_CONTROL;
				case VK_LMENU: 			return Event::Key::LEFT_ALT;
				case VK_LWIN: 		    return Event::Key::LEFT_SUPER;
				case VK_RSHIFT: 		return Event::Key::RIGHT_SHIFT;
				case VK_RCONTROL: 		return Event::Key::RIGHT_CONTROL;
				case VK_RMENU: 			return Event::Key::RIGHT_ALT;
				case VK_RWIN: 		    return Event::Key::RIGHT_SUPER;
				default: 				return Event::Key::UNKNOWN;
			}
        }

        void buildKeycodeTranslator() {

            keycodeTranslation[VSC_0] = Event::Key::ZERO;
            keycodeTranslation[VSC_1] = Event::Key::ONE;
            keycodeTranslation[VSC_2] = Event::Key::TWO;
            keycodeTranslation[VSC_3] = Event::Key::THREE;
            keycodeTranslation[VSC_4] = Event::Key::FOUR;
            keycodeTranslation[VSC_5] = Event::Key::FIVE;
            keycodeTranslation[VSC_6] = Event::Key::SIX;
            keycodeTranslation[VSC_7] = Event::Key::SEVEN;
            keycodeTranslation[VSC_8] = Event::Key::EIGHT;
            keycodeTranslation[VSC_9] = Event::Key::NINE;

            keycodeTranslation[VSC_A] = Event::Key::A;
            keycodeTranslation[VSC_B] = Event::Key::B;
            keycodeTranslation[VSC_C] = Event::Key::C;
            keycodeTranslation[VSC_D] = Event::Key::D;
            keycodeTranslation[VSC_E] = Event::Key::E;
            keycodeTranslation[VSC_F] = Event::Key::F;
            keycodeTranslation[VSC_G] = Event::Key::G;
            keycodeTranslation[VSC_H] = Event::Key::H;
            keycodeTranslation[VSC_I] = Event::Key::I;
            keycodeTranslation[VSC_J] = Event::Key::J;
            keycodeTranslation[VSC_K] = Event::Key::K;
            keycodeTranslation[VSC_L] = Event::Key::L;
            keycodeTranslation[VSC_M] = Event::Key::M;
            keycodeTranslation[VSC_N] = Event::Key::N;
            keycodeTranslation[VSC_O] = Event::Key::O;
            keycodeTranslation[VSC_P] = Event::Key::P;
            keycodeTranslation[VSC_Q] = Event::Key::Q;
            keycodeTranslation[VSC_R] = Event::Key::R;
            keycodeTranslation[VSC_S] = Event::Key::S;
            keycodeTranslation[VSC_T] = Event::Key::T;
            keycodeTranslation[VSC_U] = Event::Key::U;
            keycodeTranslation[VSC_V] = Event::Key::V;
            keycodeTranslation[VSC_W] = Event::Key::W;
            keycodeTranslation[VSC_X] = Event::Key::X;
            keycodeTranslation[VSC_Y] = Event::Key::Y;
            keycodeTranslation[VSC_Z] = Event::Key::Z;

            keycodeTranslation[VSC_APOSTROPHE] = Event::Key::APOS;
            keycodeTranslation[VSC_BACKSLASH] = Event::Key::BACKSLASH;
            keycodeTranslation[VSC_COMMA] = Event::Key::COMMA;
            keycodeTranslation[VSC_EQUAL] = Event::Key::EQUAL;
            keycodeTranslation[VSC_GRAVE] = Event::Key::GRAVE;
            keycodeTranslation[VSC_BRACKET_OPEN] = Event::Key::BRACKET_OPEN;
            keycodeTranslation[VSC_MINUS] = Event::Key::MINUS;
            keycodeTranslation[VSC_PERIOD] = Event::Key::PERIOD;
            keycodeTranslation[VSC_BRACKET_CLOSE] = Event::Key::BRACKET_CLOSE;
            keycodeTranslation[VSC_SEMICOLON] = Event::Key::SEMICOLON;
            keycodeTranslation[VSC_SLASH] = Event::Key::SLASH;
            keycodeTranslation[VSC_BACKSPACE] = Event::Key::BACKSPACE;
            keycodeTranslation[VSC_DELETE] = (Event::Key)0x105;
            keycodeTranslation[VSC_END] = Event::Key::END;
            keycodeTranslation[VSC_RETURN] = Event::Key::ENTER;
            keycodeTranslation[VSC_ESCAPE] = Event::Key::ESCAPE;
            keycodeTranslation[VSC_HOME] = Event::Key::HOME;
            keycodeTranslation[VSC_INSERT] = Event::Key::INSERT;
            keycodeTranslation[VSC_PAGE_DOWN] = Event::Key::PAGE_DOWN;
            keycodeTranslation[VSC_PAGE_UP] = Event::Key::PAGE_UP;
            keycodeTranslation[VSC_PAUSE] = Event::Key::PAUSE;
            keycodeTranslation[VSC_SPACE] = Event::Key::SPACE;
            keycodeTranslation[VSC_TAB] = Event::Key::TAB;
            keycodeTranslation[VSC_CAPS] = Event::Key::CAPS;
            keycodeTranslation[VSC_NUM] = Event::Key::NUM;
            keycodeTranslation[VSC_SCROLL] = Event::Key::SCROLL;

            keycodeTranslation[VSC_F1] = Event::Key::F1;
            keycodeTranslation[VSC_F2] = Event::Key::F2;
            keycodeTranslation[VSC_F3] = Event::Key::F3;
            keycodeTranslation[VSC_F4] = Event::Key::F4;
            keycodeTranslation[VSC_F5] = Event::Key::F5;
            keycodeTranslation[VSC_F6] = Event::Key::F6;
            keycodeTranslation[VSC_F7] = Event::Key::F7;
            keycodeTranslation[VSC_F8] = Event::Key::F8;
            keycodeTranslation[VSC_F9] = Event::Key::F9;
            keycodeTranslation[VSC_F10] = Event::Key::F10;
            keycodeTranslation[VSC_F11] = Event::Key::F11;
            keycodeTranslation[VSC_F12] = Event::Key::F12;
            keycodeTranslation[VSC_F13] = Event::Key::F13;
            keycodeTranslation[VSC_F14] = Event::Key::F14;
            keycodeTranslation[VSC_F15] = Event::Key::F15;
            keycodeTranslation[VSC_F16] = Event::Key::F16;
            keycodeTranslation[VSC_F17] = Event::Key::F17;
            keycodeTranslation[VSC_F18] = Event::Key::F18;
            keycodeTranslation[VSC_F19] = Event::Key::F19;
            keycodeTranslation[VSC_F20] = Event::Key::F20;
            keycodeTranslation[VSC_F21] = Event::Key::F21;
            keycodeTranslation[VSC_F22] = Event::Key::F22;
            keycodeTranslation[VSC_F23] = Event::Key::F23;
            keycodeTranslation[VSC_F24] = Event::Key::F24;

            keycodeTranslation[VSC_LALT] = Event::Key::LEFT_ALT;
            keycodeTranslation[VSC_LCTRL] = Event::Key::LEFT_CONTROL;
            keycodeTranslation[VSC_LSHIFT] = Event::Key::LEFT_SHIFT;
            keycodeTranslation[VSC_LSUPER] = Event::Key::LEFT_SUPER;
            keycodeTranslation[VSC_PRINT] = Event::Key::PRINT;
            keycodeTranslation[VSC_RALT] = Event::Key::RIGHT_ALT;
            keycodeTranslation[VSC_RCTRL] = Event::Key::RIGHT_CONTROL;
            keycodeTranslation[VSC_RSHIFT] = Event::Key::RIGHT_SHIFT;
            keycodeTranslation[VSC_RSUPER] = Event::Key::RIGHT_SUPER;
            keycodeTranslation[VSC_DOWN] = Event::Key::ARROW_DOWN;
            keycodeTranslation[VSC_LEFT] = Event::Key::ARROW_LEFT;
            keycodeTranslation[VSC_RIGHT] = Event::Key::ARROW_RIGHT;
            keycodeTranslation[VSC_UP] = Event::Key::ARROW_UP;

            for (size_t i = 0; i < 0x200; ++i)
				if (keycodeTranslation[i] == Event::Key::UNKNOWN) keycodeTranslation[i] = translateKeycode(MapVirtualKey(i, MAPVK_VSC_TO_VK_EX));

        }

        Event::Key resolveKeycode(int code, CodeType type) {
			switch(inputMode) {
				case RAW:
				    if (type != SCAN_CODE) code = MapVirtualKey(code, MAPVK_VK_TO_VSC);
					return keycodeTranslation[code];

				case TRANSLATED:
				    if (type != KEY_CODE) code = MapVirtualKey(code, MAPVK_VSC_TO_VK);
					return translateKeycode(code);

				default: return Event::Key::UNKNOWN;
			}
		}

		int getMods() {
            int mods = 0;
            if (HIWORD(GetKeyState(VK_SHIFT))) mods |= (int)Event::Mod::SHIFT;
            if (HIWORD(GetKeyState(VK_CONTROL))) mods |= (int)Event::Mod::CONTROL;
            if (HIWORD(GetKeyState(VK_MENU))) mods |= Event::ALT;
            if (HIWORD(GetKeyState(VK_LWIN)) || HIWORD(GetKeyState(VK_RWIN))) mods |= Event::SUPER;
            if (HIWORD(GetKeyState(VK_FN))) mods |= Event::FUNCTION;
            if (LOWORD(GetKeyState(VK_CAPITAL))) mods |= Event::CAPS_LOCK;
            if (LOWORD(GetKeyState(VK_NUMLOCK))) mods |= Event::NUM_LOCK;
            if (LOWORD(GetKeyState(VK_SCROLL))) mods |= Event::SCROLL_LOCK;
            return mods;
		}

		bool handleMoveResize(HWND hwnd, WPARAM wp, LPARAM lp, bool *tracked) {

            uint64_t state = _wmg::getWindowState(hwnd);
            if ((state & _wmg::STATE_RESIZING) && !(state & _wmg::STATE_DISALLOW_RESIZE)) {
                RECT windowRect;
                POINT cursor;
                GetWindowRect(hwnd, &windowRect);
                GetCursorPos(&cursor);

                int xOffset = 0, yOffset = 0, xMove = 0, yMove = 0, nW = (windowRect.right - windowRect.left), nH = (windowRect.bottom - windowRect.top);

                if (state & _wmg::STATE_RESIZING_RIGHT) {
                    xOffset += cursor.x - windowRect.right;
                }

                if (state & _wmg::STATE_RESIZING_LEFT) {
                    xOffset += windowRect.left - cursor.x;
                    xMove += cursor.x - windowRect.left;
                }

                if (state & _wmg::STATE_RESIZING_BOTTOM) {
                    yOffset += cursor.y - windowRect.bottom;
                }

                if (state & _wmg::STATE_RESIZING_TOP) {
                    yOffset += windowRect.top - cursor.y;
                    yMove += cursor.y - windowRect.top;
                }

                nW += xOffset;
                nH += yOffset;

                SetWindowPos(
                    hwnd,
                    NULL,
                    windowRect.left + xMove,
                    windowRect.top + yMove,
                    nW,
                    nH,
                    0
                );
                return true;
            }
            else if ((state & _wmg::STATE_MOVING) && !(state & _wmg::STATE_DISALLOW_MOVE)) {
                int cx = (state >> 16) & SIG_24, cy = (state >> 40) & SIG_24;

                RECT windowRect;
                GetWindowRect(hwnd, &windowRect);

                POINT cursor;
                GetCursorPos(&cursor);

                int movex = cursor.x - cx, movey = cursor.y - cy, nx = windowRect.left + movex, ny = windowRect.top + movey;

                SetWindowPos(
                    hwnd,
                    NULL,
                    nx,
                    ny,
                    0,
                    0,
                    SWP_NOSIZE
                );

                GetWindowRect(hwnd, &windowRect);

                _wmg::setWindowState(
                    hwnd,
                    (state & SIG_16) |
                    ((((uint64_t)(cursor.x)) & SIG_24) << 16)  |
                    ((((uint64_t)(cursor.y)) & SIG_24) << 40)
                );
                return true;
            } else {
                if (tracked && !*tracked) {
                    TRACKMOUSEEVENT tme;
                    tme.cbSize = sizeof(TRACKMOUSEEVENT);
                    tme.dwFlags = TME_HOVER | TME_LEAVE;
                    tme.dwHoverTime = 1;
                    tme.hwndTrack = hwnd;
                    TrackMouseEvent(&tme);
                    *tracked = true;
                    return true;
                } else return false;
            }
		}

		LRESULT CALLBACK eventProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

		    static bool _window_tracked_ = false;

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
                    _wmg::unregisterWindow(hwnd);
					PostQuitMessage(0);
					return 0;

				case WM_LBUTTONDOWN:
					e.event.type = Event::Type::MOUSE;
					e.event.code = Event::Button::LEFT;
					e.event.action = Event::Action::DOWN;
					break;

                case WM_NCLBUTTONUP:
				case WM_LBUTTONUP:
				    if (_wmg::getWindowState(hwnd) & _wmg::STATE_RESIZING) {
                        _wmg::setWindowState(hwnd, _wmg::getWindowState(hwnd) & ~_wmg::STATE_RESIZING);
                        ReleaseCapture();
				    }
				    else if (_wmg::getWindowState(hwnd) & _wmg::STATE_MOVING) {
                        _wmg::setWindowState(hwnd, (_wmg::getWindowState(hwnd) & SIG_16) & ~_wmg::STATE_MOVING);
                        ReleaseCapture();
				    }
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

                case WM_SYSKEYDOWN:
				case WM_KEYDOWN:
					e.event.type = Event::Type::KEY;

					if (inputMode == RAW) e.event.code = resolveKeycode((lp >> 16) & 0x01FF, SCAN_CODE);
					else e.event.code = resolveKeycode(wp, KEY_CODE);

					if (e.event.code != Event::Key::UNKNOWN) keyboardState[e.event.code] = Event::Action::DOWN;

					if (lp >> 30) e.event.action = Event::Action::REPEAT;
					else e.event.action = Event::Action::DOWN;
					e.event.mods = getMods();

					break;

                case WM_SYSKEYUP:
				case WM_KEYUP: {
					e.event.type = Event::Type::KEY;

					if (inputMode == RAW) e.event.code = resolveKeycode((lp >> 16) & 0x01FF, SCAN_CODE);
					else e.event.code = resolveKeycode(wp, KEY_CODE);

					if (e.event.code != Event::Key::UNKNOWN) keyboardState[e.event.code] = Event::Action::UP;

					e.event.action = Event::Action::UP;
                    e.event.mods = getMods();

					break;

				}

                case WM_SYSCOMMAND: {
                    int cType = wp & 0xFFF0;
                    switch (cType) {

                    case SC_CLOSE:
                        DestroyWindow(hwnd);
                        break;

                    case SC_SIZE: {
                        if (_wmg::getWindowState(hwnd) & _wmg::STATE_DISALLOW_RESIZE) break;
                        uint64_t resizeState = 0;
                        LRESULT htest = DefWindowProc(hwnd, WM_NCHITTEST, wp, lp);
                        switch(htest) {

                        case HTTOP:
                            resizeState = _wmg::STATE_RESIZING_TOP;
                            break;

                        case HTTOPRIGHT:
                            resizeState = _wmg::STATE_RESIZING_RIGHT | _wmg::STATE_RESIZING_TOP;
                            break;

                        case HTRIGHT:
                            resizeState = _wmg::STATE_RESIZING_RIGHT;
                            break;

                        case HTBOTTOMRIGHT:
                            resizeState = _wmg::STATE_RESIZING_RIGHT | _wmg::STATE_RESIZING_BOTTOM;
                            break;

                        case HTBOTTOM:
                            resizeState = _wmg::STATE_RESIZING_BOTTOM;
                            break;

                        case HTBOTTOMLEFT:
                            resizeState = _wmg::STATE_RESIZING_LEFT | _wmg::STATE_RESIZING_BOTTOM;
                            break;

                        case HTLEFT:
                            resizeState = _wmg::STATE_RESIZING_LEFT;
                            break;

                        case HTTOPLEFT:
                            resizeState = _wmg::STATE_RESIZING_LEFT | _wmg::STATE_RESIZING_TOP;
                            break;

                        }
                        _wmg::setWindowState(hwnd, (_wmg::getWindowState(hwnd) & ~_wmg::STATE_RESIZING_BORDER) | _wmg::STATE_RESIZING | resizeState);
                        SetCapture(hwnd);
                        break;
                    }

                    case SC_MOVE: {
                        if (_wmg::getWindowState(hwnd) & _wmg::STATE_DISALLOW_MOVE) break;
                        uint64_t moveState = _wmg::STATE_MOVING | _wmg::getWindowState(hwnd);
                        moveState |= ((uint64_t)(GET_X_LPARAM(lp)) & SIG_24) << 16;
                        moveState |= ((uint64_t)(GET_Y_LPARAM(lp)) & SIG_24) << 40;
                        _wmg::setWindowState(hwnd, moveState);
                        SetCapture(hwnd);
                        break;
                    }

                    }
                    return DefWindowProc(hwnd, msg, wp, lp);
                }

                case WM_NCMOUSEMOVE:
                    handleMoveResize(hwnd, wp, lp, nullptr);
                    return DefWindowProc(hwnd, msg, wp, lp);

                case WM_MOUSEMOVE:
                    if (!handleMoveResize(hwnd, wp, lp, &_window_tracked_)) {
                        e.event.type = Event::Type::CURSOR;
                        e.event.x = GET_X_LPARAM(lp);
                        e.event.y = GET_Y_LPARAM(lp);
                    }
                    break;

                case WM_SIZE:
                    e.event.type = Event::Type::RESIZE;
                    e.event.width = GET_X_LPARAM(lp);
                    e.event.height = GET_Y_LPARAM(lp);
                    break;

                case WM_MOVE:
                    e.event.type = Event::Type::MOVE;
                    e.event.x = GET_X_LPARAM(lp);
                    e.event.y = GET_Y_LPARAM(lp);
                    if (e.event.x < 0) e.event.x = 0;
                    if (e.event.y < 0) e.event.y = 0;
                    break;

                case WM_MOUSEWHEEL: {
                    double scrolled = ((double)((SHORT)(wp >> 16)) / (double)WHEEL_DELTA);
                    e.event.type = Event::Type::MOUSE_SCROLL;
                    if (scrolled >= 0) e.event.action = Event::Action::UP;
                    else {
                        scrolled *= -1;
                        e.event.action = Event::Action::DOWN;
                    }

                    while ((scrolled - 1) > 0.000001) {
                        jutil::Thread::requestGroupWait();
                        eventQueue.insert(e);
                        jutil::Thread::requestGroupResume();
                        --scrolled;
                    }

                    break;
                }

                case WM_SETFOCUS:
                    e.event.type = Event::Type::GAIN_FOCUS;
                    break;

                case WM_KILLFOCUS:
                    e.event.type = Event::Type::LOSE_FOCUS;
                    break;

                case WM_MOUSELEAVE: {
                    _window_tracked_ = false;
                    POINT cursor;
                    e.event.type = Event::Type::CURSOR_LEAVE;
                    GetCursorPos(&cursor);
                    ScreenToClient(hwnd, &cursor);
                    if (cursor.x < 0) cursor.x = 0;
                    if (cursor.y < 0) cursor.y = 0;
                    e.event.x = cursor.x;
                    e.event.y = cursor.y;
                    break;
                }

                case WM_MOUSEHOVER: {
                    POINT cursor;
                    e.event.type = Event::Type::CURSOR_ENTER;
                    GetCursorPos(&cursor);
                    ScreenToClient(hwnd, &cursor);
                    if (cursor.x < 0) cursor.x = 0;
                    if (cursor.y < 0) cursor.y = 0;
                    e.event.x = cursor.x;
                    e.event.y = cursor.y;
                    break;
                }

                case WM_ACTIVATE: {
                    if (LOWORD(wp) > 0) {
                        uint64_t state = _wmg::getWindowState(hwnd);
                        if (state & _wmg::STATE_FLASHING) {
                            _wmg::setWindowState(hwnd, state & ~_wmg::STATE_FLASHING);
                        }
                    }
                    return DefWindowProc(hwnd, msg, wp, lp);
                }

				default: return DefWindowProc(hwnd, msg, wp, lp);
			}

            jutil::Thread::requestGroupWait();
			if (e.event.type != Event::Type::NO_TYPE) eventQueue.insert(e);
			jutil::Thread::requestGroupResume();

			return r;
		}
	}
}

#endif // JUTIL_WINDOWS
