#include "PasswordUI.h"
#include "AudioManager.h"

namespace {
    void DrawChamfered(sf::RenderWindow& w, float x, float y, float wd, float ht,
                       float corner, sf::Color fill, sf::Color outline, float thick = 2.f) {
        sf::ConvexShape s(8);
        s.setPoint(0, {corner, 0.f}); s.setPoint(1, {wd - corner, 0.f});
        s.setPoint(2, {wd, corner});  s.setPoint(3, {wd, ht - corner});
        s.setPoint(4, {wd - corner, ht}); s.setPoint(5, {corner, ht});
        s.setPoint(6, {0.f, ht - corner}); s.setPoint(7, {0.f, corner});
        s.setPosition({x, y});
        s.setFillColor(fill);
        s.setOutlineColor(outline);
        s.setOutlineThickness(thick);
        w.draw(s);
    }
    void DrawCloseBtn(sf::RenderWindow& w, float cx, float cy, float cr,
                      sf::Color bg, sf::Color xc) {
        sf::CircleShape c(cr);
        c.setOrigin({cr, cr}); c.setPosition({cx, cy}); c.setFillColor(bg);
        w.draw(c);
        const float al = cr * 0.55f, aw = 2.5f;
        for (int i = 0; i < 2; ++i) {
            sf::RectangleShape arm({al * 2.f, aw});
            arm.setOrigin({al, aw / 2.f}); arm.setPosition({cx, cy});
            arm.setFillColor(xc);
            arm.setRotation(sf::degrees(i == 0 ? 45.f : -45.f));
            w.draw(arm);
        }
    }
}

PasswordUI::PasswordUI() { BuildLayout(); }

void PasswordUI::Show() {
    m_visible = true; m_input.clear(); m_errorText.clear(); m_unlocked = false;
}
void PasswordUI::Hide() { m_visible = false; }

void PasswordUI::BuildLayout() {
    const float panelW = 540.f;
    const float panelH = 620.f;
    const float panelX = (SCREEN_W - panelW) / 2.f;
    const float panelY = (SCREEN_H - panelH) / 2.f - 50.f;

    // 按钮: 3列×4行，加大点击面积
    const float btnW = 130.f, btnH = 80.f, gap = 18.f;
    const float gridW = btnW * 3 + gap * 2;
    const float gridX = panelX + (panelW - gridW) / 2.f;
    const float gridY = panelY + 190.f;

    struct { int digit; const char* text; } layout[4][3] = {
        {{1,"1"}, {2,"2"}, {3,"3"}},
        {{4,"4"}, {5,"5"}, {6,"6"}},
        {{7,"7"}, {8,"8"}, {9,"9"}},
        {{-1,"清除"}, {0,"0"}, {-2,"确认"}}
    };

    m_buttons.clear();
    for (int row = 0; row < 4; ++row)
        for (int col = 0; col < 3; ++col)
            m_buttons.push_back({
                sf::FloatRect({gridX + col * (btnW + gap), gridY + row * (btnH + gap)}, {btnW, btnH}),
                layout[row][col].text, layout[row][col].digit
            });
}

bool PasswordUI::HandleClick(sf::Vector2f pos) {
    if (!m_visible) return false;

    const float panelW = 540.f, panelH = 620.f;
    const float panelX = (SCREEN_W - panelW) / 2.f;
    const float panelY = (SCREEN_H - panelH) / 2.f - 50.f;
    const float ccx = panelX + panelW - 48.f, ccy = panelY + 24.f, ccr = 22.f;
    float d = std::sqrt((pos.x - ccx) * (pos.x - ccx) + (pos.y - ccy) * (pos.y - ccy));
    if (d <= ccr) { m_visible = false; return true; }

    for (auto& btn : m_buttons) {
        // 扩大点击区域：矩形外扩 8px
        sf::FloatRect big({btn.rect.position.x - 8.f, btn.rect.position.y - 8.f},
                          {btn.rect.size.x + 16.f, btn.rect.size.y + 16.f});
        if (big.contains(pos)) {
            m_errorText.clear();
            if (btn.digit >= 0 && btn.digit <= 9) {
                if (static_cast<int>(m_input.size()) < MAX_DIGITS)
                    m_input += std::to_string(btn.digit);
            } else if (btn.digit == -1) {
                m_input.clear();
            } else if (btn.digit == -2) {
                if (m_input == CORRECT_PW) {
                    m_unlocked = true; m_visible = false;
                } else {
                    m_errorText = "密码错误，请重试"; m_input.clear();
                }
            }
            return true;
        }
    }
    return true;
}

