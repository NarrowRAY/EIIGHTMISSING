#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"
#include <string>
#include <vector>
#include <ctime>

// ============================================================
// SavePanel — 存档/读档界面（3个存档槽位）
// ============================================================
class SavePanel {
public:
    SavePanel();

    void SetFont(const sf::Font* font) { m_font = font; }

    // mode: true=存档, false=读档
    void Show(bool saveMode);
    void Hide();
    bool IsVisible() const { return m_visible; }

    bool HandleClick(sf::Vector2f pos);
    void Draw(sf::RenderWindow& window);

    // 结果：选择的槽位号（-1=未选择/取消，0/1/2=槽位，-2=读档成功需切换场景）
    int  GetResult() const { return m_result; }
    void ResetResult() { m_result = -1; }

    // 保存/读取文件
    static bool SaveExists(int slot);
    static std::string GetSlotInfo(int slot);  // 返回槽位描述文字
    static bool WriteSave(int slot, int sceneId, float px, float py);
    static bool ReadSave(int slot, int& sceneId, float& px, float& py, bool& pwd);

    // 跨场景传递读档数据
    static void SetPendingLoad(int sceneId, float px, float py, bool pwd);
    static bool HasPendingLoad();
    static int  GetPendingScene();
    static float GetPendingX();
    static float GetPendingY();
    static bool GetPendingPwd();
    static void ClearPending();

private:
    struct SlotRect {
        sf::FloatRect rect;
        int           index;
    };

    bool m_visible   = false;
    bool m_saveMode  = true;   // true=存档, false=读档
    int  m_result    = -1;
    int  m_confirmSlot = -1;   // 读档模式下选中待确认的槽位
    sf::FloatRect m_panelRect;
    sf::FloatRect m_btnLoad, m_btnDelete;  // "载入"/"删除"按钮
    const sf::Font* m_font = nullptr;
    std::vector<SlotRect> m_slots;

    static std::string SavePath(int slot);
};
