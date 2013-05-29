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

#define TREE_DEPTH					7

struct Node{
	Node(int ID_, int parentID_, string name_, string parentName_ = ""){
		name = name_;
		ID = ID_;
		parentID = parentID_;
		parentName = parentName_;
		float r = 500;
		pos.x = ofRandom(-r,r) ;+ ofGetWidth() / 2;
		pos.y = ofRandom(-r,r) ;+ ofGetHeight() / 2;
		pos.z = ofRandom(-r,r) ;+ ofGetHeight() / 2;
		softLeaf = true;
		fixed = false;
	};

	void addRepulsion(Node* other, float repForce, float repDist, float scale = 1.0f){
		ofVec3f vec = (pos - other->pos);
		float dist = vec.length();
		if (dist < repDist){
			float percent = 1.0f - (dist / repDist);
			//percent *= percent ;
			vec.normalize();
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
		//now reset the force
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
