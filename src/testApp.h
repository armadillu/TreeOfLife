#pragma once

#include "ofMain.h"
#include "ofxTimeMeasurements.h"
#include "Constants.h"
#include "Spring.h"
#include "ofxRemoteUIServer.h"
#include "ofxFboBlur.h"
#include "Parser.h"
#include "ofxAnimatableOfPoint.h"

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

		void gatherParentsToLevel(Node* node, vector<Node*> &parents, int levels);
		void gatherChildrenToLevel(Node* node, vector<Node*> &children, int levels);


		Parser parser;

		vector<Node*> speciesAll; //access by ID, contains duplicates
		map<string, Node*> nodesByName;

		void recursiveTreeInfoBuild(Node * treeRoot, int cLevel); //sets up the level info for each node of the tree
																	//also fills in NODE* siblingR/L 
		void gatherLeaves(Node* root, vector<Node*> &leaves); //fill in leaves structure, also fill in each node's children depth (deepestLevel)
		void updateNodeDepths(const vector <Node *> &leaves);
		int countChildren( Node * root, int &numLeaves);
		int countTempChildren( Node * node , int & numLeaves, int level);

		//will alter position of those nodes, laying them out around the starting node. Fills in the list of drawn nodes too
		void layout(Node* startingNode, int level,  vector<Node*> & nodeList);
		void fillMeshes(Node* startingNode, int level, ofMesh & ptsMesh, ofMesh & linesMesh, vector<Node*> & drawnNodes);

		void position2DTree(Node*, int level, bool tempVersion);
		void updateCam(bool interpolate);

		ofVec3f camTarget, camPos;
		float camDelay;
	float camTravelDuration;
		ofVec3f camJumpOffsetFix;
	bool camJumped;


		vector<ofColor> colors;

		Node* treeRoot;
		Node* treePointer; //current node
		Node* prevTreePointer; //current node
		Node* topTreePointer;
		vector<Node*> tempTree;

		ofEasyCam cam;
		float fov;
		float camDist;

		ofMesh lines;
		ofMesh nodes;
		ofxAnimatableOfPoint nodeFollowOffsetAnimation;

		ofColor lineColor;
		ofColor nodeColor;
		ofColor bgColor;

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
		ofColor nameColor;

		float treeSpread;
		float treeAngleOffset;
		float treeWidth;
		float treeHeight;

		int blurIterations;
		float blurOffset;
		int blurOverlayGain;
		int numBlurOverlays;

		ofxFboBlur gpuBlur;

		int numLevels;
//		int pLevel;
};
