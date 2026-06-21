#include "LevelGenerator.hpp"
#include <sstream>
#include <cmath>

// ─────────────────────────────────────────────
//  Constantes de layout
// ─────────────────────────────────────────────
static constexpr float GRID      = 30.0f;  // tamaño de celda GD
static constexpr float GROUND_Y  = 105.0f; // Y del suelo
static constexpr float SECTION_W = 10;     // bloques por sección

// ─────────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────────
LevelGenerator::LevelGenerator(const GenConfig& cfg) : m_cfg(cfg) {
    uint32_t seed = (cfg.seed == 0)
        ? static_cast<uint32_t>(std::random_device{}())
        : static_cast<uint32_t>(cfg.seed);
    m_rng.seed(seed);
    applyDifficultySettings();
}

void LevelGenerator::applyDifficultySettings() {
    if (m_cfg.difficulty == "Fácil") {
        m_gapChance      = 0.05f;
        m_spikeChance    = 0.10f;
        m_platformChance = 0.30f;
        m_orbChance      = 0.15f;
        m_decoChance     = 0.40f;
        m_maxHeight      = 3;
    } else if (m_cfg.difficulty == "Normal") {
        m_gapChance      = 0.10f;
        m_spikeChance    = 0.20f;
        m_platformChance = 0.25f;
        m_orbChance      = 0.20f;
        m_decoChance     = 0.35f;
        m_maxHeight      = 4;
    } else if (m_cfg.difficulty == "Difícil") {
        m_gapChance      = 0.18f;
        m_spikeChance    = 0.35f;
        m_platformChance = 0.20f;
        m_orbChance      = 0.25f;
        m_decoChance     = 0.25f;
        m_maxHeight      = 5;
    } else { // Extremo
        m_gapChance      = 0.25f;
        m_spikeChance    = 0.50f;
        m_platformChance = 0.15f;
        m_orbChance      = 0.30f;
        m_decoChance     = 0.15f;
        m_maxHeight      = 6;
    }
}

// ─────────────────────────────────────────────
//  Generación principal
// ─────────────────────────────────────────────
std::string LevelGenerator::generate() {
    m_objects.clear();

    addSpeedPortal();
    addStartPortal();

    for (int i = 0; i < m_cfg.sections; i++) {
        generateSection(i);
    }

    return buildLevelString();
}

void LevelGenerator::addSpeedPortal() {
    // Portal de velocidad normal al inicio
    addObject(ObjID::SPEED_NORMAL, GRID * 2, GROUND_Y + GRID);
}

void LevelGenerator::addStartPortal() {
    // Portal de modo según configuración
    int portalId = ObjID::PORTAL_CUBE;
    if      (m_cfg.gameMode == "Nave")   portalId = ObjID::PORTAL_SHIP;
    else if (m_cfg.gameMode == "Bola")   portalId = ObjID::PORTAL_BALL;
    else if (m_cfg.gameMode == "UFO")    portalId = ObjID::PORTAL_UFO;
    else if (m_cfg.gameMode == "Ola")    portalId = ObjID::PORTAL_WAVE;
    else if (m_cfg.gameMode == "Robot")  portalId = ObjID::PORTAL_ROBOT;
    else if (m_cfg.gameMode == "Araña")  portalId = ObjID::PORTAL_SPIDER;

    if (portalId != ObjID::PORTAL_CUBE) {
        addObject(portalId, GRID * 5, GROUND_Y + GRID * 2);
    }
}

void LevelGenerator::generateSection(int sectionIndex) {
    float baseX = GRID * (8 + sectionIndex * SECTION_W);
    int   len   = static_cast<int>(SECTION_W);

    generateGround(baseX, len);
    generateObstacles(baseX, len);
    generatePlatforms(baseX, len);
    generateOrbs(baseX, len);
    generateDecoration(baseX, len);
}

// ─────────────────────────────────────────────
//  Suelo
// ─────────────────────────────────────────────
void LevelGenerator::generateGround(float baseX, int len) {
    bool inGap = false;
    int  gapEnd = 0;

    for (int i = 0; i < len; i++) {
        float x = baseX + i * GRID;

        if (!inGap && randBool(m_gapChance) && i < len - 2) {
            // Hueco de 1-2 bloques
            inGap = true;
            gapEnd = i + randInt(1, 2);
            continue;
        }
        if (inGap && i > gapEnd) {
            inGap = false;
        }
        if (inGap) continue;

        // Bloque de suelo (2 capas)
        int blockId = (randBool(0.7f)) ? ObjID::BLOCK_1 : ObjID::BLOCK_2;
        addObject(blockId, x, GROUND_Y);
        addObject(ObjID::BLOCK_3, x, GROUND_Y - GRID);

        // Techo (modo nave/UFO/ola)
        if (m_cfg.gameMode == "Nave" || m_cfg.gameMode == "UFO" ||
            m_cfg.gameMode == "Ola") {
            float ceilY = GROUND_Y + GRID * 8;
            addObject(blockId, x, ceilY, 180);
        }
    }
}

