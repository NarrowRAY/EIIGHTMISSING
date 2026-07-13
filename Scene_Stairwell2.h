#pragma once
#include "GameScene.h"

class Scene_Stairwell2 : public GameScene {
public:
    explicit Scene_Stairwell2(int entryParam);

protected:
    MapData      BuildMapData();
    sf::Vector2f GetSpawnPoint(int entryParam);
    void         CheckExits() override;
    sf::Vector2f GetSavePosition() override;
};
