#include "GameScene.h"
#include <cmath>

bool GameScene::s_passwordUnlocked = false;

void GameScene::LoadMap(const MapData& data, sf::Vector2f spawn) {
    m_tileMap.SetMapData(data);
    m_player.SetPosition(spawn);
    m_player.SetMap(&m_tileMap);

    m_camera.setSize({SCREEN_W, SCREEN_H});

    // 相机直接定位到最终位置，避免首帧"飞入"效果
    sf::Vector2f target = spawn;
    float mapW = static_cast<float>(data.width  * TILE_SIZE);
    float mapH = static_cast<float>(data.height * TILE_SIZE);
    if (mapW <= SCREEN_W)  target.x = mapW / 2.f;
    if (mapH <= SCREEN_H)  target.y = mapH / 2.f;
    float halfW = SCREEN_W / 2.f;
    float halfH = SCREEN_H / 2.f;
    if (mapW > SCREEN_W) {
        if (target.x < halfW) target.x = halfW;
        if (target.x > mapW - halfW) target.x = mapW - halfW;
    }
    if (mapH > SCREEN_H) {
        if (target.y < halfH) target.y = halfH;
        if (target.y > mapH - halfH) target.y = mapH - halfH;
    }
    m_camera.setCenter(target);
}

// ============================================================
// 对话系统
// ============================================================
void GameScene::StartDialogue(const std::vector<std::string>& lines, float delay) {
    if (delay > 0.f) {
        m_dialogueTimer = delay;
        m_pendingLines = lines;
        m_pendingPortraits.clear();
    } else {
        m_dialogueLines = lines;
        m_dialoguePortraits.clear();
        m_dialogueIndex = 0;
        m_dialogueTimer = 0.f;
        m_pendingLines.clear();
        m_pendingPortraits.clear();
        m_typewriterPos = 0.f;
        ApplyPortraitForLine();
    }
}

void GameScene::StartDialogue(const std::vector<std::string>& lines,
                                const std::vector<Portrait>& portraits, float delay) {
    if (delay > 0.f) {
        m_dialogueTimer = delay;
        m_pendingLines = lines;
        m_pendingPortraits = portraits;
    } else {
        m_dialogueLines = lines;
        m_dialoguePortraits = portraits;
        m_dialogueIndex = 0;
        m_dialogueTimer = 0.f;
        m_pendingLines.clear();
        m_pendingPortraits.clear();
        m_typewriterPos = 0.f;
        ApplyPortraitForLine();
    }
}

void GameScene::ApplyPortraitForLine() {
    if (m_dialogueIndex >= 0 && m_dialogueIndex < static_cast<int>(m_dialoguePortraits.size()))
        m_currentPortrait = m_dialoguePortraits[m_dialogueIndex];
}

