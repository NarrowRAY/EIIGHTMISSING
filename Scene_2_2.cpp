#include "Scene_2_2.h"

Scene_2_2::Scene_2_2(int entryParam) {
    m_bgmId = 0;
    LoadMap(BuildMapData(), GetSpawnPoint(entryParam));
    if (entryParam == 0) {
        m_player.SetFacing(Direction::Down);
        LockInput(0.5f);
    }
    if (entryParam == 1) {
        m_player.SetFacing(Direction::Up);    // 从2-3回来→背面
        LockInput(0.5f);
    }

    // ———— 清洁车精灵 (cols5-6, rows11-12, 2×2) ————
    m_cartPositions = {{5, 11}};
    if (m_cartTex.loadFromFile("assets/textures/cleaning cart.png")) {
        m_cartTex.setSmooth(true);
        for (const auto& dp : m_cartPositions) {
            auto spr = std::make_unique<sf::Sprite>(m_cartTex);
            auto ts = m_cartTex.getSize();
            spr->setScale({(2.f * TILE_SIZE) / ts.x * 1.5f, (2.f * TILE_SIZE) / ts.y * 1.5f});
            spr->setPosition({dp.x * 1.f * TILE_SIZE - 13.f, dp.y * 1.f * TILE_SIZE - 30.f});
            m_cartSprites.push_back(std::move(spr));
        }
    }
    // 清洁车碰撞 (col6, rows11-12)
    for (int c = 6; c <= 6; ++c)
        for (int r = 11; r <= 12; ++r)
            m_tileMap.SetBlocked(c, r, true);

    // ———— 打印机精灵 (col5, row1, 1×1) ————
    m_printerPositions = {{5, 1}};
    if (m_printerTex.loadFromFile("assets/textures/printer2.png")) {
        m_printerTex.setSmooth(true);
        for (const auto& dp : m_printerPositions) {
            auto spr = std::make_unique<sf::Sprite>(m_printerTex);
            auto ts = m_printerTex.getSize();
            spr->setScale({(1.f * TILE_SIZE) / ts.x * 3.0f, (1.f * TILE_SIZE) / ts.y * 3.0f});
            spr->setPosition({dp.x * 1.f * TILE_SIZE - 5.f, dp.y * 1.f * TILE_SIZE});
            m_printerSprites.push_back(std::move(spr));
        }
    }
    m_tileMap.SetBlocked(6, 2, true);

    // 初始化陷阱：随机相位，打散起点避免同步
    for (int r = 0; r < TRAP_ROWS; ++r) {
        for (int c = 0; c < TRAP_COLS; ++c) {
            m_trapState[r][c] = 0;  // 全部从安全开始
            // 用行列号生成伪随机倒计时 (0.2 ~ SAFE_MAX 之间分散)
            float seed = static_cast<float>((r * 31 + c * 17 + (r ^ c) * 7) % 100) / 100.f;
            m_trapTimer[r][c] = 0.3f + seed * (SAFE_MAX - 0.3f);
        }
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
        "       ",  // 1  打印机sprite
        "       ",  // 2
        "       ",  // 3
        "       ",  // 4  陷阱区(外观同地板，运行时闪红光)
        "       ",  // 5
        "       ",  // 6
        "       ",  // 7
        "       ",  // 8
        "       ",  // 9
        "       ",  // 10
        "       ",  // 11 清洁车sprite
        "       ",  // 12
        "       ",  // 13
    });
}

bool Scene_2_2::OnTileClick(int tile, int col, int row) {
    (void)row;
    sf::Vector2f pp = m_player.GetPosition();
    float dx = (col + 0.5f) * TILE_SIZE - pp.x;
    float dy = (row + 0.5f) * TILE_SIZE - pp.y;
    if (std::sqrt(dx * dx + dy * dy) > 160.f) return false;

    // 打印机 (col6, row2)
    for (const auto& dp : m_printerPositions) {
        (void)dp;
        float bx = 6.f * TILE_SIZE, by = 2.f * TILE_SIZE;
        float bw = 1.f * TILE_SIZE, bh = 1.f * TILE_SIZE;
        float cx = (col + 0.5f) * TILE_SIZE, cy = (row + 0.5f) * TILE_SIZE;
        if (cx >= bx && cx < bx + bw && cy >= by && cy < by + bh) {
            m_savePanel.Show(true);
            return true;
        }
    }

    // 清洁车 (cols5-6, rows11-12)
    for (const auto& dp : m_cartPositions) {
        (void)dp;
        float bx = 5.f * TILE_SIZE, by = 11.f * TILE_SIZE;
        float bw = 2.f * TILE_SIZE, bh = 2.f * TILE_SIZE;
        float cx = (col + 0.5f) * TILE_SIZE, cy = (row + 0.5f) * TILE_SIZE;
        if (cx >= bx && cx < bx + bw && cy >= by && cy < by + bh) {
            StartDialogue({"终于安全避开了清洁工的陷阱！这是她的清洁车吧..."}, {Portrait::Smile});
            m_pendingDream = true;
            return true;
        }
    }
    return false;
}

