#pragma once
#include "Scene.h"
#include "TileMap.h"
#include "Player.h"
#include "PasswordUI.h"
#include "ChoiceDialog.h"
#include "SavePanel.h"

class GameScene : public Scene {
public:
    enum class Portrait { None, Nomal, Smile, Whattt };

    GameScene() = default;
    virtual ~GameScene() = default;

    void SetFont(const sf::Font* font) {
        m_font = font;
        m_passwordUI.SetFont(font);
        m_choiceDialog.SetFont(font);
        m_savePanel.SetFont(font);
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
    // 教室门选项弹窗（isCorrect: 是否为正确的早八教室）
    void ShowDoorChoice(const std::string& text, const std::vector<std::string>& opts, bool isCorrect) {
        m_doorDialogActive     = true;
        m_isCorrectClassroom   = isCorrect;
        m_choiceDialog.Show(text, opts);
    }
    // 存档位置调整（子类可重写，让读档时出生在指定位置）
    virtual sf::Vector2f GetSavePosition() { return m_player.GetPosition(); }

    // 对话系统
    void StartDialogue(const std::vector<std::string>& lines, float delay = 0.f);
    void StartDialogue(const std::vector<std::string>& lines,
                       const std::vector<Portrait>& portraits, float delay = 0.f);
    bool IsInDialogue() const { return m_dialogueIndex >= 0; }
    void ShowPortrait(Portrait p) { m_currentPortrait = p; }

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
    int            m_sceneId = 0;

protected:
    void DrawDialogueBox(sf::RenderWindow& window);

    // 立绘
    std::unique_ptr<sf::Sprite> m_portraitNomal;
    std::unique_ptr<sf::Sprite> m_portraitSmile;
    std::unique_ptr<sf::Sprite> m_portraitWhattt;
    sf::Texture m_portraitTexNomal, m_portraitTexSmile, m_portraitTexWhattt;
    Portrait m_currentPortrait = Portrait::None;

    // 对话状态（子类 Draw 需要读取）
    float m_dialogueTimer = 0.f; // 延迟计时器

private:
    void TryInteract(const sf::RenderWindow& window);
    void ApplyPortraitForLine();
    bool m_mouseWasDown = false;
    bool m_firstUpdate  = true;
    float m_inputLockTimer = 0.f;  // 输入锁定时长

    std::vector<std::string> m_dialogueLines;
    std::vector<Portrait>  m_dialoguePortraits; // 每行对应立绘
    int   m_dialogueIndex = -1;
    std::vector<std::string> m_pendingLines;
    std::vector<Portrait>  m_pendingPortraits;
    float m_typewriterPos = 0.f;
    static constexpr float TYPE_SPEED = 40.f;

    // 教室门弹窗链状态（三跳：门选项 → 确认弹窗 → 结果信息 → 标题）
    enum class ClassroomResult { None, Wrong, Correct };
    bool            m_doorDialogActive   = false;
    bool            m_pendingConfirm     = false;  // 等待确认弹窗("是/否")结果
    bool            m_isCorrectClassroom = false;
    ClassroomResult m_classroomResult    = ClassroomResult::None;
};
