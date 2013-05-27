#pragma once

#include "ofMain.h"
#include "ofxTimeMeasurements.h"

// missing name "WTF_BUG" !!

class testApp : public ofBaseApp{

	struct Node{
		Node(int ID_, int parentID_, string name_){
			name = name_;
			ID = ID_;
			parentID = parentID_;
			parentName = "";
		};
		ofVec3f pos;
		vector<Node*> children;
		vector<Node*> parents;
		vector<string> parentNames;
		int ID;
		int parentID;
		string parentName;
		string name;
	};
	
	public:
	
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h){};
		void dragEvent(ofDragInfo dragInfo){};
		void gotMessage(ofMessage msg){};


		void parseCSV(string);

		void parseTXT(string, vector<Node*> & species);

		void filterDuplicates(	vector<Node*> &inputWithIDs,
								map<string, Node*> & nodesByName
							  );

		void* buildTree( vector<Node*> & inputWithIDs,
						map<string, Node*> & nodesByName
					   );

	
		vector<Node*> speciesAll; //access by ID, contains duplicates
		map<string, Node*> nodesByName;

		Node* treeRoot;

		ofMesh lines;
		ofMesh nodes;

		ofEasyCam cam;
};
