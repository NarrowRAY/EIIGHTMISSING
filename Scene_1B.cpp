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

    // ———— puzzle1 贴图（左窗点击展示） ————
    if (m_puzzle1Tex.loadFromFile("assets/textures/puzzle1.png")) {
        m_puzzle1Tex.setSmooth(true);
        m_puzzle1Sprite = std::make_unique<sf::Sprite>(m_puzzle1Tex);
    }

    // ———— BadEnd CG 全屏贴图 ————
    if (m_badEndTex.loadFromFile("assets/textures/badend.png")) {
        m_badEndTex.setSmooth(true);
        m_badEndSprite = std::make_unique<sf::Sprite>(m_badEndTex);
        auto ts = m_badEndTex.getSize();
        m_badEndSprite->setScale({SCREEN_W / static_cast<float>(ts.x), SCREEN_H / static_cast<float>(ts.y)});
        m_badEndSprite->setPosition({0.f, 0.f});
    }

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
        "P                             ", // 10 打印机由sprite绘制
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
    // ———— 公告栏揭纸 ————
    if (m_showBulletin) {
        sf::Vector2i mp = sf::Mouse::getPosition(window);
        sf::View uiView({SCREEN_W / 2.f, SCREEN_H / 2.f}, {SCREEN_W, SCREEN_H});
        sf::Vector2f sp = window.mapPixelToCoords(mp, uiView);
        bool down = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        // 关闭按钮
        const float pw = 800.f, ph2 = 550.f;
        const float pxx = (SCREEN_W - pw) / 2.f, pyy = (SCREEN_H - ph2) / 2.f;
        const float ccx = pxx + pw - 42.f, ccy = pyy + 20.f, ccr = 22.f;
        if (down && !m_clockClickWasDown) {
            float d = std::sqrt((sp.x - ccx) * (sp.x - ccx) + (sp.y - ccy) * (sp.y - ccy));
            if (d <= ccr) { m_showBulletin = false; m_clockClickWasDown = true; return; }
        }

        if (down && m_bulletinDragIdx < 0) {
            for (int i = static_cast<int>(m_papers.size()) - 1; i >= 0; --i) {
                if (m_papers[i].removed) continue;
                if (m_papers[i].rect.contains(sp)) {
                    m_bulletinDragIdx = i;
                    m_bulletinDragOff = sp - m_papers[i].rect.position;
                    // 移到最上层
                    auto p = m_papers[i];
                    m_papers.erase(m_papers.begin() + i);
                    m_papers.push_back(p);
                    m_bulletinDragIdx = static_cast<int>(m_papers.size()) - 1;
                    break;
                }
            }
        } else if (down && m_bulletinDragIdx >= 0) {
            auto& r = m_papers[m_bulletinDragIdx].rect;
            r.position = sp - m_bulletinDragOff;
        } else if (!down && m_bulletinDragIdx >= 0) {
            auto& r = m_papers[m_bulletinDragIdx].rect;
            // 拖到面板外 → 移除
            const float pwx = (SCREEN_W - 800) / 2.f, pwy = (SCREEN_H - 550) / 2.f;
            if (r.position.x < pwx - 40 || r.position.x > pwx + 840 ||
                r.position.y < pwy - 40 || r.position.y > pwy + 590) {
                m_papers[m_bulletinDragIdx].removed = true;
            }
            m_bulletinDragIdx = -1;
            // 全部移除 → 完成
            bool allGone = true;
            for (auto& p : m_papers) if (!p.removed) { allGone = false; break; }
            if (allGone) m_bulletinSolved = true;
        }
        m_clockClickWasDown = down;
        return;
    }

    if (m_showPuzzle1) {
        if (IsInDialogue()) {
            GameScene::HandleInput(window);          // 处理对话框点击推进
        } else {
            bool down = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
            if (down && !m_clockClickWasDown) {
                // 只有点击圆形 X 按钮才关闭
                sf::Vector2i mp = sf::Mouse::getPosition(window);
                sf::View uiView({SCREEN_W / 2.f, SCREEN_H / 2.f}, {SCREEN_W, SCREEN_H});
                sf::Vector2f sp = window.mapPixelToCoords(mp, uiView);
                const float pw = 800.f, ph = 600.f;
                const float px = (SCREEN_W - pw) / 2.f;
                const float py = (SCREEN_H - ph) / 2.f;
                const float cx = px + pw - 50.f, cy = py + 16.f, cr = 22.f;
                float dist = std::sqrt((sp.x - cx) * (sp.x - cx) + (sp.y - cy) * (sp.y - cy));
                if (dist <= cr) m_showPuzzle1 = false;
            }
            m_clockClickWasDown = down;
        }
        return;
    }
    if (m_showClockDetail) {
        bool down = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        if (down && !m_clockClickWasDown) m_showClockDetail = false;
        m_clockClickWasDown = down;
        return;  // 阻塞其他输入
    }
    // ———— 公告栏点击检测 ————
    if (!m_showBulletin && !m_bulletinSolved && !IsInDialogue()
        && !m_savePanel.IsVisible() && !m_passwordUI.IsVisible()
        && !m_choiceDialog.IsVisible() && !m_showPuzzle1 && !m_showClockDetail) {
        bool down = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        if (down && !m_clockClickWasDown) {
            sf::Vector2i px = sf::Mouse::getPosition(window);
            sf::Vector2f wp = window.mapPixelToCoords(px, m_camera);
            int c = TileMap::PixelToTile(wp.x);
            int r = TileMap::PixelToTile(wp.y);
            for (int dc = -1; dc <= 1; ++dc) {
                for (int dr = -1; dr <= 1; ++dr) {
                    int tc = c + dc, tr = r + dr;
                    if (!m_tileMap.IsInBounds(tc, tr)) continue;
                    if (m_tileMap.GetTile(tc, tr) == static_cast<int>(TileType::BulletinBoard)) {
                        float ddx = (tc + 0.5f) * TILE_SIZE - m_player.GetPosition().x;
                        float ddy = (tr + 0.5f) * TILE_SIZE - m_player.GetPosition().y;
                        if (std::sqrt(ddx * ddx + ddy * ddy) <= 160.f) {
                            m_showBulletin = true;
                            if (m_papers.empty()) {
                                const float bx = (SCREEN_W - 800) / 2.f;
                                const float by = (SCREEN_H - 550) / 2.f;
                                m_papers = {
                                    {sf::FloatRect({bx + 80,  by + 100}, {190, 140}), sf::Color(240, 220, 200), "通知", false},
                                    {sf::FloatRect({bx + 280, by + 70},  {180, 130}), sf::Color(220, 210, 230), "海报", false},
                                    {sf::FloatRect({bx + 100, by + 270}, {230, 110}), sf::Color(210, 230, 210), "通知", false},
                                    {sf::FloatRect({bx + 200, by + 170}, {180, 170}), sf::Color(230, 215, 210), "海报", false},
                                    {sf::FloatRect({bx + 40,  by + 55},  {160, 140}), sf::Color(225, 225, 215), "通知", false},
                                    {sf::FloatRect({bx + 350, by + 190}, {200, 130}), sf::Color(235, 225, 210), "海报", false},
                                    {sf::FloatRect({bx + 500, by + 100}, {170, 150}), sf::Color(215, 220, 225), "通知", false},
                                    {sf::FloatRect({bx + 250, by + 290}, {220, 120}), sf::Color(228, 218, 208), "海报", false},
                                };
                            }
                            goto skip_input;
                        }
                    }
                }
            }
        }
    }
