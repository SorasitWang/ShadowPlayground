#version 330 core

out vec4 FragColor;
in float depth;
in vec4 pos;
in vec4 world;
in vec4 glPos;

uniform bool isSecond;
uniform sampler2D posMapNear;

void mai()
{
// gl_FragDepth
}
void main()
{        
	//if (setup==true){
		//FragColor = vec4(-1000.0,-1.0,0.0,1.0);
	//}
	//else{
    //if (FragColor.y != -0.5)
	
	//else 
	float rat = 1.0;
    float offset = 0.001;
    float plus = 0.0;
	gl_FragDepth = gl_FragCoord.z;
	FragColor = vec4(gl_FragCoord.z,0,0,1.0);
	if (isSecond){
		vec3 mapNear = texture(posMapNear,pos.xy).xyz;
		//FragColor = vec4(1.0,0.0,0.0,1.0);
		if  (((abs(rat*mapNear.x-plus - world.x) <= offset) && (abs(rat*mapNear.y-plus - world.y) <=offset)  
		&& (abs(rat*mapNear.z-plus - world.z) <= offset))  ){
		//if (world.x < 0.5)
			gl_FragDepth = 1.0;
			}
			}
		
	
		//FragColor = vec4(texture(shadowMapNear, pos.xy).xyz,1.0);
	//FragColor = vec4(gl_FragCoord.z,world.xz,world.y+1.0);
	//FragColor = vec4(world.xyz,1.0);
	
	
   //world.x;//gl_FragCoord.x;
}