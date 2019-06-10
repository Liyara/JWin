#include <JWin/jwin_x11.h>
#ifdef JUTIL_LINUX

#include <JUtil/Core/limits.hpp>
#include <JUtil/Core/Thread.h>
#include <JUtil/Core/Pair.hpp>
#include <JUtil/Core/Timer.h>

namespace jwin {
	namespace input_manager {

		jutil::Queue<Event::Action> keyboardState;
		jutil::Queue<Event::Key> keycodeTranslation;
		InputMode inputMode;
		Position cursorPosition;
		
		void init() {
			inputMode = RAW;
			keyboardState.reserve(Event::Key::__KEY_COUNT__);
			keyboardState.resize(Event::Key::__KEY_COUNT__);
			keycodeTranslation.reserve(0x100);
			keycodeTranslation.resize(0x100);

			jutil::out << "Input initializing..." << jutil::endl;

			buildKeycodeTranslator();
		}

		void setInputMode(InputMode im) {
			jutil::Thread::requestGroupWait();
			inputMode = im;
			jutil::Thread::requestGroupResume();
		}

		Event::Key translateKeycode(int code) {
			KeySym ks = XkbKeycodeToKeysym(_dmg::displayData.display, code, 0, 0);
			switch(ks) {
				case XK_space: 			return Event::Key::SPACE;
				case XK_apostrophe: 	return Event::Key::APOS;
				case XK_comma: 			return Event::Key::COMMA;
				case XK_minus: 			return Event::Key::MINUS;
				case XK_period: 		return Event::Key::PERIOD;
				case XK_slash: 			return Event::Key::SLASH;
				case XK_0: 				return Event::Key::ZERO;
				case XK_1: 				return Event::Key::ONE;
				case XK_2: 				return Event::Key::TWO;
				case XK_3: 				return Event::Key::THREE;
				case XK_4:		 		return Event::Key::FOUR;
				case XK_5: 				return Event::Key::FIVE;
				case XK_6: 				return Event::Key::SIX;
				case XK_7: 				return Event::Key::SEVEN;
				case XK_8: 				return Event::Key::EIGHT;
				case XK_9: 				return Event::Key::NINE;
				case XK_semicolon: 		return Event::Key::SEMICOLON;
				case XK_equal: 			return Event::Key::EQUAL;
				case XK_a: 				return Event::Key::A;
				case XK_b: 				return Event::Key::B;
				case XK_c: 				return Event::Key::C;
				case XK_d: 				return Event::Key::D;
				case XK_e: 				return Event::Key::E;
				case XK_f: 				return Event::Key::F;
				case XK_g: 				return Event::Key::G;
				case XK_h: 				return Event::Key::H;
				case XK_i: 				return Event::Key::I;
				case XK_j: 				return Event::Key::J;
				case XK_k: 				return Event::Key::K;
				case XK_l: 				return Event::Key::L;
				case XK_m:		 		return Event::Key::M;
				case XK_n: 				return Event::Key::N;
				case XK_o: 				return Event::Key::O;
				case XK_p: 				return Event::Key::P;
				case XK_q: 				return Event::Key::Q;
				case XK_r: 				return Event::Key::R;
				case XK_s: 				return Event::Key::S;
				case XK_t: 				return Event::Key::T;
				case XK_u: 				return Event::Key::U;
				case XK_v: 				return Event::Key::V;
				case XK_w: 				return Event::Key::W;
				case XK_x: 				return Event::Key::X;
				case XK_y: 				return Event::Key::Y;
				case XK_z: 				return Event::Key::Z;
				case XK_bracketleft: 	return Event::Key::BRACKET_OPEN;
				case XK_backslash: 		return Event::Key::BACKSLASH;
				case XK_bracketright: 	return Event::Key::BRACKET_CLOSE;
				case XK_grave: 			return Event::Key::GRAVE;
				case XK_Escape: 		return Event::Key::ESCAPE;
				case XK_Return: 		return Event::Key::PERIOD;
				case XK_Tab: 			return Event::Key::TAB;
				case XK_BackSpace: 		return Event::Key::BACKSPACE;
				case XK_Insert: 		return Event::Key::INSERT;
				case XK_Delete: 		return Event::Key::DELETE;
				case XK_Right: 			return Event::Key::ARROW_RIGHT;
				case XK_Left: 			return Event::Key::ARROW_LEFT;
				case XK_Down: 			return Event::Key::ARROW_DOWN;
				case XK_Up: 			return Event::Key::ARROW_UP;
				case XK_Page_Up: 		return Event::Key::PAGE_UP;
				case XK_Page_Down: 		return Event::Key::PAGE_DOWN;
				case XK_Home: 			return Event::Key::HOME;
				case XK_End: 			return Event::Key::END;
				case XK_Caps_Lock: 		return Event::Key::CAPS;
				case XK_Scroll_Lock: 	return Event::Key::SCROLL;
				case XK_Num_Lock: 		return Event::Key::NUM;
				case XK_Print: 			return Event::Key::PRINT;
				case XK_Pause: 			return Event::Key::PAUSE;
				case XK_F1: 			return Event::Key::F1;
				case XK_F2: 			return Event::Key::F2;
				case XK_F3: 			return Event::Key::F3;
				case XK_F4: 			return Event::Key::F4;
				case XK_F5: 			return Event::Key::F5;
				case XK_F6: 			return Event::Key::F6;
				case XK_F7: 			return Event::Key::F7;
				case XK_F8: 			return Event::Key::F8;
				case XK_F9: 			return Event::Key::F9;
				case XK_F10: 			return Event::Key::F10;
				case XK_F11: 			return Event::Key::F11;
				case XK_F12: 			return Event::Key::F12;
				case XK_F13: 			return Event::Key::F13;
				case XK_F14: 			return Event::Key::F14;
				case XK_F15: 			return Event::Key::F15;
				case XK_F16: 			return Event::Key::F16;
				case XK_F17: 			return Event::Key::F17;
				case XK_F18: 			return Event::Key::F18;
				case XK_F19: 			return Event::Key::F19;
				case XK_F20: 			return Event::Key::F20;
				case XK_F21: 			return Event::Key::F21;
				case XK_F22: 			return Event::Key::F22;
				case XK_F23: 			return Event::Key::F23;
				case XK_F24: 			return Event::Key::F24;
				case XK_F25: 			return Event::Key::F25;
				case XK_Shift_L: 		return Event::Key::LEFT_SHIFT;
				case XK_Control_L: 		return Event::Key::LEFT_CONTROL;
				case XK_Alt_L: 			return Event::Key::LEFT_ALT;
				case XK_Super_L: 		return Event::Key::LEFT_SUPER;
				case XK_Shift_R: 		return Event::Key::RIGHT_SHIFT;
				case XK_Control_R: 		return Event::Key::RIGHT_CONTROL;
				case XK_Alt_R: 			return Event::Key::RIGHT_ALT;
				case XK_Super_R: 		return Event::Key::RIGHT_SUPER;
				default: 				return Event::Key::UNKNOWN;
			}
		}

