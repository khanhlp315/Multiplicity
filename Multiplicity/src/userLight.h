//
//  kinectLight.h
//  Multiplicity
//
//  Created by Ole Kristensen on 10/11/13.
//
//

#ifndef __Multiplicity__kinectLight__
#define __Multiplicity__kinectLight__

#include "ofMain.h"

class userLight : public ofLight {
public:
	userLight();
    
    void setup(ofColor color, vector<userLight> * vec);
    
private:
    vector<userLight> * vec;
    

};
#endif /* defined(__Multiplicity__kinectLight__) */
