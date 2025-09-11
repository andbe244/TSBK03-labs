#version 150

in vec2 outTexCoord;
uniform sampler2D texUnit;
uniform float threshold; // t.ex. 1.0
out vec4 out_Color;

void main(void)
{
    vec3 col = texture(texUnit, outTexCoord).rgb;

    // Smooth threshold: alla värden under threshold → 0, över threshold → gradvis
    vec3 t = smoothstep(vec3(threshold), vec3(threshold + 0.5), col);

    out_Color = vec4(col * t, 1.0);
}
