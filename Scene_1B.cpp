#include "Scene_1B.h"
#include <cmath>

Scene_1B::Scene_1B(int entryParam) {
    m_entryParam = entryParam;
    m_bgmId = 0;
    LoadMap(BuildMapData(), GetSpawnPoint(entryParam));

    // ———— 门精灵系统 ————
    // 轴对称：左门col7-10，右门col19-22，右下角锚点，右门水平翻转
    m_doorPositions = {
        {7, 2},    // 左教室门
        {19, 2},   // 右教室门
    };

    // 纹理只在构造时加载1次
    if (m_doorTex.loadFromFile("assets/textures/doors.png")) {
        m_doorTex.setSmooth(true);
        for (const auto& dp : m_doorPositions) {
            auto spr = std::make_unique<sf::Sprite>(m_doorTex);
            auto texSize = m_doorTex.getSize();
            float sx = (4.f * TILE_SIZE) / texSize.x * 1.15f;
            float sy = (4.f * TILE_SIZE) / texSize.y * 1.15f;
            bool isRight = (dp.x > 14);  // 中轴col14.5，右侧门翻转
            if (isRight) {
                // 右门：原点右下，scaleX负，锚定左下角世界坐标
                spr->setOrigin({static_cast<float>(texSize.x), static_cast<float>(texSize.y)});
                spr->setScale({-sx, sy});
                float baseL = dp.x * 1.f * TILE_SIZE - 15.f;
                float baseB = (dp.y + 4) * 1.f * TILE_SIZE - 4.f;
                spr->setPosition({baseL, baseB});
            } else {
                // 左门：原点右下，scaleX正，锚定右下角世界坐标
                spr->setOrigin({static_cast<float>(texSize.x), static_cast<float>(texSize.y)});
                spr->setScale({sx, sy});
                float baseR = (dp.x + 4) * 1.f * TILE_SIZE - 15.f;
                float baseB = (dp.y + 4) * 1.f * TILE_SIZE - 4.f;
                spr->setPosition({baseR, baseB});
            }
            m_doorSprites.push_back(std::move(spr));
        }
    }

    // ———— 显示屏精灵系统 ————
    m_displayPositions = {
        {14, 8},   // 等身电子显示屏 (col14-15, row8-10, 2×3)
    };

    if (m_displayTex.loadFromFile("assets/textures/screen.png")) {
        m_displayTex.setSmooth(true);
        for (const auto& dp : m_displayPositions) {
            auto spr = std::make_unique<sf::Sprite>(m_displayTex);
            auto texSize = m_displayTex.getSize();
            float sx = (2.f * TILE_SIZE) / texSize.x * 1.5f;  // 80/48*1.5
            float sy = (3.f * TILE_SIZE) / texSize.y * 1.2f;  // 120/48*1.2
            spr->setScale({sx, sy});
            spr->setPosition({dp.x * 1.f * TILE_SIZE - 20.f, dp.y * 1.f * TILE_SIZE});
            m_displaySprites.push_back(std::move(spr));
        }
    }

    // ———— 窗户精灵系统 ————
    m_windowPositions = {
        {3, 1},    // 左窗
        {25, 1},   // 右窗
    };

    if (m_windowTex.loadFromFile("assets/textures/window.png")) {
        m_windowTex.setSmooth(true);
        for (const auto& dp : m_windowPositions) {
            auto spr = std::make_unique<sf::Sprite>(m_windowTex);
            auto texSize = m_windowTex.getSize();
            float sx = (2.f * TILE_SIZE) / texSize.x * 2.25f;
            float sy = (2.f * TILE_SIZE) / texSize.y * 2.25f;
            float yOff = 15.f;  // 左右窗同高
            spr->setScale({sx, sy});
            spr->setPosition({dp.x * 1.f * TILE_SIZE - 55.f, dp.y * 1.f * TILE_SIZE + yOff});
            m_windowSprites.push_back(std::move(spr));
        }
    }

    // ———— 沙发精灵系统 ————
    m_sofaPositions = {
        {4, 12},   // 左沙发 左边缘 x=110
        {20, 12},  // 右沙发 左边缘 x=1050 (对称)
    };

    if (m_sofaTex.loadFromFile("assets/textures/sofa.png")) {
        m_sofaTex.setSmooth(true);
        for (const auto& dp : m_sofaPositions) {
            auto spr = std::make_unique<sf::Sprite>(m_sofaTex);
            auto texSize = m_sofaTex.getSize();
            float sx = (6.f * TILE_SIZE) / texSize.x * 1.5f;
            float sy = (2.f * TILE_SIZE) / texSize.y * 3.f * 1.5f;
            float baseY = dp.y * 1.f * TILE_SIZE - 150.f;
            bool isRight = (dp.x > 14);
            if (isRight) {
                spr->setOrigin({static_cast<float>(texSize.x), static_cast<float>(texSize.y)});
                spr->setScale({-sx, sy});
                // 对称+原点补偿：底边同左沙发
                spr->setPosition({690.f, baseY + texSize.y * sy});
            } else {
                spr->setScale({sx, sy});
                spr->setPosition({dp.x * 1.f * TILE_SIZE - 50.f, baseY});
            }
            m_sofaSprites.push_back(std::move(spr));
        }
    }

    // 显示屏碰撞 (col14-15, row9-10)
    for (int c = 14; c <= 15; ++c)
        for (int r = 9; r <= 10; ++r)
            m_tileMap.SetBlocked(c, r, true);

    // 左沙发碰撞 (col5-9, row12-13)
    for (int c = 5; c <= 9; ++c)
        for (int r = 12; r <= 13; ++r)
            m_tileMap.SetBlocked(c, r, true);

    // 右沙发碰撞 (col19-23, row12-13)
    for (int c = 19; c <= 23; ++c)
        for (int r = 12; r <= 13; ++r)
            m_tileMap.SetBlocked(c, r, true);

    // ———— 打印机精灵系统 ————
    m_printerPositions = {
        {0, 10},   // 打印机 (col0, row10, 1×1)
    };

    if (m_printerTex.loadFromFile("assets/textures/printer1.png")) {
        m_printerTex.setSmooth(true);
        for (const auto& dp : m_printerPositions) {
            auto spr = std::make_unique<sf::Sprite>(m_printerTex);
            auto texSize = m_printerTex.getSize();
            float sx = (1.f * TILE_SIZE) / texSize.x * 3.75f;
            float sy = (1.f * TILE_SIZE) / texSize.y * 3.75f;
            spr->setScale({sx, sy});
            spr->setPosition({dp.x * 1.f * TILE_SIZE - 45.f, dp.y * 1.f * TILE_SIZE - 50.f});
            m_printerSprites.push_back(std::move(spr));
        }
    }

    // 打印机碰撞 (col0, row10)
    m_tileMap.SetBlocked(0, 10, true);

    if (entryParam == 0) s_passwordUnlocked = false;  // 新游戏重置密码锁
}