		void buildKeycodeTranslator() {
			char keyName[XkbKeyNameLength + 1];
			XkbDescPtr kdp = XkbGetMap(_dmg::displayData.display, 0, XkbUseCoreKbd);
			XkbGetNames(_dmg::displayData.display, XkbKeyNamesMask, kdp);

			typedef jutil::Pair<jutil::String, Event::Key> _K;

			jutil::Queue<_K> keyNameTable = {
				_K("TLDE", 	Event::Key::GRAVE),
				_K("AE01", 	Event::Key::ONE),
				_K("AE02", 	Event::Key::TWO),
				_K("AE03", 	Event::Key::THREE),
				_K("AE04", 	Event::Key::FOUR),
				_K("AE05", 	Event::Key::FIVE),
				_K("AE06", 	Event::Key::SIX),
				_K("AE07", 	Event::Key::SEVEN),
				_K("AE08", 	Event::Key::EIGHT),
				_K("AE09", 	Event::Key::NINE),
				_K("AE10", 	Event::Key::ZERO),
				_K("AE11", 	Event::Key::MINUS),
				_K("AE12", 	Event::Key::EQUAL),
				_K("AD01", 	Event::Key::Q),
				_K("AD02", 	Event::Key::W),
				_K("AD03", 	Event::Key::E),
				_K("AD04", 	Event::Key::R),
				_K("AD05", 	Event::Key::T),
				_K("AD06", 	Event::Key::Y),
				_K("AD07", 	Event::Key::U),
				_K("AD08", 	Event::Key::I),
				_K("AD09", 	Event::Key::O),
				_K("AD10", 	Event::Key::P),
				_K("AD11", 	Event::Key::BRACKET_OPEN),
				_K("AD12", 	Event::Key::BRACKET_CLOSE),
				_K("BKSL", 	Event::Key::BACKSLASH),
				_K("AC01", 	Event::Key::A),
				_K("AC02", 	Event::Key::S),
				_K("AC03", 	Event::Key::D),
				_K("AC04", 	Event::Key::F),
				_K("AC05", 	Event::Key::G),
				_K("AC06", 	Event::Key::H),
				_K("AC07", 	Event::Key::J),
				_K("AC08", 	Event::Key::K),
				_K("AC09", 	Event::Key::L),
				_K("AC10", 	Event::Key::SEMICOLON),
				_K("AC11", 	Event::Key::APOS),
				_K("AB01", 	Event::Key::Z),
				_K("AB02", 	Event::Key::X),
				_K("AB03", 	Event::Key::C),
				_K("AB04", 	Event::Key::V),
				_K("AB05", 	Event::Key::B),
				_K("AB06", 	Event::Key::N),
				_K("AB07", 	Event::Key::M),
				_K("AB08", 	Event::Key::COMMA),
				_K("AB09", 	Event::Key::PERIOD),
				_K("AB10", 	Event::Key::SLASH),
				_K("SPCE", 	Event::Key::SPACE),
				_K("UP",   	Event::Key::ARROW_UP),
				_K("LEFT", 	Event::Key::ARROW_LEFT),
				_K("RIGHT", Event::Key::ARROW_RIGHT),
				_K("DOWN", 	Event::Key::ARROW_DOWN)
			};

			for (auto &i: keycodeTranslation) i = Event::Key::UNKNOWN;

			for (int code = kdp->min_key_code; code <= kdp->max_key_code; ++code) {
				memcpy(keyName, kdp->names->keys[code].name, XkbKeyNameLength);
				keyName[XkbKeyNameLength + 1] = 0;

				for (auto &i: keyNameTable) {
					if (i.first() == keyName) {
						keycodeTranslation[code] = i.second();
						break;
					}
				}
			}

			for (size_t i = 0; i < 0x100; ++i)
				if (keycodeTranslation[i] == Event::Key::UNKNOWN) keycodeTranslation[i] = translateKeycode(i);

		}

