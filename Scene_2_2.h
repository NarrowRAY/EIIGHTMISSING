#pragma once
#include "GameScene.h"

class Scene_2_2 : public GameScene {
public:
    explicit Scene_2_2(int entryParam);

protected:
    MapData      BuildMapData();
    sf::Vector2f GetSpawnPoint(int entryParam);
    void         CheckExits() override;
    sf::Vector2f GetSavePosition() override;
    bool         OnTileClick(int tile, int col, int row) override;
    void         Update(float dt) override;
private:
    bool m_pendingDream = false;
    float m_dreamTimer = 0.f;
};
