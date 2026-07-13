#include "TileMap.h"
#include <cmath>
#include <cstdint>

TileMap::TileMap() {
    GenerateTextures();
}

// ============================================================
// 填充矩形辅助函数
// ============================================================
void TileMap::FillRect(sf::Texture& tex, int x, int y, int w, int h, sf::Color col) {
    std::vector<std::uint8_t> pixels(w * h * 4);
    for (int i = 0; i < w * h; ++i) {
        pixels[i * 4 + 0] = col.r;
        pixels[i * 4 + 1] = col.g;
        pixels[i * 4 + 2] = col.b;
        pixels[i * 4 + 3] = col.a;
    }
    tex.update(pixels.data(), {static_cast<unsigned>(w), static_cast<unsigned>(h)}, {static_cast<unsigned>(x), static_cast<unsigned>(y)});
}

// ============================================================
// 生成所有瓦片纹理
// ============================================================
void TileMap::GenerateTextures() {
    const int S = TILE_SIZE;
    std::vector<std::uint8_t> buf(S * S * 4);

    auto makeTex = [&](auto&& fillFn) {
        sf::Texture tex;
        tex.resize({static_cast<unsigned>(S), static_cast<unsigned>(S)});
        fillFn(buf);
        (void)tex.update(buf.data());
        return tex;
    };

    // ---- Floor（木地板+网格线） ----
    m_textures[static_cast<int>(TileType::Floor)] = makeTex([&](auto& p) {
        for (int y = 0; y < S; ++y) {
            for (int x = 0; x < S; ++x) {
                int i = (y * S + x) * 4;
                // 木地板底色：暖棕色 + 细微噪点
                int r = 195 + (x * 7 + y * 13) % 12 - 6;
                int g = 165 + (x * 11 + y * 3) % 10 - 5;
                int b = 130 + (x * 5 + y * 17) % 8 - 4;
                p[i]=r; p[i+1]=g; p[i+2]=b; p[i+3]=255;
                // 木纹横线（模拟木板接缝）
                if (y % 8 == 0 && x % 3 != 0) {
                    p[i]=r-15; p[i+1]=g-15; p[i+2]=b-15;
                }
                // 瓦片边框线（粗深线，区分每个格子）
                if (x <= 1 || y <= 1 || x >= S-2 || y >= S-2) {
                    p[i]=60; p[i+1]=38; p[i+2]=20;
                }
            }
        }
    });

    // ---- Wall（砖墙纹理） ----
    m_textures[static_cast<int>(TileType::Wall)] = makeTex([&](auto& p) {
        const int BH = 8;  // 砖块高度
        for (int y = 0; y < S; ++y) {
            int brickRow = y / BH;
            bool offset = (brickRow % 2 == 1);  // 交错砖缝
            for (int x = 0; x < S; ++x) {
                int i = (y * S + x) * 4;
                // 砖块底色：深灰带微变
                int r = 92 + (brickRow * 3 + x / 5) % 7 - 3;
                int g = 88 + (brickRow * 5 + x / 3) % 6 - 3;
                int b = 96 + (brickRow * 2 + x / 7) % 5 - 2;
                // 砖缝横线
                bool mortarH = (y % BH == 0);
                // 砖缝竖线（奇数行偏移半砖）
                bool mortarV = false;
                if (offset) {
                    mortarV = (x == 0 || x == S/2);
                } else {
                    mortarV = (x == S/4 || x == S*3/4 || x == 0);
                }
                if (mortarH || mortarV) {
                    p[i]=55; p[i+1]=52; p[i+2]=58;  // 灰浆颜色
                } else {
                    p[i]=r; p[i+1]=g; p[i+2]=b;
                }
                p[i+3]=255;
            }
        }
    });

    // ---- Door（纯色木门板） ----
    m_textures[static_cast<int>(TileType::Door)] = makeTex([&](auto& p) {
        for (int y = 0; y < S; ++y) {
            for (int x = 0; x < S; ++x) {
                int i = (y * S + x) * 4;
                // 木门板底色
                int r = 155 + (x * 3 + y * 7) % 10 - 5;
                int g = 105 + (x * 5 + y * 3) % 8 - 4;
                int b = 65  + (x * 7 + y * 5) % 6 - 3;
                p[i]=r; p[i+1]=g; p[i+2]=b; p[i+3]=255;
                // 门板竖线纹理
                if (x % 10 == 0 && x > 0) {
                    p[i]=r-15; p[i+1]=g-15; p[i+2]=b-15;
                }
            }
        }
    });

    // ---- Window（纯色矩形，浅蓝玻璃） ----
    m_textures[static_cast<int>(TileType::Window)] = makeTex([&](auto& p) {
        for (int y = 0; y < S; ++y) {
            for (int x = 0; x < S; ++x) {
                int i = (y * S + x) * 4;
                p[i]=145; p[i+1]=195; p[i+2]=230; p[i+3]=255;
            }
        }
    });

    // ---- Sofa (白色) ----
    m_textures[static_cast<int>(TileType::Sofa)] = makeTex([&](auto& p) {
        for (int y = 0; y < S; ++y) {
            for (int x = 0; x < S; ++x) {
                int i = (y * S + x) * 4;
                bool edge = (x < 3 || x >= S-3 || y >= S-4);
                p[i]=edge?200:235; p[i+1]=edge?195:230; p[i+2]=edge?190:225; p[i+3]=255;
            }
        }
    });

    // ---- Table ----
    m_textures[static_cast<int>(TileType::Table)] = makeTex([&](auto& p) {
        for (int y = 0; y < S; ++y) {
            for (int x = 0; x < S; ++x) {
                int i = (y * S + x) * 4;
                bool edge = (x < 3 || x >= S-3 || y < 3 || y >= S-3);
                p[i]=edge?85:110; p[i+1]=edge?50:70; p[i+2]=edge?25:40; p[i+3]=255;
            }
        }
    });

    // ---- Printer ----
    m_textures[static_cast<int>(TileType::Printer)] = makeTex([&](auto& p) {
        for (int y = 0; y < S; ++y) {
            for (int x = 0; x < S; ++x) {
                int i = (y * S + x) * 4;
                int v = 140 + (y%5==0?5:-2);
                p[i]=v; p[i+1]=v+5; p[i+2]=v+10; p[i+3]=255;
                if (y < 4) { p[i]=180; p[i+1]=185; p[i+2]=190; }  // 顶部
            }
        }
    });

    // ---- BulletinBoard (墙挂式黑板) ----
    m_textures[static_cast<int>(TileType::BulletinBoard)] = makeTex([&](auto& p) {
        for (int y = 0; y < S; ++y) {
            for (int x = 0; x < S; ++x) {
                int i = (y * S + x) * 4;
                // 黑色黑板
                p[i]=35; p[i+1]=35; p[i+2]=40;
                // 白色粉笔标记
                if (x % 12 == 4 && y % 8 == 2) { p[i]=200; p[i+1]=200; p[i+2]=195; }
                p[i+3]=255;
            }
        }
    });

    // ---- KeypadLock (密码锁，外观同公告栏) ----
    m_textures[static_cast<int>(TileType::KeypadLock)] = makeTex([&](auto& p) {
        for (int y = 0; y < S; ++y) {
            for (int x = 0; x < S; ++x) {
                int i = (y * S + x) * 4;
                // 深色面板
                p[i]=35; p[i+1]=35; p[i+2]=40;
                // 白色粉笔标记（与公告栏一致）
                if (x % 12 == 4 && y % 8 == 2) { p[i]=200; p[i+1]=200; p[i+2]=195; }
                p[i+3]=255;
            }
        }
    });

    // ---- Clock ----
    m_textures[static_cast<int>(TileType::Clock)] = makeTex([&](auto& p) {
        int cx = S/2, cy = S/2, r = S/2 - 4;
        for (int y = 0; y < S; ++y) {
            for (int x = 0; x < S; ++x) {
                int i = (y * S + x) * 4;
                int dx = x - cx, dy = y - cy;
                bool inCircle = (dx*dx + dy*dy) <= r*r;
                bool border = std::abs(dx*dx + dy*dy - r*r) < r*4;
                if (border) { p[i]=50; p[i+1]=50; p[i+2]=55; }
                else if (inCircle) { p[i]=235; p[i+1]=235; p[i+2]=235; }
                else { p[i]=85; p[i+1]=85; p[i+2]=95; }
                p[i+3]=255;
            }
        }
    });

    // ---- VendingMachine ----
    m_textures[static_cast<int>(TileType::VendingMachine)] = makeTex([&](auto& p) {
        for (int y = 0; y < S; ++y) {
            for (int x = 0; x < S; ++x) {
                int i = (y * S + x) * 4;
                p[i]=210; p[i+1]=55; p[i+2]=55; p[i+3]=255;
                // 玻璃面板
                if (x >= 5 && x < S-5 && y >= 4 && y < S-4) {
                    p[i]=180; p[i+1]=200; p[i+2]=210;
                }
                // 条纹
                if (y % 8 == 0) { p[i]=160; p[i+1]=30; p[i+2]=30; }
            }
        }
    });

    // ---- TrashCan ----
    m_textures[static_cast<int>(TileType::TrashCan)] = makeTex([&](auto& p) {
        for (int y = 0; y < S; ++y) {
            for (int x = 0; x < S; ++x) {
                int i = (y * S + x) * 4;
                bool body = (x >= 4 && x < S-4 && y >= 2 && y < S-2);
                bool lid  = (x >= 2 && x < S-2 && y >= 0 && y < 5);
                if (lid)  { p[i]=80; p[i+1]=105; p[i+2]=65; }
                else if (body) { p[i]=110; p[i+1]=140; p[i+2]=90; }
                else { p[i]=200; p[i+1]=190; p[i+2]=155; }  // 地板
                p[i+3]=255;
            }
        }
    });

    // ---- Debris ----
    m_textures[static_cast<int>(TileType::Debris)] = makeTex([&](auto& p) {
        for (int y = 0; y < S; ++y) {
            for (int x = 0; x < S; ++x) {
                int i = (y * S + x) * 4;
                int v = 75 + (rand()%15);
                p[i]=v; p[i+1]=v-10; p[i+2]=v-20; p[i+3]=255;
                if ((x+y)%5 < 2) { p[i]=55; p[i+1]=45; p[i+2]=35; }
            }
        }
    });

    // ---- Stairs ----
    m_textures[static_cast<int>(TileType::Stairs)] = makeTex([&](auto& p) {
        for (int y = 0; y < S; ++y) {
            for (int x = 0; x < S; ++x) {
                int i = (y * S + x) * 4;
                int step = y / (S / 5);
                if (step % 2 == 0) {
                    p[i]=190; p[i+1]=180; p[i+2]=160;
                } else {
                    p[i]=120; p[i+1]=110; p[i+2]=90;
                }
                p[i+3]=255;
            }
        }
    });

    // ---- Sign (施工标志) ----
    m_textures[static_cast<int>(TileType::Sign)] = makeTex([&](auto& p) {
        for (int y = 0; y < S; ++y) {
            for (int x = 0; x < S; ++x) {
                int i = (y * S + x) * 4;
                bool stripe = ((x + y) / 6) % 2 == 0;
                p[i]=stripe?225:30; p[i+1]=stripe?210:30; p[i+2]=stripe?40:30; p[i+3]=255;
            }
        }
    });

    // ---- Cabinet ----
    m_textures[static_cast<int>(TileType::Cabinet)] = makeTex([&](auto& p) {
        for (int y = 0; y < S; ++y) {
            for (int x = 0; x < S; ++x) {
                int i = (y * S + x) * 4;
                p[i]=170; p[i+1]=145; p[i+2]=95; p[i+3]=255;
                bool edge = (x < 3 || x >= S-3 || y < 3 || y >= S-3);
                if (edge) { p[i]=135; p[i+1]=110; p[i+2]=65; }
                if (x >= S/2-1 && x < S/2+1) { p[i]=135; p[i+1]=110; p[i+2]=65; }
            }
        }
    });

    // ---- Cart (清洁车) ----
    m_textures[static_cast<int>(TileType::Cart)] = makeTex([&](auto& p) {
        for (int y = 0; y < S; ++y) {
            for (int x = 0; x < S; ++x) {
                int i = (y * S + x) * 4;
                bool body = (x >= 3 && x < S-3 && y >= 4 && y < S-4);
                bool wheel = ((x < 5 || x >= S-5) && y >= S-6);
                if (wheel) { p[i]=50; p[i+1]=50; p[i+2]=50; }
                else if (body) { p[i]=155; p[i+1]=155; p[i+2]=155; }
                else { p[i]=200; p[i+1]=190; p[i+2]=155; }  // 地板
                p[i+3]=255;
            }
        }
    });

    // ---- Display（等身显示屏：深色边框+发光屏幕） ----
    m_textures[static_cast<int>(TileType::Display)] = makeTex([&](auto& p) {
        for (int y = 0; y < S; ++y) {
            for (int x = 0; x < S; ++x) {
                int i = (y * S + x) * 4;
                bool border = (x < 4 || x >= S-4 || y < 4 || y >= S-4);
                if (border) {
                    p[i]=40; p[i+1]=40; p[i+2]=45;   // 深色边框
                } else {
                    // 发光屏幕 + 扫描线
                    int v = 80 + (y % 3) * 15;
                    p[i]=v; p[i+1]=v+55; p[i+2]=v+110;
                }
                p[i+3]=255;
            }
        }
    });

    // ---- Trap ----
    m_textures[static_cast<int>(TileType::Trap)] = makeTex([&](auto& p) {
        for (int y = 0; y < S; ++y) {
            for (int x = 0; x < S; ++x) {
                int i = (y * S + x) * 4;
                // 瓦片边框线
                if (x <= 1 || y <= 1 || x >= S-2 || y >= S-2) {
                    p[i]=100; p[i+1]=20; p[i+2]=20;  // 深红边框
                } else {
                    bool bright = ((x + y) / 4) % 2 == 0;
                    p[i]=bright?220:160; p[i+1]=bright?60:20; p[i+2]=bright?60:20;
                }
                p[i+3]=255;
            }
        }
    });
}

