#version 150

in vec2 outTexCoord;
uniform sampler2D texUnit;   // originalbild
uniform sampler2D texUnit2;  // blur av highlights
out vec4 out_Color;

void main(void)
{
    vec3 original = texture(texUnit, outTexCoord).rgb;
    vec3 bloom = texture(texUnit2, outTexCoord).rgb;
    out_Color = vec4(original + bloom, 1.0); // addera
    //out_Color = original + bloom;
}
