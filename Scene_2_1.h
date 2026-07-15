#pragma once
#include <memory>
#include <vector>
#include "GameScene.h"

class Scene_2_1 : public GameScene {
public:
    explicit Scene_2_1(int entryParam);

protected:
    MapData      BuildMapData();
    sf::Vector2f GetSpawnPoint(int entryParam);
    void         CheckExits() override;
    sf::Vector2f GetSavePosition() override;
    bool         OnTileClick(int tile, int col, int row) override;
    void         Draw(sf::RenderWindow& window) override;

private:
    // 打印机精灵
    std::vector<sf::Vector2i> m_printerPositions;
    sf::Texture               m_printerTex;
    std::vector<std::unique_ptr<sf::Sprite>> m_printerSprites;

    // 施工标识精灵
    sf::Texture m_signTex;
    std::unique_ptr<sf::Sprite> m_signSprite;
};
