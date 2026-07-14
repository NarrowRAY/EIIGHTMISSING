#include "Scene_Dream.h"

Scene_Dream::Scene_Dream(int entryParam) {
    (void)entryParam;
    m_bgmId = 0;
    LoadMap(BuildMapData(), GetSpawnPoint(0));

    // ———— 门精灵 ————
    m_doorPositions = {{7, 2}, {19, 2}};
    if (m_doorTex.loadFromFile("assets/textures/doors.png")) {
        m_doorTex.setSmooth(true);
        for (const auto& dp : m_doorPositions) {
            auto spr = std::make_unique<sf::Sprite>(m_doorTex);
            auto ts = m_doorTex.getSize();
            float sx = (4.f * TILE_SIZE) / ts.x * 1.15f;
            float sy = (4.f * TILE_SIZE) / ts.y * 1.15f;
            bool isRight = (dp.x > 14);
            if (isRight) {
                spr->setOrigin({static_cast<float>(ts.x), static_cast<float>(ts.y)});
                spr->setScale({-sx, sy});
                float baseL = dp.x * 1.f * TILE_SIZE - 15.f;
                float baseB = (dp.y + 4) * 1.f * TILE_SIZE - 4.f;
                spr->setPosition({baseL, baseB});
            } else {
                spr->setOrigin({static_cast<float>(ts.x), static_cast<float>(ts.y)});
                spr->setScale({sx, sy});
                float baseR = (dp.x + 4) * 1.f * TILE_SIZE - 15.f;
                float baseB = (dp.y + 4) * 1.f * TILE_SIZE - 4.f;
                spr->setPosition({baseR, baseB});
            }
            m_doorSprites.push_back(std::move(spr));
        }
    }

    // ———— 显示屏精灵 ————
    m_displayPositions = {{14, 8}};
    if (m_displayTex.loadFromFile("assets/textures/screen.png")) {
        m_displayTex.setSmooth(true);
        for (const auto& dp : m_displayPositions) {
            auto spr = std::make_unique<sf::Sprite>(m_displayTex);
            auto ts = m_displayTex.getSize();
            spr->setScale({(2.f * TILE_SIZE) / ts.x * 1.5f, (3.f * TILE_SIZE) / ts.y * 1.2f});
            spr->setPosition({dp.x * 1.f * TILE_SIZE - 20.f, dp.y * 1.f * TILE_SIZE});
            m_displaySprites.push_back(std::move(spr));
        }
    }
    for (int c = 14; c <= 15; ++c)
        for (int r = 9; r <= 10; ++r)
            m_tileMap.SetBlocked(c, r, true);

    // ———— 窗户精灵 ————
    m_windowPositions = {{3, 1}, {25, 1}};
    if (m_windowTex.loadFromFile("assets/textures/window.png")) {
        m_windowTex.setSmooth(true);
        for (const auto& dp : m_windowPositions) {
            auto spr = std::make_unique<sf::Sprite>(m_windowTex);
            auto ts = m_windowTex.getSize();
            spr->setScale({(2.f * TILE_SIZE) / ts.x * 2.25f, (2.f * TILE_SIZE) / ts.y * 2.25f});
            spr->setPosition({dp.x * 1.f * TILE_SIZE - 55.f, dp.y * 1.f * TILE_SIZE + 15.f});
            m_windowSprites.push_back(std::move(spr));
        }
    }

    // ———— 沙发精灵 ————
    m_sofaPositions = {{4, 12}, {20, 12}};
    if (m_sofaTex.loadFromFile("assets/textures/sofa.png")) {
        m_sofaTex.setSmooth(true);
        for (const auto& dp : m_sofaPositions) {
            auto spr = std::make_unique<sf::Sprite>(m_sofaTex);
            auto ts = m_sofaTex.getSize();
            float sx = (6.f * TILE_SIZE) / ts.x * 1.5f;
            float sy = (2.f * TILE_SIZE) / ts.y * 3.f * 1.5f;
            float baseY = dp.y * 1.f * TILE_SIZE - 150.f;
            bool isRight = (dp.x > 14);
            if (isRight) {
                spr->setOrigin({static_cast<float>(ts.x), static_cast<float>(ts.y)});
                spr->setScale({-sx, sy});
                spr->setPosition({690.f, baseY + ts.y * sy});
            } else {
                spr->setScale({sx, sy});
                spr->setPosition({dp.x * 1.f * TILE_SIZE - 50.f, baseY});
            }
            m_sofaSprites.push_back(std::move(spr));
        }
    }
    for (int c = 5; c <= 9; ++c)
        for (int r = 12; r <= 13; ++r)
            m_tileMap.SetBlocked(c, r, true);
    for (int c = 19; c <= 23; ++c)
        for (int r = 12; r <= 13; ++r)
            m_tileMap.SetBlocked(c, r, true);

    // ———— 打印机 ————
    m_printerPositions = {{0, 10}};
    if (m_printerTex.loadFromFile("assets/textures/printer1.png")) {
        m_printerTex.setSmooth(true);
        for (const auto& dp : m_printerPositions) {
            auto spr = std::make_unique<sf::Sprite>(m_printerTex);
            auto ts = m_printerTex.getSize();
            spr->setScale({(1.f * TILE_SIZE) / ts.x * 3.75f, (1.f * TILE_SIZE) / ts.y * 3.75f});
            spr->setPosition({dp.x * 1.f * TILE_SIZE - 45.f, dp.y * 1.f * TILE_SIZE - 50.f});
            m_printerSprites.push_back(std::move(spr));
        }
    }
    // 梦境存档打印机 (col26, row12)
    {
        auto spr = std::make_unique<sf::Sprite>(m_printerTex);
        auto ts = m_printerTex.getSize();
        spr->setScale({(1.f * TILE_SIZE) / ts.x * 3.0f, (1.f * TILE_SIZE) / ts.y * 3.0f});
        spr->setPosition({26.f * TILE_SIZE, 12.f * TILE_SIZE - 50.f});
        m_printerSprites.push_back(std::move(spr));
    }
    m_tileMap.SetBlocked(26, 12, true);

    // 手机弹窗素材
    if (m_phoneTex.loadFromFile("assets/textures/phone.png")) {
        m_phoneTex.setSmooth(true);
        m_phoneSprite = std::make_unique<sf::Sprite>(m_phoneTex);
        float pw = 750.f;
        float ph = 750.f / m_phoneTex.getSize().x * m_phoneTex.getSize().y * 0.9f;
        m_phoneSprite->setScale({pw / m_phoneTex.getSize().x, ph / m_phoneTex.getSize().y});
        m_phoneSprite->setPosition({(SCREEN_W - pw) / 2.f, (SCREEN_H - ph) / 2.f});
    }
    // 消息纹理
    for (int i = 0; i < 3; ++i) {
        std::string path = "assets/textures/message" + std::to_string(i + 1) + ".png";
        if (m_msgTex[i].loadFromFile(path)) {
            m_msgTex[i].setSmooth(true);
            m_msgSprites[i] = std::make_unique<sf::Sprite>(m_msgTex[i]);
        }
    }
    if (m_msgTrueTex.loadFromFile("assets/textures/ture_true.png")) {
        m_msgTrueTex.setSmooth(true);
        m_msgTrueSprite = std::make_unique<sf::Sprite>(m_msgTrueTex);
    }
    ShuffleMessages();
}

