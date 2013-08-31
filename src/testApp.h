#pragma once

#include "ofMain.h"
#include "ofxTimeMeasurements.h"
#include "Constants.h"
#include "Spring.h"
#include "ofxRemoteUIServer.h"
#include "ofxFboBlur.h"
#include "Parser.h"

// missing name "WTF_BUG" !!

class testApp : public ofBaseApp{
	
	public:

	enum TreeStyle{TREE_2D, TREE_2_3_D, TREE_3D, TREE_3D_CONE, TREE_3D_CONE2, NUM_TREE_STYLES};

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

		void exit();

		void gatherLeaves(const vector<Node*> &nodes, vector<Node*> &leaves);
		int countChildren( Node * , int &numLeaves);

		void gatherParentsToLevel(Node* node, vector<Node*> &parents, int levels);
		void gatherChildrenToLevel(Node* node, vector<Node*> &children, int levels);

		void drawDynamicTree();

		Parser parser;

		vector<Node*> speciesAll; //access by ID, contains duplicates
		map<string, Node*> nodesByName;

		void calcForces(vector<Node*> &chosenNodes, vector<Spring*> &springs);
		void updateNodeForces(vector<Node*> &chosenNodes);
	
		void forceBasedLayout(Node* startingNode, vector<Node*>nodes); //will alter position of those nodes, laying them out around the starting node

		vector<Node*> tempTree;
		vector<ofColor> colors;

		Node* treeRoot;

		Node* treePointer;

		ofEasyCam cam;
		ofMesh linesMesh;
		ofMesh pointsMesh;


	TreeStyle treeStyle;

		float SPRING_LENGTH;
		float SPRINGINESS;
		float REPULSION_FORCE;
		float REPULSION_DIST;
		float CHILD_REPULSION_DIST;
		float FRICTION;

		bool drawNames;
		int nameFilter;

		bool updateMesh;
		bool repellNN;
		float repelNNGain;
		float repelMyChildrenGain;
		float repelChildChildGain;
		float gravityGain;
		bool blurLines;

		bool addSprings;
		bool calcChildForces;
		bool liveRePositioning;
		bool adaptSpringsToSkeleton;

		float lineWidth;
		bool adapativeLineWidth;
		float pointSize;
		float lineAlpha;
		float pointAlpha;
		float nameAlpha;

		float treeSpread;
		float treeWidth;
		float treeHeight;

		int blurIterations;
		float blurOffset;
		int blurOverlayGain;
		int numBlurOverlays;

		ofxFboBlur gpuBlur;

		int level;
		int pLevel;
};
