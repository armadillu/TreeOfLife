#include "Spring.h"


//---------------------------------------------------------------------
Spring::Spring(Node * a_, Node * b_, float *springLen, float *springForce){
	a = a_;
	b = b_;
	distance = springLen;
	springiness = springForce;
}

//---------------------------------------------------------------------
void Spring::applyForces(){

	if ((a == NULL) || (b == NULL)){
		return;
	}
	
	float theirDistance = (a->pos - b->pos).length();
	float factor = 1;
	if (a->softLeaf || b->softLeaf) factor = 0.1 ;
	float SpringForce = ((*springiness) * (*distance * factor - theirDistance));
	//if (SpringForce > 0.01 * (*distance)) SpringForce = 0.01 * (*distance);
	ofVec3f frcToAdd = (a->pos - b->pos).normalized() * SpringForce;

	a->addForce(frcToAdd);
	b->addForce(-frcToAdd);
}