void GameScene::DrawDialogueBox(sf::RenderWindow& window) {
    if (!IsInDialogue()) return;

    const float dlgH = 160.f;
    const float dlgY = SCREEN_H - dlgH - 40.f;

    // ———— 立绘 ————
    auto loadPortrait = [&](std::unique_ptr<sf::Sprite>& spr, sf::Texture& tex, const char* path) {
        if (!spr && tex.loadFromFile(path)) {
            tex.setSmooth(true);
            spr = std::make_unique<sf::Sprite>(tex);
            spr->setScale({500.f / tex.getSize().x, 500.f / tex.getSize().y});
        }
    };
    loadPortrait(m_portraitNomal, m_portraitTexNomal, "assets/portraits/nomal.png");
    loadPortrait(m_portraitSmile, m_portraitTexSmile, "assets/portraits/smile.png");
    loadPortrait(m_portraitWhattt, m_portraitTexWhattt, "assets/portraits/whattt.png");

    sf::Sprite* pSprite = nullptr;
    switch (m_currentPortrait) {
        case Portrait::Nomal:  pSprite = m_portraitNomal.get(); break;
        case Portrait::Smile:  pSprite = m_portraitSmile.get(); break;
        case Portrait::Whattt: pSprite = m_portraitWhattt.get(); break;
        default: break;
    }
    if (pSprite) {
        pSprite->setPosition({230.f, dlgY - 445.f});
        window.draw(*pSprite);
    }

    // ———— 对话框 ————
    const float dlgW = 1200.f;
    const float dlgX = (SCREEN_W - dlgW) / 2.f;

    // 砍角对话框（八边形）
    const float corner = 20.f;  // 砍角大小
    sf::ConvexShape shape(8);
    shape.setPoint(0, {corner, 0.f});
    shape.setPoint(1, {dlgW - corner, 0.f});
    shape.setPoint(2, {dlgW, corner});
    shape.setPoint(3, {dlgW, dlgH - corner});
    shape.setPoint(4, {dlgW - corner, dlgH});
    shape.setPoint(5, {corner, dlgH});
    shape.setPoint(6, {0.f, dlgH - corner});
    shape.setPoint(7, {0.f, corner});
    shape.setPosition({dlgX, dlgY});
    shape.setFillColor(sf::Color(30, 30, 45, 180));
    shape.setOutlineColor(sf::Color(200, 200, 220, 220));
    shape.setOutlineThickness(4.f);
    window.draw(shape);

    // 塑料反光条（顶部高光）
    sf::ConvexShape glare(4);
    glare.setPoint(0, {corner + 2.f, 2.f});
    glare.setPoint(1, {dlgW - corner - 2.f, 2.f});
    glare.setPoint(2, {dlgW - corner - 2.f, 10.f});
    glare.setPoint(3, {corner + 2.f, 10.f});
    glare.setPosition({dlgX, dlgY});
    glare.setFillColor(sf::Color(255, 255, 255, 30));
    window.draw(glare);

    if (m_font && m_dialogueIndex < static_cast<int>(m_dialogueLines.size())) {
        std::string full = m_dialogueLines[m_dialogueIndex];
        int visible = std::min(static_cast<int>(m_typewriterPos), static_cast<int>(full.size()));
        std::string shown = full.substr(0, visible);

        sf::Text text(*m_font, U(shown), 26);
        text.setFillColor(sf::Color(240, 240, 240));
        sf::FloatRect tb = text.getLocalBounds();
        text.setPosition(sf::Vector2f(
            dlgX + 30.f,
            dlgY + (dlgH - tb.size.y) / 2.f - tb.position.y
        ));
        window.draw(text);

        // 右下角提示
        bool isLast = (m_dialogueIndex >= static_cast<int>(m_dialogueLines.size()) - 1);
        if (!isLast || m_typewriterPos < static_cast<float>(full.size())) {
            sf::Text hint(*m_font, U("▼"), 20);
            hint.setFillColor(sf::Color(180, 180, 200, 180));
            hint.setPosition(sf::Vector2f(dlgX + dlgW - 40.f, dlgY + dlgH - 28.f));
            window.draw(hint);
        }
    }
}

