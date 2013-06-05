#include "Spring.h"


//---------------------------------------------------------------------
Spring::Spring(Node * a_, Node * b_, float *springLen_, float *springForce){
	a = a_;
	b = b_;
	springLen = springLen_;
	springiness = springForce;
	useGlobalLen = true;
	uniqueLen = 0;
}

//---------------------------------------------------------------------
void Spring::applyForces(){

	if ((a == NULL) || (b == NULL)){
		return;
	}

	float theirDistance = (a->pos - b->pos).length();
	//if (a->softLeaf || b->softLeaf) factor = 0.1 ;

	//float factor = 1;// + 5.0 / ( 1 + a->children.size());
	//if (b->softLeaf) factor = 0.2;
	
	float distDiff ;
	if (useGlobalLen){
		distDiff = (*springLen) - theirDistance;
	}else{
		distDiff = uniqueLen - theirDistance;
	}

	//cout << "dd: " << distDiff << " . " << uniqueLen << endl;
	float SpringForce = (*springiness) * distDiff;
	//if (SpringForce > 0.01 * (*distance)) SpringForce = 0.01 * (*distance);
	ofVec3f frcToAdd = ((a->pos - b->pos)/(theirDistance )) * SpringForce;
//	float f = frcToAdd.length();
//	float max = 1000;
//	if ( f > max){
//		frcToAdd *= (max/f);
//	}

	//only child gets pulled, parent not pulled

	if (a->fixed) {
		b->addSpringForce( -frcToAdd );
	}else{
		a->addSpringForce( frcToAdd );
	}
	b->addSpringForce( -frcToAdd );
}

