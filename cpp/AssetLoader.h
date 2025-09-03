#pragma once
#include <cassert>
#include <map>
#include <string>
#include "Assert.h"
#include "raylib.h"
#include <fstream>
#include <vector>

#define ASSETS_PATH "C:\\Users\\asd\\Desktop\\flap\\assets"

#if defined(PLATFORM_WEB)
#define ASSETS_PATH "assets"
#endif


class AssetLoader {
private:
    std::map<std::string, Texture2D> textures;
    std::map<std::string, Sound> sounds;

    AssetLoader() = default;
    AssetLoader(const AssetLoader&) = delete;
    AssetLoader& operator=(const AssetLoader&) = delete;


    Sound loadSoundFromFileMemory(const std::string& filepath) {
    // Open file in binary mode
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    assert(file.is_open());

    // Get file size
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read file into buffer
    std::vector<unsigned char> buffer(size);
    assert(file.read((char*)buffer.data(), size));

    // Load Wave from memory
    Wave wave = LoadWaveFromMemory(".wav", buffer.data(), buffer.size());

    // Load Sound from Wave
    Sound sound = LoadSoundFromWave(wave);

    // Unload Wave, keep Sound
    UnloadWave(wave);

    return sound;
}

public:
    static AssetLoader& getInstance() {
        static AssetLoader instance;
        return instance;
    }

    // Load textures and sounds
    inline void load() {

        InitAudioDevice();

        // Textures
        const std::pair<std::string, std::string> textureFiles[] = {
            {"pipe-down", "pipe-down.png"},
            {"pipe-up", "pipe-up.png"},
            {"background", "background.png"},
            {"bird-1","yellowbird-upflap.png"},
            {"bird-2","yellowbird-midflap.png"},
            {"bird-3","yellowbird-downflap.png"}
        };
        for (auto& [key, filename] : textureFiles) {
            std::string fullpath = std::string(ASSETS_PATH) + "/" + filename;
            textures[key] = LoadTexture(fullpath.c_str());
        }

        // Sounds
        const std::pair<std::string, std::string> soundFiles[] = {
            {"flap", "sfx_wing.wav"},
            {"hit", "sfx_hit.wav"},
            {"score", "sfx_point.wav"}
        };
        for (auto& [key, filename] : soundFiles) {
            std::string fullpath = std::string(ASSETS_PATH) + "/" + filename;
            sounds[key] = LoadSound(fullpath.c_str());
        }
    }

    // Get texture
    inline Texture2D getTex(const std::string& key) {
        assert(textures.find(key) != textures.end());
        Texture2D& tex = textures[key];
        ASSERT(tex.id != 0,"failed to get texture file %s",key.c_str());
        return tex;
    }

    // Get sound
    inline Sound getSound(const std::string& key) {
        assert(sounds.find(key) != sounds.end());
        Sound& s = sounds[key];
        ASSERT(s.frameCount > 0,"failed to get sound file %s",key.c_str());
        return s;
    }

    // Unload everything
    inline void unload() {
        for (auto& [key, texture] : textures) {
            UnloadTexture(texture);
        }
        textures.clear();

        for (auto& [key, sound] : sounds) {
            UnloadSound(sound);
        }
        sounds.clear();
    }
};
