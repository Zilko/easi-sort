#pragma once

#include "../Includes.hpp"

class SortLayer;

class Cell : public CCNode {
  
private:

    CCLabelBMFont* m_nameLabel = nullptr;

    CCScale9Sprite* m_bg = nullptr;

    CCSprite* m_sprite = nullptr;
    
    CCNode* m_moveButton = nullptr;
    
    GJGameLevel* m_level = nullptr;
    
    SortLayer* m_sortLayer = nullptr;
    
    bool m_didStart = false;
    
    float m_realPositionY = 0.f;
    float m_timer = 0.f;

    void update(float dt) override;
    
    bool init() override;
        
    Cell(GJGameLevel*, SortLayer*);

public:

    CCNode* getMoveButton();
    GJGameLevel* getLevel();
    Cell* getCopy();
    
    float getRealPositionY();
    void setRealPositionY(float y);
    float getMiddleY();
    
    int getIndex();
    void setIndex(int index);
    
    void moveToY(float y);
    void setGrabbed(bool grabbed);
    
    static Cell* create(GJGameLevel*, SortLayer*);

};