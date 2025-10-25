#include "MyScrollLayer.hpp"
#include "SortLayer.hpp"

bool MyScrollLayer::ccTouchBegan(CCTouch* touch, CCEvent* event) {
    if (!m_sortLayer->isMoving()) {
        m_sortLayer->onTouch(touch->getLocation());
    }
    
    return ScrollLayer::ccTouchBegan(touch, event);
}

void MyScrollLayer::ccTouchMoved(CCTouch* touch, CCEvent* event) {
    if (m_sortLayer->isMoving()) {
        return m_sortLayer->onMove(touch->getLocation());
    }
    
    ScrollLayer::ccTouchMoved(touch, event);
}

void MyScrollLayer::ccTouchEnded(CCTouch* touch, CCEvent* event) {
    if (m_sortLayer->isMoving()) {
        m_sortLayer->onRelease(touch->getLocation());
        m_touchStartPosition = touch->getLocation();
    }
    
    ScrollLayer::ccTouchEnded(touch, event);
}

MyScrollLayer::MyScrollLayer(SortLayer* sortLayer, const CCRect& rect, bool scroll, bool vertical)
    : ScrollLayer(rect, scroll, vertical), m_sortLayer(sortLayer) {}

MyScrollLayer* MyScrollLayer::create(SortLayer* sortLayer, const CCSize& size, bool scroll, bool vertical) {
    auto ret = new MyScrollLayer(sortLayer, {0, 0, size.width, size.height}, scroll, vertical);
    ret->autorelease();
    return ret;
}