bool Scene_2_2::IsTrapDanger(int col, int row) const {
    int r = row - TRAP_ROW_START;
    if (r < 0 || r >= TRAP_ROWS || col < 0 || col >= TRAP_COLS) return false;
    return m_trapState[r][col] == 2;
}
bool Scene_2_2::IsTrapWarn(int col, int row) const {
    int r = row - TRAP_ROW_START;
    if (r < 0 || r >= TRAP_ROWS || col < 0 || col >= TRAP_COLS) return false;
    return m_trapState[r][col] == 1;
}

// 简陋伪随机（无需 <random> 头文件）
static float RandRange(float lo, float hi, int seed) {
    float t = static_cast<float>((seed * 1103515245 + 12345) & 0x7FFFFFFF) / 2147483648.f;
    return lo + t * (hi - lo);
}

void Scene_2_2::Update(float dt) {
    GameScene::Update(dt);

    // ———— 陷阱每格独立更新 ————
    static int tickSeed = 0;
    for (int r = 0; r < TRAP_ROWS; ++r) {
        for (int c = 0; c < TRAP_COLS; ++c) {
            m_trapTimer[r][c] -= dt;
            if (m_trapTimer[r][c] <= 0.f) {
                int s = m_trapState[r][c];
                ++tickSeed;
                if (s == 0) { // 安全 → 预警
                    m_trapState[r][c]  = 1;
                    m_trapTimer[r][c]  = WARN_TIME;
                } else if (s == 1) { // 预警 → 危险
                    m_trapState[r][c]  = 2;
                    m_trapTimer[r][c]  = RandRange(DANGER_MIN, DANGER_MAX, tickSeed + r * 100 + c);
                } else { // 危险 → 安全
                    m_trapState[r][c]  = 0;
                    m_trapTimer[r][c]  = RandRange(SAFE_MIN, SAFE_MAX, tickSeed + r * 200 + c);
                }
            }
        }
    }

    // ———— 检测玩家踩到危险格 ————
    if (!IsInDialogue() && !m_trapHit && !m_choiceDialog.IsVisible() && !m_savePanel.IsVisible()) {
        sf::Vector2f pos = m_player.GetPosition();
        int pc = TileMap::PixelToTile(pos.x);
        int pr = TileMap::PixelToTile(pos.y);
        if (IsTrapDanger(pc, pr)) {
            StartDialogue({"你一脚踩上光滑的地板，摔成了脑震荡，",
                           "120的声音在脑海里回荡，",
                           "果然...还是没能拥有你吗，早八。"});
            m_trapHit = true;
        }
    }

    // 陷阱命中 → 对话结束后回标题（一命通关）
    if (m_trapHit && !IsInDialogue()) {
        m_nextScene  = static_cast<int>(SceneID::Title);
        m_entryParam = 0;
    }

    // ———— 梦境转场 ————
    if (m_pendingDream && !IsInDialogue()) {
        m_dreamTimer += dt;
        if (m_dreamTimer >= 1.f) {
            m_nextScene = static_cast<int>(SceneID::Dream);
            m_entryParam = 0;
        }
    }
}

void Scene_2_2::Draw(sf::RenderWindow& window) {
    // 地图
    window.setView(m_camera);
    sf::Vector2f cam = m_camera.getCenter();
    m_tileMap.Draw(window, cam.x, cam.y);

    // 陷阱每格独立渲染
    for (int r = TRAP_ROW_START; r < TRAP_ROW_START + TRAP_ROWS; ++r) {
        for (int c = 0; c < TRAP_COLS; ++c) {
            if (IsTrapDanger(c, r)) {
                // 危险：亮红脉动
                float pulse = 170.f + std::sin(m_trapTimer[r - TRAP_ROW_START][c] * 12.f) * 50.f;
                sf::RectangleShape red({static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE)});
                red.setPosition({c * TILE_SIZE * 1.f, r * TILE_SIZE * 1.f});
                red.setFillColor(sf::Color(220, 35, 25, static_cast<uint8_t>(pulse)));
                window.draw(red);
            } else if (IsTrapWarn(c, r)) {
                // 预警：淡粉闪烁
                float w = m_trapTimer[r - TRAP_ROW_START][c] / WARN_TIME; // 1→0
                sf::RectangleShape pink({static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE)});
                pink.setPosition({c * TILE_SIZE * 1.f, r * TILE_SIZE * 1.f});
                pink.setFillColor(sf::Color(255, 120, 100, static_cast<uint8_t>(80.f + (1.f - w) * 100.f)));
                window.draw(pink);
            }
        }
    }

    // 清洁车
    for (const auto& spr : m_cartSprites)
        window.draw(*spr);
    for (const auto& spr : m_printerSprites)
        window.draw(*spr);

    // 玩家
    m_player.Draw(window);

    // 调试信息
    if (m_font) {
        sf::Vector2f pos = m_player.GetPosition();
        int col = TileMap::PixelToTile(pos.x);
        int row = TileMap::PixelToTile(pos.y);
        std::string info = "col:" + std::to_string(col)
                         + " row:" + std::to_string(row);
        sf::Text dbg(*m_font, U(info), 18);
        dbg.setFillColor(sf::Color::White);
        dbg.setOutlineColor(sf::Color::Black);
        dbg.setOutlineThickness(1.f);
        dbg.setPosition({cam.x - SCREEN_W / 2.f + 10.f, cam.y - SCREEN_H / 2.f + 10.f});
        window.draw(dbg);
    }

    // UI 叠加
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
