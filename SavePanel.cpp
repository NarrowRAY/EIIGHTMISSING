#include "SavePanel.h"
#include "GameScene.h"
#include <fstream>
#include <sstream>
#include <cstdio>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(p) _mkdir(p)
#else
#include <unistd.h>
#define MKDIR(p) mkdir(p, 0755)
#endif

namespace {
    // 砍角八边形
    void DrawChamferedRect(sf::RenderWindow& w, float x, float y, float wd, float ht,
                           float corner, sf::Color fill, sf::Color outline, float outlineThick = 2.f) {
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
        shape.setOutlineThickness(outlineThick);
        w.draw(shape);
    }
}

SavePanel::SavePanel() {
    // 槽位布局（逻辑坐标 1920×1080）
    const float panelW = 680.f;
    const float panelH = 580.f;
    const float panelX = (SCREEN_W - panelW) / 2.f;
    const float panelY = (SCREEN_H - panelH) / 2.f;
    const float slotW = 560.f;
    const float slotH = 120.f;
    const float gap   = 28.f;
    const float slotX = panelX + (panelW - slotW) / 2.f;

    for (int i = 0; i < 3; ++i) {
        float sy = panelY + 90.f + i * (slotH + gap);
        m_slots.push_back({
            sf::FloatRect({slotX, sy}, {slotW, slotH}),
            i
        });
    }

    m_panelRect = sf::FloatRect({panelX, panelY}, {panelW, panelH});
}

void SavePanel::Show(bool saveMode) {
    m_visible     = true;
    m_saveMode    = saveMode;
    m_result      = -1;
    m_confirmSlot = -1;
    MKDIR("saves");
}

void SavePanel::Hide() {
    m_visible = false;
}

// ============================================================
// 点击处理
// ============================================================
bool SavePanel::HandleClick(sf::Vector2f pos) {
    if (!m_visible) return false;

    // 关闭按钮（圆形）
    const float panelW = m_panelRect.size.x;
    const float panelH = m_panelRect.size.y;
    const float panelX = m_panelRect.position.x;
    const float panelY = m_panelRect.position.y;
    const float ccx = panelX + panelW - 48.f, ccy = panelY + 24.f, ccr = 22.f;
    float dist = std::sqrt((pos.x - ccx) * (pos.x - ccx) + (pos.y - ccy) * (pos.y - ccy));
    if (dist <= ccr) {
        if (m_confirmSlot >= 0) {
            m_confirmSlot = -1;  // 返回槽位列表
        } else {
            m_visible = false;
        }
        return true;
    }

    // ———— 确认弹窗 ————
    if (m_confirmSlot >= 0) {
        if (m_saveMode) {
            // 存档覆盖确认："确认"→写入 / "取消"→返回
            if (m_btnLoad.contains(pos)) {
                m_result = m_confirmSlot;
                m_visible = false;
                m_confirmSlot = -1;
                return true;
            }
            if (m_btnDelete.contains(pos)) {
                m_confirmSlot = -1;
                return true;
            }
        } else {
            // 读档确认："载入" / "删除"
            if (m_btnLoad.contains(pos)) {
                m_result = m_confirmSlot;
                m_visible = false;
                m_confirmSlot = -1;
                return true;
            }
            if (m_btnDelete.contains(pos)) {
                std::remove(SavePath(m_confirmSlot).c_str());
                m_confirmSlot = -1;
                return true;
            }
        }
        // 点击空白处返回
        if (!m_panelRect.contains(pos)) {
            m_confirmSlot = -1;
        }
        return true;
    }

    // ———— 槽位列表 ————
    for (auto& slot : m_slots) {
        if (slot.rect.contains(pos)) {
            if (m_saveMode) {
                // 存档模式：空槽位直接存，已占用弹确认窗
                if (SaveExists(slot.index)) {
                    m_confirmSlot = slot.index;
                } else {
                    m_result = slot.index;
                    m_visible = false;
                }
            } else {
                // 读档模式：先弹出确认窗
                if (SaveExists(slot.index)) {
                    m_confirmSlot = slot.index;
                }
            }
            return true;
        }
    }
    return true;
}

