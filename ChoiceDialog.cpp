#include "ChoiceDialog.h"

// 砍角八边形辅助
namespace {
    void DrawChamfered(sf::RenderWindow& w, float x, float y, float wd, float ht,
                       float corner, sf::Color fill, sf::Color outline, float thick = 2.f) {
        sf::ConvexShape shape(8);
        shape.setPoint(0, {corner, 0.f});
        shape.setPoint(1, {wd - corner, 0.f});
        shape.setPoint(2, {wd, corner});
        shape.setPoint(3, {wd, ht - corner});
        shape.setPoint(4, {wd - corner, ht});
        shape.setPoint(5, {corner, ht});
        shape.setPoint(6, {0.f, ht - corner});
        shape.setPoint(7, {0.f, corner});
        shape.setPosition({x, y});
        shape.setFillColor(fill);
        shape.setOutlineColor(outline);
        shape.setOutlineThickness(thick);
        w.draw(shape);
    }

    void DrawCloseBtn(sf::RenderWindow& w, float cx, float cy, float cr,
                      sf::Color bg, sf::Color xColor) {
        sf::CircleShape bgCircle(cr);
        bgCircle.setOrigin({cr, cr});
        bgCircle.setPosition({cx, cy});
        bgCircle.setFillColor(bg);
        w.draw(bgCircle);
        const float armLen = cr * 0.55f, armW = 2.5f;
        for (int i = 0; i < 2; ++i) {
            sf::RectangleShape arm({armLen * 2.f, armW});
            arm.setOrigin({armLen, armW / 2.f});
            arm.setPosition({cx, cy});
            arm.setFillColor(xColor);
            arm.setRotation(sf::degrees(i == 0 ? 45.f : -45.f));
            w.draw(arm);
        }
    }
}

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
// 构建按钮布局
// ============================================================
void ChoiceDialog::BuildLayout(int optionCount) {
    if (optionCount <= 0) optionCount = 1;
    if (optionCount > 6)  optionCount = 6;

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
            "",
            i
        });
    }
}

