#version 150

in vec2 outTexCoord;
uniform sampler2D texUnit;
uniform vec2 texelSize;
out vec4 out_Color;


void main(void)
{
    const float w0 = 1.0;
    const float w1 = 4.0;
    const float w2 = 7.0;
    const float size = (w0 + w1 + w2 + w1 + w0);

    vec3 result = vec3(0.0);

    // Vertical blur: offset in y direction
    result += w0 * texture(texUnit, outTexCoord - vec2(0.0, 2.0 * texelSize.y)).rgb;
    result += w1 * texture(texUnit, outTexCoord - vec2(0.0, texelSize.y)).rgb;
    result += w2 * texture(texUnit, outTexCoord).rgb;
    result += w1 * texture(texUnit, outTexCoord + vec2(0.0, texelSize.y)).rgb;
    result += w0 * texture(texUnit, outTexCoord + vec2(0.0, 2.0 * texelSize.y)).rgb;

    out_Color = vec4(result / size, 1.0);
}
