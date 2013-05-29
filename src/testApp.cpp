#include "testApp.h"


void testApp::setup(){

	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	glDisable(GL_POINT_SMOOTH);

	ofBackground(0);

	SPRING_LENGTH = 30;
	SPRING_FORCE = 90;
	REPULSION_FORCE = 50;
	REPULSION_DIST = 180;
	FRICTION = 0.95;

	OFX_REMOTEUI_SERVER_SETUP(10000); 	//start server

	OFX_REMOTEUI_SERVER_SHARE_PARAM(SPRING_FORCE, 0, 100);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(SPRING_LENGTH, 1, 100);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(REPULSION_FORCE, 0, 100);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(REPULSION_DIST, 1, 500);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(FRICTION, 0.5, 1);

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_COLOR( ofColor(255,0,128,32) ); // set a bg color for the upcoming params
	OFX_REMOTEUI_SERVER_SHARE_PARAM(repelNNGain, 0, 1);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(repelMyChildrenGain, 0, 1);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(repelChildChildGain, 0, 1);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(repelChildChildDistGain, 0, 1);


	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_COLOR( ofColor(255) ); // set a bg color for the upcoming params
	OFX_REMOTEUI_SERVER_SHARE_PARAM(drawNames);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(nameFilter, 1, 30);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(updateMesh);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(repellNN);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(drawForces);


	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_COLOR( ofColor(255,0,255,32) ); // set a bg color for the upcoming params
	OFX_REMOTEUI_SERVER_SHARE_PARAM(lineWidth, 0.1, 10);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(pointSize, 1, 30);

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_COLOR( ofColor(0,255,0,32) ); // set a bg color for the upcoming params
	OFX_REMOTEUI_SERVER_SHARE_PARAM(lineAlpha, 0, 1);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(pointAlpha, 0, 1);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(nameAlpha, 0, 1);




	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_COLOR( ofColor(0,0,255,32) ); // set a bg color for the upcoming params
	OFX_REMOTEUI_SERVER_SHARE_PARAM(blurIterations, 0, 4);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(blurOffset, 0.0, 10);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(blurOverlayGain, 0, 255);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(numBlurOverlays, 0, 7);

	OFX_REMOTEUI_SERVER_LOAD_FROM_XML();

	printf("INT_MAX: %d\n######################\n", INT_MAX);

	TIME_SAMPLE_DISABLE_AVERAGE();
	TIME_SAMPLE_START("parseCSV");
	parseCSV( ofToDataPath("pruned-names.csv"), speciesAll );
	TIME_SAMPLE_STOP("parseCSV");

	TIME_SAMPLE_START("filterDuplicates");
	filterDuplicates(speciesAll, nodesByName);
	TIME_SAMPLE_STOP("filterDuplicates");

	TIME_SAMPLE_START("buildTree");
	treeRoot = (Node*)buildTree(nodesByName);
	//treeRoot->fixed = true;
	treeRoot->color = ofColor(ofRandom(255), ofRandom(255), ofRandom(255), ALPHA);
	TIME_SAMPLE_STOP("buildTree");


	lines.setMode(OF_PRIMITIVE_LINES);
	nodes.setMode(OF_PRIMITIVE_POINTS);
	forces.setMode(OF_PRIMITIVE_LINES);

	int len = 100;
	int level = 0;
	int maxLevel = TREE_DEPTH;
	recursiveFillVectorAndSprings(treeRoot, level, maxLevel, chosenNodes, springs);
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

	cleanImgFBO.allocate( s );
	blurOutputFBO.allocate( s );
	blurTempFBO.allocate( s );
	blurTempFBO2.allocate( s );

	gpuBlur.setup();

	
}

void testApp::recursiveFillVectorAndSprings(Node * node, int &level, int maxLevel, vector<Node*> &chosenNodes, vector<Spring*> &springs){

	chosenNodes.push_back(node);
	int n = node->children.size();
	level++;
	node->level = level;
	node->setRandomPosAccordingToLevel(); //atempt to have a nice startup arrangement
	if (level >= maxLevel){
		node->softLeaf = true;
		level--;
		return;
	}
	node->softLeaf = false;
	ofColor c = ofColor(ofRandom(255), ofRandom(255), ofRandom(255), ALPHA);
	for(int i = 0; i < n; i++) {
		Node* child = node->children[i];
		child->color = c;;
		Spring * s = new Spring(node, child, &SPRING_LENGTH, &SPRING_FORCE);
		springs.push_back(s);
		recursiveFillVectorAndSprings(child, level, maxLevel, chosenNodes, springs);
	}
	level--;
}

