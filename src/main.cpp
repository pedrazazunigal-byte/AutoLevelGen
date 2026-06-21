#include <Geode/Geode.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include "LevelGenerator.hpp"

using namespace geode::prelude;

class $modify(MyCreatorLayer, CreatorLayer) {
    bool init() {
        if (!CreatorLayer::init()) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto btn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_plainBtn_001.png"),
            this,
            menu_selector(MyCreatorLayer::onAutoLevel)
        );

        auto menu = CCMenu::create();
        menu->addChild(btn);
        menu->setPosition(winSize.width - 50.f, 50.f);
        addChild(menu, 10);

        return true;
    }

    void onAutoLevel(CCObject*) {
        GenConfig cfg;
        cfg.seed     = Mod::get()->getSettingValue<int64_t>("level-length");
        cfg.sections = 20;
        cfg.difficulty = Mod::get()->getSettingValue<std::string>("difficulty");
        cfg.gameMode = "Cubo";

        LevelGenerator gen(cfg);
        std::string levelStr = gen.generate();

        auto level = GJGameLevel::create();
        level->m_levelString = levelStr;
        level->m_levelName = "AutoGen";
        level->m_levelType = GJLevelType::Editor;

        auto scene = LevelEditorLayer::scene(level, false);
        CCDirector::sharedDirector()->replaceScene(
            CCTransitionFade::create(0.5f, scene)
        );
    }
};

$on_mod(Loaded) {
    log::info("Auto Level Gen cargado!");
}
