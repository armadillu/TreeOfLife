#include "testApp.h"


void testApp::setup(){

	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	glDisable(GL_POINT_SMOOTH);
	ofSetSphereResolution(12);

	ofBackground(0);

	SPRING_LENGTH = 30;
	SPRINGINESS = 90;
	REPULSION_FORCE = 50;
	REPULSION_DIST = 180;
	FRICTION = 0.95;

	OFX_REMOTEUI_SERVER_SETUP(10000); 	//start server

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_COLOR( ofColor(128,255,0,32) ); // set a bg color for the upcoming params
	OFX_REMOTEUI_SERVER_SHARE_PARAM(level, 3, NUM_LINE_MESHES);

	OFX_REMOTEUI_SERVER_SHARE_PARAM(SPRINGINESS, 0, 0.05);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(SPRING_LENGTH, 1, 500);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(REPULSION_FORCE, 0, 2);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(REPULSION_DIST, 1, 1500);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(CHILD_REPULSION_DIST, 1, 300);

	OFX_REMOTEUI_SERVER_SHARE_PARAM(FRICTION, 0.5, 1.0);

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_COLOR( ofColor(255,0,128,32) ); // set a bg color for the upcoming params
	OFX_REMOTEUI_SERVER_SHARE_PARAM(repelNNGain, 0, 2);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(repelMyChildrenGain, 0, 1);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(repelChildChildGain, 0, 1);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(repelRootGain, 0, 10);

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_COLOR( ofColor(0,0,255,32) ); // set a bg color for the upcoming params
	OFX_REMOTEUI_SERVER_SHARE_PARAM(treeSpread, 0, 90);


	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_COLOR( ofColor(255) ); // set a bg color for the upcoming params
	OFX_REMOTEUI_SERVER_SHARE_PARAM(drawNames);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(nameFilter, 1, 150);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(updateMesh);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(repellNN);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(drawForces);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(drawSpringForces);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(drawSpheres);



	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_COLOR( ofColor(255,0,255,32) ); // set a bg color for the upcoming params
	OFX_REMOTEUI_SERVER_SHARE_PARAM(lineWidth, 0.1, 10);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(pointSize, 1, 30);

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_COLOR( ofColor(0,255,0,32) ); // set a bg color for the upcoming params
	OFX_REMOTEUI_SERVER_SHARE_PARAM(lineAlpha, 0, 1);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(pointAlpha, 0, 1);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(nameAlpha, 0, 1);

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_COLOR( ofColor(0,0,255,32) ); // set a bg color for the upcoming params
	OFX_REMOTEUI_SERVER_SHARE_PARAM(gpuBlur.blurPasses, 0, 4);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(gpuBlur.blurOffset, 0.0, 10);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(gpuBlur.blurOverlayGain, 0, 255);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(gpuBlur.numBlurOverlays, 0, 7);

	OFX_REMOTEUI_SERVER_LOAD_FROM_XML();

	printf("INT_MAX: %d\n######################\n", INT_MAX);

	TIME_SAMPLE_DISABLE_AVERAGE();
	//TIME_SAMPLE_START("parseCSV");
	parser.parseCSV( ofToDataPath("pruned-names.csv"), speciesAll );
	//TIME_SAMPLE_STOP("parseCSV");

	//TIME_SAMPLE_START("filterDuplicates");
	parser.filterDuplicates(speciesAll, nodesByName);
	//TIME_SAMPLE_STOP("filterDuplicates");

	//TIME_SAMPLE_START("buildTree");
	treeRoot = (Node*)parser.buildTree(nodesByName);
	treeRoot->fixed = true;
	treeRoot->color = ofColor(ofRandom(255), ofRandom(255), ofRandom(255), ALPHA);
	//TIME_SAMPLE_STOP("buildTree");

	for(int i = 0; i < NUM_LINE_MESHES; i++){
		lines[i].setMode(OF_PRIMITIVE_LINES);
	}
	nodes.setMode(OF_PRIMITIVE_POINTS);
	forces.setMode(OF_PRIMITIVE_LINES);

	int thisLevel = 0;
	int maxLevel = level;
	pLevel = level;
	treeRoot->pos = ofVec3f();
	recursiveFillVectorAndSprings(treeRoot, thisLevel, maxLevel, chosenNodes, springs);
	cout << "chosenNodes at maxLevel " << maxLevel << " : " << chosenNodes.size() << endl;
	cout << "num Springs: " << springs.size() << endl;


	// blur ///////////////////////////

	ofFbo::Settings s;
	s.width = 2048;
	s.height = 1024;
	s.internalformat = GL_RGBA;
	//	s.textureTarget = GL_TEXTURE_RECTANGLE_ARB;
	s.maxFilter = GL_LINEAR; GL_NEAREST;
	s.numSamples = 4;
	s.numColorbuffers = 3;
	s.useDepth = true;
	s.useStencil = false;

	gpuBlur.setup(s);

	
}

