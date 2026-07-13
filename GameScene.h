#pragma once
#include "Scene.h"
#include "TileMap.h"
#include "Player.h"
#include "PasswordUI.h"
#include "ChoiceDialog.h"
#include "SavePanel.h"
#include "AudioManager.h"

class GameScene : public Scene {
public:
    GameScene() = default;
    virtual ~GameScene() = default;

    void SetFont(const sf::Font* font) {
        m_font = font;
        m_passwordUI.SetFont(font);
        m_choiceDialog.SetFont(font);
        m_savePanel.SetFont(font);
    }
    void SetAudio(AudioManager* audio) {
        m_audio = audio;
        m_passwordUI.SetAudio(audio);
    }
    void SetPlayerPosition(sf::Vector2f pos) { m_player.SetPosition(pos); }
    void SetPlayerFacing(Direction dir) { m_player.SetFacing(dir); }
    void LockInput(float duration) { m_inputLockTimer = duration; }
    void SetSceneID(int id) { m_sceneId = id; }

    void HandleInput(const sf::RenderWindow& window) override;
    void Update(float dt) override;
    void Draw(sf::RenderWindow& window) override;

    int  GetNextScene()  const override { return m_nextScene; }
    int  GetEntryParam() const override { return m_entryParam; }
    void Reset() override;

protected:
    void LoadMap(const MapData& data, sf::Vector2f spawn);

    virtual void CheckExits() = 0;
    virtual void OnSceneEnter() {}  // 子类重写，首次进入时触发
    // 子类专属瓦片交互（返回 true 表示已处理）
    virtual bool OnTileClick(int tile, int col, int row) { (void)tile; (void)col; (void)row; return false; }
    // 存档位置调整（子类可重写，让读档时出生在指定位置）
    virtual sf::Vector2f GetSavePosition() { return m_player.GetPosition(); }

    // 对话系统
    void StartDialogue(const std::vector<std::string>& lines, float delay = 0.f);
    bool IsInDialogue() const { return m_dialogueIndex >= 0; }

public:
    // 密码锁状态
    static bool s_passwordUnlocked;

protected:
    TileMap        m_tileMap;
    Player         m_player;
    sf::View       m_camera;
    const sf::Font* m_font = nullptr;

    // 交互系统
    PasswordUI     m_passwordUI;
    ChoiceDialog   m_choiceDialog;
    SavePanel      m_savePanel;
    AudioManager*  m_audio = nullptr;
    int            m_sceneId = 0;
    float          m_footstepTimer = 0.f;

private:
    void TryInteract(const sf::RenderWindow& window);
    void DrawDialogueBox(sf::RenderWindow& window);
    bool m_mouseWasDown = false;
    bool m_firstUpdate  = true;
    float m_inputLockTimer = 0.f;  // 输入锁定时长

    // 对话状态
    std::vector<std::string> m_dialogueLines;
    int   m_dialogueIndex = -1;  // -1=无对话
    float m_dialogueTimer = 0.f; // 延迟计时器
    std::vector<std::string> m_pendingLines; // 等待中的台词
    float m_typewriterPos = 0.f; // 当前行已显示字符数（浮点，用于平滑计时）
    static constexpr float TYPE_SPEED = 40.f; // 每秒显示字符数
};
