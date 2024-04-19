#ifndef HASHHELPER_H
#define HASHHELPER_H
#include <glm/vec2.hpp>
#include <string>

template <>
struct std::hash<glm::ivec2> {
    std::size_t operator()(const glm::ivec2& k) const noexcept {

        using std::size_t;
        using std::hash;

        return ((hash<int>()(k.x)
            ^ (hash<int>()(k.y) << 1)) >> 1);
    }
};

template <>
struct std::hash<glm::vec2> {
    std::size_t operator()(const glm::vec2& k) const noexcept {

        using std::size_t;
        using std::hash;

        return ((hash<float>()(k.x)
            ^ (hash<float>()(k.y) << 1)) >> 1);
    }
};
#endif