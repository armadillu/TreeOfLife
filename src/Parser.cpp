//
//  Parser.cpp
//  emptyExample
//
//  Created by Oriol Ferrer Mesià on 29/05/13.
//
//

#include "Parser.h"


void Parser::parseTXT(string file, vector<Node*> &list){

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



void Parser::parseCSV(string file, vector<Node*> & list){

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


void Parser::filterDuplicates(vector<Node*> &list, map<string, Node*> &dictionary){

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
void* Parser::buildTree(map<string, Node*> &dictionary){

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