// ============================================================
// 鼠标点击检测（轮询方式）
// ============================================================
void GameScene::TryInteract(const sf::RenderWindow& window) {
    // ---- 存档面板 ----
    if (m_savePanel.IsVisible()) {
        bool down = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        if (down && !m_mouseWasDown) {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::View uiView({SCREEN_W / 2.f, SCREEN_H / 2.f}, {SCREEN_W, SCREEN_H});
            m_savePanel.HandleClick(window.mapPixelToCoords(pixelPos, uiView));
        }
        m_mouseWasDown = down;
        return;
    }

    // ---- 对话中：点击推进 ----
    if (IsInDialogue()) {
        bool down = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        if (down && !m_mouseWasDown) {
            int len = static_cast<int>(m_dialogueLines[m_dialogueIndex].size());
            if (m_typewriterPos < static_cast<float>(len)) {
                // 未打完 → 直接显示全部
                m_typewriterPos = static_cast<float>(len);
            } else {
                // 已打完 → 下一行
                m_dialogueIndex++;
                m_typewriterPos = 0.f;
                ApplyPortraitForLine();
                if (m_dialogueIndex >= static_cast<int>(m_dialogueLines.size())) {
                    m_dialogueIndex = -1;
                    m_dialogueLines.clear();
                    m_currentPortrait = Portrait::None;
                }
            }
        }
        m_mouseWasDown = down;
        return;
    }

    // ---- 模态对话框 ----
    if (m_passwordUI.IsVisible()) {
        bool down = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        if (down && !m_mouseWasDown) {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::View uiView({SCREEN_W / 2.f, SCREEN_H / 2.f}, {SCREEN_W, SCREEN_H});
            m_passwordUI.HandleClick(window.mapPixelToCoords(pixelPos, uiView));
        }
        m_mouseWasDown = down;
        return;
    }
    if (m_choiceDialog.IsVisible()) {
        bool down = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        if (down && !m_mouseWasDown) {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::View uiView({SCREEN_W / 2.f, SCREEN_H / 2.f}, {SCREEN_W, SCREEN_H});
            m_choiceDialog.HandleClick(window.mapPixelToCoords(pixelPos, uiView));
        }
        m_mouseWasDown = down;
        return;
    }

    // ---- 非模态：检测点击 ----
    bool down = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    bool clicked = down && !m_mouseWasDown;
    m_mouseWasDown = down;
    if (!clicked) return;

    // 设置按钮（屏幕坐标）→ 弹出 ChoiceDialog
    if (m_dialogueTimer <= 0.f) {
        sf::Vector2i mp = sf::Mouse::getPosition(window);
        sf::View uiView({SCREEN_W / 2.f, SCREEN_H / 2.f}, {SCREEN_W, SCREEN_H});
        sf::Vector2f sp = window.mapPixelToCoords(mp, uiView);
        sf::FloatRect gearBtn({SCREEN_W - 60.f, 10.f}, {50.f, 50.f});
        if (gearBtn.contains(sp)) {
            m_choiceDialog.Show("", {"继续游戏", "回到菜单"});
            return;
        }
    }

    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f worldClick = window.mapPixelToCoords(pixelPos, m_camera);
    int clickCol = TileMap::PixelToTile(worldClick.x);
    int clickRow = TileMap::PixelToTile(worldClick.y);

    // 搜索点击位置 ±1 格
    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            int c = clickCol + dc, r = clickRow + dr;
            if (!m_tileMap.IsInBounds(c, r)) continue;
            int tile = m_tileMap.GetTile(c, r);
            sf::Vector2f pp = m_player.GetPosition();

            // ---- 打印机（存档） ----
            if (tile == static_cast<int>(TileType::Printer)) {
                float dx = (c + 0.5f) * TILE_SIZE - pp.x;
                float dy = (r + 0.5f) * TILE_SIZE - pp.y;
                if (std::sqrt(dx * dx + dy * dy) <= 160.f) {
                    m_savePanel.Show(true);
                }
                return;
            }

            // ---- 公告栏 ----
            if (tile == static_cast<int>(TileType::BulletinBoard)) {
                float dx = (c + 0.5f) * TILE_SIZE - pp.x;
                float dy = (r + 0.5f) * TILE_SIZE - pp.y;
                if (std::sqrt(dx * dx + dy * dy) <= 160.f) {
                    m_choiceDialog.ShowInfoWide("公告栏（密码锁线索2）");
                }
                return;
            }

            // ---- 密码锁 ----
            if (tile == static_cast<int>(TileType::KeypadLock)) {
                float lx = c * TILE_SIZE + TILE_SIZE / 2.f;
                float ly = r * TILE_SIZE + TILE_SIZE / 2.f;
                if (std::sqrt((lx - pp.x) * (lx - pp.x) + (ly - pp.y) * (ly - pp.y)) <= 160.f) {
                    if (s_passwordUnlocked) {
                        m_nextScene = static_cast<int>(SceneID::Stairwell1);
                        m_entryParam = 0;
                    } else {
                        m_passwordUI.Show();
                    }
                }
                return;
            }

            // ---- 场景专属交互 ----
            if (OnTileClick(tile, c, r)) return;

            // ---- 左窗户（cols 3-6 区域的窗或墙） ----
            if ((tile == static_cast<int>(TileType::Window) ||
                 tile == static_cast<int>(TileType::Wall)) && c <= 6) {
                // 确保附近确实有窗户（不是纯墙壁区域）
                bool nearWindow = false;
                for (int wr = -1; wr <= 1 && !nearWindow; ++wr)
                    for (int wc = -1; wc <= 1; ++wc)
                        if (m_tileMap.IsInBounds(c + wc, r + wr) &&
                            m_tileMap.GetTile(c + wc, r + wr) == static_cast<int>(TileType::Window))
                            nearWindow = true;
                if (nearWindow) {
                    float dx = (c + 0.5f) * TILE_SIZE - pp.x;
                    float dy = (r + 0.5f) * TILE_SIZE - pp.y;
                    if (std::sqrt(dx * dx + dy * dy) <= 160.f) {
                        m_choiceDialog.ShowInfo("密码锁线索1");
                    }
                    return;
                }
            }

            // ---- 沙发 ----
            if (tile == static_cast<int>(TileType::Sofa)) {
                float dx = (c + 0.5f) * TILE_SIZE - pp.x;
                float dy = (r + 0.5f) * TILE_SIZE - pp.y;
                if (std::sqrt(dx * dx + dy * dy) <= 160.f) {
                    StartDialogue({"一张舒服的沙发，可以坐。"});
                }
                return;
            }
        }
    }
}

