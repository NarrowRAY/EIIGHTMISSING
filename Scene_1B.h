#pragma once
#include <memory>
#include <vector>
#include "GameScene.h"

class Scene_1B : public GameScene {
public:
    explicit Scene_1B(int entryParam);
    void OnBadEnd() override;

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
    bool m_showPuzzle1 = false;
    bool m_clockClickWasDown = false;

    // puzzle1 贴图
    sf::Texture m_puzzle1Tex;
    std::unique_ptr<sf::Sprite> m_puzzle1Sprite;

    // BadEnd CG 全屏贴图
    sf::Texture m_badEndTex;
    std::unique_ptr<sf::Sprite> m_badEndSprite;
    bool m_showBadEnd = false;

    // 公告栏揭纸拼图
    bool m_showBulletin = false;
    bool m_bulletinSolved = false;
    struct Paper { sf::FloatRect rect; sf::Color color; std::string label; bool removed; };
    std::vector<Paper> m_papers;
    int m_bulletinDragIdx = -1;
    sf::Vector2f m_bulletinDragOff;

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
