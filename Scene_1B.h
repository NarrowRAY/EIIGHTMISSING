#pragma once
#include <memory>
#include <vector>
#include "GameScene.h"

class Scene_1B : public GameScene {
public:
    explicit Scene_1B(int entryParam);

protected:
    MapData      BuildMapData();
    sf::Vector2f GetSpawnPoint(int entryParam);
    void         CheckExits() override;
    void         HandleInput(const sf::RenderWindow& window) override;
    void         Draw(sf::RenderWindow& window) override;
    void         OnSceneEnter() override;
    bool         OnTileClick(int tile, int col, int row) override;

private:
    int  m_entryParam = 0;
    bool m_showClockDetail = false;
    bool m_clockClickWasDown = false;

    // 门精灵系统
    std::vector<sf::Vector2i> m_doorPositions;
    sf::Texture               m_doorTex;
    std::vector<std::unique_ptr<sf::Sprite>> m_doorSprites;

    // 显示屏精灵系统
    std::vector<sf::Vector2i> m_displayPositions;
    sf::Texture               m_displayTex;
    std::vector<std::unique_ptr<sf::Sprite>> m_displaySprites;

    // 窗户精灵系统
    std::vector<sf::Vector2i> m_windowPositions;
    sf::Texture               m_windowTex;
    std::vector<std::unique_ptr<sf::Sprite>> m_windowSprites;

    // 沙发精灵系统
    std::vector<sf::Vector2i> m_sofaPositions;
    sf::Texture               m_sofaTex;
    std::vector<std::unique_ptr<sf::Sprite>> m_sofaSprites;

    // 打印机精灵系统
    std::vector<sf::Vector2i> m_printerPositions;
    sf::Texture               m_printerTex;
    std::vector<std::unique_ptr<sf::Sprite>> m_printerSprites;
};
