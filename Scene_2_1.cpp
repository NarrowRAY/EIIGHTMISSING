#include "Scene_2_1.h"

Scene_2_1::Scene_2_1(int entryParam) {
    m_bgmId = 0;
    LoadMap(BuildMapData(), GetSpawnPoint(entryParam));
    // 右门(row4-5,col14,B瓦片贴右墙)强制可行走
    m_tileMap.SetForceWalkable(14, 4);
    m_tileMap.SetForceWalkable(14, 5);

    // ———— 打印机精灵 (col14, row7, 1×1) ————
    m_printerPositions = {{14, 7}};
    if (m_printerTex.loadFromFile("assets/textures/printer2.png")) {
        m_printerTex.setSmooth(true);
        for (const auto& dp : m_printerPositions) {
            auto spr = std::make_unique<sf::Sprite>(m_printerTex);
            auto ts = m_printerTex.getSize();
            spr->setScale({(1.f * TILE_SIZE) / ts.x * 3.0f, (1.f * TILE_SIZE) / ts.y * 3.0f});
            spr->setPosition({dp.x * 1.f * TILE_SIZE - 45.f, dp.y * 1.f * TILE_SIZE - 45.f});
            m_printerSprites.push_back(std::move(spr));
        }
    }

    m_tileMap.SetBlocked(14, 7, true);
    // 施工区阻挡(cols3-4, rows0-2) + 贴图
    for (int c = 3; c <= 4; ++c)
        for (int r = 0; r <= 2; ++r)
            m_tileMap.SetBlocked(c, r, true);

    if (m_signTex.loadFromFile("assets/textures/sign_construction.png")) {
        m_signTex.setSmooth(true);
        m_signSprite = std::make_unique<sf::Sprite>(m_signTex);
        auto ts = m_signTex.getSize();
        // 施工区 2×3 格，左上角 (col2, row0)
        m_signSprite->setScale({
            (2.f * TILE_SIZE) / ts.x * 0.792f,
            (3.f * TILE_SIZE) / ts.y * 0.72f
        });
        m_signSprite->setPosition({
            2.f * TILE_SIZE + 10.f,
            0.f * TILE_SIZE + 35.f
        });
    }

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
        "##  ###########", // 0  施工区(地板+阻挡)
        "##  ###########", // 1
        "##  ###########", // 2
        "               ", // 3
        "              B", // 4 右门(楼梯间②)
        "              B", // 5
        "               ", // 6
        "               ", // 7 打印机sprite
    });
}

sf::Vector2f Scene_2_1::GetSpawnPoint(int entryParam) {
    if (entryParam == 1) return { TileMap::TileToPixel(1), TileMap::TileToPixel(5) };
    return { 13.5f * TILE_SIZE, TileMap::TileToPixel(4) };
}

sf::Vector2f Scene_2_1::GetSavePosition() {
    return { 12.5f * TILE_SIZE, TileMap::TileToPixel(7) };
}

bool Scene_2_1::OnTileClick(int tile, int col, int row) {
    (void)row;
    sf::Vector2f pp = m_player.GetPosition();
    float dx = (col + 0.5f) * TILE_SIZE - pp.x;
    float dy = (row + 0.5f) * TILE_SIZE - pp.y;
    if (std::sqrt(dx * dx + dy * dy) > 160.f) return false;

    // 打印机 (col14, row7)
    for (const auto& dp : m_printerPositions) {
        (void)dp;
        float bx = 14.f * TILE_SIZE;
        float by = 7.f * TILE_SIZE;
        float bw = 1.f * TILE_SIZE;
        float bh = 1.f * TILE_SIZE;
        float cx = (col + 0.5f) * TILE_SIZE;
        float cy = (row + 0.5f) * TILE_SIZE;
        if (cx >= bx && cx < bx + bw && cy >= by && cy < by + bh) {
            m_savePanel.Show(true);
            return true;
        }
    }

    // 施工标志 (cols3-4, rows0-2)
    if (col >= 3 && col <= 4 && row >= 0 && row <= 2) {
        StartDialogue({"摆着施工标志，看来这条路是走不通了"}, {Portrait::Nomal});
        return true;
    }
    return false;
}

void Scene_2_1::CheckExits() {
    sf::Vector2f pos = m_player.GetPosition();
    int tile = m_tileMap.GetTileAtPixel(pos.x, pos.y);
    if (static_cast<TileType>(tile) == TileType::BulletinBoard) {
        m_nextScene  = static_cast<int>(SceneID::Stairwell2);
        m_entryParam = 1;
    }
    int col = TileMap::PixelToTile(pos.x);
    if (col <= 0 && pos.y > 1 * TILE_SIZE && pos.y < 8 * TILE_SIZE) {
        m_nextScene  = static_cast<int>(SceneID::Corridor2_2);
        m_entryParam = 0;
    }
}

