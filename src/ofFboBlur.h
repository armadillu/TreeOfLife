/*
 *  ofFboBlur.h
 *  emptyExample
 *
 *  Created by Oriol Ferrer Mesià on 26/03/12.
 *  Copyright 2012 uri.cat. All rights reserved.
 *
 */

#include "ofMain.h"

class ofFboBlur{
	
public:
	
	void setup(){
		shaderV.load("shaders/blur.vert", "shaders/blurV.frag");
		shaderH.load("shaders/blur.vert", "shaders/blurH.frag");
	}
	
	void blur( ofFbo * input, ofFbo * output, ofFbo * buffer, ofFbo * buffer2, int iterations, int blurOffset  ){


		if( iterations > 0 ){

			buffer->begin();
				ofClear(0, 0, 0, 0);
			buffer->end();

			buffer2->begin();
				ofClear(0, 0, 0, 0);
			buffer2->end();

			ofEnableAlphaBlending();
			for (int i = 0; i < iterations; i++) {

				buffer->begin();
					shaderV.begin();
					if (i == 0){ //for first pass, we use input as src; after that, we retro-feed the output of the 1st pass
						shaderV.setUniformTexture( "texture", input->getTextureReference(), 0 );
					}else{
						shaderV.setUniformTexture( "texture", buffer2->getTextureReference(), 1 );
					}
					shaderV.setUniform1f("blurLevel", blurOffset * (i + 1) );
					input->draw(0,0);
					shaderV.end();
				buffer->end();

				buffer2->begin();
					shaderH.begin();
					shaderH.setUniformTexture( "texture", buffer->getTextureReference(), 2 );
					shaderH.setUniform1f("blurLevel", blurOffset * (i + 1) );
					buffer->draw(0,0);
					shaderH.end();
				buffer2->end();
			}
			//draw back into original fbo

			output->begin();
				ofClear(0, 0, 0, 0);
				buffer2->draw(0, 0);
			output->end();

		}else{
			output->begin();
				ofClear(0, 0, 0, 0);
				input->draw(0, 0);
			output->end();

		}
	}

	ofShader shaderV;
	ofShader shaderH;
};