#include "PasswordUI.h"
#include "AudioManager.h"

PasswordUI::PasswordUI() {
    BuildLayout();
}

void PasswordUI::Show() {
    m_visible = true;
    m_input.clear();
    m_errorText.clear();
    m_unlocked = false;
}

void PasswordUI::Hide() {
    m_visible = false;
}

// ============================================================
// 构建按钮布局（逻辑坐标 1920×1080）
// ============================================================
void PasswordUI::BuildLayout() {
    // 键盘面板居中偏上
    const float panelW = 480.f;
    const float panelH = 560.f;
    const float panelX = (SCREEN_W - panelW) / 2.f;   // 720
    const float panelY = (SCREEN_H - panelH) / 2.f - 60.f; // 420 → 上移留空间给底部对话框

    // 按钮网格: 3列 × 4行
    const float btnW = 105.f;
    const float btnH = 65.f;
    const float gap  = 14.f;
    const float gridW = btnW * 3 + gap * 2;
    const float gridX = panelX + (panelW - gridW) / 2.f;
    const float gridY = panelY + 175.f;  // 标题+显示区下方

    // 数字排列: 1 2 3 / 4 5 6 / 7 8 9 / 清除 0 确认
    struct { int digit; const char* text; } layout[4][3] = {
        {{1,"1"}, {2,"2"}, {3,"3"}},
        {{4,"4"}, {5,"5"}, {6,"6"}},
        {{7,"7"}, {8,"8"}, {9,"9"}},
        {{-1,"清除"}, {0,"0"}, {-2,"确认"}}
    };

    m_buttons.clear();
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 3; ++col) {
            float bx = gridX + col * (btnW + gap);
            float by = gridY + row * (btnH + gap);
            m_buttons.push_back({
                sf::FloatRect({bx, by}, {btnW, btnH}),
                layout[row][col].text,
                layout[row][col].digit
            });
        }
    }
}

// ============================================================
// 点击处理
// ============================================================
bool PasswordUI::HandleClick(sf::Vector2f pos) {
    if (!m_visible) return false;

    // 关闭按钮（面板右上角）
    const float panelW = 480.f;
    const float panelH = 560.f;
    const float panelX = (SCREEN_W - panelW) / 2.f;
    const float panelY = (SCREEN_H - panelH) / 2.f - 60.f;
    sf::FloatRect closeBtn({panelX + panelW - 42.f, panelY + 8.f}, {34.f, 34.f});
    if (closeBtn.contains(pos)) {
        m_visible = false;
        return true;
    }

    for (auto& btn : m_buttons) {
        if (btn.rect.contains(pos)) {
            m_errorText.clear();

            if (btn.digit >= 0 && btn.digit <= 9) {
                // 数字键
                if (static_cast<int>(m_input.size()) < MAX_DIGITS) {
                    m_input += std::to_string(btn.digit);
                }
            }
            else if (btn.digit == -1) {
                // 清除
                m_input.clear();
            }
            else if (btn.digit == -2) {
                // 确认
                if (m_input == CORRECT_PW) {
                    m_unlocked = true;
                    m_visible = false;
                } else {
                    m_errorText = "密码错误，请重试";
                    m_input.clear();
                }
            }
            return true;
        }
    }

    return true;  // 模态下点击任意位置都消费事件
}

