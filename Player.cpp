#include "Player.h"
#include <cmath>

// 帧序列: 1 → 2 → 1 → 3
static const int ANIM_SEQ[4] = {0, 1, 0, 2};

Player::Player() {
    LoadTextures();
    m_sprite = std::make_unique<sf::Sprite>(m_tex[static_cast<int>(Direction::Down)][0]);
    m_sprite->setOrigin({23.f, 47.f});   // 脚底在图片中的位置（瓦片对齐用）
    m_sprite->setScale({SPRITE_SCALE, SPRITE_SCALE});
}

void Player::LoadTextures() {
    const char* dirNames[4] = {"front", "back", "right", "left"};

    for (int d = 0; d < 4; ++d) {
        for (int f = 0; f < 3; ++f) {
            std::string path = "assets/player/player "
                             + std::string(dirNames[d])
                             + std::to_string(f + 1) + ".png";
            if (!m_tex[d][f].loadFromFile(path)) {
                // 加载失败时纹理保持空白
            }
        }
    }
}

void Player::SetPosition(sf::Vector2f pos) {
    m_spritePos = pos;
    m_sprite->setPosition(pos);
}

void Player::SetFacing(Direction dir) {
    m_facing = dir;
    int dirIdx = static_cast<int>(m_facing);
    m_sprite->setTexture(m_tex[dirIdx][0]);  // 立即更新贴图
}

void Player::HandleInput() {
    if (sf::Keyboard::isKeyPressed(Key::Up))    m_facing = Direction::Up;
    if (sf::Keyboard::isKeyPressed(Key::Down))  m_facing = Direction::Down;
    if (sf::Keyboard::isKeyPressed(Key::Left))  m_facing = Direction::Left;
    if (sf::Keyboard::isKeyPressed(Key::Right)) m_facing = Direction::Right;
}

sf::Vector2f Player::HitCenter() const {
    return {m_spritePos.x + m_hitOffset.x, m_spritePos.y + m_hitOffset.y};
}

void Player::Update(float dt) {
    if (!m_map) return;

    float dx = 0, dy = 0;
    if (sf::Keyboard::isKeyPressed(Key::Up))    { dy = -1; m_facing = Direction::Up; }
    if (sf::Keyboard::isKeyPressed(Key::Down))  { dy =  1; m_facing = Direction::Down; }
    if (sf::Keyboard::isKeyPressed(Key::Left))  { dx = -1; m_facing = Direction::Left; }
    if (sf::Keyboard::isKeyPressed(Key::Right)) { dx =  1; m_facing = Direction::Right; }

    float len = std::sqrt(dx * dx + dy * dy);
    bool moving = (len > 0.01f);

    if (moving) {
        dx /= len; dy /= len;
        sf::Vector2f hitPos = HitCenter();
        sf::Vector2f oldHit = hitPos;
        sf::Vector2f newHit = {hitPos.x + dx * m_speed * dt,
                               hitPos.y + dy * m_speed * dt};
        sf::Vector2f resolved = m_map->ResolveCollision(oldHit, newHit, PLAYER_HALF_W, PLAYER_HALF_H);
        // 将碰撞结果反推到精灵位置
        m_spritePos.x += resolved.x - oldHit.x;
        m_spritePos.y += resolved.y - oldHit.y;

        // 行走动画
        m_animTimer += dt;
        if (m_animTimer >= FRAME_DURATION) {
            m_animTimer -= FRAME_DURATION;
            if (!m_wasMoving) m_animStep = 0;
            m_animStep = (m_animStep + 1) % 4;
        }
    } else {
        m_animStep = 0;
        m_animTimer = 0.f;
    }

    m_wasMoving = moving;

    // 更新精灵
    int dirIdx = static_cast<int>(m_facing);
    int frameIdx = ANIM_SEQ[m_animStep];
    m_sprite->setTexture(m_tex[dirIdx][frameIdx]);
    m_sprite->setPosition(m_spritePos);
}

void Player::Draw(sf::RenderWindow& window) const {
    window.draw(*m_sprite);
}
