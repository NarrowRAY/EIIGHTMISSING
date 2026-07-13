#pragma once
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
};
