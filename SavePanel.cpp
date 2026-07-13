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

SavePanel::SavePanel() {
    // 构建3个槽位（逻辑坐标 1920×1080）
    const float panelW = 620.f;
    const float panelX = (SCREEN_W - panelW) / 2.f;
    const float startY = (SCREEN_H - 460.f) / 2.f;
    const float slotW = 540.f;
    const float slotH = 110.f;
    const float gap   = 20.f;
    const float slotX = panelX + (panelW - slotW) / 2.f;

    for (int i = 0; i < 3; ++i) {
        float sy = startY + 80.f + i * (slotH + gap);
        m_slots.push_back({
            sf::FloatRect({slotX, sy}, {slotW, slotH}),
            i
        });
    }
}

void SavePanel::Show(bool saveMode) {
    m_visible  = true;
    m_saveMode = saveMode;
    m_result   = -1;
    MKDIR("saves");  // 确保存档目录存在
}

void SavePanel::Hide() {
    m_visible = false;
}

// ============================================================
// 点击处理
// ============================================================
bool SavePanel::HandleClick(sf::Vector2f pos) {
    if (!m_visible) return false;

    // 关闭按钮
    const float panelW = 620.f;
    const float panelH = 460.f;
    const float panelX = (SCREEN_W - panelW) / 2.f;
    const float panelY = (SCREEN_H - panelH) / 2.f;
    sf::FloatRect closeBtn({panelX + panelW - 42.f, panelY + 8.f}, {34.f, 34.f});
    if (closeBtn.contains(pos)) {
        m_visible = false;
        return true;
    }

    // 槽位
    for (auto& slot : m_slots) {
        if (slot.rect.contains(pos)) {
            if (m_saveMode) {
                // 存档模式：直接写入
                m_result = slot.index;
                m_visible = false;
            } else {
                // 读档模式：检查是否存在
                if (SaveExists(slot.index)) {
                    m_result = slot.index;
                    m_visible = false;
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
    const float panelW = 620.f;
    const float panelH = 460.f;
    const float panelX = (SCREEN_W - panelW) / 2.f;
    const float panelY = (SCREEN_H - panelH) / 2.f;

    // 暗色遮罩
    shape.setSize({static_cast<float>(SCREEN_W), static_cast<float>(SCREEN_H)});
    shape.setPosition({0.f, 0.f});
    shape.setFillColor(sf::Color(0, 0, 0, 160));
    window.draw(shape);

    // 面板背景
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

    if (!m_font) return;

    // 标题
    sf::Text title(*m_font, U(m_saveMode ? "存档" : "读档"), 34);
    title.setFillColor(sf::Color(220, 220, 240));
    sf::FloatRect tb = title.getLocalBounds();
    title.setPosition(sf::Vector2f(
        panelX + (panelW - tb.size.x) / 2.f,
        panelY + 18.f
    ));
    window.draw(title);

    // 槽位
    for (auto& slot : m_slots) {
        // 槽位背景
        shape.setSize({slot.rect.size.x, slot.rect.size.y});
        shape.setPosition(slot.rect.position);
        shape.setFillColor(sf::Color(45, 45, 58, 255));
        shape.setOutlineColor(sf::Color(120, 120, 140));
        shape.setOutlineThickness(2.f);
        window.draw(shape);

        // 槽位信息
        std::string info = GetSlotInfo(slot.index);
        sf::Text slotText(*m_font, U(info), 24);
        slotText.setFillColor(sf::Color(200, 200, 220));
        sf::FloatRect stb = slotText.getLocalBounds();
        slotText.setPosition(sf::Vector2f(
            slot.rect.position.x + 20.f,
            slot.rect.position.y + (slot.rect.size.y - stb.size.y) / 2.f - stb.position.y
        ));
        window.draw(slotText);

        // 读档模式空槽位显示禁用
        if (!m_saveMode && !SaveExists(slot.index)) {
            shape.setSize({slot.rect.size.x, slot.rect.size.y});
            shape.setPosition(slot.rect.position);
            shape.setFillColor(sf::Color(0, 0, 0, 80));
            window.draw(shape);
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

    // 读取时间戳和场景
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
