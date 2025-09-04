#version 150

in vec3 vPos;
in vec3 vNormal;
in vec2 frag_texcoord;

out vec4 out_Color;

uniform vec3 viewPos;       // kamera
uniform vec3 ambientColor;  // globalt omgivningsljus
uniform int numLights;      // antal ljus
uniform vec3 lightPos[4];   // positioner på ljuskällor
uniform vec3 lightColor[4]; // färger på ljuskällor
uniform float shininess;    // specular styrka
uniform sampler2D exampletexture;

void main(void)
{
	vec3 texColor = texture(exampletexture, frag_texcoord).rgb; // färg från textur
    vec3 N = normalize(vNormal);
    vec3 V = normalize(viewPos - vPos);
    vec3 color = ambientColor;

    for(int i=0; i<numLights; i++){
        vec3 L = normalize(lightPos[i] - vPos);        // ljus riktning
        vec3 R = reflect(-L, N);                       // reflektionsriktning

        float diff = max(dot(N,L),0.0);                // Lambert
        float spec = pow(max(dot(R,V),0.0), shininess); // Blänk

        color += diff*texColor*lightColor[i] + spec*lightColor[i];
    }
	
    out_Color = vec4(color,1.0);
}