MapData Scene_1B::BuildMapData() {
    // 30×15 一楼前厅
    // 墙(0-4) / 地板(5-14)
    return MapBuilder::FromStrings({
        //         11111111112222222222
        // 012345678901234567890123456789
        "##############################", // 0  顶墙
        "##############################", // 1  窗由sprite绘制
        "##############################", // 2  窗由sprite绘制
        "##############################", // 3  全墙
        "##############################", // 4  全墙
        "                              ", // 5  地板
        "                              ", // 6  地板
        "                             K", // 7  密码锁
        "                             K", // 8  密码锁(显示屏由sprite绘制)
        "                              ", // 9
        "                              ", // 10 打印机由sprite绘制
        "                              ", // 11 地板
        "                             B", // 12 公告栏(沙发sprite)
        "                             B", // 13 公告栏(沙发sprite)
        "                              ", // 14 地板
    });
}

sf::Vector2f Scene_1B::GetSpawnPoint(int entryParam) {
    if (entryParam == 1)
        return { 28.f * TILE_SIZE + TILE_SIZE / 2.f, TileMap::TileToPixel(8) };  // 从楼梯间回来→密码锁旁
    return { 14.5f * TILE_SIZE, TileMap::TileToPixel(14) };       // 新游戏→默认中央
}

