#include "Scene_2_3.h"

Scene_2_3::Scene_2_3(int entryParam) {
    m_bgmId = 0;
    LoadMap(BuildMapData(), GetSpawnPoint(entryParam));
    if (entryParam == 0) {
        m_player.SetFacing(Direction::Left);
        LockInput(0.5f);
    }
}

MapData Scene_2_3::BuildMapData() {
    // 17×9 二楼走廊底部横段
    return MapBuilder::FromStrings({
        //         1111111
        // 01234567890123456
        "#################", // 0  墙壁
        "##DD#WW##DD#WW###", // 1  门+窗+门(cols9-10)+窗(cols12-13)
        "##DD#WW##DD#WW###", // 2
        "##DD#####DD######", // 3  门+门(无窗)
        "                 ", // 4  地板
        "                 ", // 5
        "                 ", // 6
        "                 ", // 7
        "             P   ", // 8  打印机(col13)
    });
}

sf::Vector2f Scene_2_3::GetSpawnPoint(int entryParam) {
    if (entryParam == 1) return { TileMap::TileToPixel(2), TileMap::TileToPixel(6) };
    return { TileMap::TileToPixel(15), TileMap::TileToPixel(6) };  // 从2-2来→右侧
}

sf::Vector2f Scene_2_3::GetSavePosition() {
    return { TileMap::TileToPixel(11), TileMap::TileToPixel(8) };  // 打印机左两格
}

bool Scene_2_3::OnTileClick(int tile, int col, int row) {
    (void)row;
    sf::Vector2f pp = m_player.GetPosition();
    float dx = (col + 0.5f) * TILE_SIZE - pp.x;
    float dy = (row + 0.5f) * TILE_SIZE - pp.y;
    if (std::sqrt(dx * dx + dy * dy) > 160.f) return false;

    // 左门(cols2-3) → 204教室
    if (tile == static_cast<int>(TileType::Door) && col <= 4) {
        m_choiceDialog.Show(
            "教室里熙熙攘攘",
            {"204教室", "这是我的教室", "这不是我的教室"}
        );
        return true;
    }
    // 右门(cols9-10) → 206教室
    if (tile == static_cast<int>(TileType::Door) && col >= 8) {
        m_choiceDialog.Show(
            "里面的寥寥数人表情严肃地坐着",
            {"206教室", "这是我的教室", "这不是我的教室"}
        );
        return true;
    }
    return false;
}

void Scene_2_3::CheckExits() {
    sf::Vector2f pos = m_player.GetPosition();
    int col = TileMap::PixelToTile(pos.x);
    int row = TileMap::PixelToTile(pos.y);

    // 右边缘(col16) → 2-2
    if (col >= 16 && row >= 3) {
        m_nextScene  = static_cast<int>(SceneID::Corridor2_2);
        m_entryParam = 1;
    }
}
