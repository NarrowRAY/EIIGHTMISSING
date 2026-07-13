#include "Scene_Title.h"
#include "Constants.h"

TitleScene::TitleScene(const sf::Font* font) : m_font(font) {
    m_bgmId = 1;  // 标题画面不播放BGM
    m_loadPanel.SetFont(font);

    m_title = std::make_unique<sf::Text>(*font, U("八点迷思"), 80);
    m_title->setFillColor(Colors::TitleGold);
    m_title->setPosition({
        SCREEN_W / 2.f - m_title->getLocalBounds().size.x / 2.f,
        SCREEN_H * 0.18f
    });

    float btnW = 260, btnH = 60;
    float btnX = SCREEN_W / 2.f - btnW / 2.f;
    float startY   = SCREEN_H * 0.38f;
    float contY    = SCREEN_H * 0.48f;
    float exitY    = SCREEN_H * 0.58f;

    auto makeBtn = [&](sf::RectangleShape& btn, std::unique_ptr<sf::Text>& label,
                        const std::string& text, float y) {
        btn.setSize({btnW, btnH});
        btn.setFillColor(Colors::ButtonNormal);
        btn.setPosition({btnX, y});
        label = std::make_unique<sf::Text>(*font, U(text), 32);
        label->setFillColor(Colors::ButtonText);
        label->setPosition({
            btnX + btnW / 2.f - label->getLocalBounds().size.x / 2.f,
            y + btnH / 2.f - 18.f
        });
    };

    makeBtn(m_btnStart,     m_btnStartLabel,    "新游戏",    startY);
    makeBtn(m_btnContinue,  m_btnContinueLabel, "继续游戏",   contY);
    makeBtn(m_btnExit,      m_btnExitLabel,     "退出游戏",   exitY);
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
        if (kp->scancode == sf::Keyboard::Scancode::Space ||
            kp->scancode == sf::Keyboard::Scancode::Enter) {
            m_nextScene  = static_cast<int>(SceneID::Classroom1B);
            m_entryParam = 0;
        }
    }
}

void TitleScene::Update(float dt) {
    (void)dt;
    m_btnStart.setFillColor(m_hoverStart ? Colors::ButtonHover : Colors::ButtonNormal);
    m_btnContinue.setFillColor(m_hoverContinue ? Colors::ButtonHover : Colors::ButtonNormal);
    m_btnExit.setFillColor(m_hoverExit ? Colors::ButtonHover : Colors::ButtonNormal);
}

void TitleScene::Draw(sf::RenderWindow& window) {
    window.draw(*m_title);
    window.draw(m_btnStart);
    window.draw(*m_btnStartLabel);
    window.draw(m_btnContinue);
    window.draw(*m_btnContinueLabel);
    window.draw(m_btnExit);
    window.draw(*m_btnExitLabel);

    if (m_showLoad) {
        sf::View uiView({SCREEN_W / 2.f, SCREEN_H / 2.f}, {SCREEN_W, SCREEN_H});
        window.setView(uiView);
        m_loadPanel.Draw(window);
    }
}
