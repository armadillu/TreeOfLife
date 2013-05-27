#include "testApp.h"


void testApp::setup(){

	ofSetVerticalSync(true);
	ofBackground(22);

	printf("INT_MAX: %d\n######################\n", INT_MAX);

	TIME_SAMPLE_DISABLE_AVERAGE();
	TIME_SAMPLE_START("parseCSV");
	//parseCSV( ofToDataPath("pruned-names.csv") );
	TIME_SAMPLE_STOP("parseCSV");

	TIME_SAMPLE_START("parseTXT");
	parseTXT( ofToDataPath("treeOfLife4.txt"), speciesAll );
	TIME_SAMPLE_STOP("parseTXT");

	TIME_SAMPLE_START("filterDuplicates");
	filterDuplicates(speciesAll, nodesByName);
	TIME_SAMPLE_STOP("filterDuplicates");

	TIME_SAMPLE_START("buildTree");
	treeRoot = (Node*)buildTree(speciesAll, nodesByName);
	TIME_SAMPLE_STOP("buildTree");


	lines.setMode(OF_PRIMITIVE_LINES);
	int n = treeRoot->children.size();
	int len = 100;

	for(int i = 0; i < n; i++) {
		if (treeRoot->children[i]->children.size() > 0 ){
			lines.addVertex( ofVec3f() );
			float ang = 2 * M_PI * i / (float)n;
			treeRoot->children[i]->pos = ofVec3f( len * cos(ang), len * sin(ang) );
			lines.addVertex( treeRoot->children[i]->pos );
		}
	}

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


void testApp::filterDuplicates(vector<Node*> &inputWithIDs, map<string, Node*> &nodesByName){

	int numNodes = inputWithIDs.size();

	//on pass to populate parentName for all nodes (some of them will be dup, so some nodes have +1 parent)
	for(int i = 0; i < numNodes; i++){
		Node* me = inputWithIDs[i];
		if (me->name != inputWithIDs[ me->parentID ]->name){
			me->parentName = inputWithIDs[ me->parentID ]->name;
		}else{
			//cout << "this node is its own father: " << me->name << endl;
		}
	}

	//build a structure where we can access a node by its name
	//doing this will disregard a few nodes; for those, we add up their parents into parentNames
	//(for the chosen node for that name)
	for(int i = 0; i < numNodes; i++){

		Node* me = inputWithIDs[i];

		if(me->parentName == "null" || me->parentName.size() == 0){ //found a root node!

			//cout << "this node has NULL parent, or empty string parent" << endl;
			if ( nodesByName.find(me->name) == nodesByName.end() ) { // if name not found in our unique nodes map
				nodesByName[me->name] = me; //add to unique nodes map
			}

		}else{ //this node has a valid parent

			
			if ( nodesByName.find(me->name) == nodesByName.end() ) { // name not found
				nodesByName[me->name] = me;
				me->parentNames.push_back(me->parentName);
			}else{
				//cout << "we already have a node named " << me->name << endl;
				nodesByName[me->name]->parentNames.push_back( me->parentName );
			}
		}
	}

}


// inputWithIDs : ALL nodes in text file, where each node's ID (and parent field) maps to the speciesAll's positions
void* testApp::buildTree(vector<Node*> & inputWithIDs, map<string, Node*> &nodesByName){

	int numNodes = nodesByName.size();

	//at this point, the map has all unique nodes, each with a list of its parents as strings
	//walk all nodes by name
	int c = 0;
	for( map<string,Node*>::iterator ii = nodesByName.begin(); ii != nodesByName.end(); ++ii ){

		string name = (*ii).first;
		Node * me = (*ii).second;


		//for each node, fill in its list of Node* parents
		for(int i = 0; i < me->parentNames.size(); i++){
			
			Node* parent = nodesByName[me->parentNames[i]];

			if (parent != NULL){

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
		}
	}
	c++;

	//at this point, the nodesByName struct has all valid nodes, with each node containing a list of parents and chidren as Node*

	vector<Node*> roots;
	vector<Node*> leaves;

	for( map<string,Node*>::iterator ii = nodesByName.begin(); ii != nodesByName.end(); ++ii ){

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

	cout << "num Nodes: " << nodesByName.size() << endl;
	cout << "num roots: " << roots.size() << endl;
	cout << "num leaves: " << leaves.size() << endl;
	cout << "done!" << endl;

	for(int i = 0; i < roots.size(); i++){
		cout << "root: " << roots[i]->name << " has " << roots[i]->children.size() << " children." << endl;
		for(int j = 0; j < roots[i]->children.size(); j++){
			cout << "child: " << roots[i]->children[j]->name << " has " << roots[i]->children[j]->children.size() << " children." << endl;
		}
	}

	return roots[0];
}


void testApp::parseCSV(string file){
    std::ifstream  data(file.c_str());

    std::string line;
    while(std::getline(data,line)){

        std::stringstream  lineStream(line);
        std::string        cell;
		int c = 0;
		string species;
		string parent;
        while(std::getline(lineStream,cell,',')){
			if (c == 0){
				species = cell;
			}else{
				unsigned int pos = cell.find_first_of(":");
				parent = cell.substr(pos + 1, cell.length() - 1 - pos);
			}
			c++;
        }
		//cout << "species: _" <<  species << "_" << endl;
		if (parent.length() > 0){
			//cout << "    parent: _" <<  parent << "_" << endl;
		}
    }
}




void testApp::update(){

	TIME_SAMPLE_START("update");

	TIME_SAMPLE_STOP("update");

}


void testApp::draw(){


	TIME_SAMPLE_START("draw");
	cam.begin();

		lines.draw();
	
	cam.end();
	TIME_SAMPLE_STOP("draw");

	TIME_SAMPLE_DRAW_TOP_LEFT();
}


void testApp::keyPressed(int key){

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


