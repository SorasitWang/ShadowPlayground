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
    mat4 model;

}; 

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    vec4 FragPosLightSpace2;
} fs_in;

#define NR 5
uniform vec3 lightPos;
uniform vec3 lightPos2;
uniform vec3 viewPos;
uniform Properties material,light;
uniform sampler2D shadowMapFar;
uniform sampler2D shadowMapBack;
uniform sampler2D shadowMapNear;
uniform sampler2D posMapFar;
uniform sampler2D posMapNear;
uniform float far_plane;
uniform float alpha;
uniform bool drawShadow;
uniform mat4 modelObj;
uniform int numObj;
uniform Obj allObj[5];
uniform Obj test;
uniform float setBias;
uniform int depthMode;
uniform bool isModel;
uniform bool useNormal;
//float PointShadowCalculation(vec3 fragPos);

bool notSamePos(vec3 a,vec3 b){
    return length(a-b) > 0.001;
}
float signedVolume(vec3 a,vec3 b,vec3 c,vec3 d){
       return sign(dot(cross(b-a,c-a),d-a));
}

vec4 findIntersection(vec3 q1,vec3 q2,vec3 p1,vec3 p2,vec3 p3)
{

    float s1 =  signedVolume(q1,p1,p2,p3); 
    float s2 =  signedVolume(q2,p1,p2,p3); 

    if (s1 != s2){

        float s3 = signedVolume(q1,q2,p1,p2);
        float s4 = signedVolume(q1,q2,p2,p3);
        float s5 = signedVolume(q1,q2,p3,p1);
        if (s3==s4 && s4 == s5){
           vec3 n = cross(p2-p1,p3-p1);
           float t = -dot(q1-p1, n) / dot(q2 - q1,n);
           return vec4(q1+t*(q2-q1),0.0);
           //return vec4(0.0);
        } 
    }
    return vec4(-1.0);
}

bool checkInRange(vec3 a,vec3 b,vec3 p)
{
    float offset = 0.000;
    if ((min(a.x,b.x) <= p.x + offset && max(a.x,b.x) >= p.x -offset)
    && (min(a.y,b.y) <= p.y + offset&& max(a.y,b.y) >= p.y-offset)
    && (min(a.z,b.z) <= p.z + offset&& max(a.z,b.z) >= p.z-offset))
        return true;
    return false;
    

}

int CalculateProbSameTexture(vec3 now,vec3 near)
{
    //return 0;
    float rat = 5.0;
    float offset = 0.01;
    float plus = 1.5;

   // if (allObj[2].model == mat4(0.0)) return 1;
    vec4 a,b,c,re;
    int i =4;
    int count = 0;
    //for (int i=0 ;i<5;i++){
       // vec3 u = allObj[i].vertex[0];
        //int uu = allObj[i].num;
        //mat4 r = allObj[i].model;
        for (int j=0;j<test.num;j+=3){
            a =  test.model *  vec4(test.vertex[j],1.0);
           b = test.model  *vec4(test.vertex[j+1],1.0);
            c = test.model  *vec4(test.vertex[j+2],1.0);
            //a = (a+plus)/rat;
            //b = (b+plus)/rat;
            //c = (b+plus)/rat;
            re = findIntersection(now,near,a.xyz,b.xyz,c.xyz);
            if (re.w == 0.0){
                if (checkInRange(now,near,re.xyz) && notSamePos(re.xyz,now) && notSamePos(re.xyz,near)){
                    count += 1;
                    //if (count>1)
                   return 0 ;
               }
            }
       }
        //}
    
    //if (count > 2) return 1;
    return -1;
}