void Scene_Dream::ShuffleMessages() {
    // 随机打乱，确保不是1,2,3
    do {
        int a = rand() % 3, b = rand() % 3;
        std::swap(m_msgOrder[a], m_msgOrder[b]);
    } while (m_msgOrder[0] == 0 && m_msgOrder[1] == 1 && m_msgOrder[2] == 2);
}

void Scene_Dream::CheckPuzzle() {
    if (m_msgOrder[0] == 0 && m_msgOrder[1] == 1 && m_msgOrder[2] == 2) {
        m_phoneSolved = true;
    }
}

sf::FloatRect Scene_Dream::GetPhoneRect() const {
    if (!m_phoneSprite) return {{0,0},{0,0}};
    auto gb = m_phoneSprite->getGlobalBounds();
    return {gb.position, gb.size};
}

sf::FloatRect Scene_Dream::GetSlotRect(int i) const {
    auto pr = GetPhoneRect();
    float marginX = pr.size.x * 0.12f;
    float slotW  = pr.size.x - marginX * 2.f;
    // 裁掉透明边距：按图片内容的实际文字高度算
    float contentRatio = 0.28f;  // 实际文字区占宽比
    float slotH  = slotW * contentRatio;
    float totalH = slotH * 3.f;
    float topPad = (pr.size.y - totalH) / 2.f;
    return {{pr.position.x + marginX, pr.position.y + topPad + i * slotH},
            {pr.size.x - marginX * 2.f, slotH}};
}

