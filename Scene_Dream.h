#pragma once
#include "GameScene.h"

class Scene_Dream : public GameScene {
public:
    explicit Scene_Dream(int entryParam);

protected:
    MapData      BuildMapData();
    sf::Vector2f GetSpawnPoint(int entryParam);
    void         CheckExits() override;
    void         Draw(sf::RenderWindow& window) override;
    bool         OnTileClick(int tile, int col, int row) override;
    void         OnSceneEnter() override;
};
