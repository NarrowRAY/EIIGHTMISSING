#pragma once
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
private:
    bool m_pendingNote = false;  // 垃圾桶对话结束后弹出纸条
};
