#include <JWin/jwin_platform.h>
#include <JUtil/IO/IO.h>
#include <JUtil/Core/Thread.h>

namespace jwin {
	namespace display_manager {
		MonitorData monitorData;
		ContextSettings desiredContextSettings;
		jutil::Queue<ContextData> contexts;
		ContextData *currentContext;
		jutil::Queue<jutil::String> supportedExtensions;

		PixelFormat getNearestConfig(const PixelFormat &cfg, const PixelFormatList &cfgs) {

			jutil::Pair<int, PixelFormat> bestMatch(-100, PixelFormat());

			for (auto &i: cfgs) {
				if (validConfig(i)) {
					int score = 100;

					score -= jml::abs(cfg.samples - i.samples);
					if ((cfg.dsMask >> 16) != (i.dsMask >> 16)) score -= 2;
					if ((cfg.dsMask << 16) != (i.dsMask << 16)) score -= 2;
					if (cfg.accumMask != i.accumMask) score -= 1;
					if (cfg.stereoColor != i.stereoColor) score -= 50;
					if (cfg.transparent != i.transparent) score -= 8;


					if (score > bestMatch.first()) {
						bestMatch.first() = score;
						bestMatch.second() = i;
					}
				}
			}

			return bestMatch.second();
		}

		void setGLVersionMajor(int v) {
			desiredContextSettings.glVersionMajor = v;
		}
		void setGLVersionMinor(int v) {
			desiredContextSettings.glVersionMinor = v;
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
	}

	namespace input_manager {
		jutil::Queue<Event::Action> keyboardState;
		jutil::Queue<Event::Key> keycodeTranslation;
		InputMode inputMode;

		void setInputMode(InputMode im) {
			jutil::Thread::requestGroupWait();
			inputMode = im;
			jutil::Thread::requestGroupResume();
		}

		const Event::Action &keyState(const Event::Key &k) {
			return keyboardState[k];
		}
	}
}