// ============================================================
// 加载地图数据
// ============================================================
void TileMap::SetMapData(const MapData& data) {
    m_width  = data.width;
    m_height = data.height;
    m_tiles  = data.tiles;
    m_blocked.assign(m_width * m_height, false);
    m_forceWalkable.assign(m_width * m_height, false);
}

void TileMap::SetBlocked(int col, int row, bool blocked) {
    if (IsInBounds(col, row))
        m_blocked[row * m_width + col] = blocked;
}

bool TileMap::IsBlocked(int col, int row) const {
    if (!IsInBounds(col, row)) return false;
    return m_blocked[row * m_width + col];
}

void TileMap::SetForceWalkable(int col, int row, bool walkable) {
    if (IsInBounds(col, row))
        m_forceWalkable[row * m_width + col] = walkable;
}

// ============================================================
// 瓦片查询
// ============================================================
int TileMap::GetTile(int col, int row) const {
    if (!IsInBounds(col, row)) return static_cast<int>(TileType::Void);
    return m_tiles[row * m_width + col];
}

int TileMap::GetTileAtPixel(float px, float py) const {
    return GetTile(PixelToTile(px), PixelToTile(py));
}

bool TileMap::IsInBounds(int col, int row) const {
    return col >= 0 && col < m_width && row >= 0 && row < m_height;
}

