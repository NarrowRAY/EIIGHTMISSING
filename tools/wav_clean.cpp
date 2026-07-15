// 清理 WAV 元数据，确保 SFML 能加载
// VS 里添加这个 cpp 编译后运行 wav_clean.exe
#include <SFML/Audio.hpp>
#include <fstream>

int main() {
    const char* names[] = {"nomai", "kazu", "timber"};
    for (auto name : names) {
        std::string path = std::string("assets/audio/music/") + name + ".wav";
        sf::SoundBuffer buf;
        if (!buf.loadFromFile(path)) { printf("FAILED: %s\n", path.c_str()); continue; }

        const sf::Int16* s = buf.getSamples();
        sf::Uint64 n = buf.getSampleCount();
        unsigned rate = buf.getSampleRate(), ch = buf.getChannelCount();

        std::string out = std::string("assets/audio/music/") + name + ".wav";
        std::ofstream f(out, std::ios::binary);
        auto w32 = [&](sf::Uint32 v) { f.write((const char*)&v, 4); };
        auto w16 = [&](sf::Uint16 v) { f.write((const char*)&v, 2); };
        sf::Uint32 ds = (sf::Uint32)(n * sizeof(sf::Int16));
        f.write("RIFF", 4); w32(36 + ds);
        f.write("WAVE", 4);
        f.write("fmt ", 4); w32(16);
        w16(1); w16((sf::Uint16)ch); w32(rate);
        w32(rate * ch * 2); w16((sf::Uint16)(ch * 2)); w16(16);
        f.write("data", 4); w32(ds);
        f.write((const char*)s, ds);
        printf("OK: %s\n", out.c_str());
    }
    return 0;
}
