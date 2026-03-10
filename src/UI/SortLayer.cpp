#include "SortLayer.hpp"
#include "MyScrollLayer.hpp"
#include <ranges>

void SortLayer::updateCells() {
    Cell* closestCell = nullptr;
    float closestDistance = 0.f;
    float comparePoint = 0.f;
    float closestIsTop = false;
    
    for (auto cell : m_cells) {
        if (cell == m_movingCell || !cell->isVisible()) {
            continue;
        }
        
        auto y = cell->getMiddleY();
        auto distance = abs(y - m_movingCellCopy->getPositionY());
        
        if (distance < closestDistance || !closestCell) {
            closestDistance = distance;
            closestCell = cell;
            comparePoint = m_movingCellCopy->getPositionY();
            closestIsTop = false;
        }
        
        distance = abs(y - (m_movingCellCopy->getPositionY() + m_movingCellCopy->getContentHeight()));
        
        if (distance < closestDistance || !closestCell) {
            closestDistance = distance;
            closestCell = cell;
            comparePoint = m_movingCellCopy->getPositionY() + m_movingCellCopy->getContentHeight();
            closestIsTop = true;
        }
    }
    
    if (!closestCell) {
        return;
    }

    auto y = closestCell->getMiddleY();
    
    Cell* prevCell = nullptr;
    bool adjacent = false;
    
    for (auto cell : m_cells) {
        if (
            prevCell == m_movingCell
            || (cell == m_movingCell && prevCell == closestCell)
        ) {
            adjacent = true;
            break;
        }
        
        prevCell = cell;
    }
    
    if (
        (closestIsTop && comparePoint > y)
        || (!closestIsTop && comparePoint < y)
        || !adjacent
    ) {
        int index = 0;
        
        for (auto cell : m_cells) {
            if (cell == m_movingCell) {
                break;
            }
            
            index++;
        }
        
        Cell* nextCell = nullptr;
        bool didSwap = false;
        
        while (true) {
            index += y > m_movingCell->getMiddleY() ? -1 : 1;
            
            if (index < 0 || index >= static_cast<int>(m_cells.size())) {
                break;
            }
            
            nextCell = m_cells[index];
            
            auto newY = m_movingCell->getRealPositionY();
            
            m_movingCell->setRealPositionY(nextCell->getRealPositionY());
            m_movingCell->setPositionY(nextCell->getRealPositionY());
            
            nextCell->moveToY(newY);
            
            for (int i = 0; i < m_cells.size(); i++) {
                if (m_cells[i] == m_movingCell) {
                    std::swap(m_cells[i], m_cells[index]);
                    
                    didSwap = true;
                    
                    break;
                }
            }

            if (m_type != SearchType::MyLevels && m_type != SearchType::Type26) {
                auto prevIndex = m_movingCell->getIndex();
                m_movingCell->setIndex(nextCell->getIndex());
                nextCell->setIndex(prevIndex);
            } else if (m_type == SearchType::Type26) {
                int l1 = -1, l2 = -1;
                auto cells = m_browserLayer->m_list->m_listView->m_tableView->m_contentLayer->getChildrenExt<LevelCell*>();
                
                for (int i = 0; i < cells.size(); i++) {
                    auto level = cells[i]->m_level;
                    
                    if (l1 <= -1 && level == m_movingCell->getLevel()) {
                        l1 = i;
                    } else if (l2 <= 2 && level == nextCell->getLevel()) {
                        l2 = i;
                    }
                    
                    if (l1 <= -1 || l2 <= -1) {
                        continue;
                    }

                    LevelBrowserLayer* layer = m_browserLayer;

                    static_cast<LevelListLayer*>(layer)->cellPerformedAction(
                        cells[l1],
                        static_cast<int>(cells[l1]->m_listType),
                        cells[l1]->m_level->m_listPosition < cells[l2]->m_level->m_listPosition ? CellAction::Down : CellAction::Up,
                        nullptr
                    );

                    break;
                }
            } else {
                int l1 = -1, l2 = -1;
                
                for (int i = 0; i < m_allLevels.size(); i++) {
                    auto level = m_allLevels[i];
                    
                    if (l1 <= -1 && level == m_movingCell->getLevel()) {
                        l1 = i;
                    } else if (l2 <= 2 && level == nextCell->getLevel()) {
                        l2 = i;
                    }
                    
                    if (l1 <= -1 || l2 <= -1) {
                        continue;
                    }
                    
                    std::swap(m_allLevels[l1], m_allLevels[l2]);
                    
                    auto lm = LocalLevelManager::get();
                    
                    lm->m_localLevels->release();
                    lm->m_localLevels = CCArray::create();
                    lm->m_localLevels->retain();
                    
                    for (auto level : m_allLevels) {
                        lm->m_localLevels->addObject(level);
                    }
                    
                    didSwap = true;
                    
                    break;
                }
            }
                        
            if (nextCell == closestCell) {
                break;
            }
        }
        
        if (didSwap && m_type != SearchType::Type26) {
            CCTouchDispatcher::get()->unregisterForcePrio(this);

            auto prevY = m_browserLayer->m_list->m_listView->m_tableView->m_contentLayer->getPositionY();
            m_browserLayer->loadPage(m_browserLayer->m_searchObject);
            m_browserLayer->m_list->m_listView->m_tableView->m_contentLayer->setPositionY(prevY);

            CCTouchDispatcher::get()->registerForcePrio(this, 2);
        }
    }
}

