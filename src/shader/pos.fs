#version 330 core

out vec4 FragColor;
in vec4 world;


void main()
{        
	//if (setup==true){
		//FragColor = vec4(-1000.0,-1.0,0.0,1.0);
	//}
	//else{
    //if (FragColor.y != -0.5)
	//if (isSecond){
		//if (texture(shadowMapNear, pos.xy).x  <= gl_FragCoord.z+0.001)
		
			//FragColor = vec4(gl_FragCoord.z+1.0,world.xy,world.z+1.0);
		
		//else		FragColor = texture(shadowMapNear, pos.xy);
	//else FragColor = vec4(gl_FragCoord.z,world.xy,world.z+1.0);
		//FragColor = vec4(texture(shadowMapNear, pos.xy).xyz,1.0);
	//FragColor = vec4(gl_FragCoord.z,world.xz,world.y+1.0);
	//FragColor = vec4(1.0);s
	float rat = 5.0;
	float offset = 1.5;
	FragColor =  vec4((world.x+offset )/rat,(world.y+offset )/rat,(world.z+offset )/rat,1.0);
	
   gl_FragDepth = gl_FragCoord.z;//world.x;//gl_FragCoord.x;
}