//
//  Constants.h
//  emptyExample
//
//  Created by Oriol Ferrer Mesià on 28/05/13.
//
//

#ifndef emptyExample_Constants_h
#define emptyExample_Constants_h

#define NUM_LINE_MESHES				25

#define	FATHER_CHILDREN_SEPARATOR	'$'
#define DT							0.1666f

#define	ALPHA						255

#define TREE_DEPTH					4

struct Node{
	Node(int ID_, int parentID_, string name_, string parentName_ = ""){
		name = name_;
		ID = ID_;
		parentID = parentID_;
		parentName = parentName_;
		softLeaf = true;
		fixed = false;
		deepestLevel = totalChildren = 0;
	};


	void addRepulsion(Node* other, float repForce, float repDist, float scale = 1.0f){
		ofVec3f vec = (pos - other->pos);
		float dist = vec.length();
		if (dist < repDist){
			float percent = 1.0f - (dist / repDist);
			vec /= dist; // normalize
			addForce( ofVec3f( vec * repForce * scale * percent) );
		}
	}

	void addForce(const ofVec3f &f ){
		force += f;
	};

	void addSpringForce(const ofVec3f &f ){
		addForce(f);
		springForces.push_back(f);
	};

	void applyForces(float dt, float fr){
		vel = vel + force ;
		vel *= fr;
		if(fixed) return;
		pos = pos + vel ;
	}

	void resetForce(){
		force.x = 0.0f, force.y = 0.0f, force.z = 0.0f;
		vel.x = vel.y = vel.z = 0.0f;
		//springForce.x = 0.0f, springForce.y = 0.0f, springForce.z = 0.0f;
		springForces.clear();
	}
	
	ofVec3f pos;
	ofVec3f vel;
	ofVec3f force;
	vector<ofVec3f> springForces;
	ofColor color;
	ofColor colorSoft;
	vector<Node*> children;
	vector<Node*> parents;
	vector<string> parentNames;
	int ID;
	int parentID;
	string parentName;
	string name;
	bool softLeaf; //if true, used as a mark that we should look for children beyond this node when drawing
	bool fixed;	//wont move
	int level;	//your level in tree
	int deepestLevel; //what level your children get to
	float angle;
	int totalChildren; // how many child + subChild
	int totalLeaves;
};



#endif
