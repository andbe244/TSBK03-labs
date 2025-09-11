#version 150

in vec2 outTexCoord;
uniform sampler2D texUnit;
uniform vec2 texelSize;
out vec4 out_Color;

void main(void)
{
    const float w0 = 1;
    const float w1 = 4;
    const float w2 = 7;
    const float size = (w0 + w1 + w2 + w1 + w0);

    vec tex_offset = 1.0 / texelSize(texUnit, 0);
    vec3 result = vec3(0.0);

    result += w0 * texture(texUnit, outTexCoord - vec2(0.0, 2*tex_offset.x)).rgb;
    result += w1 * texture(texUnit, outTexCoord - vec2(0.0, tex_offset.x)).rgb;
    result += w2 * texture(texUnit, outTexCoord).rgb;
    result += w1 * texture(texUnit, outTexCoord - vec2(0.0, tex_offset.x)).rgb;
    result += w0 * texture(texUnit, outTexCoord - vec2(0.0, 2*tex_offset.x)).rgb;

    out_Color = vec4(result/size, 1.0);

    //vec3 sum = vec3(0.0);

    // // Horisontell 3-tap
    //for(int x=-1; x<=1; x++)
    //{
    //    vec2 offset = vec2(float(x), 0.0) * texelSize;
    //    sum += texture(texUnit, outTexCoord + offset).rgb;
    //}

    //sum /= 3.0;
    //outColor = vec4(sum, 1.0);
}