// ============================================================
// 绘制
// ============================================================
void SavePanel::Draw(sf::RenderWindow& window) {
    if (!m_visible) return;

    sf::RectangleShape shape;
    const float panelW = m_panelRect.size.x;
    const float panelH = m_panelRect.size.y;
    const float panelX = m_panelRect.position.x;
    const float panelY = m_panelRect.position.y;
    const float corner = 22.f;

    // 暗色遮罩
    shape.setSize({static_cast<float>(SCREEN_W), static_cast<float>(SCREEN_H)});
    shape.setPosition({0.f, 0.f});
    shape.setFillColor(sf::Color(0, 0, 0, 160));
    window.draw(shape);

    // ———— 面板背景（乳白色砍角八边形） ————
    DrawChamferedRect(window, panelX, panelY, panelW, panelH, corner,
        sf::Color(250, 245, 235, 250),   // 乳白填充
        sf::Color(180, 170, 155, 220),   // 暖灰边框
        3.f);

    // 顶部标题栏装饰线
    sf::RectangleShape divider({panelW - 80.f, 1.f});
    divider.setPosition({panelX + 40.f, panelY + 52.f});
    divider.setFillColor(sf::Color(180, 170, 155, 100));
    window.draw(divider);

    // ———— 关闭按钮（圆形暗底 + 白色X） ————
    const float ccx = panelX + panelW - 48.f, ccy = panelY + 24.f, ccr = 18.f;
    sf::CircleShape closeBg(ccr);
    closeBg.setOrigin({ccr, ccr});
    closeBg.setPosition({ccx, ccy});
    closeBg.setFillColor(sf::Color(80, 70, 60, 180));
    window.draw(closeBg);
    {
        const float armLen = 10.f, armW = 2.5f;
        sf::RectangleShape arm1({armLen * 2.f, armW});
        arm1.setOrigin({armLen, armW / 2.f});
        arm1.setPosition({ccx, ccy});
        arm1.setFillColor(sf::Color(245, 240, 230));
        arm1.setRotation(sf::degrees(45.f));
        window.draw(arm1);
        sf::RectangleShape arm2({armLen * 2.f, armW});
        arm2.setOrigin({armLen, armW / 2.f});
        arm2.setPosition({ccx, ccy});
        arm2.setFillColor(sf::Color(245, 240, 230));
        arm2.setRotation(sf::degrees(-45.f));
        window.draw(arm2);
    }

    if (!m_font) return;

    // 标题
    sf::Text title(*m_font, U(m_saveMode ? "存档" : "加载游戏"), 32);
    title.setFillColor(sf::Color(80, 70, 55));
    sf::FloatRect tb = title.getLocalBounds();
    title.setPosition(sf::Vector2f(
        panelX + (panelW - tb.size.x) / 2.f,
        panelY + 10.f
    ));
    window.draw(title);

    // ———— 槽位（砍角八边形） ————
    for (auto& slot : m_slots) {
        bool exists = SaveExists(slot.index);
        bool isEmpty = !m_saveMode && !exists;

        // 槽位八边形背景
        sf::Color slotFill = isEmpty
            ? sf::Color(235, 230, 220, 200)       // 空槽位：略暗
            : sf::Color(255, 252, 245, 255);       // 有存档：亮白
        sf::Color slotOutline = isEmpty
            ? sf::Color(190, 185, 175, 160)
            : sf::Color(160, 145, 120, 200);

        DrawChamferedRect(window, slot.rect.position.x, slot.rect.position.y,
            slot.rect.size.x, slot.rect.size.y, 18.f, slotFill, slotOutline, 2.f);

        // 槽位编号（左侧圆形标记）
        const float cx = slot.rect.position.x + 32.f;
        const float cy = slot.rect.position.y + slot.rect.size.y / 2.f;
        sf::CircleShape numBg(16.f);
        numBg.setOrigin({16.f, 16.f});
        numBg.setPosition({cx, cy});
        numBg.setFillColor(isEmpty
            ? sf::Color(200, 195, 185)
            : sf::Color(100, 85, 65));
        window.draw(numBg);

        sf::Text numText(*m_font, U(std::to_string(slot.index + 1)), 18);
        numText.setFillColor(sf::Color(250, 245, 235));
        sf::FloatRect nb = numText.getLocalBounds();
        numText.setPosition(sf::Vector2f(
            cx - nb.size.x / 2.f,
            cy - nb.size.y / 2.f - nb.position.y
        ));
        window.draw(numText);

        // 槽位信息文字
        std::string info = GetSlotInfo(slot.index);
        sf::Text slotText(*m_font, U(info), 22);
        slotText.setFillColor(isEmpty
            ? sf::Color(170, 165, 155)
            : sf::Color(60, 50, 40));
        sf::FloatRect stb = slotText.getLocalBounds();
        slotText.setPosition(sf::Vector2f(
            slot.rect.position.x + 62.f,
            slot.rect.position.y + (slot.rect.size.y - stb.size.y) / 2.f - stb.position.y
        ));
        window.draw(slotText);

    }

    // ———— 确认弹窗 ————
    if (m_confirmSlot >= 0) {
        // 半透明遮罩覆盖面板
        shape.setSize({panelW, panelH});
        shape.setPosition({panelX, panelY});
        shape.setFillColor(sf::Color(0, 0, 0, 60));
        window.draw(shape);

        // 弹窗居中卡片
        const float cw = 380.f, ch = m_saveMode ? 180.f : 160.f;
        const float cpx = panelX + (panelW - cw) / 2.f;
        const float cpy = panelY + (panelH - ch) / 2.f;
        DrawChamferedRect(window, cpx, cpy, cw, ch, 18.f,
            sf::Color(255, 252, 245, 255),
            sf::Color(160, 145, 120, 220), 2.f);

        // 标题
        sf::Text prompt(*m_font, U("存档" + std::to_string(m_confirmSlot + 1)), 24);
        prompt.setFillColor(sf::Color(80, 70, 55));
        sf::FloatRect pb = prompt.getLocalBounds();
        prompt.setPosition(sf::Vector2f(cpx + (cw - pb.size.x) / 2.f, cpy + 18.f));
        window.draw(prompt);

        // 副标题（存档模式）
        if (m_saveMode) {
            sf::Text sub(*m_font, U("是否覆盖？"), 20);
            sub.setFillColor(sf::Color(160, 140, 120));
            sf::FloatRect sb = sub.getLocalBounds();
            sub.setPosition(sf::Vector2f(cpx + (cw - sb.size.x) / 2.f, cpy + 52.f));
            window.draw(sub);
        }

        // 按钮布局
        const float btnW = 140.f, btnH = 50.f, btnGap = 24.f;
        const float totalBtnW = btnW * 2 + btnGap;
        const float btnStartX = cpx + (cw - totalBtnW) / 2.f;
        const float btnY = cpy + ch - btnH - 22.f;

        if (m_saveMode) {
            // "确认" 按钮（淡绿）
            DrawChamferedRect(window, btnStartX, btnY, btnW, btnH, 14.f,
                sf::Color(180, 210, 160, 255),
                sf::Color(120, 150, 100, 200), 2.f);
            m_btnLoad = sf::FloatRect({btnStartX, btnY}, {btnW, btnH});

            sf::Text confirmText(*m_font, U("确认"), 26);
            confirmText.setFillColor(sf::Color(50, 80, 40));
            sf::FloatRect cb = confirmText.getLocalBounds();
            confirmText.setPosition(sf::Vector2f(
                btnStartX + (btnW - cb.size.x) / 2.f,
                btnY + (btnH - cb.size.y) / 2.f - cb.position.y));
            window.draw(confirmText);

            // "取消" 按钮（灰色）
            const float cancelX = btnStartX + btnW + btnGap;
            DrawChamferedRect(window, cancelX, btnY, btnW, btnH, 14.f,
                sf::Color(210, 205, 200, 255),
                sf::Color(170, 160, 150, 200), 2.f);
            m_btnDelete = sf::FloatRect({cancelX, btnY}, {btnW, btnH});

            sf::Text cancelText(*m_font, U("取消"), 26);
            cancelText.setFillColor(sf::Color(100, 90, 80));
            sf::FloatRect lb = cancelText.getLocalBounds();
            cancelText.setPosition(sf::Vector2f(
                cancelX + (btnW - lb.size.x) / 2.f,
                btnY + (btnH - lb.size.y) / 2.f - lb.position.y));
            window.draw(cancelText);
        } else {
            // "载入" 按钮（淡绿）
            DrawChamferedRect(window, btnStartX, btnY, btnW, btnH, 14.f,
                sf::Color(180, 210, 160, 255),
                sf::Color(120, 150, 100, 200), 2.f);
            m_btnLoad = sf::FloatRect({btnStartX, btnY}, {btnW, btnH});

            sf::Text loadText(*m_font, U("载入"), 26);
            loadText.setFillColor(sf::Color(50, 80, 40));
            sf::FloatRect lb = loadText.getLocalBounds();
            loadText.setPosition(sf::Vector2f(
                btnStartX + (btnW - lb.size.x) / 2.f,
                btnY + (btnH - lb.size.y) / 2.f - lb.position.y));
            window.draw(loadText);

            // "删除" 按钮（淡红）
            const float delX = btnStartX + btnW + btnGap;
            DrawChamferedRect(window, delX, btnY, btnW, btnH, 14.f,
                sf::Color(220, 180, 170, 255),
                sf::Color(180, 120, 110, 200), 2.f);
            m_btnDelete = sf::FloatRect({delX, btnY}, {btnW, btnH});

            sf::Text delText(*m_font, U("删除"), 26);
            delText.setFillColor(sf::Color(120, 50, 40));
            sf::FloatRect db = delText.getLocalBounds();
            delText.setPosition(sf::Vector2f(
                delX + (btnW - db.size.x) / 2.f,
                btnY + (btnH - db.size.y) / 2.f - db.position.y));
            window.draw(delText);
        }
    }
}

