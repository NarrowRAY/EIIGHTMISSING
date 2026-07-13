#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "Constants.h"

class TileMap {
public:
    TileMap();

    void SetMapData(const MapData& data);
    void Draw(sf::RenderWindow& window, float camX, float camY) const;

    // 强制阻挡（不改贴图，只改碰撞）
    void SetBlocked(int col, int row, bool blocked = true);
    bool IsBlocked(int col, int row) const;
    // 强制可行走（不改贴图）
    void SetForceWalkable(int col, int row, bool walkable = true);

    // 碰撞
    sf::Vector2f ResolveCollision(sf::Vector2f oldPos, sf::Vector2f newPos,
                                   float halfW, float halfH) const;
    bool IsWalkableAt(float px, float py, float halfW, float halfH) const;

    // 坐标转换
    static int  PixelToTile(float px)  { return static_cast<int>(px / TILE_SIZE); }
    static float TileToPixel(int tile)   { return tile * TILE_SIZE + TILE_SIZE / 2.f; }
    int  GetTile(int col, int row) const;
    int  GetTileAtPixel(float px, float py) const;
    bool IsInBounds(int col, int row) const;

    int  Width()  const { return m_width; }
    int  Height() const { return m_height; }

private:
    void GenerateTextures();
    void FillRect(sf::Texture& tex, int x, int y, int w, int h, sf::Color col);

    int m_width  = 0;
    int m_height = 0;
    std::vector<int> m_tiles;
    std::vector<bool> m_blocked;       // 强制阻挡标记
    std::vector<bool> m_forceWalkable;  // 强制可行走标记
    std::unordered_map<int, sf::Texture> m_textures;
};