skip_input:
    m_clockClickWasDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    if (!m_showBulletin) GameScene::HandleInput(window);
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

        // ———— 公告栏揭纸 ————
        if (m_showBulletin) {
            sf::RectangleShape overlay({static_cast<float>(SCREEN_W), static_cast<float>(SCREEN_H)});
            overlay.setFillColor(sf::Color(0, 0, 0, 160));
            window.draw(overlay);

            // 临时验证：大红框确保弹窗机制生效
            sf::RectangleShape test({400.f, 300.f});
            test.setPosition({SCREEN_W / 2.f - 200.f, SCREEN_H / 2.f - 150.f});
            test.setFillColor(sf::Color(200, 40, 40, 200));
            window.draw(test);

            const float pw = 800.f, ph = 550.f;
            const float px = (SCREEN_W - pw) / 2.f;
            const float py = (SCREEN_H - ph) / 2.f;

            // 面板（深棕底板）
            sf::RectangleShape panel({pw, ph});
            panel.setPosition({px, py});
            panel.setFillColor(sf::Color(55, 35, 20));
            panel.setOutlineColor(sf::Color(100, 70, 40));
            panel.setOutlineThickness(3.f);
            window.draw(panel);

            // "公告栏" 标题
            if (m_font) {
                sf::Text title(*m_font, U("公告栏"), 36);
                title.setFillColor(sf::Color(240, 230, 210));
                sf::FloatRect tb = title.getLocalBounds();
                title.setPosition(sf::Vector2f(px + (pw - tb.size.x) / 2.f, py + 14.f));
                window.draw(title);
            }

            // 巨大的 "10" 数字（底层，透过纸缝可见）
            if (m_font) {
                sf::Text bigNum(*m_font, U("10"), 180);
                bigNum.setFillColor(sf::Color(180, 160, 130, m_bulletinSolved ? 255 : 120));
                sf::FloatRect nb = bigNum.getLocalBounds();
                bigNum.setPosition(sf::Vector2f(
                    px + (pw - nb.size.x) / 2.f,
                    py + (ph - nb.size.y) / 2.f - nb.position.y - 20.f));
                window.draw(bigNum);
            }

            // 纸张（从底层到顶层绘制）
            for (auto& p : m_papers) {
                if (p.removed) continue;
                sf::RectangleShape paper({p.rect.size.x, p.rect.size.y});
                paper.setPosition(p.rect.position);
                paper.setFillColor(p.color);
                paper.setOutlineColor(sf::Color(180, 160, 140));
                paper.setOutlineThickness(1.f);
                window.draw(paper);
                if (m_font) {
                    sf::Text label(*m_font, U(p.label), 22);
                    label.setFillColor(sf::Color(80, 60, 40));
                    sf::FloatRect lb = label.getLocalBounds();
                    label.setPosition(sf::Vector2f(
                        p.rect.position.x + (p.rect.size.x - lb.size.x) / 2.f,
                        p.rect.position.y + (p.rect.size.y - lb.size.y) / 2.f - lb.position.y));
                    window.draw(label);
                }
            }

            // 收集完成提示
            if (m_bulletinSolved && m_font) {
                sf::Text done(*m_font, U("找到了数字 10！"), 30);
                done.setFillColor(sf::Color(255, 220, 140));
                done.setOutlineColor(sf::Color(0, 0, 0, 180));
                done.setOutlineThickness(2.f);
                sf::FloatRect db = done.getLocalBounds();
                done.setPosition(sf::Vector2f(px + (pw - db.size.x) / 2.f, py + ph + 10.f));
                window.draw(done);
            }

            // 关闭按钮
            const float ccx = px + pw - 42.f, ccy = py + 20.f, ccr = 18.f;
            sf::CircleShape closeBg(ccr);
            closeBg.setOrigin({ccr, ccr});
            closeBg.setPosition({ccx, ccy});
            closeBg.setFillColor(sf::Color(80, 50, 30, 200));
            window.draw(closeBg);
            for (int i = 0; i < 2; ++i) {
                const float al = 10.f, aw = 2.5f;
                sf::RectangleShape arm({al * 2.f, aw});
                arm.setOrigin({al, aw / 2.f});
                arm.setPosition({ccx, ccy});
                arm.setFillColor(sf::Color(240, 230, 210));
                arm.setRotation(sf::degrees(i == 0 ? 45.f : -45.f));
                window.draw(arm);
            }
        }

        // ———— BadEnd CG 全屏贴图（在对话框下层） ————
        if (m_showBadEnd && m_badEndSprite) {
            window.draw(*m_badEndSprite);
        }

        // ———— puzzle1 贴图（在对话框下层） ————
        if (m_showPuzzle1 && m_puzzle1Sprite) {
            sf::RectangleShape overlay({static_cast<float>(SCREEN_W), static_cast<float>(SCREEN_H)});
            overlay.setFillColor(sf::Color(0, 0, 0, 180));
            window.draw(overlay);

            const float pw = 800.f, ph = 600.f;
            const float px = (SCREEN_W - pw) / 2.f;
            const float py = (SCREEN_H - ph) / 2.f;

            auto texSize = m_puzzle1Tex.getSize();
            m_puzzle1Sprite->setScale({pw / texSize.x, ph / texSize.y});
            m_puzzle1Sprite->setPosition({px, py});
            window.draw(*m_puzzle1Sprite);

            // 关闭按钮（圆形半透明底 + 白色X）
            const float cx = px + pw - 50.f, cy = py + 16.f, cr = 18.f;
            sf::CircleShape closeBg(cr);
            closeBg.setOrigin({cr, cr});
            closeBg.setPosition({cx, cy});
            closeBg.setFillColor(sf::Color(0, 0, 0, 120));
            window.draw(closeBg);

            const float armLen = 10.f, armW = 2.5f;
            sf::RectangleShape arm1({armLen * 2.f, armW});
            arm1.setOrigin({armLen, armW / 2.f});
            arm1.setPosition({cx, cy});
            arm1.setFillColor(sf::Color(240, 240, 245));
            arm1.setRotation(sf::degrees(45.f));
            window.draw(arm1);
            sf::RectangleShape arm2({armLen * 2.f, armW});
            arm2.setOrigin({armLen, armW / 2.f});
            arm2.setPosition({cx, cy});
            arm2.setFillColor(sf::Color(240, 240, 245));
            arm2.setRotation(sf::degrees(-45.f));
            window.draw(arm2);
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

        // ———— 放大钟表盘 ————
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

    // 公告栏 — 揭纸拼图（远距离也可点击）
    if (tile == static_cast<int>(TileType::BulletinBoard)) {
        if (!m_bulletinSolved) {
            m_showBulletin = true;
            if (m_papers.empty()) {
                const float bx = (SCREEN_W - 800) / 2.f;  // 560
                const float by = (SCREEN_H - 550) / 2.f;  // 265
                m_papers = {
                    {sf::FloatRect({bx + 80,  by + 100}, {190, 140}), sf::Color(240, 220, 200), "通知", false},
                    {sf::FloatRect({bx + 280, by + 70},  {180, 130}), sf::Color(220, 210, 230), "海报", false},
                    {sf::FloatRect({bx + 100, by + 270}, {230, 110}), sf::Color(210, 230, 210), "通知", false},
                    {sf::FloatRect({bx + 200, by + 170}, {180, 170}), sf::Color(230, 215, 210), "海报", false},
                    {sf::FloatRect({bx + 40,  by + 55},  {160, 140}), sf::Color(225, 225, 215), "通知", false},
                    {sf::FloatRect({bx + 350, by + 190}, {200, 130}), sf::Color(235, 225, 210), "海报", false},
                    {sf::FloatRect({bx + 500, by + 100}, {170, 150}), sf::Color(215, 220, 225), "通知", false},
                    {sf::FloatRect({bx + 250, by + 290}, {220, 120}), sf::Color(228, 218, 208), "海报", false},
                };
            }
        }
        return true;
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

    // 左窗 (cols3-4, rows1-2) — 展示 puzzle1 + 对话框
    if (col >= 3 && col <= 4 && row >= 1 && row <= 2) {
        m_showPuzzle1 = true;
        StartDialogue({"窗户上的倒影，似乎别有深意"}, {Portrait::Nomal});
        return true;
    }

    // 打印机存档 (col0, row10)
    for (const auto& dp : m_printerPositions) {
        float bx = dp.x * 1.f * TILE_SIZE - 45.f;
        float by = dp.y * 1.f * TILE_SIZE - 50.f;
        float bw = 1.f * TILE_SIZE * 3.75f;
        float bh = 1.f * TILE_SIZE * 3.75f;
        float cx = (col + 0.5f) * TILE_SIZE;
        float cy = (row + 0.5f) * TILE_SIZE;
        if (cx >= bx && cx < bx + bw && cy >= by && cy < by + bh) {
            m_savePanel.Show(true);
            return true;
        }
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
            m_choiceDialog.ShowInfo("教室使用情况\n\n104: 空    106: 有课\n\n204: 有课  206: 有课\n\n304: --    306: --\n\n......\n......");
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
