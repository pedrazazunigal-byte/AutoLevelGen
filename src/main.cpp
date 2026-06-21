#include <Geode/Geode.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include "LevelGenerator.hpp"

using namespace geode::prelude;

// ─────────────────────────────────────────────────────────────────
//  Popup de confirmación antes de generar
// ─────────────────────────────────────────────────────────────────
class GenConfirmPopup : public FLAlertLayer, public FLAlertLayerProtocol {
public:
    static GenConfirmPopup* create() {
        auto ret = new GenConfirmPopup();
        if (ret && ret->init(
            ret,
            "Auto Level Gen",
            "¿Generar un nivel <cg>aleatorio</c> con la configuración actual?\n\n"
            "Puedes cambiar dificultad, modo y longitud en los <cy>ajustes del mod</c>.",
            "Cancelar",
            "¡Generar!"
        )) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    void FLAlert_Clicked(FLAlertLayer*, bool btn2) override {
        if (btn2) {
            generateLevel();
        }
    }

private:
    void generateLevel() {
        // Leer ajustes del mod
        GenConfig cfg;
        cfg.seed       = Mod::get()->getSettingValue<int64_t>("seed");
        cfg.sections   = Mod::get()->getSettingValue<int64_t>("level-length");
        cfg.difficulty = Mod::get()->getSettingValue<std::string>("difficulty");
        cfg.gameMode   = Mod::get()->getSettingValue<std::string>("game-mode");

        // Generar el level string
        LevelGenerator gen(cfg);
        std::string levelStr = gen.generate();

        // Crear el GJGameLevel
        auto level = GJGameLevel::create();
        level->m_levelString = levelStr;
        level->m_levelName   = "ALG-" + std::to_string(std::rand() % 9999);
        level->m_creatorName = "AutoLevelGen";
        level->m_levelDesc   = "Nivel generado automáticamente";
        level->m_levelType   = GJLevelType::Editor;

        // Abrir en el editor
        auto scene = LevelEditorLayer::scene(level, false);
        CCDirector::sharedDirector()->replaceScene(
            CCTransitionFade::create(0.5f, scene)
        );
    }
};

// ─────────────────────────────────────────────────────────────────
//  Hook en CreatorLayer — añade botón "Auto Nivel"
// ─────────────────────────────────────────────────────────────────
class $modify(MyCreatorLayer, CreatorLayer) {
    bool init() {
        if (!CreatorLayer::init()) return false;

        // Crear el botón
        auto btnSprite = CCSprite::createWithSpriteFrameName("GJ_plainBtn_001.png");
        btnSprite->setScale(0.85f);

        auto label = CCLabelBMFont::create("Auto\nNivel", "bigFont.fnt");
        label->setScale(0.45f);
        label->setPosition(btnSprite->getContentSize() / 2);
        btnSprite->addChild(label);

        auto btn = CCMenuItemSpriteExtra::create(
            btnSprite,
            this,
            menu_selector(MyCreatorLayer::onAutoLevel)
        );

        // Posicionar cerca del botón "Crear"
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        btn->setPosition({winSize.width - 50.0f, winSize.height - 120.0f});

        auto menu = CCMenu::create();
        menu->setPosition(CCPointZero);
        menu->addChild(btn);
        addChild(menu, 10);

        return true;
    }

    void onAutoLevel(CCObject*) {
        GenConfirmPopup::create()->show();
    }
};

// ─────────────────────────────────────────────────────────────────
//  Log de inicio del mod
// ─────────────────────────────────────────────────────────────────
$on_mod(Loaded) {
    log::info("Auto Level Gen cargado correctamente ✓");
    log::info("Semilla: {}", Mod::get()->getSettingValue<int64_t>("seed"));
    log::info("Dificultad: {}", Mod::get()->getSettingValue<std::string>("difficulty"));
}
