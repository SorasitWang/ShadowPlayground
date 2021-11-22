#version 330 core
out vec4 FragColor;

struct Properties { 
    vec3 diffuse;
    vec3 position;
    vec3 ambient;
    vec3 specular;    
    float shininess;

    float linear;
    float constant;
    float quadratic;
}; 

struct Obj { 
    int num;
    vec3 vertex[1000];
    mat4 modelViewProj;

}; 

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec4 FragPosLightSpace;
    vec4 FragPosLightSpace2;
} fs_in;

uniform vec3 lightPos;
uniform vec3 lightPos2;
uniform vec3 viewPos;
uniform Properties material,light;
uniform sampler2D shadowMapFar;
uniform sampler2D shadowMapNear;
uniform sampler2D posMapFar;
uniform sampler2D posMapNear;
uniform float far_plane;
uniform float alpha;
uniform bool drawShadow;

uniform int numObj;
uniform Obj allObj[10];
//float PointShadowCalculation(vec3 fragPos);



int CalculatProbSameTexture(vec4 far,vec4 near)
{
    for (int i=0 ;i<numObj;i++){

    }
    return 0;
}

float ShadowCalculation(vec4 fragPosLightSpace,sampler2D mapShadowFar,sampler2D mapShadowNear,float bias,vec3 lightP)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    vec4 mapFar = texture(shadowMapFar, projCoords.xy);
    vec4 mapNear = texture(shadowMapNear, projCoords.xy);
    vec4 posFar = texture(posMapFar, projCoords.xy);
    vec4 posNear = texture(posMapNear, projCoords.xy);

    float closestDepth = (1.0*mapFar.r + 1.0*mapNear.r)/2.0;//distance(texture(mapShadow, projCoords.xy).xyz,lightP); 1
   
    //if (abs(dot(mapNear.xyz-fs_in.FragPos,fs_in.Normal)) <= 0.03)
      //  return 0.0;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    //float bias = 0.005*0;// max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    bias = 0.00;
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    CalculatProbSameTexture(posFar,posNear);
    // PCF
    return shadow;
    shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMapFar, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(mapShadowFar, projCoords.xy + vec2(x, y) * texelSize).a; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 10.0)
        shadow = 0.0;
        
    return shadow;
}


vec3 calculate(Properties light,vec3 Normal, vec3 viewPos,vec3 FragPos,vec4 fragPosLightSpace,sampler2D mapShadowFar,sampler2D mapShadowNear,vec3 lightP){

  
    // ambient
   vec3 ambient =  material.ambient * light.ambient;
  	
    // diffuse 
    vec3 norm = normalize(fs_in.Normal);
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);  

   float distance = length(light.position - fs_in.FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    //ambient *= attenuation;
    //diffuse *= attenuation;;
    //specular *= attenuation;

    
    float bias = max(0.05 * (1.0 - dot(norm, lightDir)), 0.005);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    //vec4 mapFar = texture(mapShadowFar, projCoords.xy);
    vec4 mapNear = texture(posMapNear, projCoords.xy);
    if( length(fs_in.FragPos.xyz-mapNear.xyz) < 0.001)// && abs(fs_in.FragPos.z-mapNear.z) < 0.001)
        return vec3(1.0,0.0,0.0);
    float shadow =ShadowCalculation(fragPosLightSpace,mapShadowFar,mapShadowNear,bias,lightP);  // PointShadowCalculation(fs_in.FragPos);
   
    vec3 result = ambient+ ((1.0-shadow) * (diffuse + specular));

   // vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    
    return result;

}


void main()
{
    vec3 result =  calculate(light,fs_in.Normal,viewPos,fs_in.FragPos,fs_in.FragPosLightSpace2,shadowMapFar,shadowMapNear,lightPos2);//calculate(light,fs_in.Normal,viewPos,fs_in.FragPos,fs_in.FragPosLightSpace,shadowMap,lightPos);// +   calculate(light,fs_in.Normal,viewPos,fs_in.FragPos,fs_in.FragPosLightSpace2,shadowMap2,lightPos2);
    
    FragColor = vec4(result/2.0,alpha);
} 

float PointShadowCalculation(vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;
    // ise the fragment to light vector to sample from the depth map    
    float closestDepth = 0;//texture(shadowMap, fragToLight.xy).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    //closestDepth *= far_plane;
    closestDepth = 0.3;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    float bias = 0.005*0; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    //float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;        
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;   
    
    shadow = 0.0;
    
    float samples = 4.0;
    float offset = 0.1;
    for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    {
        for(float y = -offset; y < offset; y += offset / (samples * 0.5))
        {
            for(float z = -offset; z < offset; z += offset / (samples * 0.5))
            {
                float closestDepth = 0;//texture(shadowMap, fragToLight +vec3(x, y, z)).r;
                closestDepth *= far_plane; // undo mapping [0;1]
                if(currentDepth - bias > closestDepth)
                    shadow += 1.0;
            }
        }
    }
    shadow /= (samples * samples * samples);
    return shadow;
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
 
}