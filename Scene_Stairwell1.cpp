#include "Scene_Stairwell1.h"

Scene_Stairwell1::Scene_Stairwell1(int entryParam) {
    LoadMap(BuildMapData(), GetSpawnPoint(entryParam));
    // 楼梯顶行(row7,cols17-21)阻挡，不可踩
    for (int c = 17; c <= 21; ++c) m_tileMap.SetBlocked(c, 7);
    // 门(cols8-9,row12)强制可行走
    m_tileMap.SetForceWalkable(8, 12);
    m_tileMap.SetForceWalkable(9, 12);
    // 垃圾桶碰撞 (cols12-15, row4)
    for (int c = 12; c <= 15; ++c)
        for (int r = 4; r <= 4; ++r)
            m_tileMap.SetBlocked(c, r, true);
    // 原'R'垃圾桶区域碰撞 (cols1-2, rows9-10)
    for (int c = 1; c <= 2; ++c)
        for (int r = 9; r <= 10; ++r)
            m_tileMap.SetBlocked(c, r, true);

    // ———— 杂物精灵 (Trash can.png → 右上角cols11-14, rows4-5, 4×2) ————
    m_debrisPositions = {{11, 4}};
    if (m_debrisTex.loadFromFile("assets/textures/Trash can.png")) {
        m_debrisTex.setSmooth(true);
        for (const auto& dp : m_debrisPositions) {
            auto spr = std::make_unique<sf::Sprite>(m_debrisTex);
            auto ts = m_debrisTex.getSize();
            spr->setScale({(4.f * TILE_SIZE) / ts.x * 1.5f, (2.f * TILE_SIZE) / ts.y * 2.16f});
            spr->setPosition({dp.x * 1.f * TILE_SIZE, dp.y * 1.f * TILE_SIZE - 70.f});
            m_debrisSprites.push_back(std::move(spr));
        }
    }

    // ———— 打印机精灵 (col3, row8, 1×1) ————
    m_printerPositions = {{3, 8}};
    if (m_printerTex.loadFromFile("assets/textures/printer2.png")) {
        m_printerTex.setSmooth(true);
        for (const auto& dp : m_printerPositions) {
            auto spr = std::make_unique<sf::Sprite>(m_printerTex);
            auto ts = m_printerTex.getSize();
            spr->setScale({(1.f * TILE_SIZE) / ts.x * 3.0f, (1.f * TILE_SIZE) / ts.y * 3.0f});
            spr->setPosition({dp.x * 1.f * TILE_SIZE - 85.f, dp.y * 1.f * TILE_SIZE});
            m_printerSprites.push_back(std::move(spr));
        }
    }
    m_tileMap.SetBlocked(2, 9, true);

    // ———— 楼梯精灵 (cols17-21, rows7-9, 5×3) ————
    m_stairPositions = {{17, 7}};
    if (m_stairTex.loadFromFile("assets/textures/stair1.png")) {
        m_stairTex.setSmooth(true);
        for (const auto& dp : m_stairPositions) {
            auto spr = std::make_unique<sf::Sprite>(m_stairTex);
            auto ts = m_stairTex.getSize();
            spr->setScale({(5.f * TILE_SIZE) / ts.x * 1.5f, (3.f * TILE_SIZE) / ts.y * 1.5f * 1.05f});
            spr->setPosition({dp.x * 1.f * TILE_SIZE - 44.f, dp.y * 1.f * TILE_SIZE - 13.f});
            m_stairSprites.push_back(std::move(spr));
        }
    }

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
        "###  VV             ###", // 4  贩卖机(杂物sprite)
        "##   VV              ##", // 5  贩卖机(杂物sprite)
        "#                     #", // 6
        "#                     #", // 7  楼梯顶(楼梯sprite)
        "#                     #", // 8  楼梯中段(楼梯sprite)
        "#                     #", // 9  楼梯下段(楼梯sprite)
        "#                ######", // 10 右下缺口
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
    // 楼梯(cols17-21, rows7-9) → 楼梯间②
    if (col >= 17 && col <= 21 && row >= 7 && row <= 9) {
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

    // 垃圾桶(cols12-15, row4)
    for (const auto& dp : m_debrisPositions) {
        float bx = dp.x * 1.f * TILE_SIZE + 1.f * TILE_SIZE;
        float by = dp.y * 1.f * TILE_SIZE;
        float bw = 4.f * TILE_SIZE;
        float bh = 1.f * TILE_SIZE;
        float cx = (col + 0.5f) * TILE_SIZE;
        float cy = (row + 0.5f) * TILE_SIZE;
        if (cx >= bx && cx < bx + bw && cy >= by && cy < by + bh) {
            StartDialogue({"空无一物的垃圾桶，应该是昨晚被清洁工打扫干净了。咦，不对...里面有张小纸条。"}, {Portrait::Whattt});
            m_pendingNote = true;
            return true;
        }
    }
    // 打印机 (col3, row8)
    for (const auto& dp : m_printerPositions) {
        (void)dp;
        float bx = 3.f * TILE_SIZE;
        float by = 8.f * TILE_SIZE;
        float bw = 1.f * TILE_SIZE;
        float bh = 1.f * TILE_SIZE;
        float cx = (col + 0.5f) * TILE_SIZE;
        float cy = (row + 0.5f) * TILE_SIZE;
        if (cx >= bx && cx < bx + bw && cy >= by && cy < by + bh) {
            m_savePanel.Show(true);
            return true;
        }
    }

    // 贩卖机
    if (tile == static_cast<int>(TileType::VendingMachine)) {
        StartDialogue({"一台自动贩卖机，现在我没心情买饮料喝"}, {Portrait::Nomal});
        return true;
    }
    return false;
}

void Scene_Stairwell1::Draw(sf::RenderWindow& window) {
    // 地图+杂物+玩家（相机视图）
    window.setView(m_camera);
    sf::Vector2f cam = m_camera.getCenter();
    m_tileMap.Draw(window, cam.x, cam.y);
    for (const auto& spr : m_debrisSprites)
        window.draw(*spr);
    for (const auto& spr : m_printerSprites)
        window.draw(*spr);
    for (const auto& spr : m_stairSprites)
        window.draw(*spr);
    m_player.Draw(window);

    // 调试信息
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

    // UI层（屏幕视图）
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
            shape.setSize({bs, bs});
            shape.setPosition({bx, by});
            shape.setFillColor(sf::Color(50, 50, 60, 200));
            shape.setOutlineColor(sf::Color(150, 150, 170));
            shape.setOutlineThickness(2.f);
            window.draw(shape);
            float gcx = bx + bs / 2.f, gcy = by + bs / 2.f;
            sf::CircleShape circle(10.f);
            circle.setFillColor(sf::Color(200, 200, 220));
            circle.setOrigin({10.f, 10.f});
            circle.setPosition({gcx, gcy});
            window.draw(circle);
            for (int i = 0; i < 6; ++i) {
                float angle = i * 3.14159f / 3.f;
                float sx = gcx + std::cos(angle) * 16.f;
                float sy = gcy + std::sin(angle) * 16.f;
                sf::RectangleShape tooth(sf::Vector2f(10.f, 4.f));
                tooth.setFillColor(sf::Color(200, 200, 220));
                tooth.setOrigin({5.f, 2.f});
                tooth.setPosition({sx, sy});
                tooth.setRotation(sf::radians(angle));
                window.draw(tooth);
            }
        }
    }
}

void Scene_Stairwell1::Update(float dt) {
    GameScene::Update(dt);
    // 垃圾桶对话结束后弹出纸条
    if (m_pendingNote && !IsInDialogue()) {
        m_pendingNote = false;
        m_choiceDialog.ShowInfo("小纸条（内容待补充）");
    }
}
