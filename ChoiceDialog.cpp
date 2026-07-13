#include "ChoiceDialog.h"

void ChoiceDialog::Show(const std::string& dialogText, const std::vector<std::string>& options) {
    m_visible    = true;
    m_infoMode   = false;
    m_wideMode   = false;
    m_choice     = -1;
    m_dialogText = dialogText;
    BuildLayout(static_cast<int>(options.size()));
    for (size_t i = 0; i < options.size() && i < m_buttons.size(); ++i) {
        m_buttons[i].text = options[i];
    }
}

void ChoiceDialog::ShowInfo(const std::string& title) {
    m_visible    = true;
    m_infoMode   = true;
    m_wideMode   = false;
    m_choice     = -1;
    m_titleText  = title;
    m_dialogText.clear();
    m_buttons.clear();
}

void ChoiceDialog::ShowInfoWide(const std::string& title) {
    m_visible    = true;
    m_infoMode   = false;
    m_wideMode   = true;
    m_choice     = -1;
    m_titleText  = title;
    m_dialogText.clear();
    m_buttons.clear();
}

void ChoiceDialog::Hide() {
    m_visible = false;
}

// ============================================================
// 构建按钮布局（逻辑坐标 1920×1080）
// ============================================================
void ChoiceDialog::BuildLayout(int optionCount) {
    if (optionCount <= 0) optionCount = 1;
    if (optionCount > 6)  optionCount = 6;  // 最多6个

    // 面板居中
    const float btnW = 420.f;
    const float btnH = 60.f;
    const float gap  = 16.f;
    const float panelPad = 40.f;
    const float panelW = btnW + panelPad * 2;
    const float totalBtnH = optionCount * btnH + (optionCount - 1) * gap;
    const float panelH = totalBtnH + panelPad * 2 + 10.f;

    const float panelX = (SCREEN_W - panelW) / 2.f;
    const float panelY = (SCREEN_H - panelH) / 2.f - 60.f;

    m_buttons.clear();
    float startY = panelY + panelPad + 5.f;
    float startX = panelX + panelPad;

    for (int i = 0; i < optionCount; ++i) {
        float by = startY + i * (btnH + gap);
        m_buttons.push_back({
            sf::FloatRect({startX, by}, {btnW, btnH}),
            "",  // text set later
            i
        });
    }
}

// ============================================================
// 点击处理
// ============================================================
bool ChoiceDialog::HandleClick(sf::Vector2f pos) {
    if (!m_visible) return false;

    // 关闭按钮（面板右上角）
    sf::FloatRect closeBtn;
    if (m_infoMode) {
        const float pw = 400.f, ph = 620.f;
        const float px = (SCREEN_W - pw) / 2.f;
        const float py = (SCREEN_H - ph) / 2.f;
        closeBtn = sf::FloatRect({px + pw - 42.f, py + 8.f}, {34.f, 34.f});
    } else if (m_wideMode) {
        const float pw = 800.f, ph = 340.f;
        const float px = (SCREEN_W - pw) / 2.f;
        const float py = (SCREEN_H - ph) / 2.f;
        closeBtn = sf::FloatRect({px + pw - 42.f, py + 8.f}, {34.f, 34.f});
    } else if (!m_buttons.empty()) {
        float px = m_buttons[0].rect.position.x - 40.f;
        float py = m_buttons[0].rect.position.y - 45.f;
        float pw = m_buttons[0].rect.size.x + 80.f;
        closeBtn = sf::FloatRect({px + pw - 42.f, py + 8.f}, {34.f, 34.f});
    }
    if (closeBtn.contains(pos)) {
        m_visible = false;
        return true;
    }

    // info/wide 模式：只有关闭按钮
    if (m_infoMode || m_wideMode) return true;

    for (auto& btn : m_buttons) {
        if (btn.rect.contains(pos)) {
            m_choice  = btn.index;
            m_visible = false;
            return true;
        }
    }
    return true;
}

