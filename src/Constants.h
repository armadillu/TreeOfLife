//
//  Constants.h
//  emptyExample
//
//  Created by Oriol Ferrer Mesià on 28/05/13.
//
//

#ifndef emptyExample_Constants_h
#define emptyExample_Constants_h


#define	FATHER_CHILDREN_SEPARATOR	'$'
#define DT							0.01666f

#define	ALPHA						255

#define TREE_DEPTH					5

struct Node{
	Node(int ID_, int parentID_, string name_, string parentName_ = ""){
		name = name_;
		ID = ID_;
		parentID = parentID_;
		parentName = parentName_;
		softLeaf = true;
		fixed = false;
	};

	void setRandomPosAccordingToLevel(){
		float r = level * 150;
		float a1 = ofRandom(2*M_PI);
		float a2 = ofRandom(2*M_PI);
		pos.x = r * cos(a1) * sin(a2) ;
		pos.y = r * sin(a1) * sin(a2) ;
		pos.z = r * cos(a2) ;
	}

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

	void applyForces(float dt, float fr){
		if(fixed) return;
		vel = vel + force * dt;
		pos = pos + vel * dt;
		vel *= fr;
	}

	void resetForce(){
		force.x = 0.0f, force.y = 0.0f, force.z = 0.0f;
	}
	
	ofVec3f pos;
	ofVec3f vel;
	ofVec3f force;
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
	bool fixed;
	int level;
};



#endif