MapData Scene_Dream::BuildMapData() {
    return MapBuilder::FromStrings({
        "##############################",
        "##############################",
        "##############################",
        "##############################",
        "##############################",
        "                              ",
        "                              ",
        "                             K",
        "                             K",
        "                              ",
        "                              ",
        "                              ",
        "                             B",
        "                             B",
        "                              ",
    });
}

sf::Vector2f Scene_Dream::GetSpawnPoint(int entryParam) {
    (void)entryParam;
    return { 28.f * TILE_SIZE + TILE_SIZE / 2.f, TileMap::TileToPixel(11) };
}

sf::Vector2f Scene_Dream::GetSavePosition() {
    return { 18.f * TILE_SIZE, TileMap::TileToPixel(12) };
}

bool Scene_Dream::OnTileClick(int tile, int col, int row) {
    sf::Vector2f pp = m_player.GetPosition();
    float dx = (col + 0.5f) * TILE_SIZE - pp.x;
    float dy = (row + 0.5f) * TILE_SIZE - pp.y;
    if (std::sqrt(dx * dx + dy * dy) > 160.f) return false;

    // 显示屏
    for (const auto& dp : m_displayPositions) {
        (void)dp;
        float bx = 14.f * TILE_SIZE - 20.f;
        float by = 8.f * TILE_SIZE;
        float bw = 2.f * TILE_SIZE * 1.5f;
        float bh = 3.f * TILE_SIZE * 1.2f;
        float cx = (col + 0.5f) * TILE_SIZE;
        float cy = (row + 0.5f) * TILE_SIZE;
        if (cx >= bx && cx < bx + bw && cy >= by && cy < by + bh) {
            m_showPhone = true;
            return true;
        }
    }
    // 存档打印机 (col26, row12)
    if (col == 26 && row == 12) {
        m_savePanel.Show(true);
        return true;
    }

    // 沙发
    for (const auto& dp : m_sofaPositions) {
        (void)dp;
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
    return false;
}

void Scene_Dream::OnSceneEnter() {
    StartDialogue({
        "（一阵眩晕）",
        "这是哪？我回到了一楼？",
        "周围笼罩着一层黑暗，好像我的梦境..."
    }, {
        Portrait::None, Portrait::Whattt, Portrait::Nomal
    }, 0.5f);
}

void Scene_Dream::HandleInput(const sf::RenderWindow& window) {
    if (m_showPhone) {
        bool down = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        sf::Vector2i mp = sf::Mouse::getPosition(window);
        sf::View uiView({SCREEN_W / 2.f, SCREEN_H / 2.f}, {SCREEN_W, SCREEN_H});
        sf::Vector2f mpos = window.mapPixelToCoords(mp, uiView);

        if (m_phoneSolved) {
            if (down && !m_phoneClickWasDown) m_showPhone = false;
            m_phoneClickWasDown = down;
            return;
        }

        if (m_dragFrom < 0) {
            // 检测按下哪个槽位
            if (down && !m_phoneClickWasDown) {
                for (int i = 0; i < 3; ++i) {
                    if (GetSlotRect(i).contains(mpos)) {
                        m_dragFrom = i;
                        m_dragPos = mpos;
                        break;
                    }
                }
            }
        } else {
            m_dragPos = mpos; // 跟踪鼠标
            if (!down) {
                // 松手：检测落在哪个槽位
                int target = -1;
                for (int i = 0; i < 3; ++i) {
                    if (GetSlotRect(i).contains(mpos)) { target = i; break; }
                }
                if (target >= 0 && target != m_dragFrom) {
                    std::swap(m_msgOrder[m_dragFrom], m_msgOrder[target]);
                    CheckPuzzle();
                }
                m_dragFrom = -1;
            }
        }
        m_phoneClickWasDown = down;
        return;
    }
    m_phoneClickWasDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    GameScene::HandleInput(window);
}

void Scene_Dream::CheckExits() {}

void Scene_Dream::Draw(sf::RenderWindow& window) {
    // 地图+精灵+玩家
    window.setView(m_camera);
    sf::Vector2f cam = m_camera.getCenter();
    m_tileMap.Draw(window, cam.x, cam.y);
    for (const auto& spr : m_doorSprites) window.draw(*spr);
    for (const auto& spr : m_displaySprites) window.draw(*spr);
    for (const auto& spr : m_windowSprites) window.draw(*spr);
    for (const auto& spr : m_sofaSprites) window.draw(*spr);
    for (const auto& spr : m_printerSprites) window.draw(*spr);
    m_player.Draw(window);

    // 调试
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

    // 阴间滤镜
    {
        sf::View uiView({SCREEN_W / 2.f, SCREEN_H / 2.f}, {SCREEN_W, SCREEN_H});
        window.setView(uiView);
        sf::RectangleShape overlay({static_cast<float>(SCREEN_W), static_cast<float>(SCREEN_H)});
        overlay.setFillColor(sf::Color(15, 5, 30, 100));
        window.draw(overlay);

        // 手机弹窗
        if (m_showPhone && m_phoneSprite) {
            sf::RectangleShape bg({static_cast<float>(SCREEN_W), static_cast<float>(SCREEN_H)});
            bg.setFillColor(sf::Color(0, 0, 0, 180));
            window.draw(bg);
            window.draw(*m_phoneSprite);

            if (m_phoneSolved && m_msgTrueSprite && m_msgTrueTex.getSize().x > 0) {
                auto pr = GetPhoneRect();
                float mw = pr.size.x * 0.84f;
                float mh = mw / m_msgTrueTex.getSize().x * m_msgTrueTex.getSize().y;
                m_msgTrueSprite->setScale({mw / m_msgTrueTex.getSize().x, mh / m_msgTrueTex.getSize().y});
                m_msgTrueSprite->setPosition({pr.position.x + (pr.size.x - mw) / 2.f,
                                              pr.position.y + (pr.size.y - mh) / 2.f});
                window.draw(*m_msgTrueSprite);
            } else if (!m_phoneSolved) {
                // 渲染三个消息槽位
                for (int i = 0; i < 3; ++i) {
                    auto sr = GetSlotRect(i);
                    int mi = m_msgOrder[i];
                    if (static_cast<int>(m_dragFrom) == i) continue; // 正在拖拽的槽位留空
                    if (!m_msgSprites[mi]) continue;
                    auto& tex = m_msgTex[mi];
                    float s = std::min(sr.size.x / tex.getSize().x, sr.size.y / tex.getSize().y);
                    float w = tex.getSize().x * s, h = tex.getSize().y * s;
                    m_msgSprites[mi]->setScale({s, s});
                    m_msgSprites[mi]->setPosition({sr.position.x + (sr.size.x - w) / 2.f,
                                                    sr.position.y + (sr.size.y - h) / 2.f});
                    window.draw(*m_msgSprites[mi]);
                }
                // 渲染拖拽中的消息
                if (m_dragFrom >= 0) {
                    int mi = m_msgOrder[m_dragFrom];
                    if (m_msgSprites[mi]) {
                        auto sr = GetSlotRect(0);
                        auto& tex = m_msgTex[mi];
                        float s = std::min(sr.size.x / tex.getSize().x, sr.size.y / tex.getSize().y);
                        float w = tex.getSize().x * s, h = tex.getSize().y * s;
                        m_msgSprites[mi]->setScale({s, s});
                        m_msgSprites[mi]->setPosition({m_dragPos.x - w / 2.f, m_dragPos.y - h / 2.f});
                        window.draw(*m_msgSprites[mi]);
                    }
                }
            }
        }
    }

    // UI
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
