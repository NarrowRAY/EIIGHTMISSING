// 按钮裁剪工具 — 从 mainpage1.png 抠出三个按钮
// 用法: crop_buttons.exe <x1> <y1> <w1> <h1> <x2> <y2> <w2> <h2> <x3> <y3> <w3> <h3>
#include <SFML/Graphics.hpp>
#include <cstdlib>

int main(int argc, char* argv[]) {
    if (argc != 13) {
        printf("Usage: crop_buttons.exe x1 y1 w1 h1 x2 y2 w2 h2 x3 y3 w3 h3\n");
        return 1;
    }

    int coords[12];
    for (int i = 0; i < 12; ++i) coords[i] = std::atoi(argv[i + 1]);

    sf::Image src;
    if (!src.loadFromFile("assets/textures/mainpage1.png")) {
        printf("Failed to load mainpage1.png\n");
        return 1;
    }

    const char* names[3] = {"btn_newgame.png", "btn_continue.png", "btn_exit.png"};

    for (int i = 0; i < 3; ++i) {
        int x = coords[i * 4], y = coords[i * 4 + 1];
        int w = coords[i * 4 + 2], h = coords[i * 4 + 3];

        sf::IntRect rect({x, y}, {w, h});

        // 先把按钮区域复制出来，再把主图对应位置变透明
        // 这样按钮素材是镂空背景 + 按钮本体
        sf::Image btn;
        btn.create({static_cast<unsigned>(w), static_cast<unsigned>(h)}, sf::Color::Transparent);

        for (int dy = 0; dy < h; ++dy) {
            for (int dx = 0; dx < w; ++dx) {
                sf::Color c = src.getPixel({static_cast<unsigned>(x + dx), static_cast<unsigned>(y + dy)});
                btn.setPixel({static_cast<unsigned>(dx), static_cast<unsigned>(dy)}, c);
            }
        }

        std::string path = std::string("assets/textures/") + names[i];
        if (btn.saveToFile(path)) {
            printf("Saved %s (%dx%d)\n", path.c_str(), w, h);
        } else {
            printf("Failed to save %s\n", path.c_str());
        }
    }

    return 0;
}
