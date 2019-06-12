#include <JWin/jwin_x11.h>
#ifdef JUTIL_LINUX

#include <JUtil/Core/Pair.hpp>

namespace jwin {
	namespace display_manager {

		DisplayAttribs displayAttribs;
		FBConfig idealConfig;
		FBConfig currentConfig;
		DisplayData displayData;

		FBConfig generateConfig(const GLXFBConfig &cfg) {
			FBConfig r;
			r.glxConfig = cfg;
			unsigned cr, cb, cg, ca;

			r.visualInfo = glXGetVisualFromFBConfig(displayData.display, cfg);
			if (r.visualInfo) r.format = XRenderFindVisualFormat(displayData.display, r.visualInfo->visual);

			glXGetFBConfigAttrib(displayData.display, cfg, GLX_FBCONFIG_ID, reinterpret_cast<int*>(&(r.id)));
			glXGetFBConfigAttrib(displayData.display, cfg, GLX_VISUAL_ID, reinterpret_cast<int*>(&(r.vid)));

			glXGetFBConfigAttrib(displayData.display, cfg, GLX_X_RENDERABLE, reinterpret_cast<int*>(&(r.attribs.renderable)));
			glXGetFBConfigAttrib(displayData.display, cfg, GLX_DOUBLEBUFFER, reinterpret_cast<int*>(&(r.attribs.doubleBuffered)));
			glXGetFBConfigAttrib(displayData.display, cfg, GLX_DRAWABLE_TYPE, &(r.attribs.drawableType));
			glXGetFBConfigAttrib(displayData.display, cfg, GLX_RENDER_TYPE, &(r.attribs.renderType));
			glXGetFBConfigAttrib(displayData.display, cfg, GLX_X_VISUAL_TYPE, &(r.attribs.visualType));
			glXGetFBConfigAttrib(displayData.display, cfg, GLX_RED_SIZE, reinterpret_cast<int*>(&(cr)));
			glXGetFBConfigAttrib(displayData.display, cfg, GLX_GREEN_SIZE, reinterpret_cast<int*>(&(cg)));
			glXGetFBConfigAttrib(displayData.display, cfg, GLX_BLUE_SIZE, reinterpret_cast<int*>(&(cb)));
			glXGetFBConfigAttrib(displayData.display, cfg, GLX_ALPHA_SIZE, reinterpret_cast<int*>(&(ca)));

			r.attribs.rgbaMask = 
				(0xff000000 & (cr << 24)) | 
				(0x00ff0000 & (cg << 16)) | 
				(0x0000ff00 & (cb << 8)) | 
				(0x000000ff & ca)
			;

			glXGetFBConfigAttrib(displayData.display, cfg, GLX_DEPTH_SIZE, reinterpret_cast<int*>(&(cr)));
			glXGetFBConfigAttrib(displayData.display, cfg, GLX_STENCIL_SIZE, reinterpret_cast<int*>(&(cb)));

			r.attribs.dsMask = (0xffff0000 & (cr << 16)) | (0x0000ffff & cb);

			glXGetFBConfigAttrib(displayData.display, cfg, GLX_LEVEL, &(r.bufferLevel));
			glXGetFBConfigAttrib(displayData.display, cfg, GLX_STEREO, reinterpret_cast<int*>(&(r.stereoColor)));
			glXGetFBConfigAttrib(displayData.display, cfg, GLX_AUX_BUFFERS, &(r.nAuxBuffers));
			glXGetFBConfigAttrib(displayData.display, cfg, GLX_ACCUM_RED_SIZE, reinterpret_cast<int*>(&(cr)));
			glXGetFBConfigAttrib(displayData.display, cfg, GLX_ACCUM_GREEN_SIZE, reinterpret_cast<int*>(&(cg)));
			glXGetFBConfigAttrib(displayData.display, cfg, GLX_ACCUM_BLUE_SIZE, reinterpret_cast<int*>(&(cb)));
			glXGetFBConfigAttrib(displayData.display, cfg, GLX_ACCUM_ALPHA_SIZE, reinterpret_cast<int*>(&(ca)));

			r.accumMask = 
				(0xff000000 & (cr << 24)) | 
				(0x00ff0000 & (cg << 16)) | 
				(0x0000ff00 & (cb << 8)) | 
				(0x000000ff & ca)
			;

			if (r.format) r.transparent = (r.format->direct.alphaMask > 0? 1 : 0);

			glXGetFBConfigAttrib(displayData.display, cfg, GLX_SAMPLES, &(r.samples));

			return r;
		}