void testApp::fillMesh(vector<Node*> &chosenNodes, ofMesh & linesMesh, ofMesh & ptsMesh){

	int n = chosenNodes.size();
	for(int i = 0; i < n; i++) {
		Node* me = chosenNodes[i];
		if (!me->softLeaf){
			int nn = me->children.size();
			//ofSeedRandom(me->level + 1 ); //force repeated random sequence TODO
			//ofColor c = ofColor(ofRandom(255), ofRandom(255), ofRandom(255), ALPHA);
			for(int j = 0; j < nn; j++) {
				ofColor c1 = me->color;
				c1 *= lineAlpha;
				lines.addColor( c1 * lineAlpha);
				lines.addVertex( me->pos );
				ofColor c2 = me->children[j]->color;
				c2 *= lineAlpha;
				lines.addColor(c2);
				lines.addVertex( me->children[j]->pos );
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
		if(repellNN){
			for(int l = 0; l < n; l++) {
				if (i != l){
					Node* me2 = chosenNodes[l];
					me->addRepulsion(me2, REPULSION_FORCE, REPULSION_DIST, repelNNGain);
					me2->addRepulsion(me, REPULSION_FORCE, REPULSION_DIST, repelNNGain);
				}
			}
		}

		int numChild = me->children.size();

		for( int j = 0; j < numChild; j++ ){

			Node* ch1 = me->children[j];
			ch1->addRepulsion(me, REPULSION_FORCE, REPULSION_DIST, repelMyChildrenGain); // I repel my own children
			me->addRepulsion(ch1, REPULSION_FORCE, REPULSION_DIST, repelMyChildrenGain ); // I repel my own children

			if (!me->softLeaf){ //if not a leaf, apply forces children to to children
				for( int k = 0; k < numChild; k++ ){
					Node* ch2 = me->children[k];
					ch1->addRepulsion(ch2, REPULSION_FORCE, REPULSION_DIST * repelChildChildDistGain, repelChildChildGain); // my children repel each other
					ch2->addRepulsion(ch1, REPULSION_FORCE, REPULSION_DIST * repelChildChildDistGain, repelChildChildGain); // my children repel each other
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
	for(int i = 0; i < n; i++) {
		chosenNodes[i]->applyForces(DT, FRICTION);
	}
}


void testApp::parseCSV(string file, vector<Node*> & list){
    std::ifstream  data(file.c_str());

    std::string line;
    while(std::getline(data,line)){

        std::stringstream  lineStream(line);
        std::string        cell;
		int c = 0;
		string specie;
		string parent;
        while(std::getline(lineStream,cell, FATHER_CHILDREN_SEPARATOR)){
			if (c == 0){
				specie = cell;
			}else{
				//unsigned int pos = cell.find_first_of(":");
				//parent = cell.substr(pos + 1, cell.length() - 1 - pos);
				if (c==1){
					parent = cell;
				}
			}
			c++;
			if (c > 2){
				cout << "wtf c: " << c <<  " at line: " << specie << ", " << parent << endl;
			}
        }
//		cout << "species: _" <<  specie << "_" << endl;
//		if (parent.length() > 0){
//			cout << "    parent: _" <<  parent << "_" << endl;
//		}

		if (specie.size() > 0 && parent.size() > 0){
			Node * node = new Node(	c, -1, specie, parent );
			list.push_back( node );
		}else{
			cout << "wtf!" << endl;
		}
    }
}


void testApp::filterDuplicates(vector<Node*> &list, map<string, Node*> &dictionary){

	int numNodes = list.size();

	//build a structure where we can access a node by its name
	//doing this will disregard a few nodes; for those, we add up their parents into parentNames
	//(for the chosen node for that name)
	for(int i = 0; i < numNodes; i++){

		Node* me = list[i];
		if ( dictionary.find(me->name) == dictionary.end() ) { // name not found in dict
			dictionary[me->name] = me;
			me->parentNames.push_back(me->parentName);
		}else{
			//cout << "we already have a node named " << me->name << endl;
			dictionary[me->name]->parentNames.push_back( me->parentName );
		}
	}
	
	cout << "Fiter Duplicates ended" << endl;
}


// inputWithIDs : ALL nodes in text file, where each node's ID (and parent field) maps to the speciesAll's positions
void* testApp::buildTree(map<string, Node*> &dictionary){

	int numNodes = dictionary.size();

	//at this point, the map has all unique nodes, each with a list of its parents as strings
	//walk all nodes by name
	int c = 0;
	for( map<string,Node*>::iterator ii = dictionary.begin(); ii != dictionary.end(); ++ii ){

		string name = (*ii).first;
		Node * me = (*ii).second;
		if (me==NULL){
			cout << "im null!" << endl;
		}
		//cout << name << endl;

		//for each node, fill in its list of Node* parents
		for(int i = 0; i < me->parentNames.size(); i++){
			
			Node* parent = dictionary[me->parentNames[i]];

			if (parent == NULL){
				//cout << "this node has a parent that doesnt exist! making one" << endl;
				Node * node = new Node(	-1, -1, me->parentNames[i], "" );
				dictionary[me->parentNames[i]] = node;
				parent = node;
			}

			if (std::find(me->parents.begin(), me->parents.end(), parent) == me->parents.end()){ // parent not found in my own list of parents

				me->parents.push_back(parent); // add this parent to my list of parents

				//now, tell that parent that I am a child, if I wasnt already in this dude's children list
				if (std::find(parent->children.begin(), parent->children.end(), me) == parent->children.end()){
					parent->children.push_back(me);
				}else{
					//cout << "trying to add a children to a node that already has that children? (node: " << parent->name << ", children:" << name << ")" << endl;
				}
			
			}else{
				//cout << "trying to add a parent to a node that already has that parent? (node: " << name << ", parent:" << parent->name << ")" << endl;
			}
		}
		c++;
	}


	//at this point, the nodesByName struct has all valid nodes, with each node containing a list of parents and chidren as Node*

	vector<Node*> roots;
	vector<Node*> leaves;

	for( map<string,Node*>::iterator ii = dictionary.begin(); ii != dictionary.end(); ++ii ){

		Node * me = (*ii).second;
		string name = (*ii).first;

		if (me->parents.size() == 0){
			if (me->children.size() > 0){ //only add roots with children
				roots.push_back(me);
			}
		}
		//cout << me->name << " has " << me->parents.size() << " parents and " << me->children.size() << " children."<< endl;

		if (me->children.size() == 0){
			leaves.push_back(me);
		}
	}

	cout << "num Nodes: " << dictionary.size() << endl;
	cout << "num roots: " << roots.size() << endl;
	cout << "num leaves: " << leaves.size() << endl;
	cout << "done!" << endl;

	return roots[0];
}



void testApp::parseTXT(string file, vector<Node*> &list){

	std::ifstream  data(file.c_str());

    std::string line;
    while(std::getline(data,line)){

        std::stringstream  lineStream(line);
        std::string        cell;
		int c = 0;
		string ID;
		string specie;
		string parentID;
        while(std::getline(lineStream,cell,'$')){
			if (c==0) ID = cell;
			if (c==1) specie = cell;
			if (c==2) parentID = cell;
			c++;
		}

		Node * node = new Node(	(int)atoi(ID.c_str()), (int)atoi(parentID.c_str()), specie );
		list.push_back( node );
		//cout << "ID: _" << ID << "_ SPECIES: _" << species << "_ PARENT: _" << parent << "_" << endl << "##################" <<  endl;
	}
}


void testApp::update(){

	OFX_REMOTEUI_SERVER_UPDATE(DT);

	TIME_SAMPLE_START("update");

	if (updateMesh){
		lines.clear();
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
		fillMesh(chosenNodes, lines, nodes);
		TIME_SAMPLE_STOP("fillMesh");

	}

	TIME_SAMPLE_STOP("update");
}


void testApp::draw(){

	glPointSize(pointSize);
	ofSetLineWidth(lineWidth);

	TIME_SAMPLE_START("draw");

	ofEnableBlendMode(OF_BLENDMODE_ADD);
	//draw into fbo
	ofSetColor(255);
	cleanImgFBO.begin();
		ofClear(0, 0, 0, 0);
		cam.begin();

//			ofSetColor(255, ALPHA * 0.5);
//			lines.draw();
			nodes.draw();

		cam.end();
	cleanImgFBO.end();

	//calc the blur
	ofSetColor(255);
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	gpuBlur.blur(&cleanImgFBO, &blurOutputFBO, &blurTempFBO, &blurTempFBO2, blurIterations, blurOffset);

	//draw clean scene
	ofClear(0, 0, 0, 0);
	ofSetColor(255);
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	cleanImgFBO.getTextureReference().draw(0, cleanImgFBO.getHeight(), cleanImgFBO.getWidth(), -cleanImgFBO.getHeight());
	ofEnableBlendMode(OF_BLENDMODE_ADD);

	//overlay the blur
	ofSetColor(blurOverlayGain);
	for(int i = 0; i < numBlurOverlays; i++){
		blurOutputFBO.getTextureReference().draw(0, blurOutputFBO.getHeight(), blurOutputFBO.getWidth(), -blurOutputFBO.getHeight());
	}

	//draw the lines after?
	cam.begin();
		ofSetColor(255, lineAlpha);
		lines.draw();
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
			forces.clear();
			int n = chosenNodes.size();
			for(int i = 0; i < n; i++) {
				forces.addColor(ofColor(255,0,0));
				forces.addVertex(chosenNodes[i]->pos);
				forces.addColor(ofColor(255,0,0,0));
				forces.addVertex(chosenNodes[i]->pos + chosenNodes[i]->force * 0.1);
			}
			forces.draw();
		}
	cam.end();

	TIME_SAMPLE_STOP("draw");

	ofSetColor(33);
	TIME_SAMPLE_DRAW_TOP_LEFT();
}


void testApp::keyPressed(int key){

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
