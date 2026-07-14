#pragma once
#include <memory>
#include <vector>
#include "GameScene.h"

class Scene_2_2 : public GameScene {
public:
    explicit Scene_2_2(int entryParam);

protected:
    MapData      BuildMapData();
    sf::Vector2f GetSpawnPoint(int entryParam);
    void         CheckExits() override;
    void         Draw(sf::RenderWindow& window) override;
    sf::Vector2f GetSavePosition() override;
    bool         OnTileClick(int tile, int col, int row) override;
    void         Update(float dt) override;
private:
    bool  m_pendingDream = false;
    float m_dreamTimer   = 0.f;

    // 清洁车精灵
    std::vector<sf::Vector2i> m_cartPositions;
    sf::Texture               m_cartTex;
    std::vector<std::unique_ptr<sf::Sprite>> m_cartSprites;

    // 打印机精灵
    std::vector<sf::Vector2i> m_printerPositions;
    sf::Texture               m_printerTex;
    std::vector<std::unique_ptr<sf::Sprite>> m_printerSprites;

    // 陷阱闪光系统 — 每格独立随机闪
    static constexpr int TRAP_COLS      = 7;   // cols 0-6
    static constexpr int TRAP_ROWS      = 6;   // rows 4-9
    static constexpr int TRAP_ROW_START = 4;
    static constexpr float DANGER_MIN   = 0.35f; // 危险持续最短
    static constexpr float DANGER_MAX   = 0.55f; // 危险持续最长
    static constexpr float SAFE_MIN     = 2.2f;  // 安全持续最短
    static constexpr float SAFE_MAX     = 4.5f;  // 安全持续最长
    static constexpr float WARN_TIME    = 0.25f; // 变红前预警时间

    float m_trapTimer   [TRAP_ROWS][TRAP_COLS] = {}; // 倒计时
    int   m_trapState   [TRAP_ROWS][TRAP_COLS] = {}; // 0=安全, 1=预警, 2=危险
    bool  m_trapHit = false;

    bool IsTrapDanger(int col, int row) const;
    bool IsTrapWarn(int col, int row) const;
};
