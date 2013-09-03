#include "testApp.h"


void testApp::setup(){

	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	glDisable(GL_POINT_SMOOTH);
	ofSetSphereResolution(12);

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

	nodeFollowOffsetAnimation.setDuration(0.7);
	nodeFollowOffsetAnimation.setCurve(TANH);
	
	// params

	OFX_REMOTEUI_SERVER_SETUP(10000); 	//start server

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_COLOR( ofColor(0,0,0,32) ); // set a bg color for the upcoming params
	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("TREE");
	OFX_REMOTEUI_SERVER_SHARE_PARAM(numLevels, 1,4);

	vector<string> tsl; tsl.push_back("TREE_2D"); tsl.push_back("TREE_2_3_D");tsl.push_back("TREE_3D");
	tsl.push_back("TREE_3D_CONE"); tsl.push_back("TREE_3D_CONE2");
	OFX_REMOTEUI_SERVER_SHARE_ENUM_PARAM(treeStyle, 0, NUM_TREE_STYLES-1,tsl );

//	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
//	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("SPRINGS");
//	OFX_REMOTEUI_SERVER_SHARE_PARAM(SPRINGINESS, 0, 0.05);
//	OFX_REMOTEUI_SERVER_SHARE_PARAM(SPRING_LENGTH, 1, 500);
//	OFX_REMOTEUI_SERVER_SHARE_PARAM(REPULSION_FORCE, 0, 2);
//	OFX_REMOTEUI_SERVER_SHARE_PARAM(REPULSION_DIST, 1, 200);
//	OFX_REMOTEUI_SERVER_SHARE_PARAM(CHILD_REPULSION_DIST, 1, 300);
//	OFX_REMOTEUI_SERVER_SHARE_PARAM(FRICTION, 0.5, 1.0);

//	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("REPEL FORCE");
//	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
//	OFX_REMOTEUI_SERVER_SHARE_PARAM(repelNNGain, 0, 2);
//	OFX_REMOTEUI_SERVER_SHARE_PARAM(repelMyChildrenGain, 0, 1);
//	OFX_REMOTEUI_SERVER_SHARE_PARAM(repelChildChildGain, 0, 1);

//	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
//	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("CALCULATIONS");
//	OFX_REMOTEUI_SERVER_SHARE_PARAM(liveRePositioning);
//	OFX_REMOTEUI_SERVER_SHARE_PARAM(updateMesh);
//	OFX_REMOTEUI_SERVER_SHARE_PARAM(repellNN);
//	OFX_REMOTEUI_SERVER_SHARE_PARAM(addSprings);
//	OFX_REMOTEUI_SERVER_SHARE_PARAM(adaptSpringsToSkeleton);
//	OFX_REMOTEUI_SERVER_SHARE_PARAM(calcChildForces);

	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("TREE PARAMS");
	OFX_REMOTEUI_SERVER_SHARE_PARAM(treeSpread, 0.1, 1);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(treeAngleOffset, 0.1, 1);

	OFX_REMOTEUI_SERVER_SHARE_PARAM(treeWidth, 0, 3);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(treeHeight, 0, 3);

	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("DEBUG");

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("VISUAL TWEAKS");
	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(drawNames);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(nameFilter, 0, 150);
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(bgColor);
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(nameColor);
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(lineColor);
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(nodeColor);


	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(lineWidth, 0.1, 10);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(lineAlpha, 0, 1);

//	OFX_REMOTEUI_SERVER_SHARE_PARAM(adapativeLineWidth);
//	OFX_REMOTEUI_SERVER_SHARE_PARAM(blurLines);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(pointAlpha, 0, 1);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(pointSize, 1, 30);
	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(fov, 20, 80);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(camDist, 300, 2000);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(camDelay, 0.0, 1.0);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(camTravelDuration, 0.5, 15.0);



	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("BLUR OVERLAY");
	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(gpuBlur.blurPasses, 0, 4);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(gpuBlur.blurOffset, 0.0, 10);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(gpuBlur.blurOverlayGain, 0, 255);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(gpuBlur.numBlurOverlays, 0, 7);

	OFX_REMOTEUI_SERVER_LOAD_FROM_XML();
	//OFX_REMOTEUI_SERVER_START_THREADED(); //careful here!

	printf("INT_MAX: %d\n######################\n", INT_MAX);

	TIME_SAMPLE_START("parseCSV");
	parser.parseCSV( ofToDataPath("pruned-names.csv"), speciesAll );
	TIME_SAMPLE_STOP("parseCSV");

	TIME_SAMPLE_START("postProcessTree");
	parser.filterDuplicates(speciesAll, nodesByName);

	treeRoot = (Node*)parser.buildTree(nodesByName);
	//treeRoot->fixed = true;
	treeRoot->color = ofColor(ofRandom(255), ofRandom(255), ofRandom(255), ALPHA);
	TIME_SAMPLE_STOP("postProcessTree");

	
	recursiveTreeInfoBuild(treeRoot, 0); //fills in each node level for the whole tree
	vector<Node*> treeLeaves;
	//gatherLeaves(treeRoot, treeLeaves); //fill in treeLeaves vector with all tree leaves
	//cout << "gatherLeaves ended " << endl;
	//updateNodeDepths(treeLeaves);		//update each node's deepestLevel value, so you can guess how large is that branch
	cout << "updateNodeDepths ended " << endl;
	int numLeaves = 0;
	countChildren(treeRoot, numLeaves); //update each node's # of cummulative children
	cout << "countChildren ended " << endl;
	position2DTree(treeRoot, 1000, false/*temp version*/); //position for all nodes!
	cout << "position2DTree " << endl;

	for(int i = 0; i < speciesAll.size(); i++){ //store all full tree positions
		speciesAll[i]->absPos = speciesAll[i]->pos;
	}
	
	// setup tree pointer
	//treePointer = nodesByName["arthropoda"];
	treePointer = treeRoot;
	prevTreePointer = NULL;
//	vector<Node*> myChildren;
//	vector<Node*> myParents;
//	tempTree.clear();
//	tempTree.push_back(treePointer);
//
//	int level = 2;
//	gatherParentsToLevel(treePointer, myParents, level);
//	gatherChildrenToLevel(treePointer, myChildren, level);
//
//	for(int i = 0; i < myChildren.size(); i++){
//		tempTree.push_back(myChildren[i]);
//	}
//	for(int i = 0; i < myParents.size(); i++){
//		tempTree.push_back(myParents[i]);
//	}

	//blah
	nodes.setMode(OF_PRIMITIVE_POINTS);
	lines.setMode(OF_PRIMITIVE_LINES);
}


