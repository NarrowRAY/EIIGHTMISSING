#include "Scene_2_3.h"

Scene_2_3::Scene_2_3(int entryParam) {
    m_bgmId = 0;
    LoadMap(BuildMapData(), GetSpawnPoint(entryParam));

    // ———— 门精灵 ————
    m_doorPositions = {
        {2, 1},   // 左门 (cols2-3, rows1-3, 2×3)
        {9, 1},   // 右门 (cols9-10, rows1-3, 2×3)
    };

    if (m_doorTex.loadFromFile("assets/textures/doors.png")) {
        m_doorTex.setSmooth(true);
        for (const auto& dp : m_doorPositions) {
            auto spr = std::make_unique<sf::Sprite>(m_doorTex);
            auto ts = m_doorTex.getSize();
            float sx = (2.f * TILE_SIZE) / ts.x * 1.5f;
            float sy = (3.f * TILE_SIZE) / ts.y * 1.5f;
            bool isRight = (dp.x > 5);
            if (isRight) {
                spr->setOrigin({static_cast<float>(ts.x), static_cast<float>(ts.y)});
                spr->setScale({-sx, sy});
                spr->setPosition({dp.x * 1.f * TILE_SIZE - 20.f, dp.y * 1.f * TILE_SIZE + 158.f});
            } else {
                spr->setScale({sx, sy});
                spr->setPosition({dp.x * 1.f * TILE_SIZE - 20.f, dp.y * 1.f * TILE_SIZE - 23.f});
            }
            m_doorSprites.push_back(std::move(spr));
        }
    }

    // ———— 窗户精灵 (2×2 each) ————
    m_windowPositions = {{4, 1}, {11, 1}};
    if (m_windowTex.loadFromFile("assets/textures/window.png")) {
        m_windowTex.setSmooth(true);
        for (const auto& dp : m_windowPositions) {
            auto spr = std::make_unique<sf::Sprite>(m_windowTex);
            auto ts = m_windowTex.getSize();
            float sx = (2.f * TILE_SIZE) / ts.x * 1.98f;
            float sy = (2.f * TILE_SIZE) / ts.y * 1.98f;
            spr->setScale({sx, sy});
            spr->setPosition({dp.x * 1.f * TILE_SIZE + 10.f, dp.y * 1.f * TILE_SIZE - 13.f});
            m_windowSprites.push_back(std::move(spr));
        }
    }

    // ———— 打印机精灵 (col13, row8, 1×1) ————
    m_printerPositions = {{13, 8}};
    if (m_printerTex.loadFromFile("assets/textures/printer2.png")) {
        m_printerTex.setSmooth(true);
        for (const auto& dp : m_printerPositions) {
            auto spr = std::make_unique<sf::Sprite>(m_printerTex);
            auto ts = m_printerTex.getSize();
            spr->setScale({(1.f * TILE_SIZE) / ts.x * 3.0f, (1.f * TILE_SIZE) / ts.y * 3.0f});
            spr->setPosition({dp.x * 1.f * TILE_SIZE, dp.y * 1.f * TILE_SIZE - 40.f});
            m_printerSprites.push_back(std::move(spr));
        }
    }
    m_tileMap.SetBlocked(14, 8, true);

    // ———— GoodEnd CG 全屏贴图 ————
    if (m_goodEndTex.loadFromFile("assets/textures/goodend.png")) {
        m_goodEndTex.setSmooth(true);
        m_goodEndSprite = std::make_unique<sf::Sprite>(m_goodEndTex);
        auto ts = m_goodEndTex.getSize();
        m_goodEndSprite->setScale({SCREEN_W / static_cast<float>(ts.x), SCREEN_H / static_cast<float>(ts.y)});
        m_goodEndSprite->setPosition({0.f, 0.f});
    }

    // ———— BadEnd CG 全屏贴图 ————
    if (m_badEndTex.loadFromFile("assets/textures/badend.png")) {
        m_badEndTex.setSmooth(true);
        m_badEndSprite = std::make_unique<sf::Sprite>(m_badEndTex);
        auto ts = m_badEndTex.getSize();
        m_badEndSprite->setScale({SCREEN_W / static_cast<float>(ts.x), SCREEN_H / static_cast<float>(ts.y)});
        m_badEndSprite->setPosition({0.f, 0.f});
    }

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
        "#################", // 1  窗sprite
        "#################", // 2
        "#################", // 3
        "                 ", // 4  地板
        "                 ", // 5
        "                 ", // 6
        "                 ", // 7
        "                 ", // 8  打印机sprite
    });
}

