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
    std::unique_ptr<sf::Text>    m_title;
    sf::RectangleShape           m_btnStart;
    std::unique_ptr<sf::Text>    m_btnStartLabel;
    sf::RectangleShape           m_btnContinue;
    std::unique_ptr<sf::Text>    m_btnContinueLabel;
    sf::RectangleShape           m_btnExit;
    std::unique_ptr<sf::Text>    m_btnExitLabel;
    bool m_hoverStart    = false;
    bool m_hoverContinue = false;
    bool m_hoverExit     = false;
    bool m_wasPressed    = false;

    SavePanel m_loadPanel;
    bool m_showLoad  = false;
    bool m_firstFrame = true;
};