void SortLayer::updateScroll(float dt, const CCPoint& pos) {
    if (
        pos.y > m_border->getPositionY() + m_border->getContentHeight() - m_movingCellCopy->getContentHeight()
        && m_scroll->m_contentLayer->getPositionY() >= -m_scroll->m_contentLayer->getContentHeight() + 193.f
    ) {
        m_scroll->m_contentLayer->setPositionY(
            m_scroll->m_contentLayer->getPositionY() - dt * 150.f - 10.f * std::pow(m_scrollTimeUp, 2) * dt / (1.f / 240.f)
        );
        
        m_scrollTimeUp += dt;
    } else {
        m_scrollTimeUp = 0.f;
    }
    
    if (
        pos.y < m_border->getPositionY()
        && m_scroll->m_contentLayer->getPositionY() <= 7.f
    ) {
        m_scroll->m_contentLayer->setPositionY(
            m_scroll->m_contentLayer->getPositionY() + dt * 150.f + 10.f * std::pow(m_scrollTimeDown, 2) * dt / (1.f / 240.f)
        );
        
        m_scrollTimeDown += dt;
    } else {
        m_scrollTimeDown = 0.f;
    }
        
    m_movingCell->setVisible(false);
}

void SortLayer::updateSnap(float dt) {
    m_snapTimer += dt * (dt / (1.f / 240.f));
    
    if (m_snapTimer > 1.f) {
        m_snapTimer = 1.f;
    }
    
    auto progress = m_snapTimer / 1.f;
    auto prevY = m_snapToCell->getPositionY();
    
    m_snapToCell->setPositionY(m_snapToCell->getRealPositionY());
    
    auto targetPos = m_mainLayer->convertToNodeSpace(m_snapToCell->convertToWorldSpace({0, 0}));
    
    m_snapToCell->setPositionY(prevY);
    
    if (
        targetPos.y > m_border->getPositionY() + m_border->getContentHeight() - (m_snappingCell->getContentHeight() + 5.f)
        && m_scroll->m_contentLayer->getPositionY() >= -m_scroll->m_contentLayer->getContentHeight() + 193.f
    ) {
        auto offset = dt * 190.f + 10.f * std::pow(m_snapScrollTimeDown, 2) * dt / (1.f / 240.f);
        
        m_scroll->m_contentLayer->setPositionY(m_scroll->m_contentLayer->getPositionY() - offset);
        m_snappingCell->setPositionY(m_snappingCell->getPositionY() - offset);
        
        m_snapScrollTimeUp += dt;
    } else {
        m_snapScrollTimeUp = 0.f;
    }
    
    if (
        targetPos.y - 5.f < m_border->getPositionY()
        && m_scroll->m_contentLayer->getPositionY() <= 7.f
    ) {
        auto offset = dt * 190.f + 10.f * std::pow(m_snapScrollTimeDown, 2) * dt / (1.f / 240.f);
        
        m_scroll->m_contentLayer->setPositionY(m_scroll->m_contentLayer->getPositionY() + offset);
        m_snappingCell->setPositionY(m_snappingCell->getPositionY() + offset);
        
        m_snapScrollTimeDown += dt;
    } else {
        m_snapScrollTimeDown = 0.f;
    }
    
    m_snappingCell->setPosition((targetPos - m_snappingCell->getPosition()) * progress + m_snappingCell->getPosition());
    
    m_snapToCell->setVisible(false);
    
    if (progress >= 1.f || ccpDistance(m_snappingCell->getPosition(), targetPos) < 0.09f) {
        m_snappingCell->removeFromParentAndCleanup(true);
        m_snapToCell->setVisible(true);
        
        m_snappingCell = nullptr;
        m_snapToCell = nullptr;
    }
}

