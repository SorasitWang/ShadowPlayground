#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;


out VS_OUT {
     vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    vec4 FragPosLightSpace2;
    vec4 Norm;
    mat4 lightSpaceMatrix;

} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix2;
uniform mat4 rotation;
uniform bool isModel;
uniform sampler2D texture_normal1;

void main()
{

    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Norm = rotation * vec4(aNormal, 1.0);

    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;  
   
    vs_out.TexCoords = aTexCoord;
    //if (isModel)
           // vs_out.Normal = texture(texture_normal1,aTexCoord).xyz;  
    vs_out.FragPosLightSpace2 = lightSpaceMatrix2 * vec4(vs_out.FragPos, 1.0);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vs_out.lightSpaceMatrix = lightSpaceMatrix2;
}