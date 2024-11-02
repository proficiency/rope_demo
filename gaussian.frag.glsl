#version 330 core

uniform vec2      u_resolution;
uniform sampler2D u_tex;

in vec2 uv;

out vec4 out_col;

float get_gaussian_coeficient(vec2 offset, float kernel_size)
{
    return exp(-(offset.x * offset.x + offset.y * offset.y) / (2.0 * kernel_size * kernel_size));
}

vec4 get_gaussian_blur(sampler2D tex, vec2 st, vec2 direction, const int kernel_size)
{
    // 1 / (2*PI)
    const float kernel_norm = 0.15915494;

    // the color/weight of each pixel within the kernel 
    float total_weight = 0.0;
    vec4  total_color  = 0.0;

    // current offset within the kernel from the center pixel
    vec2 offset = vec2(0.0);
    for (int i = 0; i < kernel_size; ++i)
    {
        offset.y = -0.5 * (float(kernel_size) - 1.0) + float(i);

        for (float n = 0; n < kernel_size; ++n)
        {
            offset.x = -0.5 * (float(kernel_size) - 1.0) + float(n);

            // calculate gaussian weight
            const float weight = (kernel_norm / float(kernel_size)) * get_gaussian_coeficient(offset, kernel_size);            

            total_weight += weight;
            total_color  += weight * texture2D(u_tex, st + offset * direction);
        }
    }

    // normalize the color
    return total_color / total_weight;
}

void main() 
{
    vec2 pixel = 1.0 / u_resolution;
    vec2 st = gl_FragCoord.xy * pixel;

    out_col = get_gaussian_blur(u_tex, st, pixel, 16.0);
}
