#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"
#include "TileMap.h"

class Player {
public:
    Player();
    ~Player() = default;

    void SetMap(TileMap* map) { m_map = map; }
    void SetPosition(sf::Vector2f pos);
    void SetFacing(Direction dir);
    sf::Vector2f GetPosition() const { return m_spritePos; }
    sf::Vector2f GetHitOffset() const { return m_hitOffset; }
    Direction   GetFacing()   const { return m_facing; }
    bool        IsMoving()    const { return m_wasMoving; }

    void HandleInput();
    void Update(float dt);
    void Draw(sf::RenderWindow& window) const;
    sf::Vector2f HitCenter() const;

private:
    void LoadTextures();

    sf::Vector2f m_spritePos{0, 0};   // 精灵渲染位置（原点在脚底）
    Direction    m_facing = Direction::Down;
    float        m_speed  = PLAYER_SPEED;
    TileMap*     m_map    = nullptr;

    // 精灵动画 — 4方向 × 3帧
    sf::Texture               m_tex[4][3];
    std::unique_ptr<sf::Sprite> m_sprite;
    float        m_animTimer   = 0.f;
    int          m_animStep    = 0;
    bool         m_wasMoving   = false;

    // 碰撞中心相对于精灵位置的偏移
    sf::Vector2f m_hitOffset{15.f, 3.f};

    static constexpr float FRAME_DURATION = 0.15f;
    static constexpr float SPRITE_SCALE   = 1.5f;
};