// ─────────────────────────────────────────────
//  Obstáculos
// ─────────────────────────────────────────────
void LevelGenerator::generateObstacles(float baseX, int len) {
    for (int i = 1; i < len - 1; i++) {
        float x = baseX + i * GRID;

        if (randBool(m_spikeChance)) {
            // Grupos de spikes: 1-3 seguidos
            int count = randInt(1, 3);
            for (int s = 0; s < count && (i + s) < len - 1; s++) {
                float sx = baseX + (i + s) * GRID;
                addObject(ObjID::SPIKE, sx, GROUND_Y + GRID);
            }
            i += count; // saltar los ya usados
        }

        // Sierras giratorias (solo difícil/extremo)
        if ((m_cfg.difficulty == "Difícil" || m_cfg.difficulty == "Extremo")
             && randBool(0.08f)) {
            float sawY = GROUND_Y + GRID * randInt(2, 4);
            addObject(ObjID::SAW_BLADE, x, sawY);
        }
    }
}

// ─────────────────────────────────────────────
//  Plataformas flotantes
// ─────────────────────────────────────────────
void LevelGenerator::generatePlatforms(float baseX, int len) {
    for (int i = 1; i < len - 1; i++) {
        if (!randBool(m_platformChance)) continue;

        float x      = baseX + i * GRID;
        int   height = randInt(2, m_maxHeight);
        float y      = GROUND_Y + GRID * height;
        int   width  = randInt(2, 4);  // largo de la plataforma

        for (int w = 0; w < width && (i + w) < len; w++) {
            float px = baseX + (i + w) * GRID;
            addObject(ObjID::PLATFORM_1, px, y);

            // Spike encima de la plataforma a veces
            if (randBool(m_spikeChance * 0.5f)) {
                addObject(ObjID::SPIKE, px, y + GRID, 0, false, true);
            }
        }
        i += width; // saltar los bloques de la plataforma
    }
}

// ─────────────────────────────────────────────
//  Orbes y pads
// ─────────────────────────────────────────────
void LevelGenerator::generateOrbs(float baseX, int len) {
    for (int i = 1; i < len - 1; i++) {
        if (!randBool(m_orbChance)) continue;

        float x = baseX + i * GRID;
        float y = GROUND_Y + GRID * randInt(2, 5);

        // Elegir tipo de orbe
        int orbIds[] = {
            ObjID::ORB_YELLOW,
            ObjID::ORB_PINK,
            ObjID::ORB_RED,
            ObjID::ORB_BLUE
        };
        int orbId = orbIds[randInt(0, 3)];
        addObject(orbId, x, y);

        // Pad en el suelo asociado
        if (randBool(0.3f)) {
            addObject(ObjID::PAD_YELLOW, x - GRID, GROUND_Y + GRID);
        }
    }
}

// ─────────────────────────────────────────────
//  Decoración de fondo
// ─────────────────────────────────────────────
void LevelGenerator::generateDecoration(float baseX, int len) {
    int decoIds[] = {
        ObjID::DECO_1, ObjID::DECO_2, ObjID::DECO_3,
        ObjID::DECO_4, ObjID::DECO_5
    };

    for (int i = 0; i < len; i++) {
        if (!randBool(m_decoChance)) continue;

        float x = baseX + i * GRID;
        float y = GROUND_Y + GRID * randInt(1, 3);
        int id  = decoIds[randInt(0, 4)];

        addObject(id, x, y);
    }
}

// ─────────────────────────────────────────────
//  Helpers
// ─────────────────────────────────────────────
void LevelGenerator::addObject(int id, float x, float y,
                                int rot, bool flipX, bool flipY) {
    LevelObject obj;
    obj.id       = id;
    obj.x        = x;
    obj.y        = y;
    obj.rotation = rot;
    obj.flipX    = flipX;
    obj.flipY    = flipY;
    m_objects.push_back(obj);
}

float LevelGenerator::randFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(m_rng);
}

int LevelGenerator::randInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(m_rng);
}

bool LevelGenerator::randBool(float probability) {
    return randFloat(0.0f, 1.0f) < probability;
}

// ─────────────────────────────────────────────
//  Construir el level string de GD
// ─────────────────────────────────────────────
std::string LevelGenerator::objectToString(const LevelObject& obj, int index) {
    std::ostringstream ss;
    // Formato: 1,ID,2,X,3,Y,6,ROT,4,FLIPX,5,FLIPY;
    ss << "1,"  << obj.id
       << ",2," << static_cast<int>(obj.x)
       << ",3," << static_cast<int>(obj.y);

    if (obj.rotation != 0)
        ss << ",6," << obj.rotation;
    if (obj.flipX)
        ss << ",4,1";
    if (obj.flipY)
        ss << ",5,1";

    ss << ";";
    return ss.str();
}

std::string LevelGenerator::buildLevelString() {
    // Header del nivel (kS38 = color channels, etc.)
    std::string header =
        "kS38,1_255_2_255_3_255_4_-1_6_1000_7_1_15_1_18_0_8_1|"
        "1_0_2_0_3_0_4_-1_6_1001_7_1_15_1_18_0_8_1|,"
        "kA13,0,kA15,0,kA16,0,kA14,0,kA6,0,kA7,0,"
        "kA2,0,kA3,0,kA8,0,kA4,0,kA9,0,kA10,0,"
        "kA11,0,kA12,0,kA17,0,kA18,0,kA19,0;";

    std::ostringstream ss;
    ss << header;

    for (size_t i = 0; i < m_objects.size(); i++) {
        ss << objectToString(m_objects[i], static_cast<int>(i));
    }

    return ss.str();
}