// ============================================================
// 文件 I/O
// ============================================================
std::string SavePanel::SavePath(int slot) {
    return "saves/slot_" + std::to_string(slot + 1) + ".sav";
}

bool SavePanel::SaveExists(int slot) {
    std::ifstream f(SavePath(slot));
    return f.good();
}

std::string SavePanel::GetSlotInfo(int slot) {
    std::string path = SavePath(slot);
    std::ifstream f(path);
    if (!f.good()) return "存档" + std::to_string(slot + 1) + "：空";

    std::string line, timeStr, sceneStr;
    while (std::getline(f, line)) {
        if (line.rfind("time:", 0) == 0) timeStr = line.substr(5);
        if (line.rfind("scene:", 0) == 0) {
            int id = std::stoi(line.substr(6));
            switch (id) {
                case 1: sceneStr = "一楼前厅"; break;
                case 2: sceneStr = "楼梯间1"; break;
                case 3: sceneStr = "楼梯间2"; break;
                case 4: sceneStr = "二楼走廊A"; break;
                case 5: sceneStr = "二楼走廊B"; break;
                case 6: sceneStr = "二楼走廊C"; break;
                case 7: sceneStr = "梦境"; break;
                default: sceneStr = "场景" + std::to_string(id); break;
            }
        }
    }
    return "存档" + std::to_string(slot + 1) + "：" + sceneStr + "  " + timeStr;
}