// ============================================================
// 绘制
// ============================================================
void PasswordUI::Draw(sf::RenderWindow& window) {
    if (!m_visible) return;

    sf::RectangleShape shape;

    // ---- 1. 全屏暗色遮罩 ----
    shape.setSize({static_cast<float>(SCREEN_W), static_cast<float>(SCREEN_H)});
    shape.setPosition({0.f, 0.f});
    shape.setFillColor(sf::Color(0, 0, 0, 160));
    window.draw(shape);

    // ---- 2. 底部对话框 ----
    const float dlgW = 1100.f;
    const float dlgH = 70.f;
    const float dlgX = (SCREEN_W - dlgW) / 2.f;
    const float dlgY = SCREEN_H - 160.f;

    shape.setSize({dlgW, dlgH});
    shape.setPosition({dlgX, dlgY});
    shape.setFillColor(sf::Color(20, 20, 30, 230));
    shape.setOutlineColor(sf::Color(180, 180, 200));
    shape.setOutlineThickness(2.f);
    window.draw(shape);

    if (m_font) {
        sf::Text dlgText(*m_font, U("通往二楼的楼梯间，但是上锁了"), 26);
        dlgText.setFillColor(sf::Color(240, 240, 240));
        sf::FloatRect tb = dlgText.getLocalBounds();
        dlgText.setPosition({
            dlgX + (dlgW - tb.size.x) / 2.f,
            dlgY + (dlgH - tb.size.y) / 2.f - tb.position.y
        });
        window.draw(dlgText);
    }

    // ---- 3. 中央键盘面板 ----
    const float panelW = 480.f;
    const float panelH = 560.f;
    const float panelX = (SCREEN_W - panelW) / 2.f;
    const float panelY = (SCREEN_H - panelH) / 2.f - 60.f;

    // 面板背景
    shape.setSize({panelW, panelH});
    shape.setPosition({panelX, panelY});
    shape.setFillColor(sf::Color(30, 30, 40, 240));
    shape.setOutlineColor(sf::Color(140, 140, 160));
    shape.setOutlineThickness(3.f);
    window.draw(shape);

    // ---- 关闭按钮（右上角 ✕） ----
    const float cx = panelX + panelW - 42.f;
    const float cy = panelY + 8.f;
    const float cs = 34.f;
    shape.setSize({cs, cs});
    shape.setPosition({cx, cy});
    shape.setFillColor(sf::Color(80, 30, 30, 220));
    shape.setOutlineColor(sf::Color(200, 80, 80));
    shape.setOutlineThickness(2.f);
    window.draw(shape);
    {
        // 两条交叉线画 ✕
        const float pad = 8.f;
        sf::RectangleShape line(sf::Vector2f(cs - pad * 2, 3.f));
        line.setFillColor(sf::Color(255, 180, 180));
        line.setOrigin({line.getSize().x / 2.f, line.getSize().y / 2.f});
        line.setPosition({cx + cs / 2.f, cy + cs / 2.f});
        line.setRotation(sf::degrees(45.f));
        window.draw(line);
        line.setRotation(sf::degrees(-45.f));
        window.draw(line);
    }

    // 标题
    if (m_font) {
        sf::Text title(*m_font, U("请输入密码"), 32);
        title.setFillColor(sf::Color(220, 220, 240));
        sf::FloatRect tb = title.getLocalBounds();
        title.setPosition({
            panelX + (panelW - tb.size.x) / 2.f,
            panelY + 30.f
        });
        window.draw(title);

        // 显示区（密码占位符）
        const float dispW = 340.f;
        const float dispH = 55.f;
        const float dispX = panelX + (panelW - dispW) / 2.f;
        const float dispY = panelY + 90.f;

        shape.setSize({dispW, dispH});
        shape.setPosition({dispX, dispY});
        shape.setFillColor(sf::Color(15, 15, 20, 255));
        shape.setOutlineColor(sf::Color(100, 100, 120));
        shape.setOutlineThickness(2.f);
        window.draw(shape);

        // 显示已输入数字（用 * 代替）
        std::string display;
        for (size_t i = 0; i < m_input.size(); ++i) display += "*";
        for (int i = static_cast<int>(m_input.size()); i < MAX_DIGITS; ++i) display += "_";
        // 数字间加空格
        std::string spaced;
        for (size_t i = 0; i < display.size(); ++i) {
            if (i > 0) spaced += "  ";
            spaced += display[i];
        }

        sf::Text dispText(*m_font, U(spaced), 30);
        dispText.setFillColor(sf::Color(180, 220, 255));
        sf::FloatRect dtb = dispText.getLocalBounds();
        dispText.setPosition(sf::Vector2f(
            dispX + (dispW - dtb.size.x) / 2.f,
            dispY + (dispH - dtb.size.y) / 2.f - dtb.position.y
        ));
        window.draw(dispText);

        // 错误提示
        if (!m_errorText.empty()) {
            sf::Text errText(*m_font, U(m_errorText), 22);
            errText.setFillColor(sf::Color(255, 100, 100));
            sf::FloatRect etb = errText.getLocalBounds();
            errText.setPosition({
                panelX + (panelW - etb.size.x) / 2.f,
                panelY + panelH - 38.f
            });
            window.draw(errText);
        }

        // 按钮
        for (auto& btn : m_buttons) {
            // 按钮背景
            shape.setSize({btn.rect.size.x, btn.rect.size.y});
            shape.setPosition(btn.rect.position);
            if (btn.digit == -2)
                shape.setFillColor(sf::Color(50, 100, 50, 255));   // 确认-绿色
            else if (btn.digit == -1)
                shape.setFillColor(sf::Color(100, 50, 50, 255));   // 清除-红色
            else
                shape.setFillColor(sf::Color(55, 55, 70, 255));    // 数字-深灰
            shape.setOutlineColor(sf::Color(150, 150, 170));
            shape.setOutlineThickness(2.f);
            window.draw(shape);

            // 按钮文字
            sf::Text btnText(*m_font, U(btn.text), 28);
            btnText.setFillColor(sf::Color(230, 230, 240));
            sf::FloatRect btb = btnText.getLocalBounds();
            btnText.setPosition({
                btn.rect.position.x + (btn.rect.size.x - btb.size.x) / 2.f,
                btn.rect.position.y + (btn.rect.size.y - btb.size.y) / 2.f - btb.position.y
            });
            window.draw(btnText);
        }
    }
}
