#version 330 core

out vec4 FragColor;
in float depth;
in vec4 pos;
in vec4 world;
in vec4 glPos;


void main()
{        
	//if (setup==true){
		//FragColor = vec4(-1000.0,-1.0,0.0,1.0);
	//}
	//else{
    //if (FragColor.y != -0.5)
	
	//else 
	FragColor = vec4(gl_FragCoord.z,glPos.z,world.x,world.y+1.0);
		//FragColor = vec4(texture(shadowMapNear, pos.xy).xyz,1.0);
	//FragColor = vec4(gl_FragCoord.z,world.xz,world.y+1.0);
	//FragColor = vec4(world.xyz,1.0);
	
	
   //gl_FragDepth = gl_FragCoord.z;//world.x;//gl_FragCoord.x;
}