void PasswordUI::Draw(sf::RenderWindow& window) {
    if (!m_visible) return;

    sf::RectangleShape shape;
    const sf::Color cream(250,245,235,250), warmOut(180,170,155,220);
    const sf::Color dark(80,70,55), btnFill(255,252,245,255);
    const sf::Color btnOut(160,145,120,200), closeBg(80,70,60,180);

    shape.setSize({SCREEN_W, SCREEN_H});
    shape.setPosition({0,0});
    shape.setFillColor(sf::Color(0,0,0,160));
    window.draw(shape);

    // 底部对话框（砍角）
    const float dlgW = 1100.f, dlgH = 70.f;
    const float dlgX = (SCREEN_W - dlgW) / 2.f, dlgY = SCREEN_H - 160.f;
    DrawChamfered(window, dlgX, dlgY, dlgW, dlgH, 14.f,
        sf::Color(250,245,235,230), sf::Color(180,170,155,200), 2.f);
    if (m_font) {
        sf::Text t(*m_font, U("通往二楼的楼梯间，但是上锁了"), 26);
        t.setFillColor(dark);
        sf::FloatRect tb = t.getLocalBounds();
        t.setPosition({dlgX + (dlgW - tb.size.x) / 2.f, dlgY + (dlgH - tb.size.y) / 2.f - tb.position.y});
        window.draw(t);
    }

    // 中央键盘面板（砍角）
    const float panelW = 540.f, panelH = 620.f;
    const float panelX = (SCREEN_W - panelW) / 2.f, panelY = (SCREEN_H - panelH) / 2.f - 50.f;
    DrawChamfered(window, panelX, panelY, panelW, panelH, 22.f, cream, warmOut, 3.f);

    DrawCloseBtn(window, panelX + panelW - 48.f, panelY + 24.f, 18.f, closeBg, sf::Color(245,240,230));

    if (!m_font) return;

    // 标题
    sf::Text title(*m_font, U("请输入密码"), 32);
    title.setFillColor(dark);
    sf::FloatRect tb = title.getLocalBounds();
    title.setPosition({panelX + (panelW - tb.size.x) / 2.f, panelY + 30.f});
    window.draw(title);

    // 显示区
    const float dispW = 360.f, dispH = 60.f;
    const float dispX = panelX + (panelW - dispW) / 2.f, dispY = panelY + 100.f;
    DrawChamfered(window, dispX, dispY, dispW, dispH, 10.f,
        sf::Color(235,230,220,255), sf::Color(170,160,145,200), 1.5f);

    std::string display;
    for (size_t i = 0; i < m_input.size(); ++i) display += "*";
    for (int i = static_cast<int>(m_input.size()); i < MAX_DIGITS; ++i) display += "_";
    std::string spaced;
    for (size_t i = 0; i < display.size(); ++i) { if (i > 0) spaced += "  "; spaced += display[i]; }

    sf::Text dispText(*m_font, U(spaced), 32);
    dispText.setFillColor(sf::Color(60,50,40));
    sf::FloatRect dtb = dispText.getLocalBounds();
    dispText.setPosition({dispX + (dispW - dtb.size.x) / 2.f, dispY + (dispH - dtb.size.y) / 2.f - dtb.position.y});
    window.draw(dispText);

    // 错误提示
    if (!m_errorText.empty()) {
        sf::Text err(*m_font, U(m_errorText), 20);
        err.setFillColor(sf::Color(200,60,50));
        sf::FloatRect eb = err.getLocalBounds();
        err.setPosition({panelX + (panelW - eb.size.x) / 2.f, panelY + panelH - 42.f});
        window.draw(err);
    }

    // 按钮（砍角）
    for (auto& btn : m_buttons) {
        sf::Color fill, outline;
        if (btn.digit == -2)      { fill = {180,210,160,255}; outline = {120,150,100,200}; }
        else if (btn.digit == -1) { fill = {220,180,170,255}; outline = {180,120,110,200}; }
        else                      { fill = btnFill;           outline = btnOut; }

        DrawChamfered(window, btn.rect.position.x, btn.rect.position.y,
            btn.rect.size.x, btn.rect.size.y, 14.f, fill, outline, 2.f);

        sf::Text bt(*m_font, U(btn.text), 28);
        bt.setFillColor(dark);
        sf::FloatRect bb = bt.getLocalBounds();
        bt.setPosition({btn.rect.position.x + (btn.rect.size.x - bb.size.x) / 2.f,
                        btn.rect.position.y + (btn.rect.size.y - bb.size.y) / 2.f - bb.position.y});
        window.draw(bt);
    }
}