void SortLayer::update(float dt) {    
    auto dtSum = dt * (dt / (1.f / 240.f));
    
    if (m_snappingCell && m_snapToCell) {
        updateSnap(dt);
    }
    
    if (!m_isMoving || !m_movingCell || !m_movingCellCopy) {
        return;
    }
            
    auto pos = m_movingCellCopy->getPosition();
    auto cellSize = m_movingCellCopy->getContentSize();
    
    if (
        pos.x > m_border->getPositionX() + m_border->getContentWidth()
        || pos.x < m_border->getPositionX() - cellSize.width
        || pos.y > m_border->getPositionY() + m_border->getContentHeight()
        || pos.y < m_border->getPositionY() - cellSize.height
    ) {
        return;
    }
    
    updateScroll(dt, pos);
    updateCells();
}

bool SortLayer::isMoving() {
    return m_isMoving;
}

void SortLayer::onTouch(const CCPoint& pos) {
    auto borderRect = CCRect{0, 0, m_border->getContentWidth(), m_border->getContentHeight()};
    
    if (
        m_isMoving
        || !borderRect.containsPoint(m_border->convertToNodeSpace(pos))
    ) {
        return;
    }

    for (auto cell : m_cells) {
        auto node = cell->getMoveButton();
        auto rect = CCRect{0, 0, node->getContentWidth(), node->getContentHeight()};
        
        if (rect.containsPoint(node->convertToNodeSpace(pos))) {
            if (!cell->isVisible() || cell == m_snapToCell) {
                break;
            }
            
            m_startMovePos = pos;
            m_isMoving = true;
            
            m_movingCell = cell;
            m_movingCell->setGrabbed(true);
            m_movingCell->setVisible(false);
            
            m_movingCellCopy = cell->getCopy();
            m_movingCellCopy->setPosition(m_mainLayer->convertToNodeSpace(pos) - cell->convertToNodeSpace(pos));
            m_movingCellCopy->setZOrder(103);
            m_movingCellCopy->setGrabbed(true);
            
            m_mainLayer->addChild(m_movingCellCopy);
            
            m_startCellPos = m_movingCellCopy->getPosition();
            
            if (m_snappingCell && m_snapToCell) {
                m_snappingCell->removeFromParentAndCleanup(true);
                m_snapToCell->setVisible(true);
                
                m_snappingCell = nullptr;
                m_snapToCell = nullptr;
            }
            
            break;
        }
    }
}

void SortLayer::onMove(const CCPoint& pos) {
    if (!m_isMoving) {
        return;
    }
    
    m_movingCellCopy->setPosition(m_startCellPos + pos - m_startMovePos);
    m_currentMovePos = pos;
}

void SortLayer::onRelease(const CCPoint& pos) {
    if (!m_isMoving) {
        return;
    }
    
    m_isMoving = false;
    
    m_snapToCell = m_movingCell;
    m_movingCell = nullptr;
    
    m_snappingCell = m_movingCellCopy;
    m_movingCellCopy = nullptr;
    
    m_snapTimer = 0.f;
    
    m_snappingCell->setGrabbed(false);
    m_snapToCell->setGrabbed(false);
}

