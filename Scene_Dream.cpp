#include "Scene_Dream.h"

Scene_Dream::Scene_Dream(int entryParam) {
    m_bgmId = 0;
    LoadMap(BuildMapData(), GetSpawnPoint(entryParam));
}

MapData Scene_Dream::BuildMapData() {
    // 30×15 — 梦境版一楼(同Scene_1B)
    return MapBuilder::FromStrings({
        //         11111111112222222222
        // 012345678901234567890123456789
        "##############################", // 0  顶墙
        "###WW####################WW###", // 1  窗
        "###WW###DD##########DD###WW###", // 2  窗+门
        "########DD##########DD########", // 3  门
        "########DD##########DD########", // 4  门底
        "                              ", // 5  地板
        "                              ", // 6  地板
        "                              ", // 7  地板
        "              ||              ", // 8  显示屏(上)
        "              ||              ", // 9  显示屏(中)
        "P             ||              ", // 10 打印机+显示屏(下)
        "                              ", // 11 地板
        "    SSSSSS          SSSSSS   B", // 12 双沙发+公告栏
        "    SSSSSS          SSSSSS   B", // 13 双沙发+公告栏
        "                              ", // 14 地板
    });
}

sf::Vector2f Scene_Dream::GetSpawnPoint(int entryParam) {
    (void)entryParam;
    return { 28.f * TILE_SIZE + TILE_SIZE / 2.f, TileMap::TileToPixel(11) };  // 右侧
}

bool Scene_Dream::OnTileClick(int tile, int col, int row) {
    (void)row;
    sf::Vector2f pp = m_player.GetPosition();
    float dx = (col + 0.5f) * TILE_SIZE - pp.x;
    float dy = (row + 0.5f) * TILE_SIZE - pp.y;
    if (std::sqrt(dx * dx + dy * dy) > 160.f) return false;

    // 电子显示屏(梦境版)
    if (tile == static_cast<int>(TileType::Display)) {
        m_choiceDialog.ShowInfo("一块巨大的手机屏幕，展示了混乱的qq聊天记录");
        return true;
    }
    // 左门
    if (tile == static_cast<int>(TileType::Door) && col <= 10) {
        m_choiceDialog.Show(
            "教室里坐满了人...",
            {"106教室", "这是我的教室", "这不是我的教室"}
        );
        return true;
    }
    // 右门
    if (tile == static_cast<int>(TileType::Door) && col >= 17) {
        m_choiceDialog.Show(
            "教室里零星有人入座...",
            {"108教室", "这是我的教室", "这不是我的教室"}
        );
        return true;
    }
    return false;
}

void Scene_Dream::OnSceneEnter() {
    StartDialogue({
        "（一阵眩晕）",
        "这是哪？我回到了一楼？",
        "周围笼罩着一层黑暗，好像我的梦境..."
    }, 0.5f);
}

void Scene_Dream::CheckExits() {
    // 暂无出口
}

void Scene_Dream::Draw(sf::RenderWindow& window) {
    GameScene::Draw(window);
    // 阴间滤镜：暗蓝紫遮罩
    sf::View uiView({SCREEN_W / 2.f, SCREEN_H / 2.f}, {SCREEN_W, SCREEN_H});
    window.setView(uiView);
    sf::RectangleShape overlay({static_cast<float>(SCREEN_W), static_cast<float>(SCREEN_H)});
    overlay.setFillColor(sf::Color(15, 5, 30, 100));
    window.draw(overlay);
}
