#pragma once
#include "GameScene.h"

class Scene_1B : public GameScene {
public:
    explicit Scene_1B(int entryParam);

protected:
    MapData      BuildMapData();
    sf::Vector2f GetSpawnPoint(int entryParam);
    void         CheckExits() override;
    void         OnSceneEnter() override;
    bool         OnTileClick(int tile, int col, int row) override;
};
