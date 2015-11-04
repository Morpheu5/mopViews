#pragma once

namespace mop {
	class mopViewsApp {
	public:
		virtual const vec2 screenToWorld(const vec2&) = 0;
	};
}