		void printConfig(const FBConfig &cfg) {
			jutil::out << "RGBA: " << cfg.attribs.rgbaMask << jutil::endl;
			jutil::out << "DS: " << cfg.attribs.dsMask << jutil::endl;
			jutil::out << "accum: " << cfg.accumMask << jutil::endl;
			jutil::out << "transparent: " << (bool)cfg.transparent << jutil::endl;
			jutil::out << "samples: " << cfg.samples << jutil::endl;
			jutil::out << jutil::endl;
		}

		bool validConfig(const FBConfig &cfg) {
			if (!cfg.id) return true; //idealConfig passed, ignore.
			return (
				cfg.visualInfo &&
				cfg.attribs.doubleBuffered && 
				cfg.attribs.rgbaMask == (JWIN_RGBA_32) && 
				cfg.attribs.renderable &&  
				(cfg.attribs.renderType & GLX_RGBA_BIT) && 
				cfg.attribs.visualType == GLX_TRUE_COLOR &&
				(cfg.attribs.drawableType & GLX_WINDOW_BIT) 
			);
		}

		FBConfigList getAllConfigs() {
			jutil::out << "Getting all compatible configurations..." << jutil::endl;
			GLXFBConfig *fbc;
			int fbcount;
			FBConfigList r;
			fbc = glXGetFBConfigs(displayData.display, displayData.currentScreenID, &fbcount);

			r.reserve(fbcount);

			GLXFBConfig bestConfig = {0};

			for (size_t i = 0; i < fbcount; ++i) {
				FBConfig cfg = generateConfig(fbc[i]);
				
				if (validConfig(cfg)) {
					r.insert(cfg);
					jutil::out << "Got config " << cfg.id << jutil::endl;
					printConfig(cfg);
				}
			}

			XFree(fbc);

			return r;
		}

		FBConfig getNearestConfig(const FBConfig &cfg, const FBConfigList &cfgs) {
			jutil::out << "Atrempting to find nearest config..." << jutil::endl;
			//printConfig(cfg);
			//if (!validConfig(cfg)) return FBConfig();

			jutil::Pair<int, FBConfig> bestMatch(-100, FBConfig());

			for (auto &i: cfgs) {
				if (validConfig(i)) {
					int score = 100;

					score -= jml::abs(cfg.samples - i.samples);
					if ((cfg.attribs.dsMask >> 16) != (i.attribs.dsMask >> 16)) score -= 2;
					if ((cfg.attribs.dsMask << 16) != (i.attribs.dsMask << 16)) score -= 2;
					if (cfg.accumMask != i.accumMask) score -= 1;
					if (cfg.stereoColor != i.stereoColor) score -= 50;
					if (cfg.transparent != i.transparent) score -= 8;

					jutil::out << "cfg " << i.id << ": " << score << jutil::endl;

					if (score > bestMatch.first()) {
						bestMatch.first() = score;
						bestMatch.second() = i;
					}
				}
			}

			jutil::out << bestMatch.second().id << " wins." << jutil::endl;

			return bestMatch.second();
		}

		ContextData createContext(const FBConfig &cfg) {
			jutil::out << "Creating context..." << jutil::endl;
			ContextData r;
			r.config = cfg;
			if (supportedGLXExtensions.find(jutil::String("GLX_ARB_create_context"))) {
				jutil::out << "GLX_ARB_create_context supported." << jutil::endl;
				if (!displayData.glVersionMajor) displayData.glVersionMajor = 4;
				if (!displayData.glVersionMinor) displayData.glVersionMinor = 0;
				jutil::out << "GL Version: " << displayData.glVersionMajor << "." << displayData.glVersionMinor << jutil::endl;
				int contextAttribs[] = {
					GLX_CONTEXT_MAJOR_VERSION_ARB, displayData.glVersionMajor,
					GLX_CONTEXT_MINOR_VERSION_ARB, displayData.glVersionMinor,
					GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
					None
				};
				r.context = JWinCreateContextAttribs(displayData.display, cfg.glxConfig, NULL, True, contextAttribs);
			} else {
				jutil::out << "GLX_ARB_create_context not supported." << jutil::endl;
				r.context = glXCreateNewContext(displayData.display, cfg.glxConfig, GLX_RGBA_TYPE, NULL, True);
			}
			return r;
		}

