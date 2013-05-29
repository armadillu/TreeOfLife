#extension GL_ARB_texture_rectangle : enable

uniform float blurLevel;
uniform sampler2DRect texture;

 
void main(void){

	float blurSize = blurLevel ;
	vec4 sum = vec4(0.0); 
	
	sum += texture2DRect(texture, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y - 4.0 * blurSize)) * 0.05;
	sum += texture2DRect(texture, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y - 3.0 * blurSize)) * 0.09;
	sum += texture2DRect(texture, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y - 2.0 * blurSize)) * 0.12;
	sum += texture2DRect(texture, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y - blurSize)) * 0.15;
	sum += texture2DRect(texture, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y)) * 0.2;
	sum += texture2DRect(texture, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + blurSize)) * 0.15;
	sum += texture2DRect(texture, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + 2.0 * blurSize)) * 0.12;
	sum += texture2DRect(texture, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + 3.0 * blurSize)) * 0.09;
	sum += texture2DRect(texture, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + 4.0 * blurSize)) * 0.05;
	
	if (sum.a > 0.0) sum.a = 1.0;
	gl_FragColor = sum;
}