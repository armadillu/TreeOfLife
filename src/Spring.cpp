#include "Spring.h"


//---------------------------------------------------------------------
Spring::Spring(Node * a_, Node * b_){
	a = a_;
	b = b_;
	distance = SPRING_LENGTH;
	springiness = SPRING_FORCE;
}

//---------------------------------------------------------------------
void Spring::applyForces(){

	if ((a == NULL) || (b == NULL)){
		return;
	}
	
	float theirDistance = (a->pos - b->pos).length();
	float SpringForce = (springiness * (distance - theirDistance));
	ofVec2f frcToAdd = (a->pos - b->pos).normalized() * SpringForce;
	
	a->addForce(frcToAdd);
	b->addForce(-frcToAdd);
}

