#version 150

in vec2 outTexCoord;

uniform sampler2D texUnit;
uniform vec2 texelSize; // 1.0 / texture dimensions

out vec4 out_Color;

void main(void)
{
    vec3 sum = vec3(0.0);

    // Loopa över 3x3 kringliggande texlar
    for(int x=-1; x<=1; x++)
        for(int y=-1; y<=1; y++)
        {
            vec2 offset = vec2(x, y) * texelSize;
            sum += texture(texUnit, outTexCoord + offset).rgb;
        }

    sum /= 9.0; // Normalisera
    //out_Color = texture(texUnit, outTexCoord);
    //out_Color = vec4(1.0, 0.0, 0.0, 1.0); // Allt rött - test
    out_Color = vec4(sum, 1.0);
}
