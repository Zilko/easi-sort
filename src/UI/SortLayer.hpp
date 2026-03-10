#include "../Includes.hpp"

#include "Cell.hpp"

class MyScrollLayer;

class SortLayer : public Popup {
  
private:

    Ref<LevelBrowserLayer> m_browserLayer = nullptr;

    GJCommentListLayer* m_list = nullptr;
    
    Border* m_border = nullptr;
    
    MyScrollLayer* m_scroll = nullptr;
    
    Cell* m_movingCell = nullptr;
    Cell* m_movingCellCopy = nullptr;
    Cell* m_snappingCell = nullptr;
    Cell* m_snapToCell = nullptr;
 
    std::vector<Ref<GJGameLevel>> m_allLevels;
    std::vector<Cell*> m_cells;
    
    CCPoint m_startMovePos = {0, 0};
    CCPoint m_startCellPos = {0, 0};
    CCPoint m_currentMovePos = {0, 0};
    
    SearchType m_type;
    
    int m_folder = 0;
    
    float m_scrollTimeUp = 0.f;
    float m_scrollTimeDown = 0.f;
    float m_snapScrollTimeUp = 0.f;
    float m_snapScrollTimeDown = 0.f;
    float m_snapOutTimer = 0.f;
    float m_snapTimer = 0.f;
    
    bool m_isMoving = false;
    bool m_lockSnap = false;

    bool init() override;
    
    void updateCells();
    void updateScroll(float, const CCPoint&);
    void updateSnap(float);
    void update(float) override;
    
    SortLayer(Ref<LevelBrowserLayer>);

public:

    bool isMoving();

    void onTouch(const CCPoint&);
    void onMove(const CCPoint&);
    void onRelease(const CCPoint&);
    
    static SortLayer* create(Ref<LevelBrowserLayer>);

};