#include "Scene_2_1.h"

Scene_2_1::Scene_2_1(int entryParam) {
    m_bgmId = 0;
    LoadMap(BuildMapData(), GetSpawnPoint(entryParam));
    // 右门(row4-5,col14,B瓦片贴右墙)强制可行走
    m_tileMap.SetForceWalkable(14, 4);
    m_tileMap.SetForceWalkable(14, 5);
    // 从2-2回来→强制右侧身+0.5秒锁方向
    if (entryParam == 1) {
        m_player.SetFacing(Direction::Right);
        LockInput(0.5f);
    }
}

MapData Scene_2_1::BuildMapData() {
    // 15×8 二楼走廊顶部横段
    return MapBuilder::FromStrings({
        //         11111
        // 012345678901234
        "###!!##########", // 0  施工区(cols3-4)
        "###!!##########", // 1
        "###!!##########", // 2
        "               ", // 3
        "              B", // 4 右门(楼梯间②)
        "              B", // 5
        "               ", // 6
        "              P", // 7 打印机(右下角)
    });
}

sf::Vector2f Scene_2_1::GetSpawnPoint(int entryParam) {
    if (entryParam == 1) return { TileMap::TileToPixel(1), TileMap::TileToPixel(5) };  // 左边缘右一格
    return { 13.5f * TILE_SIZE, TileMap::TileToPixel(4) };  // 从楼梯间出来→右门左侧
}

sf::Vector2f Scene_2_1::GetSavePosition() {
    return { 12.5f * TILE_SIZE, TileMap::TileToPixel(7) };  // 打印机左2格
}

bool Scene_2_1::OnTileClick(int tile, int col, int row) {
    (void)row;
    sf::Vector2f pp = m_player.GetPosition();
    float dx = (col + 0.5f) * TILE_SIZE - pp.x;
    float dy = (row + 0.5f) * TILE_SIZE - pp.y;
    if (std::sqrt(dx * dx + dy * dy) > 160.f) return false;

    if (tile == static_cast<int>(TileType::Sign)) {
        StartDialogue({"摆着施工标志，看来这条路是走不通了"});
        return true;
    }
    return false;
}

void Scene_2_1::CheckExits() {
    sf::Vector2f pos = m_player.GetPosition();
    int tile = m_tileMap.GetTileAtPixel(pos.x, pos.y);
    // 右门(B瓦片) → 回楼梯间②
    if (static_cast<TileType>(tile) == TileType::BulletinBoard) {
        m_nextScene  = static_cast<int>(SceneID::Stairwell2);
        m_entryParam = 1;
    }
    // 左边缘 → 2-2 (碰撞箱触碰才触发)
    int col = TileMap::PixelToTile(pos.x);
    if (col <= 0 && pos.y > 1 * TILE_SIZE && pos.y < 8 * TILE_SIZE) {
        m_nextScene  = static_cast<int>(SceneID::Corridor2_2);
        m_entryParam = 0;
    }
}