bool SavePanel::WriteSave(int slot, int sceneId, float px, float py) {
    std::string path = SavePath(slot);
    std::ofstream f(path);
    if (!f.good()) return false;

    time_t now = time(nullptr);
    char buf[64];
    struct tm tm_info;
#ifdef _WIN32
    localtime_s(&tm_info, &now);
#else
    localtime_r(&now, &tm_info);
#endif
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm_info);

    f << "scene:" << sceneId << "\n";
    f << "posx:" << px << "\n";
    f << "posy:" << py << "\n";
    f << "time:" << buf << "\n";
    f << "pwd:" << (GameScene::s_passwordUnlocked ? "1" : "0") << "\n";
    return true;
}

bool SavePanel::ReadSave(int slot, int& sceneId, float& px, float& py, bool& pwd) {
    std::string path = SavePath(slot);
    std::ifstream f(path);
    if (!f.good()) return false;

    std::string line;
    while (std::getline(f, line)) {
        if (line.rfind("scene:", 0) == 0) sceneId = std::stoi(line.substr(6));
        if (line.rfind("posx:", 0) == 0)  px = std::stof(line.substr(5));
        if (line.rfind("posy:", 0) == 0)  py = std::stof(line.substr(5));
        if (line.rfind("pwd:", 0) == 0)   pwd = (line.substr(4) == "1");
    }
    return true;
}

// ============================================================
// 静态读档数据（跨场景传递）
// ============================================================
static int   g_pendingScene = -1;
static float g_pendingX = 0, g_pendingY = 0;
static bool  g_pendingPwd = false;

void SavePanel::SetPendingLoad(int sceneId, float px, float py, bool pwd) {
    g_pendingScene = sceneId;
    g_pendingX = px;
    g_pendingY = py;
    g_pendingPwd = pwd;
}
bool SavePanel::HasPendingLoad() { return g_pendingScene >= 0; }
int  SavePanel::GetPendingScene() { return g_pendingScene; }
float SavePanel::GetPendingX() { return g_pendingX; }
float SavePanel::GetPendingY() { return g_pendingY; }
bool SavePanel::GetPendingPwd() { return g_pendingPwd; }
void SavePanel::ClearPending() { g_pendingScene = -1; }
