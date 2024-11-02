#pragma once

#include <deque>

#include <glm/glm.hpp>

enum Offscreen_Region : u8
{
    REGION_LEFT = 0,
    REGION_RIGHT,
    REGION_TOP,
    REGION_BOTTOM,

    REGION_MAX,
};

class Circle
{
    glm::vec2 get_random_offscreen_point(Offscreen_Region region);
    ImU32     get_random_color();

public:
    Circle();

    glm::vec2             m_pos;
    float                 m_radius;
    float                 m_speed;
    ImU32                 m_color;
    std::deque<glm::vec2> m_path;
    Offscreen_Region      m_starting_region;
    Offscreen_Region      m_ending_region;

    void update();
    bool finished_path();
};
