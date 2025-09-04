#version 150

in  vec3  in_Position;
in  vec3  in_Normal;
in  vec2  in_TexCoord;

uniform mat4 projectionMatrix;
uniform mat4 modelToWorldToView;
uniform float uTime;

//out float shade;
out vec3 vPos;
out vec3 vNormal;
out vec2 frag_texcoord;

void main(void)
{
	//original pos
	vec3 p = in_Position;

	//---- WAVE DEFORMATION --- 
	//simple deformation along y-axis - sin wave solution 
	float amplitude = 0.05;
    float k = 5.0;
    float speed = 2.0;
    p += normalize(in_Normal) * amplitude * sin(k * p.y + speed * uTime);

	// --- To frag shader ---
    vPos = (modelToWorldToView * vec4(p,1.0)).xyz;
    vNormal = normalize(mat3(modelToWorldToView) * in_Normal);

	// ---- Texture -----
	frag_texcoord = in_TexCoord;

    gl_Position = projectionMatrix * vec4(vPos,1.0);


	//-----ORIGINAL CODE -------
	//shade = (mat3(modelToWorldToView)*in_Normal).z; // Fake shading
	//gl_Position=projectionMatrix*modelToWorldToView*vec4(in_Position, 1.0);
}