void Scene_1B::HandleInput(const sf::RenderWindow& window) {
    if (m_showClockDetail) {
        bool down = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        if (down && !m_clockClickWasDown) m_showClockDetail = false;
        m_clockClickWasDown = down;
        return;  // 阻塞其他输入
    }
    m_clockClickWasDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    GameScene::HandleInput(window);
}

void Scene_1B::Draw(sf::RenderWindow& window) {
    // ———— 第一层：地图（相机视图） ————
    window.setView(m_camera);
    sf::Vector2f cam = m_camera.getCenter();
    m_tileMap.Draw(window, cam.x, cam.y);

    // ———— 门精灵：墙体之上，逐扇绘制 ————
    for (const auto& spr : m_doorSprites) {
        window.draw(*spr);
    }

    // ———— 显示屏精灵 ————
    for (const auto& spr : m_displaySprites) {
        window.draw(*spr);
    }

    // ———— 窗户精灵 ————
    for (const auto& spr : m_windowSprites) {
        window.draw(*spr);
    }

    // ———— 沙发精灵 ————
    for (const auto& spr : m_sofaSprites) {
        window.draw(*spr);
    }

    // ———— 打印机精灵 ————
    for (const auto& spr : m_printerSprites) {
        window.draw(*spr);
    }

    // ———— 第二层：墙上钟表（地图之后，玩家之前，会被玩家遮挡） ————
    const float cx = 14.5f * TILE_SIZE;                 // 两扇门的正中间
    const float cy = 1.f * TILE_SIZE + TILE_SIZE / 2.f; // row 1 中心（门上方墙壁，远离门区）
    const float cr = 28.f;

    sf::CircleShape clockFace(cr);
    clockFace.setOrigin({cr, cr});
    clockFace.setPosition({cx, cy});
    clockFace.setFillColor(sf::Color(240, 240, 240));
    clockFace.setOutlineColor(sf::Color(50, 50, 55));
    clockFace.setOutlineThickness(3.f);
    window.draw(clockFace);

    // 时针（8点方向 = SFML 60°：矩形默认朝下6点，顺时针转60°到8点）
    sf::RectangleShape hourHand({4.f, 18.f});
    hourHand.setOrigin({2.f, 2.f});
    hourHand.setPosition({cx, cy});
    hourHand.setFillColor(sf::Color(40, 40, 45));
    hourHand.setRotation(sf::degrees(60.f));
    window.draw(hourHand);

    // 分针（12点方向 = SFML 180°：矩形默认朝下，转180°朝上）
    sf::RectangleShape minHand({3.f, 24.f});
    minHand.setOrigin({1.5f, 2.f});
    minHand.setPosition({cx, cy});
    minHand.setFillColor(sf::Color(40, 40, 45));
    minHand.setRotation(sf::degrees(180.f));
    window.draw(minHand);

    // 中心圆点
    sf::CircleShape dot(3.f);
    dot.setOrigin({3.f, 3.f});
    dot.setPosition({cx, cy});
    dot.setFillColor(sf::Color(50, 50, 55));
    window.draw(dot);

    // ———— 第三层：玩家 ————
    m_player.Draw(window);

    // ———— 调试信息 ————
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

    // ———— 第四层：UI 叠加 ————
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

        // ———— 第五层：放大钟表盘 ————
        if (m_showClockDetail) {
            sf::RectangleShape overlay({static_cast<float>(SCREEN_W), static_cast<float>(SCREEN_H)});
            overlay.setFillColor(sf::Color(0, 0, 0, 180));
            window.draw(overlay);

            const float ccx = SCREEN_W / 2.f;
            const float ccy = SCREEN_H / 2.f - 40.f;
            const float cr  = 160.f;

            // 表盘背景
            sf::CircleShape face(cr);
            face.setOrigin({cr, cr});
            face.setPosition({ccx, ccy});
            face.setFillColor(sf::Color(245, 240, 230));
            face.setOutlineColor(sf::Color(60, 55, 50));
            face.setOutlineThickness(6.f);
            window.draw(face);

            // 外圈装饰环
            sf::CircleShape ring(cr - 10.f);
            ring.setOrigin({cr - 10.f, cr - 10.f});
            ring.setPosition({ccx, ccy});
            ring.setFillColor(sf::Color::Transparent);
            ring.setOutlineColor(sf::Color(60, 55, 50));
            ring.setOutlineThickness(2.f);
            window.draw(ring);

            // 刻度线 + 数字（1-12）
            if (m_font) {
                for (int h = 1; h <= 12; ++h) {
                    float a = (static_cast<float>(h) / 12.f) * 2.f * 3.14159265f - 3.14159265f / 2.f;
                    float innerR = cr - 22.f;
                    float outerR = cr - 8.f;
                    float ix = ccx + std::cos(a) * innerR;
                    float iy = ccy + std::sin(a) * innerR;
                    float ox = ccx + std::cos(a) * outerR;
                    float oy = ccy + std::sin(a) * outerR;

                    sf::RectangleShape tick({3.f, outerR - innerR});
                    tick.setOrigin({1.5f, 0.f});
                    tick.setPosition({ix, iy});
                    tick.setFillColor(sf::Color(40, 35, 30));
                    tick.setRotation(sf::radians(a + 3.14159265f / 2.f));
                    window.draw(tick);

                    // 数字
                    float nx = ccx + std::cos(a) * (cr - 42.f);
                    float ny = ccy + std::sin(a) * (cr - 42.f);
                    sf::Text num(*m_font, U(std::to_string(h)), 26);
                    num.setFillColor(sf::Color(40, 35, 30));
                    sf::FloatRect nb = num.getLocalBounds();
                    num.setPosition({nx - nb.size.x / 2.f, ny - nb.size.y / 2.f - nb.position.y});
                    window.draw(num);
                }
            }

            // 时针（8点方向 = 矩形默认朝下6点, 顺时针转60°到8点）
            sf::RectangleShape hourHand({8.f, 70.f});
            hourHand.setOrigin({4.f, 8.f});
            hourHand.setPosition({ccx, ccy});
            hourHand.setFillColor(sf::Color(30, 25, 20));
            hourHand.setRotation(sf::degrees(60.f));
            window.draw(hourHand);

            // 分针（12点方向 = 矩形默认朝下, 转180°朝上）
            sf::RectangleShape minHand({5.f, 105.f});
            minHand.setOrigin({2.5f, 8.f});
            minHand.setPosition({ccx, ccy});
            minHand.setFillColor(sf::Color(50, 45, 40));
            minHand.setRotation(sf::degrees(180.f));
            window.draw(minHand);

            // 中心圆点
            sf::CircleShape dot(10.f);
            dot.setOrigin({10.f, 10.f});
            dot.setPosition({ccx, ccy});
            dot.setFillColor(sf::Color(160, 50, 40));
            window.draw(dot);

            // 底部对话框
            if (m_font) {
                const float dw = 900.f, dh = 70.f;
                const float dx = (SCREEN_W - dw) / 2.f, dy = SCREEN_H - 140.f;
                sf::RectangleShape dlg({dw, dh});
                dlg.setPosition({dx, dy});
                dlg.setFillColor(sf::Color(20, 20, 30, 230));
                dlg.setOutlineColor(sf::Color(180, 180, 200));
                dlg.setOutlineThickness(2.f);
                window.draw(dlg);

                sf::Text dlgText(*m_font, U("指针一动也不动，看来是坏了"), 28);
                dlgText.setFillColor(sf::Color(240, 240, 240));
                sf::FloatRect tb = dlgText.getLocalBounds();
                dlgText.setPosition({dx + (dw - tb.size.x) / 2.f, dy + (dh - tb.size.y) / 2.f - tb.position.y});
                window.draw(dlgText);
            }
        }
    }
}

