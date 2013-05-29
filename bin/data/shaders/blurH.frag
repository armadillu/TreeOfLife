#extension GL_ARB_texture_rectangle : enable

uniform float blurLevel;
uniform sampler2DRect texture;


void main(void){

	float blurSize = blurLevel;
	vec4 sum = vec4(0.0);
	vec2 st = gl_TexCoord[0].st;

	sum += texture2DRect(texture, vec2(gl_TexCoord[0].x - 4.0 * blurSize, gl_TexCoord[0].y)) * 0.05;
	sum += texture2DRect(texture, vec2(gl_TexCoord[0].x - 3.0 * blurSize, gl_TexCoord[0].y)) * 0.09;
	sum += texture2DRect(texture, vec2(gl_TexCoord[0].x - 2.0 * blurSize, gl_TexCoord[0].y)) * 0.12;
	sum += texture2DRect(texture, vec2(gl_TexCoord[0].x - blurSize, gl_TexCoord[0].y)) * 0.15;
	sum += texture2DRect(texture, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y)) * 0.2;
	sum += texture2DRect(texture, vec2(gl_TexCoord[0].x + blurSize, gl_TexCoord[0].y)) * 0.15;
	sum += texture2DRect(texture, vec2(gl_TexCoord[0].x + 2.0 * blurSize, gl_TexCoord[0].y)) * 0.12;
	sum += texture2DRect(texture, vec2(gl_TexCoord[0].x + 3.0 * blurSize, gl_TexCoord[0].y)) * 0.09;
	sum += texture2DRect(texture, vec2(gl_TexCoord[0].x + 4.0 * blurSize, gl_TexCoord[0].y)) * 0.05;

	if (sum.a > 0.0) sum.a = 1.0;
	gl_FragColor = sum;
}