// ============================================================
// 碰撞检测
// ============================================================
bool TileMap::IsWalkableAt(float px, float py, float halfW, float halfH) const {
    int left   = PixelToTile(px - halfW);
    int right  = PixelToTile(px + halfW);
    int top    = PixelToTile(py - halfH);
    int bottom = PixelToTile(py + halfH);

    for (int row = top; row <= bottom; ++row) {
        for (int col = left; col <= right; ++col) {
            if (!IsInBounds(col, row)) return false;
            if (m_forceWalkable[row * m_width + col]) continue;  // 强制可行走
            if (IsBlocked(col, row)) return false;  // 强制阻挡
            int tile = GetTile(col, row);
            if (::IsWalkable(tile)) continue;
            // 公告栏/密码锁特殊处理：只挡右半边，左半边可走
            if (tile == static_cast<int>(TileType::BulletinBoard) ||
                tile == static_cast<int>(TileType::KeypadLock)) {
                float boardX = col * TILE_SIZE + TILE_SIZE / 2.f;  // 右半边左边缘
                if (px + halfW <= boardX) continue;  // 没碰到右半边，可走
            }
            return false;
        }
    }
    return true;
}

sf::Vector2f TileMap::ResolveCollision(sf::Vector2f oldPos, sf::Vector2f newPos,
                                        float halfW, float halfH) const {
    sf::Vector2f resolved = newPos;

    // 尝试X轴单独移动
    sf::Vector2f xOnly = {newPos.x, oldPos.y};
    if (!IsWalkableAt(xOnly.x, xOnly.y, halfW, halfH))
        resolved.x = oldPos.x;

    // 尝试Y轴单独移动
    sf::Vector2f yOnly = {resolved.x, newPos.y};
    if (!IsWalkableAt(yOnly.x, yOnly.y, halfW, halfH))
        resolved.y = oldPos.y;

    // 边界 clamp
    float maxX = static_cast<float>(m_width  * TILE_SIZE - 1);
    float maxY = static_cast<float>(m_height * TILE_SIZE - 1);
    if (resolved.x < halfW)     resolved.x = halfW;
    if (resolved.y < halfH)     resolved.y = halfH;
    if (resolved.x > maxX - halfW) resolved.x = maxX - halfW;
    if (resolved.y > maxY - halfH) resolved.y = maxY - halfH;

    return resolved;
}

