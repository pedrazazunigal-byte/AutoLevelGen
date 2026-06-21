#pragma once
#include <Geode/Geode.hpp>
#include <random>
#include <vector>
#include <string>

using namespace geode::prelude;

// ─────────────────────────────────────────────
//  IDs de objetos de Geometry Dash
// ─────────────────────────────────────────────
namespace ObjID {
    // Bloques sólidos
    constexpr int BLOCK_1       = 1;
    constexpr int BLOCK_2       = 2;
    constexpr int BLOCK_3       = 3;
    constexpr int SPIKE         = 8;
    constexpr int SPIKE_SMALL   = 395;

    // Plataformas
    constexpr int PLATFORM_1    = 6;
    constexpr int PLATFORM_2    = 7;

    // Obstáculos
    constexpr int SAW_BLADE     = 183;
    constexpr int SAW_BLADE_2   = 184;

    // Portales de modo
    constexpr int PORTAL_CUBE   = 12;
    constexpr int PORTAL_SHIP   = 13;
    constexpr int PORTAL_BALL   = 47;
    constexpr int PORTAL_UFO    = 111;
    constexpr int PORTAL_WAVE   = 660;
    constexpr int PORTAL_ROBOT  = 745;
    constexpr int PORTAL_SPIDER = 1331;

    // Portales de velocidad
    constexpr int SPEED_SLOW    = 200;
    constexpr int SPEED_NORMAL  = 201;
    constexpr int SPEED_FAST    = 202;
    constexpr int SPEED_FASTER  = 203;
    constexpr int SPEED_FASTEST = 1334;

    // Orbes y pads
    constexpr int ORB_YELLOW    = 36;
    constexpr int ORB_PINK      = 141;
    constexpr int ORB_RED       = 1333;
    constexpr int ORB_BLUE      = 84;
    constexpr int PAD_YELLOW    = 35;
    constexpr int PAD_PINK      = 140;

    // Decoración
    constexpr int DECO_1        = 17;
    constexpr int DECO_2        = 18;
    constexpr int DECO_3        = 19;
    constexpr int DECO_4        = 21;
    constexpr int DECO_5        = 22;
}

// ─────────────────────────────────────────────
//  Estructura de un objeto en el nivel
// ─────────────────────────────────────────────
struct LevelObject {
    int id;
    float x;
    float y;
    int rotation = 0;  // grados
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    bool flipX = false;
    bool flipY = false;
    int colorChannel1 = 1;
    int colorChannel2 = 2;
};

// ─────────────────────────────────────────────
//  Configuración del generador
// ─────────────────────────────────────────────
struct GenConfig {
    int seed          = 0;
    int sections      = 20;  // longitud del nivel
    std::string difficulty = "Normal";
    std::string gameMode   = "Cubo";
};

// ─────────────────────────────────────────────
//  Clase principal del generador
// ─────────────────────────────────────────────
class LevelGenerator {
public:
    explicit LevelGenerator(const GenConfig& cfg);

    // Genera y devuelve el level string de GD
    std::string generate();

private:
    GenConfig m_cfg;
    std::mt19937 m_rng;
    std::vector<LevelObject> m_objects;

    // Parámetros calculados según dificultad
    float m_gapChance;       // probabilidad de hueco en el suelo
    float m_spikeChance;     // probabilidad de spike
    float m_platformChance;  // probabilidad de plataforma flotante
    float m_orbChance;       // probabilidad de orbe
    float m_decoChance;      // probabilidad de decoración
    int   m_maxHeight;       // altura máxima de plataformas

    void applyDifficultySettings();
    void generateSection(int sectionIndex);
    void generateGround(float x, int sectionLen);
    void generateObstacles(float x, int sectionLen);
    void generatePlatforms(float x, int sectionLen);
    void generateOrbs(float x, int sectionLen);
    void generateDecoration(float x, int sectionLen);
    void addStartPortal();
    void addSpeedPortal();

    void addObject(int id, float x, float y,
                   int rot = 0, bool flipX = false, bool flipY = false);

    std::string buildLevelString();
    std::string objectToString(const LevelObject& obj, int index);

    // Helpers RNG
    float randFloat(float min, float max);
    int   randInt(int min, int max);
    bool  randBool(float probability);
};