void GameScene::HandleInput(const sf::RenderWindow& window) {
    TryInteract(window);
    if (IsInDialogue() || m_passwordUI.IsVisible() || m_choiceDialog.IsVisible() || m_savePanel.IsVisible()) return;
    if (m_inputLockTimer > 0.f) return;  // 输入锁定中
    m_player.HandleInput();
}

void GameScene::Update(float dt) {
    // 首次进入回调
    if (m_firstUpdate) {
        m_firstUpdate = false;
        OnSceneEnter();
    }

    // 输入锁定倒计时
    if (m_inputLockTimer > 0.f) m_inputLockTimer -= dt;

    // ---- 相机始终跟随玩家（对话/模态期间也更新） ----
    sf::Vector2f target = m_player.GetPosition();
    float mapW = static_cast<float>(m_tileMap.Width()  * TILE_SIZE);
    float mapH = static_cast<float>(m_tileMap.Height() * TILE_SIZE);

    if (mapW <= SCREEN_W)  target.x = mapW / 2.f;
    if (mapH <= SCREEN_H)  target.y = mapH / 2.f;

    float halfW = SCREEN_W / 2.f;
    float halfH = SCREEN_H / 2.f;
    if (mapW > SCREEN_W) {
        if (target.x < halfW) target.x = halfW;
        if (target.x > mapW - halfW) target.x = mapW - halfW;
    }
    if (mapH > SCREEN_H) {
        if (target.y < halfH) target.y = halfH;
        if (target.y > mapH - halfH) target.y = mapH - halfH;
    }

    sf::Vector2f curCenter = m_camera.getCenter();
    float lerp = 5.f * dt;
    m_camera.setCenter({
        curCenter.x + (target.x - curCenter.x) * lerp,
        curCenter.y + (target.y - curCenter.y) * lerp
    });

    // ---- 对话延迟计时器 ----
    if (m_dialogueTimer > 0.f) {
        m_dialogueTimer -= dt;
        if (m_dialogueTimer <= 0.f) {
            m_dialogueLines = std::move(m_pendingLines);
            m_dialoguePortraits = std::move(m_pendingPortraits);
            m_dialogueIndex = 0;
            m_typewriterPos = 0.f;
            ApplyPortraitForLine();
        }
    }

    // ---- 对话中：推进打字机 ----
    if (IsInDialogue()) {
        int len = static_cast<int>(m_dialogueLines[m_dialogueIndex].size());
        if (m_typewriterPos < static_cast<float>(len)) {
            m_typewriterPos += TYPE_SPEED * dt;
            if (m_typewriterPos > static_cast<float>(len))
                m_typewriterPos = static_cast<float>(len);
        }
        return;
    }

    // 处理选项对话框结果
    if (m_choiceDialog.GetChoice() >= 0) {
        int c = m_choiceDialog.GetChoice();
        m_choiceDialog.ResetChoice();

        // ---- 确认弹窗结果（"是/否"） ----
        if (m_pendingConfirm) {
            m_pendingConfirm = false;
            if (c == 0) {  // "是" → 展示结果
                if (m_isCorrectClassroom) {
                    m_classroomResult = ClassroomResult::Correct;
                    m_choiceDialog.ShowInfoWide("呃我这样的屌丝也有走对教室的一天啊......");
                } else {
                    m_classroomResult = ClassroomResult::Wrong;
                    m_choiceDialog.ShowInfoWide("病急也不能乱投医，你坐在陌生的天花板下任凭第一节课溜走。");
                }
            }
            // c==1: "否" → 不做任何事，直接关闭
            return;
        }

        // ---- 教室门选项结果 ----
        if (m_doorDialogActive) {
            m_doorDialogActive = false;
            if (c == 1) {  // "这是我的教室" → 弹出确认弹窗
                m_pendingConfirm = true;
                m_choiceDialog.Show(
                    "落座错误的教室将导致游戏结束，确认吗？",
                    {"是", "否"}
                );
            }
            // c==0: 教室名(纯信息,已关闭)  c==2: "这不是我的教室"
            return;
        }

        // ---- 设置菜单：0=继续游戏, 1=回到菜单 ----
        if (c == 1) {
            m_nextScene = static_cast<int>(SceneID::Title);
            m_entryParam = 0;
            return;
        }
    }

    // ---- 教室结果信息面板关闭 → 回标题 ----
    if (m_classroomResult != ClassroomResult::None && !m_choiceDialog.IsVisible()) {
        m_classroomResult = ClassroomResult::None;
        m_nextScene = static_cast<int>(SceneID::Title);
        m_entryParam = 0;
        return;
    }

    // 存档面板结果
    if (m_savePanel.GetResult() >= 0) {
        int slot = m_savePanel.GetResult();
        m_savePanel.ResetResult();
        sf::Vector2f savePos = GetSavePosition();
        SavePanel::WriteSave(slot, m_sceneId, savePos.x, savePos.y);
    }
    if (m_savePanel.IsVisible()) return;

    if (m_passwordUI.IsVisible() || m_choiceDialog.IsVisible()) return;

    // 输入锁定期不更新玩家移动和朝向
    if (m_inputLockTimer > 0.f) { CheckExits(); return; }

    m_player.Update(dt);
    CheckExits();

    // 密码正确 → 切换场景
    if (m_passwordUI.IsUnlocked()) {
        m_passwordUI.ResetUnlocked();
        s_passwordUnlocked = true;
        m_nextScene = static_cast<int>(SceneID::Stairwell1);
        m_entryParam = 0;
        return;
    }
}

