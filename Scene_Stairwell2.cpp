#include "Scene_Stairwell2.h"

Scene_Stairwell2::Scene_Stairwell2(int entryParam) {
    LoadMap(BuildMapData(), GetSpawnPoint(entryParam));
    // 门(cols4-5,row5)强制可行走
    m_tileMap.SetForceWalkable(4, 5);
    m_tileMap.SetForceWalkable(5, 5);

    // ———— 楼梯精灵 (cols1-3, rows1-2, 3×2) ————
    m_stairPositions = {{1, 1}};
    if (m_stairTex.loadFromFile("assets/textures/stair1.png")) {
        m_stairTex.setSmooth(true);
        for (const auto& dp : m_stairPositions) {
            auto spr = std::make_unique<sf::Sprite>(m_stairTex);
            auto ts = m_stairTex.getSize();
            spr->setScale({(3.f * TILE_SIZE) / ts.x * 2.25f, (2.f * TILE_SIZE) / ts.y * 2.25f});
            spr->setPosition({dp.x * 1.f * TILE_SIZE - 47.f, dp.y * 1.f * TILE_SIZE - 30.f});
            m_stairSprites.push_back(std::move(spr));
        }
    }

    // ———— 打印机精灵 (col9, row4, 1×1) ————
    m_printerPositions = {{9, 4}};
    if (m_printerTex.loadFromFile("assets/textures/printer2.png")) {
        m_printerTex.setSmooth(true);
        for (const auto& dp : m_printerPositions) {
            auto spr = std::make_unique<sf::Sprite>(m_printerTex);
            auto ts = m_printerTex.getSize();
            spr->setScale({(1.f * TILE_SIZE) / ts.x * 3.0f, (1.f * TILE_SIZE) / ts.y * 3.0f});
            spr->setPosition({dp.x * 1.f * TILE_SIZE, dp.y * 1.f * TILE_SIZE - 80.f});
            m_printerSprites.push_back(std::move(spr));
        }
    }
    m_tileMap.SetBlocked(10, 3, true);

    if (entryParam == 0)
        m_player.SetFacing(Direction::Right);
}

MapData Scene_Stairwell2::BuildMapData() {
    // 12×7 二楼楼梯间②
    return MapBuilder::FromStrings({
        //         111
        // 012345678901
        "############", // 0 顶墙
        "#          #", // 1  楼梯sprite
        "#          #", // 2  楼梯sprite
        "#          #", // 3
        "#          #", // 4  打印机sprite
        "#   DD     #", // 5  门(cols5-6)
        "############", // 6  底墙
    });
}

sf::Vector2f Scene_Stairwell2::GetSpawnPoint(int entryParam) {
    if (entryParam == 0)
        return { TileMap::TileToPixel(4), TileMap::TileToPixel(2) };  // 从楼梯上来
    if (entryParam == 1)
        return { 5.5f * TILE_SIZE, TileMap::TileToPixel(4) };  // 从二楼走廊回来
    return { TileMap::TileToPixel(7), TileMap::TileToPixel(3) };
}

sf::Vector2f Scene_Stairwell2::GetSavePosition() {
    return { 7.5f * TILE_SIZE, TileMap::TileToPixel(4) };  // 打印机左2.5格
}

void Scene_Stairwell2::CheckExits() {
    sf::Vector2f pos = m_player.GetPosition();
    int tile = m_tileMap.GetTileAtPixel(pos.x, pos.y);
    int col = TileMap::PixelToTile(pos.x);
    int row = TileMap::PixelToTile(pos.y);
    // 楼梯(cols1-3, rows1-2) → 楼梯间1
    if (col >= 1 && col <= 3 && row >= 1 && row <= 2) {
        m_nextScene  = static_cast<int>(SceneID::Stairwell1);
        m_entryParam = 1;
    }
    // 门 → 二楼走廊(踩上即触发)
    if (static_cast<TileType>(tile) == TileType::Door) {
        m_nextScene  = static_cast<int>(SceneID::Corridor2_1);
        m_entryParam = 0;
    }
}

