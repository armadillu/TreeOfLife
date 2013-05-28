#ifndef SPRING_H
#define SPRING_H

#include "ofMain.h"
#include "Constants.h"


class Spring {

	public:

		Spring(Node * a, Node * b, float *springLen, float *springForce);

		Node *a;
		Node *b;

		float *distance;
		float *springiness;	 // this is the k, Springiness constant
	
		void applyForces();

};


#endif