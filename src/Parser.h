//
//  Parser.h
//  emptyExample
//
//  Created by Oriol Ferrer Mesià on 29/05/13.
//
//

#ifndef __emptyExample__Parser__
#define __emptyExample__Parser__

#include <iostream>
#include "ofMain.h"
#include "Constants.h"
#include "Spring.h"

class Parser{

public:
	
	void parseCSV(string, vector<Node*> & species);
	void parseTXT(string, vector<Node*> & species);

	void filterDuplicates(vector<Node*> &inputWithIDs, map<string, Node*> & nodesByName );
	void* buildTree(map<string, Node*> & nodesByName);


};



#endif /* defined(__emptyExample__Parser__) */
