#include "Scene_Title.h"
#include "Constants.h"

TitleScene::TitleScene(const sf::Font* font) : m_font(font) {
    m_loadPanel.SetFont(font);

    // 背景图
    if (m_bgTex.loadFromFile("assets/textures/mainpage1.png")) {
        m_bgTex.setSmooth(true);
        m_bgSprite = std::make_unique<sf::Sprite>(m_bgTex);
        auto ts = m_bgTex.getSize();
        m_bgSprite->setScale({SCREEN_W / static_cast<float>(ts.x), SCREEN_H / static_cast<float>(ts.y)});
    }

    // ———— 三个异形按钮（默认透明，悬停时显示） ————
    auto setupQuad = [](sf::ConvexShape& shape, float x0, float y0, float x1, float y1,
                        float x2, float y2, float x3, float y3) {
        shape.setPointCount(4);
        shape.setPoint(0, {x0, y0});
        shape.setPoint(1, {x1, y1});
        shape.setPoint(2, {x2, y2});
        shape.setPoint(3, {x3, y3});
        shape.setFillColor(sf::Color::Transparent);
        shape.setOutlineThickness(0.f);
    };

    // 新游戏: TL→TR→BR→BL
    setupQuad(m_btnStart, 1380, 184, 1920, 0, 1920, 345, 1380, 440);
    // 继续游戏
    setupQuad(m_btnContinue, 1380, 453, 1920, 360, 1920, 792, 1380, 661);
    // 退出游戏
    setupQuad(m_btnExit, 1380, 673, 1920, 801, 1920, 1080, 1380, 898);
}

void TitleScene::HandleInput(const sf::RenderWindow& window) {
    // 首帧：跳过点击检测，防止上一场景的鼠标残留触发误操作
    if (m_firstFrame) {
        m_firstFrame = false;
        m_wasPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        return;
    }

    // 读档面板
    if (m_showLoad) {
        bool down = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        if (down && !m_wasPressed) {
            sf::Vector2i mp = sf::Mouse::getPosition(window);
            sf::Vector2f wp = window.mapPixelToCoords(mp);
            m_loadPanel.HandleClick(wp);

            if (m_loadPanel.GetResult() >= 0) {
                int slot = m_loadPanel.GetResult();
                m_loadPanel.ResetResult();
                int sceneId = 0; float px = 0, py = 0; bool pwd = false;
                if (SavePanel::ReadSave(slot, sceneId, px, py, pwd)) {
                    SavePanel::SetPendingLoad(sceneId, px, py, pwd);
                    m_nextScene  = sceneId;
                    m_entryParam = -1;  // -1 表示读档，跳过开场对话
                    m_showLoad   = false;
                }
            }
            if (!m_loadPanel.IsVisible()) m_showLoad = false;
        }
        m_wasPressed = down;
        return;
    }

    // 主菜单
    sf::Vector2i mp = sf::Mouse::getPosition(window);
    sf::Vector2f wp = window.mapPixelToCoords(mp);
    m_hoverStart    = m_btnStart.getGlobalBounds().contains(wp);
    m_hoverContinue = m_btnContinue.getGlobalBounds().contains(wp);
    m_hoverExit     = m_btnExit.getGlobalBounds().contains(wp);

    bool pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    if (pressed && !m_wasPressed) {
        if (m_hoverStart) {
            m_nextScene  = static_cast<int>(SceneID::Classroom1B);
            m_entryParam = 0;
        } else if (m_hoverContinue) {
            m_showLoad = true;
            m_loadPanel.Show(false);  // 读档模式
        } else if (m_hoverExit) {
            m_nextScene = -2;
        }
    }
    m_wasPressed = pressed;
}

void TitleScene::HandleEvent(const sf::Event& event) {
    if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {
        auto code = kp->scancode;

        if (code == sf::Keyboard::Scancode::Tab) {
            m_showCoords = !m_showCoords;
            return;
        }

        if (code == sf::Keyboard::Scancode::Space ||
            code == sf::Keyboard::Scancode::Enter) {
            m_nextScene  = static_cast<int>(SceneID::Classroom1B);
            m_entryParam = 0;
        }
    }
}

void TitleScene::Update(float dt) {
    (void)dt;
    auto applyHover = [](sf::ConvexShape& shape, bool hover) {
        if (hover) {
            shape.setFillColor(sf::Color(255, 255, 255, 80));
        } else {
            shape.setFillColor(sf::Color::Transparent);
        }
    };
    applyHover(m_btnStart,    m_hoverStart);
    applyHover(m_btnContinue, m_hoverContinue);
    applyHover(m_btnExit,     m_hoverExit);
}

void TitleScene::Draw(sf::RenderWindow& window) {
    if (m_bgSprite) window.draw(*m_bgSprite);

    window.draw(m_btnStart);
    window.draw(m_btnContinue);
    window.draw(m_btnExit);

    if (m_showLoad) {
        sf::View uiView({SCREEN_W / 2.f, SCREEN_H / 2.f}, {SCREEN_W, SCREEN_H});
        window.setView(uiView);
        m_loadPanel.Draw(window);
    }

    // ———— 坐标显示 ————
    if (m_showCoords && m_font) {
        sf::Vector2i mp = sf::Mouse::getPosition(window);
        sf::Vector2f wp = window.mapPixelToCoords(mp);
        std::string info = "pixel: " + std::to_string(mp.x) + ", " + std::to_string(mp.y)
                         + "  |  logic: " + std::to_string(static_cast<int>(wp.x)) + ", " + std::to_string(static_cast<int>(wp.y));

        sf::Text t(*m_font, U(info), 20);
        t.setFillColor(sf::Color(255, 255, 100));
        t.setOutlineColor(sf::Color(0, 0, 0, 200));
        t.setOutlineThickness(2.f);
        t.setPosition({10.f, 10.f});
        window.draw(t);

        sf::Text hint(*m_font, U("[Tab] 隐藏坐标"), 16);
        hint.setFillColor(sf::Color(180, 180, 180));
        hint.setOutlineColor(sf::Color(0, 0, 0, 180));
        hint.setOutlineThickness(1.f);
        hint.setPosition({10.f, 36.f});
        window.draw(hint);
    }
}
