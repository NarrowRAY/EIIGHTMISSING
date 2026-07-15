#pragma once
#include <memory>
#include <vector>
#include "GameScene.h"

class Scene_2_3 : public GameScene {
public:
    explicit Scene_2_3(int entryParam);
    void OnGoodEnd() override { m_showGoodEnd = true; }
    void OnBadEnd()  override { m_showBadEnd = true; }

protected:
    MapData      BuildMapData();
    sf::Vector2f GetSpawnPoint(int entryParam);
    void         CheckExits() override;
    sf::Vector2f GetSavePosition() override;
    bool         OnTileClick(int tile, int col, int row) override;
    void         Draw(sf::RenderWindow& window) override;

private:
    // 门精灵
    std::vector<sf::Vector2i> m_doorPositions;
    sf::Texture               m_doorTex;
    std::vector<std::unique_ptr<sf::Sprite>> m_doorSprites;

    // 窗户精灵
    std::vector<sf::Vector2i> m_windowPositions;
    sf::Texture               m_windowTex;
    std::vector<std::unique_ptr<sf::Sprite>> m_windowSprites;

    // 打印机精灵
    std::vector<sf::Vector2i> m_printerPositions;
    sf::Texture               m_printerTex;
    std::vector<std::unique_ptr<sf::Sprite>> m_printerSprites;

    // GoodEnd / BadEnd CG 全屏贴图
    sf::Texture m_goodEndTex;
    std::unique_ptr<sf::Sprite> m_goodEndSprite;
    bool m_showGoodEnd = false;

    sf::Texture m_badEndTex;
    std::unique_ptr<sf::Sprite> m_badEndSprite;
    bool m_showBadEnd = false;
};
