#include "Scene_1B.h"

Scene_1B::Scene_1B(int entryParam) {
    m_entryParam = entryParam;
    LoadMap(BuildMapData(), GetSpawnPoint(entryParam));
    if (entryParam == 0) s_passwordUnlocked = false;  // 新游戏重置密码锁
}

MapData Scene_1B::BuildMapData() {
    // 30×15 一楼前厅
    // 墙(0-4) / 地板(5-14)
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
        "                             K", // 7  密码锁(右边缘)
        "              ||             K", // 8  显示屏(上)+密码锁
        "              ||              ", // 9  显示屏(中)
        "P             ||              ", // 10 打印机+显示屏(下)
        "                              ", // 11 地板
        "    SSSSSS          SSSSSS   B", // 12 双沙发+公告栏
        "    SSSSSS          SSSSSS   B", // 13 双沙发+公告栏
        "                              ", // 14 地板
    });
}

sf::Vector2f Scene_1B::GetSpawnPoint(int entryParam) {
    if (entryParam == 1)
        return { 28.f * TILE_SIZE + TILE_SIZE / 2.f, TileMap::TileToPixel(8) };  // 从楼梯间回来→密码锁旁
    return { 14.5f * TILE_SIZE, TileMap::TileToPixel(14) };       // 新游戏→默认中央
}

void Scene_1B::CheckExits() {
    // 一楼独立调试阶段，暂不连接其他场景
}

bool Scene_1B::OnTileClick(int tile, int col, int row) {
    (void)row;
    sf::Vector2f pp = m_player.GetPosition();
    float dx = (col + 0.5f) * TILE_SIZE - pp.x;
    float dy = (row + 0.5f) * TILE_SIZE - pp.y;
    if (std::sqrt(dx * dx + dy * dy) > 160.f) return false;

    // 电子显示屏
    if (tile == static_cast<int>(TileType::Display)) {
        m_choiceDialog.ShowInfo("教室使用情况");
        return true;
    }

    // 左门（cols 8-9）
    if (tile == static_cast<int>(TileType::Door) && col <= 10) {
        float dx = (col + 0.5f) * TILE_SIZE - pp.x;
        float dy = (row + 0.5f) * TILE_SIZE - pp.y;
        if (std::sqrt(dx * dx + dy * dy) <= 160.f) {
            m_choiceDialog.Show(
                "教室里坐满了人...",
                {"106教室", "这是我的教室", "这不是我的教室"}
            );
            return true;
        }
    }
    // 右门（cols 18-19）
    if (tile == static_cast<int>(TileType::Door) && col >= 17) {
        float dx = (col + 0.5f) * TILE_SIZE - pp.x;
        float dy = (row + 0.5f) * TILE_SIZE - pp.y;
        if (std::sqrt(dx * dx + dy * dy) <= 160.f) {
            m_choiceDialog.Show(
                "教室里零星有人入座...",
                {"108教室", "这是我的教室", "这不是我的教室"}
            );
            return true;
        }
    }
    return false;
}

void Scene_1B::OnSceneEnter() {
    if (m_entryParam != 0) return;  // 非首次进入（如从楼梯间返回）不触发
    StartDialogue({
        "今天是我在这所学校上课的第一天，",
        "昨晚打丝之歌打到了凌晨，头好痛...",
        "好不容易在迟到之前赶到教学楼，教室是哪间来着...",
        "拿出手机查一下吧。",
        ".......欸？？？！！我的手机呢？",
        "该不会，落在宿舍了吧..."
    }, 0.5f);  // 半秒后弹出
}