// ============================================================
// 点击处理
// ============================================================
bool ChoiceDialog::HandleClick(sf::Vector2f pos) {
    if (!m_visible) return false;

    // 关闭按钮（圆形，与 Draw 保持一致）
    float ccx, ccy, ccr = 22.f;
    if (m_infoMode) {
        const float pw = 500.f, ph = 620.f;
        ccx = (SCREEN_W - pw) / 2.f + pw - 42.f;
        ccy = (SCREEN_H - ph) / 2.f + 20.f;
    } else if (m_wideMode) {
        const float pw = 800.f, ph = 340.f;
        ccx = (SCREEN_W - pw) / 2.f + pw - 42.f;
        ccy = (SCREEN_H - ph) / 2.f + 20.f;
    } else if (!m_buttons.empty()) {
        float px = m_buttons[0].rect.position.x - 40.f;
        float py = m_buttons[0].rect.position.y - 45.f;
        float pw = m_buttons[0].rect.size.x + 80.f;
        ccx = px + pw - 42.f;
        ccy = py + 20.f;
    } else { ccx = 0; ccy = 0; }
    float dist = std::sqrt((pos.x - ccx) * (pos.x - ccx) + (pos.y - ccy) * (pos.y - ccy));
    if (dist <= ccr) {
        m_visible = false;
        return true;
    }

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

    const sf::Color creamFill(250, 245, 235, 250);
    const sf::Color warmOutline(180, 170, 155, 220);
    const sf::Color textDark(80, 70, 55);
    const sf::Color btnFill(255, 252, 245, 255);
    const sf::Color btnOutline(160, 145, 120, 200);
    const sf::Color closeBg(80, 70, 60, 180);

    // ============================================================
    // 信息展示模式：竖长方面板
    // ============================================================
    if (m_infoMode) {
        const float pw = 500.f, ph = 620.f;
        const float px = (SCREEN_W - pw) / 2.f;
        const float py = (SCREEN_H - ph) / 2.f;

        DrawChamfered(window, px, py, pw, ph, 20.f, creamFill, warmOutline, 3.f);
        DrawCloseBtn(window, px + pw - 42.f, py + 20.f, 18.f, closeBg, sf::Color(245, 240, 230));

        if (m_font && !m_titleText.empty()) {
            auto& raw = m_titleText;
            size_t pos = raw.find('\n');
            std::string titleLine = (pos != std::string::npos) ? raw.substr(0, pos) : raw;
            std::string bodyText  = (pos != std::string::npos) ? raw.substr(pos + 1) : "";

            if (!titleLine.empty()) {
                sf::Text title(*m_font, U(titleLine), 30);
                title.setFillColor(textDark);
                sf::FloatRect ttl = title.getLocalBounds();
                title.setPosition({px + (pw - ttl.size.x) / 2.f, py + 40.f});
                window.draw(title);
            }
            if (!bodyText.empty()) {
                sf::Text body(*m_font, U(bodyText), 24);
                body.setFillColor(sf::Color(100, 90, 70));
                sf::FloatRect bb = body.getLocalBounds();
                body.setPosition({px + (pw - bb.size.x) / 2.f, py + (titleLine.empty() ? 60.f : 110.f)});
                window.draw(body);
            }
        }
        return;
    }

    // ============================================================
    // 横长方面板模式
    // ============================================================
    if (m_wideMode) {
        const float pw = 800.f, ph = 340.f;
        const float px = (SCREEN_W - pw) / 2.f;
        const float py = (SCREEN_H - ph) / 2.f;

        DrawChamfered(window, px, py, pw, ph, 20.f, creamFill, warmOutline, 3.f);

        DrawCloseBtn(window, px + pw - 42.f, py + 20.f, 18.f, closeBg, sf::Color(245, 240, 230));

        if (m_font && !m_titleText.empty()) {
            sf::Text text(*m_font, U(m_titleText), 30);
            text.setFillColor(textDark);
            sf::FloatRect tb = text.getLocalBounds();
            text.setPosition(sf::Vector2f(
                px + (pw - tb.size.x) / 2.f, py + 30.f));
            window.draw(text);
        }
        return;
    }

    // ============================================================
    // 选项模式
    // ============================================================

    // ---- 底部对话框(仅在有文字时显示) ----
    if (m_font && !m_dialogText.empty()) {
        const float dlgW = 1100.f;
        const float dlgH = 70.f;
        const float dlgX = (SCREEN_W - dlgW) / 2.f;
        const float dlgY = SCREEN_H - 160.f;

        DrawChamfered(window, dlgX, dlgY, dlgW, dlgH, 14.f,
            sf::Color(250, 245, 235, 230), sf::Color(180, 170, 155, 200), 2.f);

        sf::Text dlgText(*m_font, U(m_dialogText), 26);
        dlgText.setFillColor(textDark);
        sf::FloatRect tb = dlgText.getLocalBounds();
        dlgText.setPosition(sf::Vector2f(
            dlgX + (dlgW - tb.size.x) / 2.f,
            dlgY + (dlgH - tb.size.y) / 2.f - tb.position.y
        ));
        window.draw(dlgText);
    }

    // ---- 选项按钮面板 ----
    if (m_buttons.empty()) return;

    float panelX = m_buttons[0].rect.position.x - 40.f;
    float panelY = m_buttons[0].rect.position.y - 45.f;
    float panelW = m_buttons[0].rect.size.x + 80.f;
    float lastBottom = m_buttons.back().rect.position.y + m_buttons.back().rect.size.y;
    float panelH = lastBottom - panelY + 45.f;

    DrawChamfered(window, panelX, panelY, panelW, panelH, 20.f, creamFill, warmOutline, 3.f);

    DrawCloseBtn(window, panelX + panelW - 42.f, panelY + 20.f, 18.f, closeBg, sf::Color(245, 240, 230));

    // 按钮
    if (m_font) {
        for (auto& btn : m_buttons) {
            DrawChamfered(window, btn.rect.position.x, btn.rect.position.y,
                btn.rect.size.x, btn.rect.size.y, 14.f,
                btnFill, btnOutline, 2.f);

            sf::Text btnText(*m_font, U(btn.text), 26);
            btnText.setFillColor(textDark);
            sf::FloatRect btb = btnText.getLocalBounds();
            btnText.setPosition(sf::Vector2f(
                btn.rect.position.x + (btn.rect.size.x - btb.size.x) / 2.f,
                btn.rect.position.y + (btn.rect.size.y - btb.size.y) / 2.f - btb.position.y
            ));
            window.draw(btnText);
        }
    }
}
