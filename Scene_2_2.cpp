#include "Scene_2_2.h"

Scene_2_2::Scene_2_2(int entryParam) {
    LoadMap(BuildMapData(), GetSpawnPoint(entryParam));
    if (entryParam == 0) {
        m_player.SetFacing(Direction::Down);
        LockInput(0.5f);
    }
    if (entryParam == 1) {
        m_player.SetFacing(Direction::Up);    // 从2-3回来→背面
        LockInput(0.5f);
    }
}

sf::Vector2f Scene_2_2::GetSpawnPoint(int entryParam) {
    if (entryParam == 1) return { TileMap::TileToPixel(3), TileMap::TileToPixel(11) };
    return { TileMap::TileToPixel(3), TileMap::TileToPixel(1) };  // 顶部中央
}

MapData Scene_2_2::BuildMapData() {
    // 7×14 二楼走廊中段(竖)
    return MapBuilder::FromStrings({
        //         1
        // 012345678901
        "       ",  // 0
        "     P ",  // 1  打印机(row1,col5)
        "       ",  // 2
        "       ",  // 3
        "ttttttt",  // 4  红色地板
        "ttttttt",  // 5
        "ttttttt",  // 6
        "ttttttt",  // 7
        "ttttttt",  // 8
        "ttttttt",  // 9
        "       ",  // 10
        "     **",  // 11 清洁车
        "     **",  // 12
        "       ",  // 13
    });
}

bool Scene_2_2::OnTileClick(int tile, int col, int row) {
    (void)row;
    sf::Vector2f pp = m_player.GetPosition();
    float dx = (col + 0.5f) * TILE_SIZE - pp.x;
    float dy = (row + 0.5f) * TILE_SIZE - pp.y;
    if (std::sqrt(dx * dx + dy * dy) > 160.f) return false;

    if (tile == static_cast<int>(TileType::Cart)) {
        StartDialogue({"终于安全避开了清洁工的陷阱！这是她的清洁车吧..."});
        m_pendingDream = true;
        return true;
    }
    return false;
}

void Scene_2_2::Update(float dt) {
    GameScene::Update(dt);
    if (m_pendingDream && !IsInDialogue()) {
        m_dreamTimer += dt;
        if (m_dreamTimer >= 1.f) {
            m_nextScene = static_cast<int>(SceneID::Dream);
            m_entryParam = 0;
        }
    }
}

sf::Vector2f Scene_2_2::GetSavePosition() {
    return { TileMap::TileToPixel(3), TileMap::TileToPixel(1) };  // 打印机左两格
}

void Scene_2_2::CheckExits() {
    sf::Vector2f pos = m_player.GetPosition();
    int col = TileMap::PixelToTile(pos.x);
    int row = TileMap::PixelToTile(pos.y);
    int h = m_tileMap.Height();

    // 顶部 → 2-1 (col1-4,避开右上打印机)
    if (pos.y < TILE_SIZE && col >= 1 && col <= 4) {
        m_nextScene  = static_cast<int>(SceneID::Corridor2_1);
        m_entryParam = 1;
    }
    // 底部 → 2-3
    if (row >= h - 1 && col >= 1 && col <= 5) {
        m_nextScene  = static_cast<int>(SceneID::Corridor2_3);
        m_entryParam = 0;
    }
}
