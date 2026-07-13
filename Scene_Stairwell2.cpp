#include "Scene_Stairwell2.h"

Scene_Stairwell2::Scene_Stairwell2(int entryParam) {
    m_bgmId = 0;
    LoadMap(BuildMapData(), GetSpawnPoint(entryParam));
    // 门(cols4-5,row5)强制可行走
    m_tileMap.SetForceWalkable(4, 5);
    m_tileMap.SetForceWalkable(5, 5);
    if (entryParam == 0)
        m_player.SetFacing(Direction::Right);
}

MapData Scene_Stairwell2::BuildMapData() {
    // 12×7 二楼楼梯间②
    return MapBuilder::FromStrings({
        //         111
        // 012345678901
        "############", // 0 顶墙
        "#^^^       #", // 1  楼梯(最左,cols1-3)
        "#^^^       #", // 2  楼梯下段
        "#          #", // 3
        "#         P#", // 4  打印机(贴右墙)
        "#   DD     #", // 5  门(cols5-6)
        "############", // 6  底墙
    });
}

sf::Vector2f Scene_Stairwell2::GetSpawnPoint(int entryParam) {
    if (entryParam == 0)
        return { TileMap::TileToPixel(4), TileMap::TileToPixel(2) };  // 从楼梯上来
    if (entryParam == 1)
        return { 5.5f * TILE_SIZE, TileMap::TileToPixel(4) };  // 从二楼走廊回来
    return { TileMap::TileToPixel(7), TileMap::TileToPixel(3) };
}

sf::Vector2f Scene_Stairwell2::GetSavePosition() {
    return { 8.f * TILE_SIZE + TILE_SIZE / 2.f, TileMap::TileToPixel(4) };  // 打印机左2格
}

void Scene_Stairwell2::CheckExits() {
    sf::Vector2f pos = m_player.GetPosition();
    int tile = m_tileMap.GetTileAtPixel(pos.x, pos.y);
    int col = TileMap::PixelToTile(pos.x);
    if (static_cast<TileType>(tile) == TileType::Stairs) {
        m_nextScene  = static_cast<int>(SceneID::Stairwell1);
        m_entryParam = 1;
    }
    // 门 → 二楼走廊(踩上即触发)
    if (static_cast<TileType>(tile) == TileType::Door) {
        m_nextScene  = static_cast<int>(SceneID::Corridor2_1);
        m_entryParam = 0;
    }
}
