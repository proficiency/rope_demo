#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>
#include <imgui.h>

#include "circles.h"

class Rope;

class Node
{
    friend class Rope;

protected:
    // nodes spawned by clicking don't have physics
    bool m_static;

public:
    Node() = default;
    Node(bool is_static, const glm::vec2& pos);
    void simulate();
    void constrain(Node& next_node);
    void collide(const Circle& circle);

    glm::vec2 m_pos;
    glm::vec2 m_last_pos;
    glm::vec2 m_velocity;

    static constexpr float m_rest_length = 10.f;
};

class Rope
{
    void spawn_circles();

public:
    Rope();
    void simulate();
    void draw();

    std::vector<Node>   m_nodes;
    std::vector<Circle> m_circles;
};
