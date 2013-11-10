//
//  kinectLight.cpp
//  Multiplicity
//
//  Created by Ole Kristensen on 10/11/13.
//
//

#include "userLight.h"

void userLight::setup(ofFloatColor diffuseColor, vector<userLight> * vec){
    this->vec = vec;
    ofLight::setup();
    setDiffuseColor(diffuseColor);
    dead = true;
}

void userLight::setPosition(float px, float py, float pz){
    ofLight::setPosition(px, py, pz);
    lastTimeMoved = ofGetElapsedTimef();
}

void userLight::update(){

    float secondsToLive = 10;
    float secondsToFade = 2;
    float now = ofGetElapsedTimef();
    float timeSincePoke = now - lastTimeMoved;
    
    
    if(timeSincePoke < secondsToLive){
        
        float fadeFactor = (timeSincePoke - (secondsToLive+secondsToFade))/secondsToFade;
        
        if (fadeFactor >= 0) {
            
            ofLight::setDiffuseColor(ofFloatColor(getDiffuseColor().r,getDiffuseColor().g,getDiffuseColor().b, fadeFactor));
        } else {
            dead = true;
        }
    } else {
        ofLight::setDiffuseColor(ofFloatColor(getDiffuseColor().r,getDiffuseColor().g,getDiffuseColor().b, 1.0));
    }
}