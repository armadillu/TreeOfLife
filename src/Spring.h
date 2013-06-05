#ifndef SPRING_H
#define SPRING_H

#include "ofMain.h"
#include "Constants.h"


class Spring {

	public:

		Spring(Node * a, Node * b, float *springLen,
			   float *springForce );

		Node *a;
		Node *b;

		float *springLen;
		float uniqueLen;
		float *springiness;	 // this is the k, Springiness constant

		void applyForces();
		bool useGlobalLen; //if true, we will use springLen for dist
						//otherwise, we use uniqueLen

};


#endif