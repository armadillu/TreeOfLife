
uniform sampler2DRect tex;
uniform float width;
uniform float height;


float step_w = 1.0 ;
float step_h = 1.0 ;


#define KERNEL_SIZE 9
vec2 offset[KERNEL_SIZE];


void main(){
	
	vec4 color = texture2DRect(tex, gl_TexCoord[0].st);
	
	offset[0] = vec2(-step_w, -step_h);
	offset[1] = vec2(0.0, -step_h);
	offset[2] = vec2(step_w, -step_h);

	offset[3] = vec2(-step_w, 0.0);
	offset[4] = vec2(0.0, 0.0);
	offset[5] = vec2(step_w, 0.0);

	offset[6] = vec2(-step_w, step_h);
	offset[7] = vec2(0.0, step_h);
	offset[8] = vec2(step_w, step_h);	

	float maskMin = 1.0;

	vec4 maxSample = color;
	for(int i = 0; i < KERNEL_SIZE; i++){
	
   		vec2 newCoord = gl_TexCoord[0].st + offset[i];
   		vec4 sample = texture2DRect(tex, newCoord.st); 
      	if ( sample.r > maxSample.r && sample.g > maxSample.g && sample.b > maxSample.b){
      		maxSample = sample;
      		maxSample.a = 1.0;
      	}
	}
		gl_FragColor = maxSample;


}

