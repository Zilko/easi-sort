#include "../Includes.hpp"

class SortLayer;

class MyScrollLayer : public ScrollLayer {
  
private:
    
    SortLayer* m_sortLayer = nullptr;
    
    bool ccTouchBegan(CCTouch*, CCEvent*) override;
    void ccTouchMoved(CCTouch*, CCEvent*) override;
    void ccTouchEnded(CCTouch*, CCEvent*) override;

    MyScrollLayer(SortLayer*, const CCRect&, bool, bool);
    
public:

    static MyScrollLayer* create(SortLayer*, const CCSize&, bool, bool);
    
};