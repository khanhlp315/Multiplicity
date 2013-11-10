//
//  userLight.h
//  Multiplicity
//
//  Created by Ole Kristensen on 10/11/13.
//
//

#ifndef __Multiplicity__userLight__
#define __Multiplicity__userLight__

#include "ofMain.h"

class userLight : public ofLight {
public:
	userLight();
    
    void setup(ofFloatColor diffuseColor, vector<userLight> * vec);
    
    void setPosition(float px, float py, float pz);
    
    void update();
    
    
private:
    vector<userLight> * vec;
    float lastTimeMoved;
    bool dead;

};
#endif /* defined(__Multiplicity__userLight__) */