void testApp::fillMesh(vector<Node*> &chosenNodes, ofMesh & ptsMesh){

	int n = chosenNodes.size();
	for(int i = 0; i < n; i++) {
		Node* me = chosenNodes[i];
		if (!me->softLeaf){
			int nn = me->children.size();
			//ofSeedRandom(me->level + 1 ); //force repeated random sequence TODO
			//ofColor c = ofColor(ofRandom(255), ofRandom(255), ofRandom(255), ALPHA);
			for(int j = 0; j < nn; j++) {
				int lev = me->children[j]->level;
				float percent = 0.2 + 0.8 * ( (level - lev) / (float)level ); // 0.4 fixed alpha, up tp +0.6 depending on level
				ofColor c1 = me->color;
				c1 *= lineAlpha * percent;
				lines[lev].addColor( c1 * lineAlpha);
				lines[lev].addVertex( me->pos );
				ofColor c2 = me->children[j]->color;
				c2 *= lineAlpha * percent;
				lines[lev].addColor(c2);
				lines[lev].addVertex( me->children[j]->pos );
			}
		}
		//ofSeedRandom(me->level);
		ofColor c = me->color;
		c.a *= pointAlpha;
		ptsMesh.addColor(c);
		ptsMesh.addVertex(me->pos);
	}
}


void testApp::calcForces(vector<Node*> &chosenNodes, vector<Spring*> &springs){

	int n = chosenNodes.size();

	for(int i = 0; i < n; i++) {

		Node* me = chosenNodes[i]; //each node repells each other, ALL NODES!

		
		//me->addForce( ofVec3f(0, gravityGain, 0) );
		//if (me->level < 3){
			me->addRepulsion(treeRoot, REPULSION_FORCE, REPULSION_DIST * 100000, repelRootGain);
		//}

		if(repellNN){
			for(int l = 0; l < n; l++) {
				if (i != l){
					Node* me2 = chosenNodes[l];
					if ( me != treeRoot && me2 != treeRoot){
						me->addRepulsion(me2, REPULSION_FORCE, REPULSION_DIST, repelNNGain);
						//me2->addRepulsion(me, REPULSION_FORCE, REPULSION_DIST, repelNNGain);
					}
				}
			}
		}

		int numChild = me->children.size();

		for( int j = 0; j < numChild; j++ ){

			Node* ch1 = me->children[j];
			ch1->addRepulsion(me, REPULSION_FORCE, CHILD_REPULSION_DIST, repelMyChildrenGain); // I repel my own children
			me->addRepulsion(ch1, REPULSION_FORCE, CHILD_REPULSION_DIST, repelMyChildrenGain ); // I repel my own children

			if (!me->softLeaf){ //if not a leaf, apply forces children to to children
				for( int k = 0; k < numChild; k++ ){
					Node* ch2 = me->children[k];
					ch1->addRepulsion(ch2, REPULSION_FORCE, CHILD_REPULSION_DIST, repelChildChildGain); // my children repel each other
					ch2->addRepulsion(ch1, REPULSION_FORCE, CHILD_REPULSION_DIST, repelChildChildGain); // my children repel each other
				}
			}
		}
	}

	n = springs.size();
	for(int i = 0; i < n; i++) {
		Spring * s = springs[i];
		s->applyForces(); //applies spring force to its attached nodes
	}
}


void testApp::updateNodeForces(vector<Node*> &chosenNodes){

	int n = chosenNodes.size();
//	for(int i = 0; i < n; i++) {
//		chosenNodes[i]->applyForces(DT, FRICTION);
//	}

	for(int i = n-1; i >= 0; i--) {
		chosenNodes[i]->applyForces(DT, FRICTION);
	}

}



void testApp::recursiveFillVectorAndSprings(Node * node, int &level, int maxLevel, vector<Node*> &chosenNodes, vector<Spring*> &springs){

	chosenNodes.push_back(node);
	int n = node->children.size();
	level++;
	node->level = level;
	ofVec2f angles;
	//node->setRandomPosAccordingToLevel(); //atempt to have a nice startup arrangement
	if (level >= maxLevel){
		node->softLeaf = true;
		level--;
		return;
	}
	node->softLeaf = false;
	ofColor c = ofColor(ofRandom(255), ofRandom(255), ofRandom(255), ALPHA);
	for(int i = 0; i < n; i++) {
		Node* child = node->children[i];
		child->color = c;
		Spring * s = new Spring(node, child, &SPRING_LENGTH, &SPRINGINESS);
		springs.push_back(s);
		if (node == treeRoot){
			child->setRandomPosAccordingToLevel(); //atempt to have a nice startup arrangement
		}else{
			ofVec3f dir = node->pos - node->parents[0]->pos;
			child->spreadGivenFatherAndDirection(node->pos, dir, treeSpread );
		}

		recursiveFillVectorAndSprings(child, level, maxLevel, chosenNodes, springs);
	}
	level--;
}


