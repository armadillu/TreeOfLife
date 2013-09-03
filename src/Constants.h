//
//  Constants.h
//  emptyExample
//
//  Created by Oriol Ferrer Mesià on 28/05/13.
//
//

#ifndef emptyExample_Constants_h
#define emptyExample_Constants_h

#define NUM_LINE_MESHES				16

#define	FATHER_CHILDREN_SEPARATOR	'$'
#define DT							0.01666f

#define	ALPHA						255

#define TREE_DEPTH					4


class SuperNode {
public:
	ofVec3f			pos;
	ofVec3f			absPos;
	//ofVec3f			vel;
	//ofVec3f			force;
	ofColor			color;
	//ofColor			colorSoft;

	int				ID;			//not used for csv
	int				parentID;	//idem
	//bool			fixed;	//wont move
	string			parentName;
	vector<SuperNode*>	parents;
	vector<string>	parentNames;
};

class Node : public SuperNode{
public:
	
	Node(int ID_, int parentID_, string name_, string parentName_ = ""){
		name = name_;
		siblingR = NULL;
		siblingL = NULL;
		ID = ID_;
		parentID = parentID_;
		parentName = parentName_;
		softLeaf = false;
		//fixed = false;
		deepestLevel = totalChildren = 0;
		float s = 20;
		pos = ofVec3f(ofRandom(-s, s), ofRandom(-s, s), ofRandom(-s, s));
	};


//	void addRepulsion(Node* other, float repForce, float repDist, float scale = 1.0f){
//		ofVec3f vec = (pos - other->pos);
//		float dist = vec.length();
//		if (dist < repDist){
//			float percent = 1.0f - (dist / repDist);
//			vec /= dist; // normalize
//			addForce( ofVec3f( vec * repForce * scale * percent) );
//		}
//	}

//	void addForce(const ofVec3f &f ){
//		force += f;
//	};
//
//	void addSpringForce(const ofVec3f &f ){
//		addForce(f);
//		springForces.push_back(f);
//	};

//	void applyForces(float dt, float fr){
//		vel = vel + force ;
//		vel *= fr;
//		if(fixed) return;
//		pos = pos + vel ;
//	}
//
//	void resetForce(){
//		force.x = 0.0f, force.y = 0.0f, force.z = 0.0f;
//		vel.x = vel.y = vel.z = 0.0f;
//		//springForce.x = 0.0f, springForce.y = 0.0f, springForce.z = 0.0f;
//		springForces.clear();
//	}

	Node* getParent(){
		if(parents.size() > 0)
			return (Node*)parents[0];
		else return NULL;
	}

	Node* getRandomChild(){
		Node* c = NULL;
		if(children.size() > 0){
			int index = floor(ofRandom(children.size()-1));
			//printf("%d [%d] \n", index, children.size()-1 );
			 c = children[index];
		}
		return c;
	}
	
	vector<Node*>	children;
	//vector<ofVec3f> springForces;
	string			name;

	bool			softLeaf; //if true, used as a mark that we should look for children beyond this node when drawing
	int				level;	//your level in tree
	int				deepestLevel; //what level your children get to
	//float			angle;
	int				totalChildren; // how many child + subChild
	int				totalLeaves;

	//used for tree navigation
	int				tempLevel;
	int				tempTotalChildren;
	int				tempTotalLeaves;
	Node *			siblingR;
	Node *			siblingL;

};



#endif
