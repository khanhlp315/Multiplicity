#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxAssimpModelLoader.h"
#include "ofxProCamToolkit.h"
#include "ofxAutoControlPanel.h"
#include "ofxKinect.h"
#include "LineArt.h"
#include "shadowMapLight.h"
#include "modelDisplay.h"
#include "userLight.h"


class testApp : public ofBaseApp {
public:
	void setb(string name, bool value);
	void seti(string name, int value);
	void setf(string name, float value);
	bool getb(string name);
	int geti(string name);
	float getf(string name);
	
	void setup();
	void update();
	void draw();
	void exit();
	void keyPressed(int key);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);

	void setupControlPanel();
    void setupLights();
    
    void updatePointCloud();
    void drawPointCloud();
    
	ofxKinect kinect;
    ofMesh kinectMesh;
    ofVec3f kincetClosestPoint;
    ofVec3f kinectOrigin;
    ofVec3f kinectRotation;
    float kinectScale;
    
    modelDisplay topDisplay;
    modelDisplay bottomDisplay;
	
	bool bThreshWithOpenCV;
	bool bDrawPointCloud;
	
	int nearThreshold;
	int farThreshold;
	
	int angle;
    
    float lightAngle;

	ofLight light;
    
    vector<userLight> lights;
    
	ofxAutoControlPanel panel;
	
    ShadowMapLight m_shadowLight;
    ofShader m_shader;

	Poco::Timestamp lastFragTimestamp, lastVertTimestamp;
	ofShader shader;
};