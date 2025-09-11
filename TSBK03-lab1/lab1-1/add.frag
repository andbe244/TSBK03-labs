#version 150

in vec2 outTexCoord;
uniform sampler2D texUnit;   // originalbild
uniform sampler2D texUnit2;  // blur av highlights
out vec4 out_Color;

void main(void)
{
    vec3 c1 = texture(texUnit, outTexCoord).rgb;
    vec3 c2 = texture(texUnit2, outTexCoord).rgb;
    out_Color = vec4(c1 + c2, 1.0); // addera
}