bool Scene_Stairwell2::OnTileClick(int tile, int col, int row) {
    sf::Vector2f pp = m_player.GetPosition();
    float dx = (col + 0.5f) * TILE_SIZE - pp.x;
    float dy = (row + 0.5f) * TILE_SIZE - pp.y;
    if (std::sqrt(dx * dx + dy * dy) > 160.f) return false;

    // 打印机 (col9-10, row3-4)
    for (const auto& dp : m_printerPositions) {
        (void)dp;
        float bx = 9.f * TILE_SIZE;
        float by = 3.f * TILE_SIZE;
        float bw = 2.f * TILE_SIZE;
        float bh = 2.f * TILE_SIZE;
        float cx = (col + 0.5f) * TILE_SIZE;
        float cy = (row + 0.5f) * TILE_SIZE;
        if (cx >= bx && cx < bx + bw && cy >= by && cy < by + bh) {
            m_savePanel.Show(true);
            return true;
        }
    }
    return false;
}

void Scene_Stairwell2::Draw(sf::RenderWindow& window) {
    // 地图+楼梯+玩家
    window.setView(m_camera);
    sf::Vector2f cam = m_camera.getCenter();
    m_tileMap.Draw(window, cam.x, cam.y);
    for (const auto& spr : m_stairSprites)
        window.draw(*spr);
    for (const auto& spr : m_printerSprites)
        window.draw(*spr);
    m_player.Draw(window);

    // 调试信息
    if (m_font) {
        sf::Vector2f pos = m_player.GetPosition();
        int col = TileMap::PixelToTile(pos.x);
        int row = TileMap::PixelToTile(pos.y);
        int tile = m_tileMap.GetTileAtPixel(pos.x, pos.y);
        auto off = m_player.GetHitOffset();
        std::string info = "col:" + std::to_string(col)
                         + " row:" + std::to_string(row)
                         + " tile:" + std::to_string(tile)
                         + " off:" + std::to_string(static_cast<int>(off.x))
                         + "," + std::to_string(static_cast<int>(off.y));
        sf::Text dbg(*m_font, U(info), 18);
        dbg.setFillColor(sf::Color::White);
        dbg.setOutlineColor(sf::Color::Black);
        dbg.setOutlineThickness(1.f);
        dbg.setPosition({cam.x - SCREEN_W / 2.f + 10.f, cam.y - SCREEN_H / 2.f + 10.f});
        window.draw(dbg);
    }

    // UI层
    {
        sf::View uiView({SCREEN_W / 2.f, SCREEN_H / 2.f}, {SCREEN_W, SCREEN_H});
        window.setView(uiView);
        bool hasModal = IsInDialogue() || m_passwordUI.IsVisible() || m_choiceDialog.IsVisible() || m_savePanel.IsVisible();
        if (hasModal) {
            if (IsInDialogue())            DrawDialogueBox(window);
            if (m_passwordUI.IsVisible())  m_passwordUI.Draw(window);
            if (m_choiceDialog.IsVisible()) m_choiceDialog.Draw(window);
            if (m_savePanel.IsVisible())   m_savePanel.Draw(window);
        }
        if (!IsInDialogue() && m_dialogueTimer <= 0.f) {
            sf::RectangleShape shape;
            const float bx = SCREEN_W - 60.f, by = 10.f, bs = 50.f;
            shape.setSize({bs, bs});
            shape.setPosition({bx, by});
            shape.setFillColor(sf::Color(50, 50, 60, 200));
            shape.setOutlineColor(sf::Color(150, 150, 170));
            shape.setOutlineThickness(2.f);
            window.draw(shape);
            float gcx = bx + bs / 2.f, gcy = by + bs / 2.f;
            sf::CircleShape circle(10.f);
            circle.setFillColor(sf::Color(200, 200, 220));
            circle.setOrigin({10.f, 10.f});
            circle.setPosition({gcx, gcy});
            window.draw(circle);
            for (int i = 0; i < 6; ++i) {
                float angle = i * 3.14159f / 3.f;
                float sx = gcx + std::cos(angle) * 16.f;
                float sy = gcy + std::sin(angle) * 16.f;
                sf::RectangleShape tooth(sf::Vector2f(10.f, 4.f));
                tooth.setFillColor(sf::Color(200, 200, 220));
                tooth.setOrigin({5.f, 2.f});
                tooth.setPosition({sx, sy});
                tooth.setRotation(sf::radians(angle));
                window.draw(tooth);
            }
        }
    }
}