		Event::Key resolveKeycode(int kc) {
			switch(inputMode) {
				case RAW:
					return keycodeTranslation[kc];

				case TRANSLATED:
					return translateKeycode(kc);

				default: return Event::Key::UNKNOWN;
			}
		}

		void keyEvent(const XEvent &e, int kcr, Event::Action action, Event *r) {
			r->type = Event::Type::KEY;
			if (action == Event::Action::DOWN && keyboardState[kcr] == Event::Action::DOWN) {
				r->action = Event::Action::REPEAT;
			} else {
				r->action = action;
			}
			r->code = kcr;
			r->mods = 0;
			if (e.xkey.state & ShiftMask) r->mods |= Event::Mod::SHIFT;
			if (e.xkey.state & ControlMask) r->mods |= Event::Mod::CONTROL;
			if (e.xkey.state & Mod1Mask) r->mods |= Event::Mod::ALT;
			if (e.xkey.state & Mod4Mask) r->mods |= Event::Mod::SUPER;
			if (e.xkey.state & Mod5Mask) r->mods |= Event::Mod::FUNCTION;
			r->x = e.xkey.x;
			r->y = e.xkey.y;
			r->time = e.xkey.time;
			keyboardState[kcr] = (action == Event::Action::REPEAT? Event::Action::DOWN : action);
		}

		void buttonEvent(const XEvent &e, Event *r) {
			if (e.xbutton.button < 4 || e.xbutton.button > 5) {
				r->type = Event::Type::MOUSE;
				r->action = Event::Action::DOWN;
			} else {
				r->type = Event::Type::MOUSE_SCROLL;
				if (e.xbutton.button == 4) r->action = Event::Action::UP;
				else r->action = Event::Action::DOWN;
			}
			switch(e.xbutton.button) {
				case 0:

				case 1:
					r->code = Event::Button::LEFT;
					break;

				case 2:
					r->code = Event::Button::MIDDLE;
					break;

				case 3:
					r->code = Event::Button::RIGHT;
					break;

				case 4:
					r->code = Event::Button::MIDDLE;
					break;

				case 5:
					r->code = Event::Button::MIDDLE;
					break;

				default:
					r->code = Event::Button::SPECIAL + (e.xbutton.button - 6);
			}
			r->x = e.xbutton.x;
			r->y = e.xbutton.y;
			r->time = e.xbutton.time;
		}

		void moveEvent(Event *r, int x, int y) {
			r->type = Event::Type::MOVE;
			r->x = x;
			r->y = y;
		}

