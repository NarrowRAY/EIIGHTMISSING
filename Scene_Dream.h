#pragma once
#include <memory>
#include <vector>
#include "GameScene.h"

class Scene_Dream : public GameScene {
public:
    explicit Scene_Dream(int entryParam);

protected:
    MapData      BuildMapData();
    sf::Vector2f GetSpawnPoint(int entryParam);
    sf::Vector2f GetSavePosition() override;
    void         CheckExits() override;
    void         Draw(sf::RenderWindow& window) override;
    void         HandleInput(const sf::RenderWindow& window) override;
    bool         OnTileClick(int tile, int col, int row) override;
    void         OnSceneEnter() override;

private:
    // 门精灵
    std::vector<sf::Vector2i> m_doorPositions;
    sf::Texture               m_doorTex;
    std::vector<std::unique_ptr<sf::Sprite>> m_doorSprites;
    // 显示屏精灵
    std::vector<sf::Vector2i> m_displayPositions;
    sf::Texture               m_displayTex;
    std::vector<std::unique_ptr<sf::Sprite>> m_displaySprites;
    // 窗户精灵
    std::vector<sf::Vector2i> m_windowPositions;
    sf::Texture               m_windowTex;
    std::vector<std::unique_ptr<sf::Sprite>> m_windowSprites;
    // 沙发精灵
    std::vector<sf::Vector2i> m_sofaPositions;
    sf::Texture               m_sofaTex;
    std::vector<std::unique_ptr<sf::Sprite>> m_sofaSprites;
    // 打印机精灵
    std::vector<sf::Vector2i> m_printerPositions;
    sf::Texture               m_printerTex;
    std::vector<std::unique_ptr<sf::Sprite>> m_printerSprites;

    // 手机弹窗
    bool m_showPhone = false;
    bool m_phoneClickWasDown = false;
    bool m_phoneSolved = false;
    sf::Texture m_phoneTex;
    std::unique_ptr<sf::Sprite> m_phoneSprite;

    // 解谜：消息拖拽
    int m_msgOrder[3] = {0, 1, 2}; // 槽位中消息的编号(0-based)
    int m_dragFrom = -1;           // -1=未拖拽, 0/1/2=拖拽的槽位
    sf::Vector2f m_dragPos;        // 鼠标位置
    sf::Texture m_msgTex[3];
    sf::Texture m_msgTrueTex;
    std::unique_ptr<sf::Sprite> m_msgSprites[3];
    std::unique_ptr<sf::Sprite> m_msgTrueSprite;

    void ShuffleMessages();
    void CheckPuzzle();
    sf::FloatRect GetPhoneRect() const;
    sf::FloatRect GetSlotRect(int i) const;
};
