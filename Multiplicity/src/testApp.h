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
	
	void setupControlPanel();
	void setupMesh();
	void drawLabeledPoint(int label, ofVec2f position, ofColor color, ofColor bg = ofColor::black, ofColor fg = ofColor::white);
	void updateRenderMode();
	void drawSelectionMode();
	void drawRenderMode();
	void render();
    void loadCalibration();
	void saveCalibration();
    void setupLights();

    void drawPointCloud();
	
	ofxKinect kinect;
    
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