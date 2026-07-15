#pragma once
#include "Scene.h"
#include "SavePanel.h"
#include <memory>

class TitleScene : public Scene {
public:
    explicit TitleScene(const sf::Font* font);

    void HandleInput(const sf::RenderWindow& window) override;
    void HandleEvent(const sf::Event& event) override;
    void Update(float dt) override;
    void Draw(sf::RenderWindow& window) override;

    int  GetNextScene()  const override { return m_nextScene; }
    int  GetEntryParam() const override { return m_entryParam; }

private:
    const sf::Font*              m_font;
    sf::Texture                  m_bgTex;
    std::unique_ptr<sf::Sprite>  m_bgSprite;
    sf::ConvexShape              m_btnStart;
    sf::ConvexShape              m_btnContinue;
    sf::ConvexShape              m_btnExit;
    bool m_hoverStart    = false;
    bool m_hoverContinue = false;
    bool m_hoverExit     = false;
    bool m_wasPressed    = false;

    SavePanel m_loadPanel;
    bool m_showLoad  = false;
    bool m_firstFrame = true;

    // ———— 坐标显示（Tab 切换） ————
    bool m_showCoords = false;
};