void Scene_2_1::Draw(sf::RenderWindow& window) {
    window.setView(m_camera);
    sf::Vector2f cam = m_camera.getCenter();
    m_tileMap.Draw(window, cam.x, cam.y);
    if (m_signSprite) window.draw(*m_signSprite);
    for (const auto& spr : m_printerSprites)
        window.draw(*spr);
    m_player.Draw(window);

    if (m_font) {
        sf::Vector2f pos = m_player.GetPosition();
        int col = TileMap::PixelToTile(pos.x);
        int row = TileMap::PixelToTile(pos.y);
        int tile = m_tileMap.GetTileAtPixel(pos.x, pos.y);
        auto off = m_player.GetHitOffset();
        std::string info = "col:" + std::to_string(col)
                         + " row:" + std::to_string(row)
                         + " tile:" + std::to_string(tile)
                         + " off:" + std::to_string(static_cast<int>(off.x))
                         + "," + std::to_string(static_cast<int>(off.y));
        sf::Text dbg(*m_font, U(info), 18);
        dbg.setFillColor(sf::Color::White);
        dbg.setOutlineColor(sf::Color::Black);
        dbg.setOutlineThickness(1.f);
        dbg.setPosition({cam.x - SCREEN_W / 2.f + 10.f, cam.y - SCREEN_H / 2.f + 10.f});
        window.draw(dbg);
    }

    {
        sf::View uiView({SCREEN_W / 2.f, SCREEN_H / 2.f}, {SCREEN_W, SCREEN_H});
        window.setView(uiView);
        bool hasModal = IsInDialogue() || m_passwordUI.IsVisible() || m_choiceDialog.IsVisible() || m_savePanel.IsVisible();
        if (hasModal) {
            if (IsInDialogue())            DrawDialogueBox(window);
            if (m_passwordUI.IsVisible())  m_passwordUI.Draw(window);
            if (m_choiceDialog.IsVisible()) m_choiceDialog.Draw(window);
            if (m_savePanel.IsVisible())   m_savePanel.Draw(window);
        }
        if (!IsInDialogue() && m_dialogueTimer <= 0.f) {
            sf::RectangleShape shape;
            const float bx = SCREEN_W - 60.f, by = 10.f, bs = 50.f;
            const float cx = bx + bs / 2.f, cy = by + bs / 2.f;

            // 背景：砍角方形（暖白配色）
            sf::ConvexShape bg(8);
            const float c2 = 8.f;
            bg.setPoint(0, {c2, 0.f}); bg.setPoint(1, {bs - c2, 0.f});
            bg.setPoint(2, {bs, c2}); bg.setPoint(3, {bs, bs - c2});
            bg.setPoint(4, {bs - c2, bs}); bg.setPoint(5, {c2, bs});
            bg.setPoint(6, {0.f, bs - c2}); bg.setPoint(7, {0.f, c2});
            bg.setPosition({bx, by});
            bg.setFillColor(sf::Color(240, 235, 225, 220));
            bg.setOutlineColor(sf::Color(160, 145, 120, 180));
            bg.setOutlineThickness(1.5f);
            window.draw(bg);

            // 齿轮：8齿 + 外环 + 中心圆
            const float outerR = 16.f, innerR = 11.f, toothLen = outerR - innerR;
            const sf::Color gearColor(100, 85, 65);
            for (int i = 0; i < 8; ++i) {
                float a = i * 3.14159265f * 2.f / 8.f;
                float mx = cx + std::cos(a) * (innerR + toothLen / 2.f);
                float my = cy + std::sin(a) * (innerR + toothLen / 2.f);
                sf::RectangleShape tooth({5.f, toothLen + 2.f});
                tooth.setFillColor(gearColor);
                tooth.setOrigin({2.5f, toothLen / 2.f + 1.f});
                tooth.setPosition({mx, my});
                tooth.setRotation(sf::radians(a + 3.14159265f / 2.f));
                window.draw(tooth);
            }
            sf::CircleShape ring(innerR);
            ring.setOrigin({innerR, innerR});
            ring.setPosition({cx, cy});
            ring.setFillColor(sf::Color::Transparent);
            ring.setOutlineColor(gearColor);
            ring.setOutlineThickness(3.f);
            window.draw(ring);
            sf::CircleShape hub(5.f);
            hub.setOrigin({5.f, 5.f});
            hub.setPosition({cx, cy});
            hub.setFillColor(gearColor);
            window.draw(hub);
        }
    }
}
