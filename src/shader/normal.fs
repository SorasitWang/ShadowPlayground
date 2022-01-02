#version 330 core

out vec4 FragColor;
in vec4 normal;
in vec4 world;
in vec4 pos;
uniform bool isSecond;
uniform sampler2D posMapNear;
void main()
{        

    float offset = 0.001;

	gl_FragDepth = gl_FragCoord.z;
	FragColor =  vec4(normal.xyz,1.0);
	if (isSecond){
		vec3 mapNear = texture(posMapNear,pos.xy).xyz;
		//FragColor = vec4(1.0,0.0,0.0,1.0);
		if  (((abs(mapNear.x - world.x) <= offset) && (abs(mapNear.y- world.y) <=offset)  
		&& (abs(mapNear.z - world.z) <= offset))  ){
		//if (world.x < 0.5)
			gl_FragDepth = 1.0;
			}
			}
		//if (texture(shadowMapNear, pos.xy).x  <= gl_FragCoord.z+0.001)
		
			//FragColor = vec4(gl_FragCoord.z+1.0,world.xy,world.z+1.0);
		
		//else		FragColor = texture(shadowMapNear, pos.xy);
	//else FragColor = vec4(gl_FragCoord.z,world.xy,world.z+1.0);
		//FragColor = vec4(texture(shadowMapNear, pos.xy).xyz,1.0);
	//FragColor = vec4(gl_FragCoord.z,world.xz,world.y+1.0);
	//FragColor = vec4(1.0);s

	
	
}