bool SortLayer::init() {
    Popup::init(213.f, 262.f);
    setTitle("Sort Levels");
    
    float gap = 5.f;
    
    m_scroll = MyScrollLayer::create(this, {162, 200}, true, true);
    m_scroll->m_contentLayer->setLayout(
        ColumnLayout::create()
            ->setGap(gap)
            ->setAxisReverse(true)
            ->setAxisAlignment(AxisAlignment::End)
            ->setAutoGrowAxis(200.f)
    );
    
    auto filler = CCNode::create();
    filler->setContentHeight(gap / 4.f);
    m_scroll->m_contentLayer->addChild(filler);
    
    for (const auto& level : m_allLevels) {
        if (
            (m_folder == 0 || level->m_levelFolder == m_folder || m_type == SearchType::Type26)
            && (m_type != SearchType::FavouriteLevels || level->m_levelFavorited)
        ) {
            auto cell = Cell::create(level, this);
            m_scroll->m_contentLayer->addChild(cell);
            m_cells.push_back(cell);
        }
    }
    
    if (m_cells.empty()) {
        return false;
    }
        
    filler = CCNode::create();
    filler->setContentHeight(gap / 4.f);
    m_scroll->m_contentLayer->addChild(filler);
    
    m_scroll->m_contentLayer->updateLayout();
    
    for (auto cell : m_cells) {
        cell->setRealPositionY(cell->getPositionY());
    }
    
    m_border = Border::create(m_scroll, {138, 77, 46, 255}, {162, 200}, {0, 0});
    m_border->setPaddingBottom(0.3f);
    m_border->setPosition({25, 22});

    m_mainLayer->addChild(m_border);
    
    auto m_scrollbar = Scrollbar::create(m_scroll);
    m_scrollbar->setPosition(m_border->getPosition() + ccp(168, 0));
    m_scrollbar->setAnchorPoint({0, 0});
    m_mainLayer->addChild(m_scrollbar, 5);
    
    m_scroll->scrollToTop();
    
    schedule(schedule_selector(SortLayer::update));
    
    return true;
}

SortLayer::SortLayer(Ref<LevelBrowserLayer> browserLayer) : m_browserLayer(browserLayer) {
    m_folder = browserLayer->m_searchObject->m_folder;
    m_type = browserLayer->m_searchObject->m_searchType;
    
    if (
        m_type != SearchType::FavouriteLevels
        && m_type != SearchType::SavedLevels
        && m_type != SearchType::MyLevels
        && m_type != SearchType::Type26 // list
    ) {
        return;
    }

    if (m_type == SearchType::Type26) {
        for (const auto& cell : m_browserLayer->m_list->m_listView->m_tableView->m_contentLayer->getChildrenExt<LevelCell*>()) {
            m_allLevels.push_back(cell->m_level);
        }

        return;
    }
    
    auto levels = m_type == SearchType::MyLevels
        ? LocalLevelManager::get()->getCreatedLevels(0)
        : GameLevelManager::get()->getSavedLevels(false, 0);
        
    for (const auto& level : CCArrayExt<GJGameLevel*>(levels)) {
        m_allLevels.push_back(level);
    }
    
    if (m_type != SearchType::MyLevels) {
        std::sort(m_allLevels.begin(), m_allLevels.end(),
            [](const GJGameLevel* a, const GJGameLevel* b) {
                return a->m_levelIndex > b->m_levelIndex;
            });
        
        for (size_t i = 1; i < m_allLevels.size(); ++i) {
            if (m_allLevels[i]->m_levelIndex >= m_allLevels[i - 1]->m_levelIndex) {
                m_allLevels[i]->m_levelIndex = m_allLevels[i - 1]->m_levelIndex - 1;
            }
        }
    }
}

SortLayer* SortLayer::create(Ref<LevelBrowserLayer> browserLayer) {
    auto ret = new SortLayer(browserLayer);
    
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    delete ret;
    return nullptr;
}