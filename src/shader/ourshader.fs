#version 330 core

out vec4 FragColor;
in float depth;
in vec4 pos;
in vec4 world;
in vec4 glPos;

uniform bool isSecond;
uniform sampler2D posMapNear;

void main()
{        
	//if (setup==true){
		//FragColor = vec4(-1000.0,-1.0,0.0,1.0);
	//}
	//else{
    //if (FragColor.y != -0.5)
	
	//else 
	gl_FragDepth = gl_FragCoord.z;
	if (isSecond){
		if (length(texture(posMapNear, pos.xy).x - gl_FragCoord.z) < 0.05){
		//if (world.x < 0.5)
			gl_FragDepth = 10.0;
			FragColor = vec4(gl_FragCoord.z,world.x,world.z,0.0);
			}

		
			else{
			FragColor = vec4(texture(posMapNear, pos.xy).x,world.x,world.z,world.y+1.0);
			}
		
	}
	else 
	FragColor = vec4(gl_FragCoord.z,world.x,world.z,world.y+1.0);
		//FragColor = vec4(texture(shadowMapNear, pos.xy).xyz,1.0);
	//FragColor = vec4(gl_FragCoord.z,world.xz,world.y+1.0);
	//FragColor = vec4(world.xyz,1.0);
	
	
   //world.x;//gl_FragCoord.x;
}