void testApp::recursiveTreeInfoBuild(Node * node, int cLevel){

	node->level = cLevel;
	cLevel++;
	int n = node->children.size();
	if (n == 0){
		return;
	}

	for(int i = 0; i < n; i++) {
		Node* child = node->children[i];
		if (i == 0){
			if(n > 1){
				child->siblingL = NULL;
				child->siblingR = node->children[i+1];
			}
		}else{
			if (i == n-1){
				if (n > 1){
					child->siblingL = node->children[i-1];
					child->siblingR = NULL;
				}
			}else{
				child->siblingL = node->children[i-1];
				child->siblingR = node->children[i+1];
			}
		}
		recursiveTreeInfoBuild(child, cLevel);
	}
}


int testApp::countChildren( Node * node , int & numLeaves){

	if ( node->children.size() == 0){
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


int testApp::countTempChildren( Node * node , int & numLeaves, int level){

	int n = node->children.size();
	if (n == 0 || level <= 1){
		node->tempTotalChildren = 0;
		node->tempTotalLeaves = 1;
		numLeaves ++;
		return 0;
	}

	int nc = n;
	int prevL = numLeaves;
	int l = 0;
	for(int i = 0; i < n; i++) {
		nc += countTempChildren(node->children[i], numLeaves, level - 1);
		l += node->children[i]->tempTotalLeaves;
	}
	node->tempTotalLeaves = numLeaves - prevL;
	node->tempTotalChildren = nc;
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


void testApp::position2DTree( Node * node, int levels, bool tempVersion){

	if(levels < 0){
		return;
	}
	int n = node->children.size();

	float unit = 50.0f * treeWidth;
	int nodeTotalLeaves = node->totalLeaves;
	if(tempVersion) nodeTotalLeaves = node->tempTotalLeaves;
	float totalW =  nodeTotalLeaves * unit;
	float off = -totalW * 0.5f;

	for(int i = 0; i < n; i++){

		Node* child = node->children[i];
		int childTotalLeaves = child->totalLeaves;
		if(tempVersion) childTotalLeaves = child->tempTotalLeaves;
		// flat 2d tree
		child->pos = node->pos + ofVec3f(
										 off + childTotalLeaves * unit * 0.5f ,
										 - treeHeight * 100.0f * (1 + node->level * 0.2),
										 0.0f //r * sin( parentAngle + M_PI * 2 * percent)
										 );

		if (child->children.size() == 0){
			off += unit ;
		}else{
			off += unit * childTotalLeaves ;
			position2DTree(child, levels-1, tempVersion/*temp version*/);
		}
	}
}

void testApp::updateCam(bool camJumped){

	cam.setFarClip(1000000);
	cam.setFov(fov);
	camTarget = nodeFollowOffsetAnimation.getCurrentPosition();
	//camTarget = nodeFollowOffsetAnimation.getTargetPosition();
	//cout << camTarget << endl;
	float r = camDelay;
	if(camJumped){
		camPos = camTarget;
	}else{
		camPos = r * camPos + (1-r) * camTarget;
	}
	cam.setGlobalPosition(camPos + ofVec3f(0,0,camDist));
	cam.setTarget(camTarget);
}


void testApp::update(){

	OFX_REMOTEUI_SERVER_UPDATE(DT);
	nodeFollowOffsetAnimation.update(DT);

	TIME_SAMPLE_START("update");

		int level = numLevels ;

		TIME_SAMPLE_START("layout");

		if(prevTreePointer != treePointer){ //we just started an anim! rebuild tree!


			//find the  starting node //////////////////////////
			topTreePointer = treePointer;
			ofVec3f posOffset;
			float dist = 0;
			if(prevTreePointer){
				posOffset = prevTreePointer->pos - treePointer->pos;
				dist = (prevTreePointer->pos).distance(treePointer->pos);
			}
			int l = level ;
			while( l > 0 ){
				if(topTreePointer->getParent()){
					topTreePointer = topTreePointer->getParent();
				}else{
					break;
				}
				l--;
			}

			topTreePointer->pos = ofVec3f();
			int doubledLevel = level * 2 + 1;
			tempTree.clear();
			int numL = 0;
			countTempChildren(topTreePointer, numL, doubledLevel);
			position2DTree(topTreePointer, doubledLevel, true/*temp version*/);
			//camJumpOffsetFix = -treePointer->pos;
			//cout << "camJumpOffsetFix: "<< camJumpOffsetFix << endl;

			nodes.clear();
			lines.clear();
			fillMeshes(topTreePointer, doubledLevel, nodes, lines, tempTree);

			nodeFollowOffsetAnimation.setDuration( camTravelDuration * ofMap(dist, 10, 5000, 0.1, 1, true ) );
			nodeFollowOffsetAnimation.setPosition( treePointer->pos + posOffset);
			nodeFollowOffsetAnimation.animateTo(treePointer->pos );
			camJumped = true;

			prevTreePointer = treePointer; //done!
		}

		TIME_SAMPLE_STOP("layout");

	TIME_SAMPLE_STOP("update");
}


void testApp::draw(){

	ofBackground(bgColor);

	glPointSize(pointSize);
	ofSetLineWidth(lineWidth);

	//cm
	updateCam(camJumped);
	if(camJumped){
		camJumped = false;
	}

	TIME_SAMPLE_START("draw");

	ofEnableBlendMode(OF_BLENDMODE_ADD);
	//draw into fbo
	ofSetColor(255);
	gpuBlur.beginDrawScene();
		ofClear(0, 0, 0, 0);
		cam.begin();
			nodes.draw();
			lines.draw();

			//current node in cyan
			ofMesh m;
			m.setMode(OF_PRIMITIVE_POINTS);
			m.addVertex(treePointer->pos);
			ofSetColor(ofColor::orange);
			m.draw();
			ofDrawAxis(20);
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

	ofEnableBlendMode(OF_BLENDMODE_ADD);
	cam.begin();
		
		if(drawNames){
			ofSetColor(nameColor);
			for(int i = 0; i < tempTree.size(); i++){
				if (tempTree[i]->children.size() > nameFilter || tempTree[i]->level < 3){
					ofDrawBitmapString(tempTree[i]->name + " L:" + ofToString(tempTree[i]->level), tempTree[i]->pos );
					//ofDrawBitmapString(tempTree[i]->name + " L:" + ofToString(tempTree[i]->level) + " NLeaves:" + ofToString(tempTree[i]->tempTotalLeaves), tempTree[i]->pos);
				}
			}
		}
		//ofDrawBitmapString("ROOT", treePointer->pos);

	ofEnableAlphaBlending();

	cam.end();

	ofSetupScreen();
	TIME_SAMPLE_STOP("draw");

	ofSetColor(128);
	//TIME_SAMPLE_DRAW_BOTTOM_RIGHT();
	if(treePointer){
		ofDrawBitmapStringHighlight(
									"tree Ptr: " + treePointer->name + "\n"
									"anim: " + ofToString(nodeFollowOffsetAnimation.getCurrentPosition()) + "\n" +
									"nodesDrawn: " + ofToString(tempTree.size())
									,20, 20
									);
	}
}

void testApp::fillMeshes(Node* node, int level, ofMesh & ptsMesh, ofMesh & linesMesh, vector<Node*> & drawnNodes){

	if(level == 0){
		return;
	}
	int nc = node->children.size();
	ofColor c = nodeColor;
	if(node->children.size() == 0){
		c = ofColor(0,64,0);
	}
	c.a *= pointAlpha;
	ptsMesh.addColor(c);
	ptsMesh.addVertex(node->pos );

	drawnNodes.push_back(node);

	if(level > 1){ //dont draw lines to children on last level
		for(int i = 0; i < nc; i++){
			Node *n = node->children[i];
			//mesh
			linesMesh.addColor( lineColor );
			linesMesh.addVertex( node->pos );
			linesMesh.addColor(lineColor);
			linesMesh.addVertex( n->pos );
			//recursion
			fillMeshes(n, level-1, ptsMesh, linesMesh, drawnNodes);
		}
	}
}

void testApp::keyPressed(int key){

	colors.clear();
	for(int i = 0; i < 10000; i++){
		colors.push_back( ofColor(ofRandom(255), ofRandom(255), ofRandom(255)) );
	}
	bool moved = false;

	switch(key){

		case 'z':
			if(treePointer->children.size() > 0 ){
				prevTreePointer = treePointer;
				treePointer = treePointer->children[floor(ofRandom(treePointer->children.size()))];
				moved = true;
			}
			break;
		case 'a':
			if(treePointer->getParent() ){
				prevTreePointer = treePointer;
				treePointer = treePointer->getParent();
				moved = true;
			}	
			break;

		case ',':
			if(treePointer->siblingL > 0 ){
				prevTreePointer = treePointer;
				treePointer = treePointer->siblingL;
				moved = true;
			}
			break;

		case '.':
			if(treePointer->siblingR > 0 ){
				prevTreePointer = treePointer;
				treePointer = treePointer->siblingR;
				moved = true;
			}
			break;

		case 'r':
				prevTreePointer = treePointer;
				treePointer = speciesAll[(int)ofRandom(speciesAll.size()-1)];
				moved = true;
			break;

		default: break;
	}
	if(moved){		

	}

	//pLevel = -1;
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

void testApp::layout(Node* node, int levels, vector<Node*> & nodeList){

	if(levels == 0){
		node->softLeaf = true;
		return;
	}
	float R = 200 * (levels / (numLevels * 2) );
	float angle =  treeSpread * 2 * M_PI;
	float angleOff = -treeAngleOffset * 2 * M_PI;

	int nc = node->children.size();
	node->softLeaf = false;
	if(levels > 1){ //dont draw lines to children on last level
		nodeList.push_back(node);
		for(int i = 0; i < nc; i++){
			Node *n = node->children[i];
			float loopP = float(i)/(nc);
			float aa = angleOff + angle * loopP;
			n->pos = node->pos + ofVec3f( R * cos(aa), R * sin(aa) );
			//recursion
			layout(n, levels-1, nodeList);
		}
	}
}


void testApp::gatherLeaves( Node* n, vector<Node*> &leaves){

	for (int i = 0; i < n->children.size(); i++) {
		if ( n->children[i]->children.size() == 0 ){
			leaves.push_back(n);
			return;
		}else{
			gatherLeaves(n->children[i], leaves );
		}
	}
}


void testApp::updateNodeDepths(const vector <Node *> &leaves){

	for(int i = 0; i < leaves.size(); i++){
		Node * me = leaves[i];
		me->deepestLevel = me->level;
		int deepestLevel = me->level;
		while(me->parents.size() > 0){
			if(me == me->getParent()){
				cout << "I am my own father! wtf!" << endl;
				break;
			}
			me = me->getParent();
			if(me->deepestLevel < deepestLevel){
				me->deepestLevel = deepestLevel;
			}
		}
	}
}

