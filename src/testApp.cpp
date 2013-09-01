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

	for(int i = 0; i < 10000; i++){
		colors.push_back( ofColor(ofRandom(255), ofRandom(255), ofRandom(255)) );
	}

	// blur ///////////////////////////

	ofFbo::Settings s;
	s.width = ofGetWidth();
	s.height = ofGetHeight();
	s.internalformat = GL_RGBA;
	//	s.textureTarget = GL_TEXTURE_RECTANGLE_ARB;
	s.maxFilter = GL_LINEAR; GL_NEAREST;
	s.numSamples = 4;
	s.numColorbuffers = 3;
	s.useDepth = true;
	s.useStencil = false;

	gpuBlur.setup(s);
	
	// params

	OFX_REMOTEUI_SERVER_SETUP(10000); 	//start server

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_COLOR( ofColor(0,0,0,32) ); // set a bg color for the upcoming params
	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("TREE");
	OFX_REMOTEUI_SERVER_SHARE_PARAM(level, 3, NUM_LINE_MESHES);

	vector<string> tsl; tsl.push_back("TREE_2D"); tsl.push_back("TREE_2_3_D");tsl.push_back("TREE_3D");
	tsl.push_back("TREE_3D_CONE"); tsl.push_back("TREE_3D_CONE2");
	OFX_REMOTEUI_SERVER_SHARE_ENUM_PARAM(treeStyle, 0, NUM_TREE_STYLES-1,tsl );

	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("SPRINGS");
	OFX_REMOTEUI_SERVER_SHARE_PARAM(SPRINGINESS, 0, 0.05);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(SPRING_LENGTH, 1, 500);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(REPULSION_FORCE, 0, 2);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(REPULSION_DIST, 1, 200);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(CHILD_REPULSION_DIST, 1, 300);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(FRICTION, 0.5, 1.0);

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("REPEL FORCE");
	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(repelNNGain, 0, 2);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(repelMyChildrenGain, 0, 1);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(repelChildChildGain, 0, 1);

	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("CALCULATIONS");
	OFX_REMOTEUI_SERVER_SHARE_PARAM(liveRePositioning);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(updateMesh);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(repellNN);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(addSprings);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(adaptSpringsToSkeleton);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(calcChildForces);

	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("TREE PARAMS");
	OFX_REMOTEUI_SERVER_SHARE_PARAM(treeSpread, 0, 3);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(treeWidth, 0, 3);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(treeHeight, 0, 3);

	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("DEBUG");

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("VISUAL TWEAKS");
	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(drawNames);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(nameFilter, 1, 150);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(nameAlpha, 0, 1);

	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(lineWidth, 0.1, 10);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(lineAlpha, 0, 1);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(adapativeLineWidth);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(blurLines);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(pointAlpha, 0, 1);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(pointSize, 1, 30);

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("BLUR OVERLAY");
	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
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


	// setup tree pointer
	treePointer = nodesByName["arthropoda"];
	vector<Node*> myChildren;
	vector<Node*> myParents;
	tempTree.clear();
	tempTree.push_back(treePointer);

	int level = 2;
	gatherParentsToLevel(treePointer, myParents, level);
	gatherChildrenToLevel(treePointer, myChildren, level);

	for(int i = 0; i < myChildren.size(); i++){
		tempTree.push_back(myChildren[i]);
	}
	for(int i = 0; i < myParents.size(); i++){
		tempTree.push_back(myParents[i]);
	}

	//springs
	int thisLevel = 0;
	int maxLevel = level;
	recursiveFillVectorAndSprings(treeRoot, thisLevel, maxLevel, tempTree, springs);
	maxLevel = -level;
	recursiveFillVectorAndSpringsParents(treeRoot, thisLevel, maxLevel, tempTree, springs);


	//blah
	linesMesh.setMode(OF_PRIMITIVE_LINES);
	pointsMesh.setMode(OF_PRIMITIVE_POINTS);
	nodes.setMode(OF_PRIMITIVE_POINTS);
	lines.setMode(OF_PRIMITIVE_LINES);

}


void testApp::gatherLeaves( const vector<Node*> &nodes, vector<Node*> &leaves){

	leaves.clear();
	for (int i = 0; i < nodes.size(); i++) {
		if(nodes[i]->softLeaf){
			leaves.push_back(nodes[i]);
		}
	}

	for (int i = 0; i < leaves.size(); i++) {
		Node* me = leaves[i];
		me->deepestLevel = me->level;
		int deepestLevel = me->level;
		while (me->parents.size() > 0) {
			if(me == me->parents[0]){
				break;
			}
			me = me->parents[0];
			if (me->deepestLevel < deepestLevel) me->deepestLevel = deepestLevel;			
		}
	}
}


int testApp::countChildren( Node * node , int & numLeaves){

	if (node->softLeaf){
		node->totalChildren = 0;
		node->totalLeaves = 0;
		numLeaves ++;
		return node->totalChildren;
	}

	int n = node->children.size();
	int nc = n;
	int prevL = numLeaves;
	for(int i = 0; i < n; i++) {
		Node* child = node->children[i];
		nc += countChildren(child, numLeaves);
	}
	int l = 0;
	for(int i = 0; i < n; i++) {
		l += node->children[i]->totalLeaves;
	}

	node->totalLeaves = numLeaves - prevL;
	node->totalChildren = nc;
	return nc;
}