void GameScene::Draw(sf::RenderWindow& window) {
    // 地图 + 玩家（相机视图）
    window.setView(m_camera);
    sf::Vector2f cam = m_camera.getCenter();
    m_tileMap.Draw(window, cam.x, cam.y);
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

    // UI 叠加层（屏幕视图）
    {
        sf::View uiView({SCREEN_W / 2.f, SCREEN_H / 2.f}, {SCREEN_W, SCREEN_H});
        window.setView(uiView);

        // 小地图（右上角）
        DrawMinimap(window);

        bool hasModal = IsInDialogue() || m_passwordUI.IsVisible() || m_choiceDialog.IsVisible() || m_savePanel.IsVisible();
        if (hasModal) {
            if (IsInDialogue())            DrawDialogueBox(window);
            if (m_passwordUI.IsVisible())  m_passwordUI.Draw(window);
            if (m_choiceDialog.IsVisible()) m_choiceDialog.Draw(window);
            if (m_savePanel.IsVisible())   m_savePanel.Draw(window);
        }
        // 设置齿轮图标（小地图下方）
        if (!IsInDialogue() && m_dialogueTimer <= 0.f) {
            sf::RectangleShape shape;
            const float bx = SCREEN_W - 60.f, bs = 50.f;
            float gearY = 10.f;
            int mapW = m_tileMap.Width(), mapH = m_tileMap.Height();
            if (mapW > 0 && mapH > 0) {
                float ts = std::min(150.f / mapW, 150.f / mapH);
                float mmBottom = 10.f + ts * mapH + 8.f;
                if (mmBottom > gearY) gearY = mmBottom + 8.f;
            }
            shape.setSize({bs, bs});
            shape.setPosition({bx, gearY});
            shape.setFillColor(sf::Color(50, 50, 60, 200));
            shape.setOutlineColor(sf::Color(150, 150, 170));
            shape.setOutlineThickness(2.f);
            window.draw(shape);

            float cx = bx + bs / 2.f, cy = gearY + bs / 2.f;
            sf::CircleShape circle(10.f);
            circle.setFillColor(sf::Color(200, 200, 220));
            circle.setOrigin({10.f, 10.f});
            circle.setPosition({cx, cy});
            window.draw(circle);

            for (int i = 0; i < 6; ++i) {
                float angle = i * 3.14159f / 3.f;
                float sx = cx + std::cos(angle) * 16.f;
                float sy = cy + std::sin(angle) * 16.f;
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

// ============================================================
// 小地图（右上角，仿元气骑士风格）
// ============================================================
void GameScene::DrawMinimap(sf::RenderWindow& window) {
    int mapW = m_tileMap.Width();
    int mapH = m_tileMap.Height();
    if (mapW <= 0 || mapH <= 0) return;

    const float MM_MAX = 150.f;
    const float MM_RIGHT = SCREEN_W - 70.f;
    float tileSz = std::min(MM_MAX / mapW, MM_MAX / mapH);
    float mmW = tileSz * mapW;
    float mmH = tileSz * mapH;
    float mmX = MM_RIGHT - mmW;
    float mmY = 10.f;
    const float PAD = 4.f;

    // 半透明背景 + 边框
    sf::RectangleShape bg;
    bg.setSize({mmW + PAD * 2, mmH + PAD * 2});
    bg.setPosition({mmX - PAD, mmY - PAD});
    bg.setFillColor(sf::Color(0, 0, 0, 170));
    bg.setOutlineColor(sf::Color(140, 140, 160, 200));
    bg.setOutlineThickness(2.f);
    window.draw(bg);

    // 瓦片
    sf::RectangleShape tile;
    for (int row = 0; row < mapH; ++row) {
        for (int col = 0; col < mapW; ++col) {
            int t = m_tileMap.GetTile(col, row);
            sf::Color c;
            auto type = static_cast<TileType>(t);
            switch (type) {
                case TileType::Wall:           c = sf::Color(120, 120, 130); break;
                case TileType::Door:           c = sf::Color(200, 140, 70);  break;
                case TileType::Window:         c = sf::Color(100, 160, 220); break;
                case TileType::Stairs:         c = sf::Color(180, 160, 90);  break;
                case TileType::Void:           c = sf::Color(15, 15, 20);    break;
                case TileType::Trap:           c = sf::Color(200, 50, 40);   break;
                case TileType::BulletinBoard:  c = sf::Color(140, 120, 80);  break;
                case TileType::KeypadLock:     c = sf::Color(140, 90, 160);  break;
                case TileType::Display:        c = sf::Color(80, 150, 220);  break;
                default:
                    c = ::IsWalkable(t) ? sf::Color(45, 45, 55)
                                        : sf::Color(80, 80, 90);
                    break;
            }
            tile.setSize({tileSz, tileSz});
            tile.setPosition({mmX + col * tileSz, mmY + row * tileSz});
            tile.setFillColor(c);
            window.draw(tile);
        }
    }

    // 玩家位置
    sf::Vector2f pp = m_player.GetPosition();
    float px = mmX + (pp.x / (mapW * TILE_SIZE)) * mmW;
    float py = mmY + (pp.y / (mapH * TILE_SIZE)) * mmH;

    sf::CircleShape glow(5.f);
    glow.setFillColor(sf::Color(100, 255, 100, 80));
    glow.setOrigin({5.f, 5.f});
    glow.setPosition({px, py});
    window.draw(glow);

    sf::CircleShape dot(3.f);
    dot.setFillColor(sf::Color(100, 255, 100));
    dot.setOrigin({3.f, 3.f});
    dot.setPosition({px, py});
    window.draw(dot);

    if (m_font) {
        sf::Text label(*m_font, U("小地图"), 15);
        label.setFillColor(sf::Color(160, 160, 180, 200));
        label.setPosition({mmX, mmY - 19.f});
        window.draw(label);
    }
}

void GameScene::Reset() {
    Scene::Reset();
    m_passwordUI.Hide();
    m_choiceDialog.Hide();
    m_dialogueIndex = -1;
    m_dialogueLines.clear();
    m_dialogueTimer = 0.f;
    m_pendingLines.clear();
    m_pendingPortraits.clear();
    m_dialoguePortraits.clear();
    m_currentPortrait = Portrait::None;
    m_typewriterPos = 0.f;
    m_mouseWasDown = false;
    m_firstUpdate  = true;
    m_doorDialogActive   = false;
    m_pendingConfirm     = false;
    m_classroomResult    = ClassroomResult::None;
    m_isCorrectClassroom = false;
}
