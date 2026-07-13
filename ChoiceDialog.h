#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"
#include <string>
#include <vector>

// ============================================================
// ChoiceDialog — 通用选项弹窗（暗色遮罩 + 底部对话框 + 居中选项按钮）
// ============================================================
class ChoiceDialog {
public:
    ChoiceDialog() = default;

    void SetFont(const sf::Font* font) { m_font = font; }

    // dialogText: 底部对话框文字; options: 按钮文字列表
    void Show(const std::string& dialogText, const std::vector<std::string>& options);
    // 信息展示模式（竖长方面板，无选项按钮）
    void ShowInfo(const std::string& title);
    // 信息展示模式（横长方面板）
    void ShowInfoWide(const std::string& title);
    void Hide();
    bool IsVisible() const { return m_visible; }

    // 处理屏幕坐标下的鼠标点击，返回 true 表示已消费
    bool HandleClick(sf::Vector2f pos);

    // 绘制（调用前需设置好 UI 视图）
    void Draw(sf::RenderWindow& window);

    // 玩家做出了选择（-1=未选择，>=0=按钮序号）
    int  GetChoice() const { return m_choice; }
    void ResetChoice() { m_choice = -1; }

private:
    struct Btn {
        sf::FloatRect rect;
        std::string   text;
        int           index;
    };

    void BuildLayout(int optionCount);

    bool        m_visible  = false;
    bool        m_infoMode = false;  // 竖长方面板
    bool        m_wideMode = false;  // 横长方面板
    int         m_choice   = -1;
    std::string m_dialogText;
    std::string m_titleText;
    const sf::Font* m_font = nullptr;
    std::vector<Btn> m_buttons;
};