void testApp::gatherParentsToLevel(Node* node, vector<Node*> &parents, int levels){

	if (levels == 0) return;
	if (node->getParent() == NULL) return;
	parents.push_back(node);
	gatherParentsToLevel(node, parents, levels-1);
}


void testApp::gatherChildrenToLevel(Node* node, vector<Node*> &children, int levels){

	if (levels == 0) return;
	if (node->children.size() == 0) return;
	for(int i = 0; i < node->children.size(); i++){
		children.push_back(node->children[i]);
		gatherChildrenToLevel(node->children[i], children, levels-1);
	}
}


void testApp::recursiveTreeInfoBuild(Node * node, int cLevel){

	int n = node->children.size();
	node->level = cLevel;
	cLevel++;
	//node->setRandomPosAccordingToLevel(); //atempt to have a nice startup arrangement
	if (n == 0){
		return;
	}
	node->softLeaf = false;

	for(int i = 0; i < n; i++) {
		Node* child = node->children[i];
		recursiveTreeInfoBuild(child, cLevel);
	}
	cLevel--;
}


void testApp::setupSprings(vector<Node*> &nodes,  vector<Spring*> &springs_){


	for(int i = 0; i < nodes.size; i++) {
		Node* child = node->children[i];

		if(addSprings){
			Spring * s = new Spring(node, child, &SPRING_LENGTH, &SPRINGINESS);
			springs_.push_back(s);
		}

		recursiveFillVectorAndSprings(child, cLevel, maxLevel, chosenNodes, springs);
	}
	cLevel--;
}



void testApp::fillMesh(vector<Node*> &chosenNodes, ofMesh & ptsMesh, ofMesh & linesMesh_){

	int n = chosenNodes.size();

	for(int i = 0; i < n; i++) {
		Node* me = chosenNodes[i];
		if (!me->softLeaf){
			for(int j = 0; j < me->children.size(); j++) {
				linesMesh_.addColor( ofColor::red );
				linesMesh_.addVertex( me->pos );
				linesMesh_.addColor(ofColor::red);
				linesMesh_.addVertex( me->children[j]->pos );
			}
		}
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

		if(calcChildForces){
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
	}

	n = springs.size();
	for(int i = 0; i < n; i++) {
		springs[i]->applyForces(); //applies spring force to its attached nodes
	}
}


void testApp::updateNodeForces(vector<Node*> &chosenNodes){

	int n = chosenNodes.size();
	for(int i = n-1; i >= 0; i--) {
		chosenNodes[i]->applyForces(DT, FRICTION);
	}
}

void testApp::drawDynamicTree(){

	vector<Node*> parents;
	vector<Node*> children;
	int levels = 3;
	gatherParentsToLevel(treePointer, parents, levels);
	gatherChildrenToLevel(treePointer, children, levels);

	linesMesh.clear();
	pointsMesh.clear();

	//fill lines mesh for children
	for(int i = 0; i < children.size(); i++){
		Node * n = children[i];
		for(int i = 0; i < n->children.size(); i++){
			pointsMesh.addVertex(n->pos);
			linesMesh.addVertex(n->pos);
			linesMesh.addVertex(n->children[i]->pos);
		}
	}
	linesMesh.draw();
	pointsMesh.draw();

}

void testApp::forceBasedLayout(Node* startingNode, vector<Node*>nodes){

	vector<Spring*> springs;
	
	for(int i = 0; i < nodes.size(); i++){

	}
}

void testApp::update(){

	OFX_REMOTEUI_SERVER_UPDATE(DT);

	TIME_SAMPLE_START("update");

	nodes.clear();
	lines.clear();

	TIME_SAMPLE_START("calcForces");
	calcForces(tempTree, springs);
	TIME_SAMPLE_STOP("calcForces");
	updateNodeForces(tempTree);


	TIME_SAMPLE_START("fillMesh");
	fillMesh(tempTree, nodes, lines);
	TIME_SAMPLE_STOP("fillMesh");

	TIME_SAMPLE_STOP("update");
}



void testApp::draw(){

	glPointSize(pointSize);
	ofSetLineWidth(lineWidth);
	cam.setFarClip(100000);

	TIME_SAMPLE_START("draw");

	ofEnableBlendMode(OF_BLENDMODE_ADD);
	//draw into fbo
	ofSetColor(255);
	gpuBlur.beginDrawScene();
		ofClear(0, 0, 0, 0);
		cam.begin();

			nodes.draw();
			lines.draw();
			//drawDynamicTree();

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
		if(drawNames){
			ofSetColor(255, 255 * nameAlpha);
		}
		ofSetColor(255);
		ofDrawBitmapString("\nROOT", treeRoot->pos);

	ofEnableAlphaBlending();

	//ofDrawAxis(50);
	cam.end();

	TIME_SAMPLE_STOP("draw");

	ofSetColor(64);
	TIME_SAMPLE_DRAW_TOP_LEFT();
}


void testApp::keyPressed(int key){

	colors.clear();
	for(int i = 0; i < 10000; i++){
		colors.push_back( ofColor(ofRandom(255), ofRandom(255), ofRandom(255)) );
	}

	pLevel = -1;
}


void testApp::keyReleased(int key){
}

void testApp::mouseMoved(int x, int y){
}

void testApp::mouseDragged(int x, int y, int button){
}

void testApp::mousePressed(int x, int y, int button){
}

void testApp::mouseReleased(int x, int y, int button){
}

void testApp::exit(){
	OFX_REMOTEUI_SERVER_SAVE_TO_XML();
	OFX_REMOTEUI_SERVER_CLOSE();
}
