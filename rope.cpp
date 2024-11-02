#include <print>

#include "render.h"
#include "rope.h"

// largely based off of https://www.cs.cmu.edu/afs/cs/academic/class/15462-s13/www/lec_slides/Jakobsen.pdf

Node::Node(bool is_static, const glm::vec2& pos) : m_static(is_static), m_pos(pos), m_last_pos(pos), m_force() {}

void Node::simulate()
{
    // we're a static node, don't need physics
    if (m_static)
        return;

    const float timestep = ImGui::GetIO().DeltaTime;

    constexpr glm::vec2 gravity = glm::vec2(0.0f, 70.0f);

    m_velocity = m_pos - m_last_pos;
    m_last_pos = std::exchange(m_pos, m_pos + (m_velocity + gravity) * timestep);
}

void Node::constrain(Node& next_node)
{
    const glm::vec2 dir  = m_pos - next_node.m_pos;
    const float     dist = glm::length(dir);

    // just incase we try to divide by zero
    if (dist < 1e-6f)
        return;

    const float     diff   = (dist - m_rest_length) / dist;
    const glm::vec2 offset = dir * 0.5f * diff;

    if (!m_static)
        m_pos = glm::clamp(m_pos - offset, (glm::vec2)g_render->m_min, (glm::vec2)g_render->m_max);

    if (!next_node.m_static)
        next_node.m_pos = glm::clamp(next_node.m_pos + offset, (glm::vec2)g_render->m_min, (glm::vec2)g_render->m_max);
}

void Node::collide(const Circle& circle)
{
    // static nodes don't have collision
    if (m_static)
        return;

    const glm::vec2 dir  = m_pos - circle.m_pos;
    const float     dist = glm::length(dir);

    // are we colliding with the circle?
    if (dist > circle.m_radius)
        return;

    m_pos += dir * ((circle.m_radius - dist) / dist);
}

Rope::Rope() : m_nodes()
{
    glm::vec2 pos = (g_render->m_max - g_render->m_min) * 0.5f;
    for (u32 i = 0u; i < 30u; ++i)
    {
        pos.x += float(i) * Node::m_rest_length;
        m_nodes.push_back(Node(i == 0, pos));
    }
}

void Rope::simulate()
{
    const glm::vec2  mouse_pos      = ImGui::GetMousePos();
    static glm::vec2 last_mouse_pos = mouse_pos;
    const glm::vec2  delta          = mouse_pos - std::exchange(last_mouse_pos, mouse_pos);

    spawn_circles();

    // update & draw all the circles
    for (u32 i = 0; i < m_circles.size(); ++i)
    {
        auto& circle = m_circles[i];

        if (circle.finished_path())
        {
            m_circles.erase(m_circles.begin() + i);
            continue;
        }

        circle.update();
    }

    for (u32 i = 0u; i < m_nodes.size() - 1u; ++i)
    {
        auto& node      = m_nodes[i];
        auto& next_node = m_nodes[i + 1u];

        if (node.m_static)
            node.m_pos = mouse_pos;

        // collide all the circles against the nodes of our rope
        for (auto& circle : m_circles)
            node.collide(circle);

        // perform verlet integration, apply gravity, etc
        node.simulate();

        for (u32 iter = 1u; iter <= 16u; ++iter)
            node.constrain(next_node);

        g_render->get_dl("game")->PathLineTo(node.m_pos);
    }
}

void Rope::draw()
{
    g_render->get_dl("game")->PathStroke(IM_COL32_WHITE, 0, 4.0f);
}

void Rope::spawn_circles()
{
    static double time_since_spawn = ImGui::GetTime();

    // do we have too many circles already?
    if (m_circles.size() >= 32u)
        return;

    // if there's no circles alive or it's been long enough since the last one was spawned, spawn one
    if (m_circles.empty() || ImGui::GetTime() - time_since_spawn > 0.25)
    {
        m_circles.push_back(Circle());
        time_since_spawn = ImGui::GetTime();
    }
}
