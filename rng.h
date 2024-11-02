#pragma once

#include <random>

#include <glm/glm.hpp>

#include "types.h"

class RNG
{
    std::random_device m_rd;
    std::mt19937       m_gen;

    template <typename T>
    using Distribution = std::conditional_t<std::is_integral_v<T>, std::uniform_int_distribution<T>, std::uniform_real_distribution<T>>;

    template <typename T>
    Distribution<T> get_distribution(T min, T max)
    {
        static_assert(std::is_arithmetic_v<T>, "arithmetic types are required for rng");
        return Distribution<T>(min, max);
    }

public:
    RNG() : m_gen(m_rd()) {}

    template <typename T>
    T get_random(T min, T max)
    {
        return get_distribution<T>(min, max)(m_gen);
    }

    glm::vec2 get_random(glm::vec2 min, glm::vec2 max)
    {
        return glm::vec2(get_random(min.x, max.x), get_random(min.y, max.y));
    }

    glm::vec4 get_random(glm::vec4 min, glm::vec4 max)
    {
        return glm::vec4(get_random(min.x, max.x), get_random(min.y, max.y), get_random(min.z, max.z), get_random(min.w, max.w));
    }
};

inline RNG g_rng;
