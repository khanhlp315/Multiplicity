//
//  modelDisplay.h
//  Multiplicity
//
//  Created by Ole Kristensen on 09/11/13.
//
//

#ifndef __Multiplicity__modelDisplay__
#define __Multiplicity__modelDisplay__

#include <iostream>
#include "ofMain.h"
#include "ofxCv.h"
#include "ofxAssimpModelLoader.h"
#include "ofxProCamToolkit.h"
#include "ofxAutoControlPanel.h"

class modelDisplay {
public:
    
	void setb(string name, bool value);
	void seti(string name, int value);
	void setf(string name, float value);
	bool getb(string name);
	int geti(string name);
	float getf(string name);
    
	void setup(string name, const ofRectangle &displayRect, string fileName, ofxAutoControlPanel * appPanel);
	void update();
	void draw();
	
	void setupMesh(string fileName);
	void drawLabeledPoint(int label, ofVec2f position, ofColor color, ofColor bg = ofColor::black, ofColor fg = ofColor::white);
	void updateRenderMode();
	void drawSelectionMode();
	void drawRenderMode();
	void render();
    void setupControlPanel();
    void loadCalibration();
	void saveCalibration();
    
    void drawPointCloud();
		
	int nearThreshold;
	int farThreshold;
	
    ofRectangle displayRect;
	ofxAssimpModelLoader model;
	ofEasyCam cam;
	ofVboMesh objectMesh;
	ofMesh imageMesh;
	ofxAutoControlPanel panel;
    ofxAutoControlPanel * appPanel;
	   
	vector<cv::Point3f> objectPoints;
	vector<cv::Point2f> imagePoints;
	vector<bool> referencePoints;
	
	cv::Mat rvec, tvec;
	ofMatrix4x4 modelMatrix;
	ofxCv::Intrinsics intrinsics;
	bool calibrationReady;
    string name;
};



#endif /* defined(__Multiplicity__modelDisplay__) */

