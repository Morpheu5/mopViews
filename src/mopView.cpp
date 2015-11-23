#include "mopViewController.h"
#include "mopView.h"
#include "mopViewsApp.h"

namespace mop {
	
	unsigned long View::s_id = 0;

	View::View() {
		_id = ++s_id;
		setSize(_size);
	}
	
	void View::setSize(vec2 size) {
		_size = size;
		auto shader = gl::getStockShader(gl::ShaderDef().color());
		auto color = ColorAf(1,1,1,0.333f);
		_bounds = Rectf(-_size.x/2, -_size.y/2, _size.x/2, _size.y/2);
		_boundsBatch = gl::Batch::create(geom::Rect().rect(Rectf(-_size.x/2, _size.y/2, _size.x/2, -_size.y/2)).colors(color, color, color, color), shader);
	}

	void View::draw() {
		gl::color(1,1,1);
		_boundsBatch->draw();
		drawSubViews();
	}
	
	void View::drawSubViews() {
		_subViewsMtx.lock();
		for(auto subView : _subViews) {
			auto t = glm::translate(vec3(subView->_position, 0.0)) * glm::rotate(subView->_angle, vec3(0,0,1));
			gl::ScopedModelMatrix mm;
			gl::multModelMatrix(t);
			subView->draw();
		}
		_subViewsMtx.unlock();
	}
	
	void View::removeSubView(shared_ptr<mop::View> subView) {
		_subViewsMtx.lock();
		auto v = find(_subViews.begin(), _subViews.end(), subView);
		if(v != _subViews.end()) {
			_subViews.erase(v);
			_subViewsMtx.unlock();
		} else {
			_subViewsMtx.unlock();
			removeSubView(subView);
		}
	}

	bool View::hitTest(vec2 point) {
		auto p = vec2(rotate(-_angle, vec3(0, 0, 1)) * translate(vec3(-_position, 0)) * vec4(point, 0, 1));
		if(_bounds.contains(p)) {
			return true;
		} else {
			return false;
		}
	}
	
	void View::propagateTouches(ci::app::TouchEvent &event, TouchEventType type) {
		auto touches = vector<ci::app::TouchEvent::Touch>(event.getTouches());
		_propagateTouches(touches, type);
	}
	
	void View::_propagateTouches(vector<ci::app::TouchEvent::Touch>& touches, TouchEventType type) {
		for(auto i = _subViews.rbegin(); i != _subViews.rend(); ++i) {
			auto subView = *i;
			subView->_propagateTouches(touches, type);
		}
//		for(auto subView : _subViews) {
//			subView->_propagateTouches(touches, type);
//		}
		// If the App class inherits correctly, the dynamic_cast works.
		auto theApp = dynamic_cast<mop::mopViewsApp*>(App::get());
		for(auto touchIt = touches.begin(); touchIt != touches.end(); ) {
			auto touch = *touchIt;
			// Here is the big moment where we convert from screen to world coordinates.
			vec2 p = theApp->screenToWorld(touch.getPos());
			switch(type) {
				case TouchBegan: {
					if(hitTest(p)) {
						_touchesBeganInside.insert(touch.getId());
						_touchDownInside.emit(this, TouchDownInside, p, theApp->screenToWorld(touch.getPrevPos()));
						touchIt = touches.erase(touchIt);
					} else {
						++touchIt;
					}
					break;
				}
				case TouchMoved: {
					if(hitTest(p)) {
						if(_touchesBeganInside.find(touch.getId()) != _touchesBeganInside.end()) {
							_touchDragInside.emit(this, TouchDragInside, p, theApp->screenToWorld(touch.getPrevPos()));
						} else {
							_touchMovedInside.emit(this, TouchMovedInside, p, theApp->screenToWorld(touch.getPrevPos()));
						}
						touchIt = touches.erase(touchIt);
					} else {
						++touchIt;
					}
					break;
				}
				case TouchEnded: {
					if(hitTest(p)) {
						if(_touchesBeganInside.find(touch.getId()) != _touchesBeganInside.end()) {
							_touchesBeganInside.erase(touch.getId());
							_touchUpInside.emit(this, TouchUpInside, p, theApp->screenToWorld(touch.getPrevPos()));
							touchIt = touches.erase(touchIt);
						} else {
							++touchIt;
						}
					} else {
						if(_touchesBeganInside.find(touch.getId()) != _touchesBeganInside.end()) {
							_touchesBeganInside.erase(touch.getId());
							_touchUpOutside.emit(this, TouchUpOutside, p, theApp->screenToWorld(touch.getPrevPos()));
						}
						++touchIt;
					}
					break;
				}
			}
		}
	}
}