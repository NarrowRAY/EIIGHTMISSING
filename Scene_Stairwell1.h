#pragma once
#include <memory>
#include <vector>
#include "GameScene.h"

class Scene_Stairwell1 : public GameScene {
public:
    explicit Scene_Stairwell1(int entryParam);

protected:
    MapData      BuildMapData();
    sf::Vector2f GetSpawnPoint(int entryParam);
    void         CheckExits() override;
    sf::Vector2f GetSavePosition() override;
    bool         OnTileClick(int tile, int col, int row) override;
    void         Update(float dt) override;
    void         Draw(sf::RenderWindow& window) override;

private:
    bool m_pendingNote = false;

    // 杂物精灵
    std::vector<sf::Vector2i> m_debrisPositions;
    sf::Texture               m_debrisTex;
    std::vector<std::unique_ptr<sf::Sprite>> m_debrisSprites;

    // 打印机精灵
    std::vector<sf::Vector2i> m_printerPositions;
    sf::Texture               m_printerTex;
    std::vector<std::unique_ptr<sf::Sprite>> m_printerSprites;

    // 楼梯精灵
    std::vector<sf::Vector2i> m_stairPositions;
    sf::Texture               m_stairTex;
    std::vector<std::unique_ptr<sf::Sprite>> m_stairSprites;
};
