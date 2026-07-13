#include "Scene_Stairwell1.h"

Scene_Stairwell1::Scene_Stairwell1(int entryParam) {
    m_bgmId = 0;
    LoadMap(BuildMapData(), GetSpawnPoint(entryParam));
    // 楼梯顶行(row7,cols17-21)阻挡，不可踩
    for (int c = 17; c <= 21; ++c) m_tileMap.SetBlocked(c, 7);
    // 门(cols8-9,row12)强制可行走
    m_tileMap.SetForceWalkable(8, 12);
    m_tileMap.SetForceWalkable(9, 12);
    if (entryParam == 1)
        m_player.SetFacing(Direction::Right);
}

MapData Scene_Stairwell1::BuildMapData() {
    // 23×15 横向圆润长方形，右下角切掉 6×5 方块，顶部加3行墙壁
    return MapBuilder::FromStrings({
        //         1111111111222222
        // 01234567890123456789012
        "#######################", // 0  墙壁
        "#######################", // 1  墙壁
        "#######################", // 2  墙壁
        "#####VV################", // 3  墙壁+贩卖机上段(cols5-6)
        "###  VV    XXXX     ###", // 4  贩卖机+杂物(cols11-14)
        "##   VV    XXXX      ##", // 5  贩卖机+杂物
        "#                     #", // 6
        "#                ^^^^^#", // 7  楼梯顶(阻挡)
        "#   P            ^^^^^#", // 8  楼梯中段(可行走→切换)
        "#RR              ^^^^^#", // 9  楼梯下段(可行走→切换)
        "#RR              ######", // 10 垃圾桶+右下缺口
        "##               ######", // 11 左圆角+缺口
        "###     DD       ######", // 12 门(上移一行)
        "#######################", // 13 底墙
    });
}

sf::Vector2f Scene_Stairwell1::GetSpawnPoint(int entryParam) {
    if (entryParam == 1)
        return { 16.f * TILE_SIZE, TileMap::TileToPixel(8) };  // 从楼梯间②下来
    return { 8.5f * TILE_SIZE, TileMap::TileToPixel(11) };      // 门上方(紧贴门)
}

sf::Vector2f Scene_Stairwell1::GetSavePosition() {
    // 打印机 col4 row8，出生在它右边一格
    return { 5.5f * TILE_SIZE, TileMap::TileToPixel(8) };
}

void Scene_Stairwell1::CheckExits() {
    sf::Vector2f pos = m_player.GetPosition();
    int tile = m_tileMap.GetTileAtPixel(pos.x, pos.y);
    int col = TileMap::PixelToTile(pos.x);
    int row = TileMap::PixelToTile(pos.y);
    // 楼梯(rows8-9) → 楼梯间②
    if (static_cast<TileType>(tile) == TileType::Stairs) {
        m_nextScene  = static_cast<int>(SceneID::Stairwell2);
        m_entryParam = 0;
    }
    // 门 → 一楼(踩上即触发)
    if (static_cast<TileType>(tile) == TileType::Door) {
        m_nextScene  = static_cast<int>(SceneID::Classroom1B);
        m_entryParam = 1;
    }
}

bool Scene_Stairwell1::OnTileClick(int tile, int col, int row) {
    (void)row;
    sf::Vector2f pp = m_player.GetPosition();
    float dx = (col + 0.5f) * TILE_SIZE - pp.x;
    float dy = (row + 0.5f) * TILE_SIZE - pp.y;
    if (std::sqrt(dx * dx + dy * dy) > 160.f) return false;

    // 杂物
    if (tile == static_cast<int>(TileType::Debris)) {
        StartDialogue({"一堆杂物，乱糟糟的"});
        return true;
    }
    // 贩卖机
    if (tile == static_cast<int>(TileType::VendingMachine)) {
        StartDialogue({"一台自动贩卖机，现在我没心情买饮料喝"});
        return true;
    }
    // 垃圾桶
    if (tile == static_cast<int>(TileType::TrashCan)) {
        StartDialogue({"空无一物的垃圾桶，应该是昨晚被清洁工打扫干净了。咦，不对...里面有张小纸条。"});
        m_pendingNote = true;
        return true;
    }
    return false;
}

void Scene_Stairwell1::Update(float dt) {
    GameScene::Update(dt);
    // 垃圾桶对话结束后弹出纸条
    if (m_pendingNote && !IsInDialogue()) {
        m_pendingNote = false;
        m_choiceDialog.ShowInfo("小纸条（内容待补充）");
    }
}
