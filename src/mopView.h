#pragma once

#include "cinder/gl/gl.h"
#include <unordered_set>

using namespace std;
using namespace ci;
using namespace ci::app;

namespace mop {
	
	enum TouchEventType {
		TouchBegan,
		TouchMoved,
		TouchEnded
	};
	
	enum TouchSignalType {
		TouchDownInside,
		TouchMovedInside,
		TouchDragInside,
		TouchUpInside,
		TouchUpOutside
	};
	
	class View;
	typedef ci::signals::Signal<void(View*, TouchSignalType, vec2, vec2)> TouchEventSignal;

	class View {
		static unsigned long s_id;
		void _propagateTouches(vector<ci::app::TouchEvent::Touch>& touches, TouchEventType type);
		
	protected:
		unsigned long _id = -1;
		vec2 _size = vec2(100.0f, 100.0f);
		vec2 _position = vec2(0.0f, 0.0f);
		float _angle = 0.0f;
		bool _touchOpaque = false;
		
		Rectf _bounds;
		gl::BatchRef _boundsBatch;
		
		mutex _subViewsMtx;
		list<shared_ptr<View>> _subViews;
		unordered_set<unsigned long> _touchesBeganInside;
		
		/* SIGNALS */
		
		TouchEventSignal _touchDownInside;
		TouchEventSignal _touchMovedInside;
		TouchEventSignal _touchDragInside;
		TouchEventSignal _touchUpInside;
		TouchEventSignal _touchUpOutside;

		void drawSubViews();

	public:
		View();
		
		const unsigned long id() { return _id; }
		
		virtual void setSize(vec2 size);
		virtual void setPosition(vec2 position) { _position = position; }
		virtual void setAngle(float angle) { _angle = angle; }
		virtual void setTouchOpaque(bool touchOpaque = true) { _touchOpaque = touchOpaque; }

		virtual const vec2&	getSize() { return _size; }
		virtual const vec2&	getPosition() { return _position; }
		virtual const float	getAngle() { return _angle; }
		virtual const bool	isTouchOpaque()	{ return _touchOpaque; }
		const list<shared_ptr<View>>& getSubviews()	{ return _subViews; }

		void addSubView(shared_ptr<View> subView) { _subViews.push_back(subView); }
		void removeSubView(shared_ptr<View> subView);

		virtual bool hitTest(vec2 position);

		virtual void draw();
		virtual void update() { for(auto view : _subViews) view->update(); }; // TODO Betterize this.
		
		void propagateTouches(ci::app::TouchEvent& event, TouchEventType type);
		
		TouchEventSignal& getTouchDownInside() { return _touchDownInside; }
		TouchEventSignal& getTouchMovedInside() { return _touchMovedInside; }
		TouchEventSignal& getTouchDragInside() { return _touchDragInside; }
		TouchEventSignal& getTouchUpInside() { return _touchUpInside; }
		TouchEventSignal& getTouchUpOutside() { return _touchUpOutside; }
};

}