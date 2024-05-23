#include "render.h"

// todo: particle system heavily blurred in the background

int main()
{
    g_render = std::make_shared<Render>();
    g_render->run();

    return 0;
}