void testApp::update(){

	OFX_REMOTEUI_SERVER_UPDATE(DT);

	TIME_SAMPLE_START("update");

	bool newTree = false;
	if (level != pLevel){
		newTree = true;
		for(int i = 0; i < springs.size(); i++) {
			delete springs[i];
		}
		springs.clear();
		chosenNodes.clear();
		
		int thisLevel = 0;
		int maxLevel = level;
		recursiveFillVectorAndSprings(treeRoot, thisLevel, maxLevel, chosenNodes, springs);
		cout << "chosenNodes at maxLevel " << maxLevel << " : " << chosenNodes.size() << endl;
		cout << "num Springs: " << springs.size() << endl;
		pLevel = level;
	}

	if (updateMesh || newTree){
		for(int i = 0; i < NUM_LINE_MESHES; i++){
			lines[i].clear();
		}
		nodes.clear();

		//reset forces
		int n = chosenNodes.size();
		for(int i = 0; i < n; i++) {
			chosenNodes[i]->resetForce(); //once we filld the mesh, we are done
		}

		TIME_SAMPLE_START("calcForces");
		calcForces(chosenNodes, springs);
		TIME_SAMPLE_STOP("calcForces");
		updateNodeForces(chosenNodes);

		TIME_SAMPLE_START("fillMesh");
		fillMesh(chosenNodes, nodes);
		TIME_SAMPLE_STOP("fillMesh");

	}

	TIME_SAMPLE_STOP("update");
}


void testApp::draw(){

	glPointSize(pointSize);
	ofSetLineWidth(lineWidth);
	cam.setFarClip(10000);

	TIME_SAMPLE_START("draw");

	ofEnableBlendMode(OF_BLENDMODE_ADD);
	//draw into fbo
	ofSetColor(255);
	gpuBlur.beginDrawScene();
		ofClear(0, 0, 0, 0);
		cam.begin();
//			ofSetColor(255, ALPHA * 0.5);
//			lines.draw();
			nodes.draw();
		cam.end();
	gpuBlur.endDrawScene();

	//calc the blur
	ofSetColor(255);
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	gpuBlur.performBlur();

	//draw clean scene
	ofSetColor(255);
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	gpuBlur.drawSceneFBO();
	ofEnableBlendMode(OF_BLENDMODE_ADD);

	//overlay the blur
	gpuBlur.drawBlurFbo(); 
	
	//draw the lines after?
	cam.begin();
		ofSetColor(255, lineAlpha);
		for(int i = 0; i <= level; i++){
			float percent = (level - i) / (float)(level + 1);
			glLineWidth(1 + lineWidth * percent);
			lines[i].drawWireframe();
		}
		if(drawNames){
			ofSetColor(255, 255 * nameAlpha);
			for(int i = 0; i < chosenNodes.size(); i++){
				if (chosenNodes[i]->children.size() > nameFilter){
					ofDrawBitmapString(chosenNodes[i]->name, chosenNodes[i]->pos);
				}
			}
		}
		ofSetColor(255);
		ofDrawBitmapString("ROOT", treeRoot->pos);

		if (drawForces){
			glLineWidth(1);
			forces.clear();
			int n = chosenNodes.size();
			float gain = 10;
			for(int i = 0; i < n; i++) {
				//normal
				forces.addColor(ofColor(128,0,0));
				forces.addVertex(chosenNodes[i]->pos);
				forces.addColor(ofColor(128,0,0,0));
				forces.addVertex(chosenNodes[i]->pos + chosenNodes[i]->force * gain * 10);

				//spring
				if (drawSpringForces){
					for(int k = 0; k < chosenNodes[i]->springForces.size(); k++ ){
						forces.addColor(ofColor(128,128,0));
						forces.addVertex(chosenNodes[i]->pos);
						forces.addColor(ofColor(128,128,0,0));
						forces.addVertex(chosenNodes[i]->pos + chosenNodes[i]->springForces[k] * gain);
					}
				}
			}
			forces.draw();
		}
	ofEnableAlphaBlending();
	if(drawSpheres){
		ofSetColor(255,12);
		ofNoFill();
		glLineWidth(1);
		for(int i = 0; i < level; i++){
			ofSphere(treeRoot->pos, 100*i);
		}
	}
	cam.end();

	TIME_SAMPLE_STOP("draw");

	ofSetColor(64);
	TIME_SAMPLE_DRAW_TOP_LEFT();
}


void testApp::keyPressed(int key){

	pLevel = -1;
}


void testApp::keyReleased(int key){

}


void testApp::mouseMoved(int x, int y){

	//treeRoot->pos.x = x;
	//treeRoot->pos.y = y;
}


void testApp::mouseDragged(int x, int y, int button){

}


void testApp::mousePressed(int x, int y, int button){

}


void testApp::mouseReleased(int x, int y, int button){

}

void testApp::exit(){
	OFX_REMOTEUI_SERVER_SAVE_TO_XML();
}
