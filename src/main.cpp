#include "Includes.hpp"

#include "UI/SortLayer.hpp"

#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/modify/LevelListLayer.hpp>
#include <Geode/modify/LevelCell.hpp>

class $modify(LevelListLayer) {

    void onToggleEditMode(CCObject*) {
        if (auto layer = SortLayer::create(this)) {
            layer->show();
        }
    }

};

class $modify(LevelCell) {

    void loadCustomLevelCell() {
        LevelCell::loadCustomLevelCell();

        if (!m_compactView || m_listType != BoomListType::Level4) {
            return;
        }

        auto mainLayer = getChildByID("main-layer");

        if (!mainLayer) {
            return;
        }

        auto menu = mainLayer->getChildByID("main-menu");

        if (!menu) {
            return;
        }

        auto spr = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
        spr->setScale(0.45f);

        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(LevelCell::onClick));
        btn->setPosition({-3.5f, -135});
        btn->setID("delete-button"_spr);
        btn->setTag(4);

        menu->addChild(btn);

        auto viewBtn = menu->getChildByID("view-button");

        if (!viewBtn) {
            return;
        }

        static_cast<CCMenuItemSpriteExtra*>(viewBtn)->m_pNormalImage->setScale(0.85f);
    }

};

class $modify(ProLevelBrowserLayer, LevelBrowserLayer) {
    
    struct Fields {
        CCMenuItemSpriteExtra* m_button = nullptr;
    };
    
    void onButton(CCObject*) {
        if (auto layer = SortLayer::create(this)) {
            layer->show();
        }
    }
  
    bool init(GJSearchObject* p0) {
        if (!LevelBrowserLayer::init(p0)) return false;

        if (!m_folderBtn) return true;
        
        auto menu = getChildByID("page-menu");
        
        if (!menu) return true;
                
        auto square = CCSprite::create("GJ_button_01.png");
        square->setScale(0.713f);
        square->addChildAtPosition(CCSprite::create("icon.png"_spr), Anchor::Center, {0.5f, 0.5f});
        
        auto f = m_fields.self();
        
        f->m_button = CCMenuItemSpriteExtra::create(square, this, menu_selector(ProLevelBrowserLayer::onButton));
		
		menu->addChild(f->m_button);
		menu->updateLayout();
        
        return true;
    }
    
    void loadPage(GJSearchObject* p0) {
        LevelBrowserLayer::loadPage(p0);
        
        auto f = m_fields.self();
        
        if (!f->m_button) return;
        
        auto type = m_searchObject->m_searchType;
        auto folder = m_searchObject->m_folder;
        
        if (
            type != SearchType::FavouriteLevels
            && type != SearchType::SavedLevels
            && type != SearchType::MyLevels
        ) {
            return f->m_button->setVisible(false);
        }
        
        auto levels = type == SearchType::MyLevels
            ? LocalLevelManager::get()->getCreatedLevels(0)
            : GameLevelManager::get()->getSavedLevels(false, 0);
            
        for (const auto& level : CCArrayExt<GJGameLevel*>(levels)) {
            if (
                (folder == 0 || level->m_levelFolder == folder)
                && (type != SearchType::FavouriteLevels || level->m_levelFavorited)
            ) {
                return f->m_button->setVisible(true);
            }
        }
        
        f->m_button->setVisible(false);
    }
    
};