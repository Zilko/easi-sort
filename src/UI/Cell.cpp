#include "Cell.hpp"
#include "SortLayer.hpp"

Cell* Cell::getCopy() {
    return Cell::create(m_level, m_sortLayer);
}

GJGameLevel* Cell::getLevel() {
    return m_level;
}

CCNode* Cell::getMoveButton() {
    return m_moveButton;
}

float Cell::getRealPositionY() {
    return m_realPositionY;
}

void Cell::setRealPositionY(float y) {
    m_realPositionY = y;
    
    if (!m_didStart) {
        m_didStart = true;
        schedule(schedule_selector(Cell::update));
    }
}

float Cell::getMiddleY() {
    float prevY = getPositionY();
    
    setPositionY(getRealPositionY());
    
    auto ret = m_sortLayer->m_mainLayer->convertToNodeSpace(convertToWorldSpace({0, 0})).y + getContentHeight() * 0.5f;
    
    setPositionY(prevY);
    
    return ret;
}

void Cell::moveToY(float y) {
    setRealPositionY(y);
    m_timer = 0.f;
}

int Cell::getIndex() {
    return m_level->m_levelIndex;
}

void Cell::setIndex(int index) {
    m_level->m_levelIndex = index;
}

void Cell::setGrabbed(bool grabbed) {
    m_nameLabel->runAction(CCEaseSineOut::create(CCFadeTo::create(0.22f, grabbed ? 255 : 230)));
    m_sprite->runAction(CCEaseSineOut::create(CCScaleTo::create(0.22f, grabbed ? 0.29f : 0.28f)));
    m_sprite->runAction(CCEaseSineOut::create(CCFadeTo::create(0.22f, grabbed ? 135 : 115)));
    m_bg->runAction(CCEaseSineOut::create(CCFadeTo::create(0.22f, grabbed ? 50 : 25)));
    this->runAction(CCEaseSineOut::create(CCScaleTo::create(0.22f, grabbed ? 1.03f : 1.f)));
}

void Cell::update(float dt) {
    m_timer += dt * (dt / (1.f / 240.f));
    
    if (m_timer > 1.f) {
        m_timer = 1.f;
    }
    
    auto progress = m_timer / 1.f;
    
    setPositionY((getRealPositionY() - getPositionY()) * progress + getPositionY());
}

bool Cell::init() {
    
    setContentSize({162, 23});
    
    m_bg = CCScale9Sprite::create("square02b_001.png");
	m_bg->setColor({0, 0, 0});
    m_bg->setOpacity(25);
    m_bg->setPosition(getContentSize() / 2.f);
    m_bg->setAnchorPoint({0.5f, 0.5f});
    m_bg->setContentSize(ccp(150, getContentHeight()) * 4.f);
    m_bg->setScale(0.25f);
    
    addChild(m_bg);
    
    m_nameLabel = CCLabelBMFont::create(m_level->m_levelName.c_str(), "bigFont.fnt");
    m_nameLabel->setAnchorPoint({0, 0.5f});
    m_nameLabel->setPosition({31.5f, 11.5f});
    m_nameLabel->setOpacity(225);
    m_nameLabel->limitLabelWidth(117, 0.38f, 0.0001f);
    
    addChild(m_nameLabel);
    
    m_sprite = CCSprite::create("sort.png"_spr);
    m_sprite->setPosition({19, 11});
    m_sprite->setScale(0.28f);
    m_sprite->setOpacity(115);
    
    addChild(m_sprite);
    
    m_moveButton = CCNode::create();
    m_moveButton->setPositionX(7);
    m_moveButton->setContentSize({23, 23});
    
    addChild(m_moveButton);
    
    return true;
}

Cell::Cell(GJGameLevel* level, SortLayer* sortLayer) : m_level(level), m_sortLayer(sortLayer) {}

Cell* Cell::create(GJGameLevel* level, SortLayer* sortLayer) {
    Cell* ret = new Cell(level, sortLayer);

    ret->init();
    ret->autorelease();
    
    return ret;
}