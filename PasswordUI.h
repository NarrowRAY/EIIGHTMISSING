#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"
#include <string>
#include <vector>

// ============================================================
// PasswordUI — 密码锁弹窗（暗色遮罩 + 底部对话框 + 中央键盘）
// 所有绘制使用逻辑屏幕坐标 (1920×1080)
// ============================================================
class PasswordUI {
public:
    PasswordUI();

    void SetFont(const sf::Font* font) { m_font = font; }

    void Show();
    void Hide();
    bool IsVisible() const { return m_visible; }

    // 处理屏幕坐标下的鼠标点击，返回 true 表示已消费
    bool HandleClick(sf::Vector2f pos);

    // 绘制遮罩、对话框、键盘（调用前需设置好 UI 视图）
    void Draw(sf::RenderWindow& window);

    // 密码正确解锁
    bool IsUnlocked() const { return m_unlocked; }
    void ResetUnlocked() { m_unlocked = false; }

private:
    struct Btn {
        sf::FloatRect rect;
        std::string   text;
        int           digit;  // 0~9=数字, -1=清除, -2=确认
    };

    void BuildLayout();

    bool        m_visible   = false;
    bool        m_unlocked  = false;
    std::string m_input;          // 已输入的数字串
    std::string m_errorText;      // 错误提示
    const sf::Font* m_font = nullptr;
    std::vector<Btn> m_buttons;

    static constexpr const char* CORRECT_PW = "1024";
    static constexpr int MAX_DIGITS = 4;
};