float ShadowCalculation(vec4 fragPosLightSpace,sampler2D mapShadowFar,sampler2D mapShadowNear,float bias,vec3 lightP)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    vec4 mapBack = texture(shadowMapBack, projCoords.xy);
    vec4 mapFar= texture(shadowMapFar, projCoords.xy);
    vec4 mapNear = texture(shadowMapNear, projCoords.xy);
    vec4 posFar = texture(posMapFar, projCoords.xy);
    vec4 posNear = texture(posMapNear, projCoords.xy);
    float closestDepth = 0.0;
    bias = 0.0001;
     float rat = 5.0;
    float offset = 0.01;
    float plus = 1.5;
    if (depthMode == 1){ //normal
        closestDepth= mapNear.r;//distance(texture(mapShadow, projCoords.xy).xyz,lightP); 1
        bias = setBias;
    }
    else if (depthMode == 2){
        closestDepth = (1.0*mapNear.r + 1.0*mapBack.r )/2.0;
    }
    else {
        closestDepth = min((1.0*mapNear.r + 1.0*mapFar.r )/2.0,mapNear.r+0.005);
    }
    //if (abs(dot(mapNear.xyz-fs_in.FragPos,fs_in.Normal)) <= 0.03)
      //  return 0.0;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
   // vec3 normal = normalize(fs_in.Normal);
    //vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    //float bias = 0.005*0;// max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    //if(projCoords.z > 5.0)
      //  return 0.0;
    posNear.xyz = rat*posNear.xyz - plus;
    //if (length(posNear.xyz-fs_in.FragPos) < 0.1)
      //  return CalculateProbSameTexture(fs_in.FragPos,posNear.xyz);//posNear.xyz
    // PCF
    //return 1.0;
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

    //if (allObj[2].model == mat4(0.0)) return vec3(0.0);
    // ambient
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    vec4 mapNear = texture(posMapNear, projCoords.xy);
    float rat = 5.0;
    float offset = 0.01;
    float plus = 1.5;
    if ((abs(rat*mapNear.x-plus - fs_in.FragPos.x) <= offset) && (abs(rat*mapNear.y-plus - fs_in.FragPos.y) <=offset)  && (abs(rat*mapNear.z-plus - fs_in.FragPos.z) <= offset)  ){
       // return vec3(1.0,0.0,0.0);
    }
    //if( length(projCoords.z-mapNear.xyz) < 0.1)// && abs(fs_in.FragPos.z-mapNear.z) < 0.001)
        //return vec3(1.0,0.0,0.0);
    //return vec3(fs_in.FragPos.x) ;//vec3(projCoords.z,fs_in.FragPos.xz);
    float bias = 0.05;
    float shadow =0;// ShadowCalculation(fragPosLightSpace,mapShadowFar,mapShadowNear,bias,lightP);
    //return vec3(1-shadow);
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

    
    bias = max(0.05 * (1.0 - dot(norm, lightDir)), 0.005);
    projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    //vec4 mapFar = texture(mapShadowFar, projCoords.xy);
    vec4 posNear = texture(posMapNear, projCoords.xy);
    //if( length(fs_in.FragPos.xyz-mapNear.xyz) < 0.001)// && abs(fs_in.FragPos.z-mapNear.z) < 0.001)
        //return vec3(1.0,0.0,0.0);
    shadow = ShadowCalculation(fragPosLightSpace,mapShadowFar,mapShadowNear,bias,lightP);  // PointShadowCalculation(fs_in.FragPos);
    vec3 result;
         result = ambient+ ((1.0-shadow)* (diffuse*0 + specular));
         result = (1.0-shadow)*vec3(0.0,0.7,0.3);


   // mapNear.x = rat*mapNear.x-plus;
   // mapNear.y = rat*mapNear.y-plus;
   // mapNear.z = rat*mapNear.z-plus;
   // float x = (fs_in.FragPos.x+plus)/rat;
    //float y = (fs_in.FragPos.y+plus)/rat;
    //float z = (fs_in.FragPos.z+plus)/rat;
    posNear.xyz = rat*posNear.xyz - plus;
    int check = -1;
    if (length(posNear.xyz-fs_in.FragPos) < 0.2){
        //check = CalculateProbSameTexture(fs_in.FragPos,posNear.xyz);//posNear.xyz
        }
    if ( check == 0){
        return vec3(1.0,0,0);
    }
    if (check == 1) {
        return vec3(0.0,1.0,0.0);
    }
    if (check ==2){
       return vec3(0.0,0.0,1.0);
    }
    if(check==3){
        return vec3(1.0,1.0,0);
    }
    if (check == 4){
        return vec3(0,1.0,1.0);
    }
    
   // vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    

    return result;//CalculateProbSameTexture(fs_in.FragPos, mapNear.xyz);

}


void main()
{
   
    vec3 result =  calculate(light,fs_in.Normal,viewPos,fs_in.FragPos,fs_in.FragPosLightSpace2,shadowMapFar,shadowMapNear,lightPos2);//calculate(light,fs_in.Normal,viewPos,fs_in.FragPos,fs_in.FragPosLightSpace,shadowMap,lightPos);// +   calculate(light,fs_in.Normal,viewPos,fs_in.FragPos,fs_in.FragPosLightSpace2,shadowMap2,lightPos2);
    
    FragColor = vec4(result,1.0);
   
    
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
