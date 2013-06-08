#pragma once

#include "ofMain.h"
#include "ofxTimeMeasurements.h"
#include "Constants.h"
#include "Spring.h"
#include "ofxRemoteUIServer.h"
#include "ofFboBlur.h"
#include "Parser.h"

// missing name "WTF_BUG" !!

#define DRAW_3D		false
#define DRAW_CONE	false
#define DRAW_2D		true

class testApp : public ofBaseApp{
	
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

		void exit();

		void recursiveFillVectorAndSprings(Node * node, int &level, int maxLevel,
										   vector<Node*> &chosenNodes, vector<Spring*> &springs);

		void calcForces(vector<Node*> &chosenNodes, vector<Spring*> &springs);
		void updateNodeForces(vector<Node*> &chosenNodes);
		void fillMesh(vector<Node*> &chosenNodes, ofMesh & ptsMesh);

		void gatherLeaves(const vector<Node*> &nodes, vector<Node*> &leaves);
		void position2DTree( Node * );
		void position23DTree( Node * );
		void position3DTree( Node * );
		void position3DConeTree( Node * );
		void position3DConeTree2( Node * );
		int countChildren( Node * , int &numLeaves);

		Parser parser;

		vector<Node*> speciesAll; //access by ID, contains duplicates
		map<string, Node*> nodesByName;

		vector<Node*> chosenNodes;
		vector<Node*> softLeaves;
		vector<Spring*> springs;
		vector<ofColor> colors;

		Node* treeRoot;

		ofMesh lines[NUM_LINE_MESHES];
		ofMesh nodes;
		ofMesh forces;

		ofMesh line2d;
		ofMesh point2d;

		ofEasyCam cam;

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
		float repelRootGain;
		float repelMyChildrenGain;
		float repelChildChildGain;
		float gravityGain;
		bool drawForces;
		bool drawSpringForces;
	bool drawSpheres;
	bool blurLines;

	bool addSprings;
	bool calcChildForces;

		float lineWidth;
		float pointSize;
		float lineAlpha;
		float pointAlpha;
		float nameAlpha;

		float treeSpread;

		int blurIterations;
		float blurOffset;
		int blurOverlayGain;
		int numBlurOverlays;

		ofFboBlur gpuBlur;

		int level;
		int pLevel;
};