		void setGLVersionMajor(int v) {
			displayData.glVersionMajor = v;
		}
		void setGLVersionMinor(int v) {
			displayData.glVersionMinor = v;
		}

		void makeContextCurrent(Handle win, const ContextData &context) {
			if (glXMakeCurrent(
				displayData.display,
				*((XWindow*)win),
				context.context
			)) jutil::out << "Context made current." << jutil::endl;

			if (glewInit() == GLEW_OK) jutil::out << "GLEW initialized." << jutil::endl;
		}

		bool createDisplay() {

			jutil::out << "Creating display..." << jutil::endl;

			XRRScreenResources *screenR;
			XRRCrtcInfo *crtcInfo;
			XRROutputInfo *info;

			#ifdef JWIN_SET_DISPLAY_ADAPTER
				displayData.display = XOpenDisplay(JWIN_SET_DISPLAY_ADAPTER);
			#else
				displayData.display = XOpenDisplay(NULL);
			#endif

			if (displayData.display) {

				jutil::out << "Display opened." << jutil::endl;

				displayData.currentScreenID = XDefaultScreen(displayData.display);
			   	displayData.rootWindow = XDefaultRootWindow(displayData.display);

			   	supportedGLXExtensions = jutil::split(
					jutil::String(
						glXQueryExtensionsString(
							displayData.display, 
							displayData.currentScreenID
						)
					), char(' ')
				);

				jutil::out << "Supported extensions:" << supportedGLXExtensions << jutil::endl; 

				currentConfig = getNearestConfig(idealConfig, getAllConfigs());
				displayData.contextData = createContext(currentConfig);

				XSync(displayData.display, False);

				if (!displayData.contextData.context) {
					jutil::out << "Failed to create context!" << jutil::endl;
				} else {
					jutil::out << "Context created successfuly." << jutil::endl;
				}

			   	screenR = XRRGetScreenResources(displayData.display, displayData.rootWindow);
			   	RROutput prim = XRRGetOutputPrimary(displayData.display, displayData.rootWindow);

			   	windowState 			= XInternAtom(displayData.display, "_NET_WM_STATE", True);	 
				windowMaximizeX 		= XInternAtom(displayData.display, "_NET_WM_STATE_MAXIMIZED_VERT", True);
				windowMaximizeY 		= XInternAtom(displayData.display, "_NET_WM_STATE_MAXIMIZED_HORZ", True);
				windowMinimize 			= XInternAtom(displayData.display, "_NET_WM_STATE_HIDDEN", True);
				windowIgnoreTaskbar		= XInternAtom(displayData.display, "_NET_WM_STATE_SKIP_TASKBAR", True);
				windowIgnorePager 		= XInternAtom(displayData.display, "_NET_WM_STATE_SKIP_PAGER", True);
				windowFullscreen 		= XInternAtom(displayData.display, "_NET_WM_STATE_FULLSCREEN", True);
				windowAlert 			= XInternAtom(displayData.display, "_NET_WM_STATE_DEMANDS_ATTENTION", True);
				windowOnTop 			= XInternAtom(displayData.display, "_NET_WM_STATE_ABOVE", True);
				windowOnBottom 			= XInternAtom(displayData.display, "_NET_WM_STATE_BELOW", True);

				windowType 				= XInternAtom(displayData.display, "_NET_WM_WINDOW_TYPE", True);
				windowTypeDesktop		= XInternAtom(displayData.display, "_NET_WM_WINDOW_TYPE_DESKTOP", True);
				windowTypeDock			= XInternAtom(displayData.display, "_NET_WM_WINDOW_TYPE_DOCK", True);
				windowTypeToolbar		= XInternAtom(displayData.display, "_NET_WM_WINDOW_TYPE_TOOLBAR", True);
				windowTypeMenu			= XInternAtom(displayData.display, "_NET_WM_WINDOW_TYPE_MENU", True);
				windowTypeUtility		= XInternAtom(displayData.display, "_NET_WM_WINDOW_TYPE_UTILITY", True);
				windowTypeSplash		= XInternAtom(displayData.display, "_NET_WM_WINDOW_TYPE_SPLASH", True);
				windowTypeDialog		= XInternAtom(displayData.display, "_NET_WM_WINDOW_TYPE_DIALOG", True);
				windowTypeNormal		= XInternAtom(displayData.display, "_NET_WM_WINDOW_TYPE_NORMAL", True);
				windowTypeUndecorated   = XInternAtom(displayData.display, "_MOTIF_WM_HINTS", True);

				windowPermissions		= XInternAtom(displayData.display, "_NET_WM_ALLOWED_ACTIONS", True);
				windowAllowMove			= XInternAtom(displayData.display, "_NET_WM_ACTION_MOVE", True);
				windowAllowResize		= XInternAtom(displayData.display, "_NET_WM_ACTION_RESIZE", True);
				windowAllowMinimize		= XInternAtom(displayData.display, "_NET_WM_ACTION_MINIMIZE", True);
				windowAllowMaximizeX	= XInternAtom(displayData.display, "_NET_WM_ACTION_MAXIMIZE_HORZ", True);
				windowAllowMaximizeY	= XInternAtom(displayData.display, "_NET_WM_ACTION_MAXIMIZE_VERT", True);
				windowAllowFullscreen	= XInternAtom(displayData.display, "_NET_WM_ACTION_FULLSCREEN", True);

				windowDestroy			= XInternAtom(displayData.display, "WM_DELETE_WINDOW", True);
				windowOpacity			= XInternAtom(displayData.display, "_NET_WM_WINDOW_OPACITY", True);

			   	for (size_t i = 0; i < screenR->ncrtc; ++i) {
					for (size_t j = 0; j < screenR->noutput; ++j) {
						if (screenR) {
							if (screenR->crtcs[i] != None && screenR->outputs[j] != None) {
								info = XRRGetOutputInfo(displayData.display, screenR, screenR->outputs[j]);
								if (info) {

									if (info->crtc != None && info->crtc == screenR->crtcs[i]) {
										bool dupe = false;
										for (auto &k: monitorData.monitors) {
											if (k->getID() == screenR->outputs[j]) {
												dupe = true;
												break;
											}
										}

										if (!dupe) {
											crtcInfo = XRRGetCrtcInfo(displayData.display, screenR, screenR->crtcs[i]);
											if (crtcInfo) {

												monitorData.monitors.insert(new Monitor(
													jutil::String(info->name),
													screenR->outputs[j],
													{static_cast<size_t>(crtcInfo->width), static_cast<size_t>(crtcInfo->height)},
													{crtcInfo->x, crtcInfo->y}
												));

												if (prim == screenR->outputs[j]) monitorData.primaryMonitor = monitorData.monitors.last();

												XRRFreeCrtcInfo(crtcInfo);
					   							crtcInfo = nullptr;

											} else jutil::err << "JWin: failed to retrieve info about CRTC " << screenR->crtcs[i] << "." << jutil::endl;
										}

									}

									XRRFreeOutputInfo(info);
			   						info = nullptr;

								} else jutil::err << "JWin: failed to retrieve info about output " << screenR->outputs[j] << "." << jutil::endl;
							}

						} else jutil::err << "JWin: failed to retrieve screen info from XServer." << jutil::endl;
					}
			   	}

			   	if (monitorData.monitors.empty()) {
			   		jutil::err << "JWin: Unable to find any connected monitors." << jutil::endl;
			   		return false;
			   	} else if (!monitorData.primaryMonitor) monitorData.primaryMonitor = monitorData.monitors.first();

			} else {
				jutil::err << "JWin: Invalid display adapter." << jutil::endl;
				return false;
			}

			XRRFreeScreenResources(screenR);
			screenR = nullptr;

			return true;
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

		void setVSync(Handle handle, bool s) {
			int interval = (s? 1 : 0);
			if (supportedGLXExtensions.find(jutil::String("GLX_EXT_swap_control"))) {
				JWinSwapIntervalEXT(displayData.display, *((XWindow*)handle), interval);
			} else if (supportedGLXExtensions.find(jutil::String("GLX_MESA_swap_control"))) {
				JWinSwapIntervalMESA(interval);
			} else if (supportedGLXExtensions.find(jutil::String("GLX_SGI_swap_control"))) {
				JWinSwapIntervalSGI(interval);
			}
		}

		void swapBuffers(Handle handle) {
			if (_wmg::windowIsRegistered(handle)) glXSwapBuffers(displayData.display, *((XWindow*)handle));
		}

		void destroyDisplay() {
			if (displayData.display) XCloseDisplay(displayData.display);
			displayData.display = nullptr;
			for (auto &i: monitorData.monitors) delete i;
			monitorData.monitors.clear();
			monitorData.primaryMonitor = nullptr;
		}
	}
}

#endif