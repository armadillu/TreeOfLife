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
#define SPRING_LENGTH				30.0f
#define SPRING_FORCE				90.0f
#define REPULSION_FORCE				50.9f
#define REPULSION_DIST				180.0f
#define FRICTION					0.95f
#define DT							0.01666f

#define	ALPHA						64

#define TREE_DEPTH					6

struct Node{
	Node(int ID_, int parentID_, string name_, string parentName_ = ""){
		name = name_;
		ID = ID_;
		parentID = parentID_;
		parentName = parentName_;
		float r = 10;
		pos.x = ofRandom(-r,r) ;+ ofGetWidth() / 2;
		pos.y = ofRandom(-r,r) ;+ ofGetHeight() / 2;
		softLeaf = true;
		fixed = false;
	};

	void addRepulsion(Node* other, float scale = 1.0f){
		ofVec2f vec = (pos - other->pos);
		float dist = vec.length();
		if (dist < REPULSION_DIST){
			float percent = 1.0f - (dist / REPULSION_DIST);
			percent *= percent;
			vec.normalize();
			addForce( ofVec2f( vec * REPULSION_FORCE * scale * percent) );
		}
	}

	void addForce(const ofVec3f &f ){
		force += f;
	};

	void applyForces(float dt){
		if(fixed) return;
		vel = vel + force * dt;
		pos = pos + vel * dt;
		vel *= FRICTION;
		//now reset the force
		force.x = 0.0f, force.y = 0.0f, force.z = 0.0f;
	}
	
	ofVec3f pos;
	ofVec3f vel;
	ofVec3f force;
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
