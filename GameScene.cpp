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
    } else {
        m_dialogueLines = lines;
        m_dialogueIndex = 0;
        m_dialogueTimer = 0.f;
        m_pendingLines.clear();
        m_typewriterPos = 0.f;
    }
}

void GameScene::DrawDialogueBox(sf::RenderWindow& window) {
    if (!IsInDialogue()) return;

    const float dlgW = 1200.f;
    const float dlgH = 80.f;
    const float dlgX = (SCREEN_W - dlgW) / 2.f;
    const float dlgY = SCREEN_H - 150.f;

    sf::RectangleShape shape;
    shape.setSize({dlgW, dlgH});
    shape.setPosition({dlgX, dlgY});
    shape.setFillColor(sf::Color(20, 20, 30, 230));
    shape.setOutlineColor(sf::Color(180, 180, 200));
    shape.setOutlineThickness(2.f);
    window.draw(shape);

    if (m_font && m_dialogueIndex < static_cast<int>(m_dialogueLines.size())) {
        // 打字机效果：只显示已打出的字符
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
            if (m_audio) m_audio->PlaySFX("click");
        }
        m_mouseWasDown = down;
        return;
    }

    // ---- 对话中：点击推进 ----
    if (IsInDialogue()) {
        bool down = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        if (down && !m_mouseWasDown) {
            if (m_audio) m_audio->PlaySFX("click");
            int len = static_cast<int>(m_dialogueLines[m_dialogueIndex].size());
            if (m_typewriterPos < static_cast<float>(len)) {
                m_typewriterPos = static_cast<float>(len);
            } else {
                m_dialogueIndex++;
                m_typewriterPos = 0.f;
                if (m_dialogueIndex >= static_cast<int>(m_dialogueLines.size())) {
                    m_dialogueIndex = -1;
                    m_dialogueLines.clear();
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
            if (m_audio) m_audio->PlaySFX("click");
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
                    if (m_audio) m_audio->PlaySFX("interact");
                    m_savePanel.Show(true);
                }
                return;
            }

            // ---- 公告栏 ----
            if (tile == static_cast<int>(TileType::BulletinBoard)) {
                float dx = (c + 0.5f) * TILE_SIZE - pp.x;
                float dy = (r + 0.5f) * TILE_SIZE - pp.y;
                if (std::sqrt(dx * dx + dy * dy) <= 160.f) {
                    if (m_audio) m_audio->PlaySFX("interact");
                    m_choiceDialog.ShowInfoWide("公告栏上贴满了社团招新海报，角落有一张褪色的通知：\n「楼道门禁密码请咨询教务处。」\n但有人用圆珠笔在下面写了几个数字……开头好像是 08。");
                }
                return;
            }

            // ---- 密码锁 ----
            if (tile == static_cast<int>(TileType::KeypadLock)) {
                float lx = c * TILE_SIZE + TILE_SIZE / 2.f;
                float ly = r * TILE_SIZE + TILE_SIZE / 2.f;
                if (std::sqrt((lx - pp.x) * (lx - pp.x) + (ly - pp.y) * (ly - pp.y)) <= 160.f) {
                    if (m_audio) m_audio->PlaySFX("interact");
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
                        if (m_audio) m_audio->PlaySFX("interact");
                        m_choiceDialog.ShowInfo("透过窗户能看到教室里的挂钟，\n时间停在了 8:31。");
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
            m_dialogueIndex = 0;
            m_typewriterPos = 0.f;
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
        // 设置菜单：0=继续游戏, 1=回到菜单
        if (c == 1) {
            m_nextScene = static_cast<int>(SceneID::Title);
            m_entryParam = 0;
            return;
        }
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

    // ---- 脚步声音效 ----
    m_player.Update(dt);
    if (m_audio && m_player.IsMoving()) {
        if (m_footstepTimer <= 0.f) {
            m_footstepTimer = 0.35f;
            m_audio->PlayRandomSFX("footstep", 3);
        }
        m_footstepTimer -= dt;
    }
    if (!m_player.IsMoving()) m_footstepTimer = 0.f;

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
        // 设置齿轮图标（非对话+非对话等待时显示，小地图下方）
        if (!IsInDialogue() && m_dialogueTimer <= 0.f) {
            sf::RectangleShape shape;
            const float bx = SCREEN_W - 60.f, bs = 50.f;

            // 如果小地图区域会与齿轮重叠，将齿轮下移
            float gearY = 10.f;
            int mapW = m_tileMap.Width();
            int mapH = m_tileMap.Height();
            if (mapW > 0 && mapH > 0) {
                float tileSz = std::min(150.f / mapW, 150.f / mapH);
                float mmBottom = 10.f + tileSz * mapH + 8.f;
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

    const float MM_MAX = 150.f;                      // 最大边长
    const float MM_RIGHT = SCREEN_W - 70.f;          // 右侧留70给齿轮按钮
    float tileSz = std::min(MM_MAX / mapW, MM_MAX / mapH);
    float mmW = tileSz * mapW;
    float mmH = tileSz * mapH;
    float mmX = MM_RIGHT - mmW;                      // 右对齐
    float mmY = 10.f;                                // 顶部
    const float PAD = 4.f;

    // ---- 半透明背景 + 边框 ----
    sf::RectangleShape bg;
    bg.setSize({mmW + PAD * 2, mmH + PAD * 2});
    bg.setPosition({mmX - PAD, mmY - PAD});
    bg.setFillColor(sf::Color(0, 0, 0, 170));
    bg.setOutlineColor(sf::Color(140, 140, 160, 200));
    bg.setOutlineThickness(2.f);
    window.draw(bg);

    // ---- 瓦片 ----
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
                    if (::IsWalkable(t))
                        c = sf::Color(45, 45, 55);   // 地板暗色
                    else
                        c = sf::Color(80, 80, 90);   // 家具等
                    break;
            }
            tile.setSize({tileSz, tileSz});
            tile.setPosition({mmX + col * tileSz, mmY + row * tileSz});
            tile.setFillColor(c);
            window.draw(tile);
        }
    }

    // ---- 玩家位置（绿色脉冲圆点） ----
    sf::Vector2f pp = m_player.GetPosition();
    float px = mmX + (pp.x / (mapW * TILE_SIZE)) * mmW;
    float py = mmY + (pp.y / (mapH * TILE_SIZE)) * mmH;

    // 外圈光晕
    sf::CircleShape glow(5.f);
    glow.setFillColor(sf::Color(100, 255, 100, 80));
    glow.setOrigin({5.f, 5.f});
    glow.setPosition({px, py});
    window.draw(glow);

    // 核心亮点
    sf::CircleShape dot(3.f);
    dot.setFillColor(sf::Color(100, 255, 100));
    dot.setOrigin({3.f, 3.f});
    dot.setPosition({px, py});
    window.draw(dot);

    // ---- 标题 ----
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
    m_typewriterPos = 0.f;
    m_mouseWasDown = false;
    m_firstUpdate  = true;
}
