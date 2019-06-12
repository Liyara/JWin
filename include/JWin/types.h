#ifndef JWIN_TYPES_H
#define JWIN_TYPES_H

#include <JML/Vector.hpp>

namespace jwin {

	typedef void* Handle;
	typedef jml::Vector<long, 2> Position;
	typedef jml::Vector<size_t, 2> Dimensions;

	struct Geometry {
		Position position;
		Dimensions size;
		Geometry() {}
		Geometry(const Position &p, const Dimensions &d) : position(p), size(d) {}
	};

	enum InputMode {
		RAW,
		TRANSLATED
	};
}

#endif