#include <array>
#include <print>

#include "render.h"
#include "rng.h"
#include "circles.h"

#include <imgui_internal.h>

glm::vec2 Circle::get_random_offscreen_point(Offscreen_Region region)
{
    const ImRect screen_area = {g_render->m_min, g_render->m_max};

    glm::vec2 offscreen{};
    switch (region)
    {
        case 0u:
        {
            offscreen.x = g_rng.get_random(screen_area.Min.x - 100.0f, screen_area.Min.x - m_radius);
            offscreen.y = g_rng.get_random(screen_area.Min.y, screen_area.Max.y);
            break;
        }

        case 1u:
        {
            offscreen.x = g_rng.get_random(screen_area.Max.x + m_radius, screen_area.Max.x + 100.0f);
            offscreen.y = g_rng.get_random(screen_area.Min.y, screen_area.Max.y);
            break;
        }

        // top
        case 2u:
        {
            offscreen.x = g_rng.get_random(screen_area.Min.x, screen_area.Max.x);
            offscreen.y = g_rng.get_random(screen_area.Max.y + m_radius, screen_area.Max.y + 100.0f);
            break;
        }

        // bottom
        case 3u:
        {
            offscreen.x = g_rng.get_random(screen_area.Min.x, screen_area.Max.x);
            offscreen.y = g_rng.get_random(screen_area.Min.y - 100.0f, screen_area.Min.y - m_radius);
            break;
        }

        default:
            std::unreachable();
    }

    return offscreen;
}

ImU32 Circle::get_random_color()
{
    const float h = g_rng.get_random(0.0f, 1.0f);
    const float s = g_rng.get_random(0.5f, 1.0f);
    const float v = g_rng.get_random(0.8f, 1.0f);

    float r, g, b;
    ImGui::ColorConvertHSVtoRGB(h, s, v, r, g, b);

    return IM_COL32(r * 255, g * 255, b * 255, 255);
}


Circle::Circle() : m_path()
{
    std::array<glm::vec2, 4u> control_points = 
    {
    };

    // pick a random color
    m_color = get_random_color();

    // pick a random radius
    m_radius = g_rng.get_random(13.0f, 35.0f);

    // pick a random speed
    m_speed = g_rng.get_random(5.0f, 15.0f);

    // pick a random side of the screen area to start from
    m_starting_region = (Offscreen_Region)g_rng.get_random<u32>(REGION_LEFT, REGION_BOTTOM);

    // set our starting position to a random point offscreen
    m_pos = get_random_offscreen_point(m_starting_region);

    // get our destination region
    do
    {
        m_ending_region = (Offscreen_Region) g_rng.get_random<u32>(REGION_LEFT, REGION_BOTTOM);
    }
    while (m_ending_region == m_starting_region);

    // get our destination point
    const glm::vec2 dst = get_random_offscreen_point(m_ending_region);

    // start and end points
    control_points[0] = m_pos;
    control_points[3] = dst;

    // points along the path
    control_points[1] = glm::vec2((m_pos.x + dst.x) * 0.5f, m_pos.y);
    control_points[2] = glm::vec2((m_pos.x + dst.x) * 0.5f, dst.y);

    constexpr u32 num_nodes = 64u;
    for (u32 i = 1; i <= num_nodes; ++i)
    {
        const float t = float(i) / num_nodes;

        m_path.push_back(ImBezierCubicCalc(control_points[0], control_points[1], control_points[2], control_points[3], t));
    }
}

void Circle::update()
{
    while (!m_path.empty() && glm::distance(m_pos, m_path.front()) < m_radius)
        m_path.pop_front();

    if (m_path.empty())
        return;

    const glm::vec2 dir      = glm::normalize(m_path.front() - m_pos);
    const glm::vec2 velocity = dir * m_speed;

    m_pos += velocity * ImGui::GetIO().DeltaTime;

    g_render->get_dl("bg")->AddCircleFilled(m_pos, m_radius, m_color);
}

bool Circle::finished_path()
{
    return m_path.empty();
}
