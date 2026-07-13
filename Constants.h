#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

// ============================================================
// 窗口与瓦片常量
// ============================================================
constexpr int   SCREEN_W       = 1920;
constexpr int   SCREEN_H       = 1080;
constexpr int   TILE_SIZE      = 40;
constexpr float PLAYER_SPEED   = 220.f;
constexpr float PLAYER_W       = 32.f;
constexpr float PLAYER_H       = 40.f;
constexpr float PLAYER_HALF_W  = 20.f;  // 一整个半格
constexpr float PLAYER_HALF_H  = 14.f;

// ============================================================
// 瓦片类型枚举
// ============================================================
enum class TileType : int {
    Void            = 0,
    Floor           = 1,
    Wall            = 2,
    Door            = 3,
    Window          = 4,
    Sofa            = 5,
    Table           = 6,
    Printer         = 7,
    BulletinBoard   = 8,
    Clock           = 9,
    VendingMachine  = 10,
    TrashCan        = 11,
    Debris          = 12,
    Stairs          = 13,
    Sign            = 14,
    Cabinet         = 15,
    Cart            = 16,
    Trap            = 17,
    Display         = 18,  // 等身显示屏
    KeypadLock      = 19   // 密码锁（外观同公告栏，贴墙长条）
};

inline bool IsWalkable(int tileType) {
    auto t = static_cast<TileType>(tileType);
    return t == TileType::Floor || t == TileType::Stairs || t == TileType::Trap;
}

// ============================================================
// 场景 ID 枚举
// ============================================================
enum class SceneID : int {
    Title       = 0,
    Classroom1B = 1,
    Stairwell1  = 2,
    Stairwell2  = 3,
    Corridor2_1 = 4,
    Corridor2_2 = 5,
    Corridor2_3 = 6,
    Dream       = 7
};

// ============================================================
// 方向枚举
// ============================================================
enum class Direction { Down, Up, Left, Right };

// ============================================================
// 地图数据结构
// ============================================================
struct MapData {
    int width  = 0;
    int height = 0;
    std::vector<int> tiles;  // [y * width + x]
};

// ============================================================
// 地图构建辅助 — 用字符定义地图，直观可读
// ============================================================
namespace MapBuilder {
    inline int CharToTile(char c) {
        switch (c) {
            case '#': return static_cast<int>(TileType::Wall);
            case ' ': return static_cast<int>(TileType::Floor);
            case 'D': return static_cast<int>(TileType::Door);
            case 'W': return static_cast<int>(TileType::Window);
            case 'S': return static_cast<int>(TileType::Sofa);
            case 'T': return static_cast<int>(TileType::Table);
            case 'P': return static_cast<int>(TileType::Printer);
            case 'B': return static_cast<int>(TileType::BulletinBoard);
            case '@': return static_cast<int>(TileType::Clock);
            case 'V': return static_cast<int>(TileType::VendingMachine);
            case 'R': return static_cast<int>(TileType::TrashCan);
            case 'X': return static_cast<int>(TileType::Debris);
            case '^': return static_cast<int>(TileType::Stairs);
            case '!': return static_cast<int>(TileType::Sign);
            case 'C': return static_cast<int>(TileType::Cabinet);
            case '*': return static_cast<int>(TileType::Cart);
            case 't': return static_cast<int>(TileType::Trap);
            case '|': return static_cast<int>(TileType::Display);
            case 'K': return static_cast<int>(TileType::KeypadLock);
            default:  return static_cast<int>(TileType::Floor);
        }
    }

    inline MapData FromStrings(const std::vector<std::string>& rows) {
        MapData data;
        data.height = static_cast<int>(rows.size());
        data.width  = data.height > 0 ? static_cast<int>(rows[0].size()) : 0;
        data.tiles.resize(data.width * data.height);
        for (int y = 0; y < data.height; ++y) {
            for (int x = 0; x < data.width; ++x) {
                char c = x < static_cast<int>(rows[y].size()) ? rows[y][x] : ' ';
                data.tiles[y * data.width + x] = CharToTile(c);
            }
        }
        return data;
    }
}

// ============================================================
// 颜色命名空间
// ============================================================
namespace Colors {
    const sf::Color Floor        (210, 195, 160);
    const sf::Color FloorDark    (190, 175, 140);
    const sf::Color Wall         (85,  85,  95);
    const sf::Color WallDark     (65,  65,  75);
    const sf::Color WallTop      (100, 100, 110);
    const sf::Color Door         (150, 100, 55);
    const sf::Color DoorFrame    (110, 70,  35);
    const sf::Color WindowGlass  (155, 205, 235);
    const sf::Color WindowFrame  (70,  70,  80);
    const sf::Color Sofa         (165, 75,  55);
    const sf::Color SofaDark     (140, 60,  40);
    const sf::Color Table        (110, 70,  40);
    const sf::Color TableDark    (85,  50,  25);
    const sf::Color Printer      (140, 145, 150);
    const sf::Color PrinterDark  (110, 115, 120);
    const sf::Color Bulletin     (195, 155, 105);
    const sf::Color BulletinDark (165, 125, 75);
    const sf::Color ClockFace    (235, 235, 235);
    const sf::Color ClockBorder  (50,  50,  55);
    const sf::Color VendingRed   (210, 55,  55);
    const sf::Color VendingDark  (160, 30,  30);
    const sf::Color TrashCan     (110, 140, 90);
    const sf::Color TrashCanDark (80,  105, 65);
    const sf::Color Debris       (75,  65,  55);
    const sf::Color StairsLight  (190, 180, 160);
    const sf::Color StairsDark   (120, 110, 90);
    const sf::Color SignYel      (225, 210, 40);
    const sf::Color SignBlack    (30,  30,  30);
    const sf::Color CabinetLit   (170, 145, 95);
    const sf::Color CabinetDark  (135, 110, 65);
    const sf::Color Cart         (155, 155, 155);
    const sf::Color CartDark     (120, 120, 120);
    const sf::Color TrapRed      (220, 60,  60);
    const sf::Color TrapDark     (160, 20,  20);
    const sf::Color DisplayFrame (40,  40,  45);   // 显示屏边框
    const sf::Color DisplayScreen(80,  140, 200);  // 显示屏发光

    const sf::Color PlayerBody   (60,  100, 180);
    const sf::Color PlayerHead   (255, 210, 160);
    const sf::Color PlayerLeg    (40,  70,  130);
    const sf::Color PlayerEye    (30,  30,  30);
    const sf::Color BG           (20,  20,  30);
    const sf::Color TitleGold    (240, 210, 100);
    const sf::Color ButtonNormal (70,  70,  90);
    const sf::Color ButtonHover  (100, 100, 130);
    const sf::Color ButtonText   (220, 220, 220);
}

// ============================================================
// UTF-8 转 UTF-32 辅助
// ============================================================
inline sf::String U(const std::string& utf8) {
    return sf::String::fromUtf8(utf8.begin(), utf8.end());
}

// ============================================================
// 按键绑定
// ============================================================
namespace Key {
    constexpr auto Up    = sf::Keyboard::Scancode::W;
    constexpr auto Down  = sf::Keyboard::Scancode::S;
    constexpr auto Left  = sf::Keyboard::Scancode::A;
    constexpr auto Right = sf::Keyboard::Scancode::D;
    constexpr auto Interact = sf::Keyboard::Scancode::E;
}