sf::Vector2f Scene_2_3::GetSpawnPoint(int entryParam) {
    if (entryParam == 1) return { TileMap::TileToPixel(2), TileMap::TileToPixel(6) };
    return { TileMap::TileToPixel(15), TileMap::TileToPixel(6) };
}

sf::Vector2f Scene_2_3::GetSavePosition() {
    return { TileMap::TileToPixel(11), TileMap::TileToPixel(8) };
}

bool Scene_2_3::OnTileClick(int tile, int col, int row) {
    (void)row;
    sf::Vector2f pp = m_player.GetPosition();
    float dx = (col + 0.5f) * TILE_SIZE - pp.x;
    float dy = (row + 0.5f) * TILE_SIZE - pp.y;
    if (std::sqrt(dx * dx + dy * dy) > 160.f) return false;

    // 打印机 (col13, row8)
    for (const auto& dp : m_printerPositions) {
        (void)dp;
        float bx = 14.f * TILE_SIZE, by = 8.f * TILE_SIZE;
        float bw = 1.f * TILE_SIZE, bh = 1.f * TILE_SIZE;
        float cx = (col + 0.5f) * TILE_SIZE, cy = (row + 0.5f) * TILE_SIZE;
        if (cx >= bx && cx < bx + bw && cy >= by && cy < by + bh) {
            m_savePanel.Show(true);
            return true;
        }
    }

    // 左门(cols2-3, rows1-3) → 204教室
    for (const auto& dp : m_doorPositions) {
        if (dp.x > 5) continue;  // 只检查左门
        float bx = dp.x * 1.f * TILE_SIZE;
        float by = dp.y * 1.f * TILE_SIZE;
        float bw = 2.f * TILE_SIZE;
        float bh = 3.f * TILE_SIZE;
        float cx = (col + 0.5f) * TILE_SIZE;
        float cy = (row + 0.5f) * TILE_SIZE;
        if (cx >= bx && cx < bx + bw && cy >= by && cy < by + bh) {
            ShowDoorChoice(
                "教室里熙熙攘攘",
                {"204教室", "这是我的教室", "这不是我的教室"},
                true
            );
            return true;
        }
    }
    // 右门(cols9-10, rows1-3) → 206教室
    for (const auto& dp : m_doorPositions) {
        if (dp.x < 5) continue;
        float bx = dp.x * 1.f * TILE_SIZE;
        float by = dp.y * 1.f * TILE_SIZE;
        float bw = 2.f * TILE_SIZE;
        float bh = 3.f * TILE_SIZE;
        float cx = (col + 0.5f) * TILE_SIZE;
        float cy = (row + 0.5f) * TILE_SIZE;
        if (cx >= bx && cx < bx + bw && cy >= by && cy < by + bh) {
            ShowDoorChoice(
                "里面的寥寥数人表情严肃地坐着",
                {"206教室", "这是我的教室", "这不是我的教室"},
                false
            );
            return true;
        }
    }
    return false;
}

void Scene_2_3::CheckExits() {
    sf::Vector2f pos = m_player.GetPosition();
    int col = TileMap::PixelToTile(pos.x);
    int row = TileMap::PixelToTile(pos.y);

    if (col >= 16 && row >= 3) {
        m_nextScene  = static_cast<int>(SceneID::Corridor2_2);
        m_entryParam = 1;
    }
}

void Scene_2_3::Draw(sf::RenderWindow& window) {
    window.setView(m_camera);
    sf::Vector2f cam = m_camera.getCenter();
    m_tileMap.Draw(window, cam.x, cam.y);
    for (const auto& spr : m_doorSprites)
        window.draw(*spr);
    for (const auto& spr : m_windowSprites)
        window.draw(*spr);
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

        // ———— CG 全屏贴图（在对话框下层） ————
        if (m_showGoodEnd && m_goodEndSprite) {
            window.draw(*m_goodEndSprite);
        }
        if (m_showBadEnd && m_badEndSprite) {
            window.draw(*m_badEndSprite);
        }

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