// ============================================================
// 绘制
// ============================================================
void ChoiceDialog::Draw(sf::RenderWindow& window) {
    if (!m_visible) return;

    sf::RectangleShape shape;

    // ---- 1. 全屏暗色遮罩 ----
    shape.setSize({static_cast<float>(SCREEN_W), static_cast<float>(SCREEN_H)});
    shape.setPosition({0.f, 0.f});
    shape.setFillColor(sf::Color(0, 0, 0, 160));
    window.draw(shape);

    // ============================================================
    // 信息展示模式：竖长方面板
    // ============================================================
    if (m_infoMode) {
        const float pw = 400.f;
        const float ph = 620.f;
        const float px = (SCREEN_W - pw) / 2.f;
        const float py = (SCREEN_H - ph) / 2.f;

        // 面板背景
        shape.setSize({pw, ph});
        shape.setPosition({px, py});
        shape.setFillColor(sf::Color(30, 30, 40, 240));
        shape.setOutlineColor(sf::Color(140, 140, 160));
        shape.setOutlineThickness(3.f);
        window.draw(shape);

        // 关闭按钮
        const float cx = px + pw - 42.f;
        const float cy = py + 8.f;
        const float cs = 34.f;
        shape.setSize({cs, cs});
        shape.setPosition({cx, cy});
        shape.setFillColor(sf::Color(80, 30, 30, 220));
        shape.setOutlineColor(sf::Color(200, 80, 80));
        shape.setOutlineThickness(2.f);
        window.draw(shape);
        {
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

        // 标题文字
        if (m_font && !m_titleText.empty()) {
            sf::Text text(*m_font, U(m_titleText), 30);
            text.setFillColor(sf::Color(220, 220, 240));
            sf::FloatRect tb = text.getLocalBounds();
            text.setPosition(sf::Vector2f(
                px + (pw - tb.size.x) / 2.f,
                py + 60.f
            ));
            window.draw(text);

            // 待贴图提示
            sf::Text hint(*m_font, U("（待贴图）"), 22);
            hint.setFillColor(sf::Color(150, 150, 180));
            sf::FloatRect hb = hint.getLocalBounds();
            hint.setPosition(sf::Vector2f(
                px + (pw - hb.size.x) / 2.f,
                py + ph / 2.f
            ));
            window.draw(hint);
        }

        return;
    }

    // ============================================================
    // 横长方面板模式
    // ============================================================
    if (m_wideMode) {
        const float pw = 800.f;
        const float ph = 340.f;
        const float px = (SCREEN_W - pw) / 2.f;
        const float py = (SCREEN_H - ph) / 2.f;

        // 面板背景
        shape.setSize({pw, ph});
        shape.setPosition({px, py});
        shape.setFillColor(sf::Color(30, 30, 40, 240));
        shape.setOutlineColor(sf::Color(140, 140, 160));
        shape.setOutlineThickness(3.f);
        window.draw(shape);

        // 关闭按钮
        const float cx = px + pw - 42.f;
        const float cy = py + 8.f;
        const float cs = 34.f;
        shape.setSize({cs, cs});
        shape.setPosition({cx, cy});
        shape.setFillColor(sf::Color(80, 30, 30, 220));
        shape.setOutlineColor(sf::Color(200, 80, 80));
        shape.setOutlineThickness(2.f);
        window.draw(shape);
        {
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

        // 标题文字
        if (m_font && !m_titleText.empty()) {
            sf::Text text(*m_font, U(m_titleText), 30);
            text.setFillColor(sf::Color(220, 220, 240));
            sf::FloatRect tb = text.getLocalBounds();
            text.setPosition(sf::Vector2f(
                px + (pw - tb.size.x) / 2.f,
                py + 30.f
            ));
            window.draw(text);

            // 待贴图提示
            sf::Text hint(*m_font, U("（待贴图）"), 22);
            hint.setFillColor(sf::Color(150, 150, 180));
            sf::FloatRect hb = hint.getLocalBounds();
            hint.setPosition(sf::Vector2f(
                px + (pw - hb.size.x) / 2.f,
                py + ph / 2.f
            ));
            window.draw(hint);
        }

        return;
    }

    // ============================================================
    // 选项模式（原有逻辑）
    // ============================================================

    // ---- 2. 底部对话框(仅在有文字时显示) ----
    if (m_font && !m_dialogText.empty()) {
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

        sf::Text dlgText(*m_font, U(m_dialogText), 26);
        dlgText.setFillColor(sf::Color(240, 240, 240));
        sf::FloatRect tb = dlgText.getLocalBounds();
        dlgText.setPosition(sf::Vector2f(
            dlgX + (dlgW - tb.size.x) / 2.f,
            dlgY + (dlgH - tb.size.y) / 2.f - tb.position.y
        ));
        window.draw(dlgText);
    }

    // ---- 3. 选项按钮面板 ----
    if (m_buttons.empty()) return;

    float panelX = m_buttons[0].rect.position.x - 40.f;
    float panelY = m_buttons[0].rect.position.y - 45.f;
    float panelW = m_buttons[0].rect.size.x + 80.f;
    float lastBottom = m_buttons.back().rect.position.y + m_buttons.back().rect.size.y;
    float panelH = lastBottom - panelY + 45.f;

    shape.setSize({panelW, panelH});
    shape.setPosition({panelX, panelY});
    shape.setFillColor(sf::Color(30, 30, 40, 240));
    shape.setOutlineColor(sf::Color(140, 140, 160));
    shape.setOutlineThickness(3.f);
    window.draw(shape);

    // 关闭按钮
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

    // 按钮
    if (m_font) {
        for (auto& btn : m_buttons) {
            shape.setSize({btn.rect.size.x, btn.rect.size.y});
            shape.setPosition(btn.rect.position);
            shape.setFillColor(sf::Color(55, 55, 70, 255));
            shape.setOutlineColor(sf::Color(150, 150, 170));
            shape.setOutlineThickness(2.f);
            window.draw(shape);

            sf::Text btnText(*m_font, U(btn.text), 26);
            btnText.setFillColor(sf::Color(230, 230, 240));
            sf::FloatRect btb = btnText.getLocalBounds();
            btnText.setPosition(sf::Vector2f(
                btn.rect.position.x + (btn.rect.size.x - btb.size.x) / 2.f,
                btn.rect.position.y + (btn.rect.size.y - btb.size.y) / 2.f - btb.position.y
            ));
            window.draw(btnText);
        }
    }
}