		void resizeEvent(Event *r, int x, int y) {
			r->type = Event::Type::RESIZE;
			r->width = x;
			r->height = y;
		}


		const Position &getCursorPosition(Handle) {
			XWindow rr, cr;
			int a, b;
			unsigned int m;
			if (_dmg::displayData.display) 
				XQueryPointer(_dmg::displayData.display, _dmg::displayData.rootWindow, &rr, &cr, &a, &b, &(cursorPosition.x()), &(cursorPosition.y()), &m);
			return cursorPosition;
		}

		const Event::Action &keyState(const Event::Key &k) {
			return keyboardState[k];
		}

		Event pollEvent(Handle handle, Geometry *geo) {
			Event r;
			XEvent e, ne;
			int kcr;
			r.type = Event::Type::NO_TYPE;
			if (window_manager::windowIsRegistered(handle)) {
				if (XCheckWindowEvent(_dmg::displayData.display, *((XWindow*)(handle)), window_manager::WINDOW_EVENT_MASK, &e)) {
					switch(e.type) {
						case Expose:
							if (e.xexpose.count == 0) {
								XClearWindow(_dmg::displayData.display, *((XWindow*)(handle)));
								r.type = Event::Type::EXPOSE;
							}
							break;

						case ConfigureNotify:
							if (geo) {
								int nw = static_cast<int>(geo->size.x());
								int nh = static_cast<int>(geo->size.y());
								int nx = e.xconfigure.x + (e.xconfigure.width / 2);
								int ny = e.xconfigure.y + (e.xconfigure.height / 2);
								if (e.xconfigure.width != nw || e.xconfigure.height != nh) {
									resizeEvent(&r, e.xconfigure.width, e.xconfigure.height);
									geo->size.x() = static_cast<size_t>(e.xconfigure.width);
									geo->size.y() = static_cast<size_t>(e.xconfigure.height);
								} else if (nx != geo->position.x() || ny != geo->position.y()) {
									moveEvent(&r, nx, ny);
									geo->position.x() = nx;
									geo->position.y() = ny;
								}
							}
							break;

						case KeyPress:
							kcr = resolveKeycode(e.xkey.keycode);
							keyEvent(e, kcr, Event::Action::DOWN, &r);
							break;

						case KeyRelease:
							kcr = resolveKeycode(e.xkey.keycode);
							keyEvent(e, kcr, Event::Action::UP, &r);

							//Check if an extra KeyPress was sent at the same time, indicating that this KeyRelease is not real.
							if (XCheckWindowEvent(_dmg::displayData.display, *((XWindow*)(handle)), window_manager::WINDOW_EVENT_MASK, &ne)) {
								if (ne.type == KeyPress && e.xkey.time == ne.xkey.time && ne.xkey.keycode == e.xkey.keycode) {
									//If so, then the keyboard state did not really change (key is being held down)
									keyboardState[kcr] = Event::Action::DOWN;
									r.action = Event::Action::REPEAT;
								} else XPutBackEvent(_dmg::displayData.display, &ne);
							}
							break;

						case ButtonPress:
							buttonEvent(e, &r);
							break;

						case ButtonRelease:
							buttonEvent(e, &r);
							if (r.type == Event::Type::MOUSE && r.action == Event::Action::DOWN) r.action = Event::Action::UP;
							break;

						case MotionNotify:
							r.type = Event::Type::CURSOR;
							r.x = e.xmotion.x;
							r.y = e.xmotion.y;
							break;

						case EnterNotify:
							r.type = Event::Type::CURSOR_ENTER;
							r.x = e.xcrossing.x;
							r.y = e.xcrossing.y;
							break;

						case LeaveNotify:
							r.type = Event::Type::CURSOR_LEAVE;
							r.x = e.xcrossing.x;
							r.y = e.xcrossing.y;
							break;

						case FocusIn:
							window_manager::issueCommands(handle, window_manager::ALERT, window_manager::UNSET);
							r.type = Event::Type::GAIN_FOCUS;
							break;

						case FocusOut:
							r.type = Event::Type::LOSE_FOCUS;
							break;
					}

				} else if (XCheckTypedWindowEvent(_dmg::displayData.display, *((XWindow*)(handle)), ClientMessage, &e)) {
					if ((Atom)(e.xclient.data.l[0]) == windowDestroy) {
						r.type = Event::Type::CLOSE;
					}
				}
			}

			return r;
		}
	}
}

#endif