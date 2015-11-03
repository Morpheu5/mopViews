#pragma once

namespace mop {
	class mopViewsApp {
	public:
		virtual vec2 screenToWorld(vec2&) = 0;
	};
}