void Scene_1B::CheckExits() {
    // 一楼独立调试阶段，暂不连接其他场景
}

bool Scene_1B::OnTileClick(int tile, int col, int row) {
    (void)row;
    sf::Vector2f pp = m_player.GetPosition();
    float dx = (col + 0.5f) * TILE_SIZE - pp.x;
    float dy = (row + 0.5f) * TILE_SIZE - pp.y;
    // 钟表优先检测（在墙壁上方，玩家从地板够不到常规距离）
    {
        const float ccx = 14.5f * TILE_SIZE;
        const float ccy = TileMap::TileToPixel(1);
        float d = std::sqrt((pp.x - ccx) * (pp.x - ccx) + (pp.y - ccy) * (pp.y - ccy));
        if (d <= 200.f && col >= 13 && col <= 16 && row >= 0 && row <= 2) {
            m_showClockDetail = true;
            return true;
        }
    }

    if (std::sqrt(dx * dx + dy * dy) > 160.f) return false;

    // 沙发
    for (const auto& dp : m_sofaPositions) {
        float bx = dp.x * 1.f * TILE_SIZE;
        float by = dp.y * 1.f * TILE_SIZE;
        float bw = 6.f * TILE_SIZE;
        float bh = 2.f * TILE_SIZE;
        float cx = (col + 0.5f) * TILE_SIZE;
        float cy = (row + 0.5f) * TILE_SIZE;
        if (cx >= bx && cx < bx + bw && cy >= by && cy < by + bh) {
            StartDialogue({"一张舒服的沙发，可以坐。"}, {Portrait::Smile});
            return true;
        }
    }

    // 左窗 (cols3-4, rows1-2)
    if (col >= 3 && col <= 4 && row >= 1 && row <= 2) {
        m_choiceDialog.ShowInfo("密码锁线索1");
        return true;
    }

    // 电子显示屏 (col14-15, row8-10)
    for (const auto& dp : m_displayPositions) {
        float bx = dp.x * 1.f * TILE_SIZE - 20.f;
        float by = dp.y * 1.f * TILE_SIZE;
        float bw = 2.f * TILE_SIZE * 1.5f;
        float bh = 3.f * TILE_SIZE * 1.2f;
        float cx = (col + 0.5f) * TILE_SIZE;
        float cy = (row + 0.5f) * TILE_SIZE;
        if (cx >= bx && cx < bx + bw && cy >= by && cy < by + bh) {
            m_choiceDialog.ShowInfo("教室使用情况");
            return true;
        }
    }

    // 教室门（左右对称）
    for (const auto& dp : m_doorPositions) {
        float bx = dp.x * 1.f * TILE_SIZE - 15.f;
        float by = dp.y * 1.f * TILE_SIZE - 4.f;
        float bw = 4.f * TILE_SIZE;
        float bh = 4.f * TILE_SIZE;
        float cx = (col + 0.5f) * TILE_SIZE;
        float cy = (row + 0.5f) * TILE_SIZE;
        if (cx >= bx && cx < bx + bw && cy >= by && cy < by + bh) {
            float ddx = cx - pp.x;
            float ddy = cy - pp.y;
            if (std::sqrt(ddx * ddx + ddy * ddy) <= 300.f) {
                bool isLeft = (dp.x < 14);
                ShowDoorChoice(
                    isLeft ? "教室里坐满了人..." : "教室里零星有人入座...",
                    isLeft ? std::vector<std::string>{"106教室", "这是我的教室", "这不是我的教室"}
                           : std::vector<std::string>{"108教室", "这是我的教室", "这不是我的教室"},
                    false
                );
                return true;
            }
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
    }, {
        Portrait::Nomal, Portrait::Nomal, Portrait::Nomal,
        Portrait::Nomal, Portrait::Whattt, Portrait::Nomal
    }, 0.5f);
}