// ============================================================
// 渲染 — 简洁两遍：地板 → 墙壁/家具
// ============================================================
void TileMap::Draw(sf::RenderWindow& window, float camX, float camY) const {
    int sc = std::max(0, PixelToTile(camX - SCREEN_W/2) - 1);
    int ec = std::min(m_width,  PixelToTile(camX + SCREEN_W/2) + 2);
    int sr = std::max(0, PixelToTile(camY - SCREEN_H/2) - 1);
    int er = std::min(m_height, PixelToTile(camY + SCREEN_H/2) + 2);

    sf::RectangleShape shape({TILE_SIZE, TILE_SIZE});

    // ===== 第1遍：地板/楼梯/陷阱(可行走瓦片) =====
    for (int row = sr; row < er; ++row) {
        for (int col = sc; col < ec; ++col) {
            int t = GetTile(col, row);
            if (!::IsWalkable(t)) continue;
            auto it = m_textures.find(t);
            if (it == m_textures.end()) continue;

            shape.setSize({TILE_SIZE, TILE_SIZE});
            shape.setPosition({col * 1.f * TILE_SIZE, row * 1.f * TILE_SIZE});
            shape.setTexture(&it->second);
            window.draw(shape);
        }
    }

    // ===== 第2遍：墙壁/门窗/家具 =====
    for (int row = sr; row < er; ++row) {
        for (int col = sc; col < ec; ++col) {
            int t = GetTile(col, row);
            if (t == static_cast<int>(TileType::Void)) continue;
            if (::IsWalkable(t)) continue;  // 地板类已在第1遍画过

            auto it = m_textures.find(t);
            if (it == m_textures.end()) continue;

            if (t == static_cast<int>(TileType::BulletinBoard) ||
                t == static_cast<int>(TileType::KeypadLock)) {
                // 公告栏/密码锁：先画地板，再半格贴右边缘
                auto fit = m_textures.find(static_cast<int>(TileType::Floor));
                if (fit != m_textures.end()) {
                    shape.setSize({TILE_SIZE, TILE_SIZE});
                    shape.setPosition({col * TILE_SIZE * 1.f, row * TILE_SIZE * 1.f});
                    shape.setTexture(&fit->second);
                    window.draw(shape);
                }
                shape.setSize({TILE_SIZE / 2.f, TILE_SIZE});
                shape.setPosition({col * TILE_SIZE + TILE_SIZE / 2.f, row * TILE_SIZE * 1.f});
                shape.setTexture(&it->second);
                window.draw(shape);
            } else {
                shape.setSize({TILE_SIZE, TILE_SIZE});
                shape.setPosition({col * TILE_SIZE * 1.f, row * TILE_SIZE * 1.f});
                shape.setTexture(&it->second);
                window.draw(shape);
            }
        }
    }
}
