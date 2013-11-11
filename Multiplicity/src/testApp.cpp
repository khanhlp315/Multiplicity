#include "testApp.h"

using namespace ofxCv;
using namespace cv;

void testApp::setb(string name, bool value) {
	panel.setValueB(name, value);
}
void testApp::seti(string name, int value) {
	panel.setValueI(name, value);
}
void testApp::setf(string name, float value) {
	panel.setValueF(name, value);
}
bool testApp::getb(string name) {
	return panel.getValueB(name);
}
int testApp::geti(string name) {
	return panel.getValueI(name);
}
float testApp::getf(string name) {
	return panel.getValueF(name);
}

void testApp::setup() {
    
    lightAngle = 0.0;
    
	// enable depth->video image calibration
	kinect.setRegistration(true);
    
	kinect.init();
	//kinect.init(true); // shows infrared instead of RGB video image
	//kinect.init(false, false); // disable video image (faster fps)
	
	kinect.open();		// opens first available kinect

    // print the intrinsic IR sensor values
	if(kinect.isConnected()) {
		ofLogNotice() << "sensor-emitter dist: "    << kinect.getSensorEmitterDistance()    << "cm";
		ofLogNotice() << "sensor-camera dist:  "    << kinect.getSensorCameraDistance()     << "cm";
		ofLogNotice() << "zero plane pixel size: "  << kinect.getZeroPlanePixelSize()       << "mm";
		ofLogNotice() << "zero plane dist: "        << kinect.getZeroPlaneDistance()        << "mm";
	}
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	ofSetVerticalSync(true);
    ofSetWindowPosition(0, 0);
    int screenWidth = ofGetScreenWidth();
    int screenHeight = ofGetScreenWidth()*(9/(2*16.));
    ofSetWindowShape(screenWidth, screenHeight);
    
    topDisplay.setup("top", ofRectangle(ofPoint(0,0), screenWidth/2, screenHeight), "model.dae", &panel);
    bottomDisplay.setup("bottom", ofRectangle(ofPoint(screenWidth/2,0), screenWidth/2, screenHeight), "model.dae", &panel);

    setupControlPanel();
    
    m_shader.load( "shaders/mainScene.vert", "shaders/mainScene.frag" );
    setupLights();
	ofSetWindowTitle("Multiplicity");
    setb("loadCalibration", true);
    panel.hide();
    topDisplay.panel.hide();
    bottomDisplay.panel.hide();
}

void testApp::update() {
    
   // kinect.update();
    
    // there is a new frame and we are connected
	if(kinect.isFrameNew()) {
		
        updatePointCloud();
        
        
//      grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);

		// update the cv images
//		grayImage.flagImageChanged();

	}
    
	if(getb("randomLighting")) {
		setf("lightX", ofSignedNoise(ofGetElapsedTimef()*.5, 1, 1) * 100);
		setf("lightY", ofSignedNoise(1, ofGetElapsedTimef()*.5, 1) * 100);
		setf("lightZ", 100+ofSignedNoise(1, 1, ofGetElapsedTimef()*.5) * 100);
	}
	light.setPosition(getf("lightX"), getf("lightY"), getf("lightZ"));
    m_shadowLight.setPosition(getf("lightX"), getf("lightY"), getf("lightZ"));
    
    if(getb("setupMode") == false){
        topDisplay.panel.setValueB("setupMode", false);
        bottomDisplay.panel.setValueB("setupMode", false);
    } else {
        if(geti("setupDisplay") == 0){
            topDisplay.panel.setValueB("setupMode", true);
            bottomDisplay.panel.setValueB("setupMode", false);
        }
        
        if(geti("setupDisplay") == 1){
            topDisplay.panel.setValueB("setupMode", false);
            bottomDisplay.panel.setValueB("setupMode", true);
        }
    }
    
    if(getb("loadCalibration")) {
		topDisplay.loadCalibration();
		bottomDisplay.loadCalibration();
		setb("loadCalibration", false);
	}
	if(getb("saveCalibration")) {
		topDisplay.saveCalibration();
        bottomDisplay.saveCalibration();
		setb("saveCalibration", false);
	}
    
    topDisplay.update();
    bottomDisplay.update();
    
}

void testApp::setupLights() {
    // ofxShadowMapLight extends ofLight - you can use it just like a regular light
    // it's set up as a spotlight, all the shadow work + lighting must be handled in a shader
    // there's an example shader in
    // shadow map resolution (must be power of 2), field of view, near, far
    // the larger the shadow map resolution, the better the detail, but slower
    m_shadowLight.setup( 2048, 5.0f, 0.1f, 1500.0f );
    m_shadowLight.setBlurLevel(4.0f); // amount we're blurring to soften the shadows
    
    m_shadowLight.setAmbientColor( ofFloatColor( 0.0f, 0.0f, 0.0f, 1.0f ) );
    m_shadowLight.setDiffuseColor( ofFloatColor( 0.9f, 0.9f, 0.9f, 1.0f ) );
    m_shadowLight.setSpecularColor( ofFloatColor( 1.0f, 1.0f, 1.0f, 1.0f ) );
        
    m_shadowLight.setPosition( 1000.0f, -1000.0f, 450.0f );
    
    ofSetGlobalAmbientColor( ofFloatColor( 0.01f, 0.01f, 0.01f ) );
        
    light.setDiffuseColor(ofFloatColor::wheat);
    
}

void testApp::draw() {
    ofViewport();

	ofBackground(geti("backgroundColor"));

    topDisplay.draw();
    bottomDisplay.draw();
    
    ofViewport();
	
}

void testApp::keyPressed(int key) {
    
    int setupDisplay = geti("setupDisplay");
    
    ofxAutoControlPanel * thePanel;
    modelDisplay * theModel;
    
        if (setupDisplay == 0) {
            theModel = &topDisplay;
        }
        if (setupDisplay == 1){
            theModel = &bottomDisplay;
        }
    
        thePanel = &(theModel->panel);
        
        if(key == OF_KEY_LEFT || key == OF_KEY_UP || key == OF_KEY_RIGHT|| key == OF_KEY_DOWN){
            int choice = thePanel->getValueI("selectionChoice");
            thePanel->setValueB("arrowing", true);
            if(choice > 0){
                Point2f& cur = theModel->imagePoints[choice];
                switch(key) {
                    case OF_KEY_LEFT: cur.x -= 1; break;
                    case OF_KEY_RIGHT: cur.x += 1; break;
                    case OF_KEY_UP: cur.y -= 1; break;
                    case OF_KEY_DOWN: cur.y += 1; break;
                }
            }
        } else {
            thePanel->setValueB("arrowing",false);
        }
        if(key == OF_KEY_BACKSPACE) { // delete selected
            if(thePanel->getValueB("selected")) {
                thePanel->setValueB("selected", false);
                int choice = thePanel->getValueI("selectionChoice");
                theModel->referencePoints[choice] = false;
                theModel->imagePoints[choice] = Point2f();
            }
        }
        if(key == '\n') { // deselect
            thePanel->setValueB("selected", false);
        }
    
    if(key == ' ') { // toggle render/select mode
        thePanel->setValueB("selectionMode", !thePanel->getValueB("selectionMode"));
    }

    
    if(key == '1'){
        seti("setupDisplay", 0);
    }
    if(key == '2'){
        seti("setupDisplay", 1);
    }
}

void testApp::mousePressed(int x, int y, int button) {
    
    int setupDisplay = geti("setupDisplay");
    
    ofxAutoControlPanel * thePanel;
    modelDisplay * theModel;
    
    if (setupDisplay == 0) {
        theModel = &topDisplay;
    }
    if (setupDisplay == 1){
        theModel = &bottomDisplay;
    }
    
    thePanel = &(theModel->panel);

    
        thePanel->setValueB("selected", thePanel->getValueB("hoverSelected"));
        thePanel->setValueI("selectionChoice", thePanel->getValueI("hoverChoice"));
        if(thePanel->getValueB("selected")) {
            thePanel->setValueB("dragging", true);
        }
    
}

void testApp::mouseReleased(int x, int y, int button) {
    
    int setupDisplay = geti("setupDisplay");
    
    ofxAutoControlPanel * thePanel;
    modelDisplay * theModel;
    
    if (setupDisplay == 0) {
        theModel = &topDisplay;
    }
    if (setupDisplay == 1){
        theModel = &bottomDisplay;
    }
    
    thePanel = &(theModel->panel);

        thePanel->setValueB("dragging", false);
}

void testApp::setupControlPanel() {
	panel.setup();
    panel.setPosition(300, 10);
    panel.setXMLFilename("settings.xml");
	panel.msg = "tab hides the panel, space toggles render/selection mode, 'f' toggles fullscreen.";
	
	panel.addPanel("Interaction");
	panel.addSlider("backgroundColor", 0, 0, 255, true);
	panel.addToggle("setupMode", false);
	panel.addMultiToggle("setupDisplay", 0, variadic("top")("bottom"));
	panel.addMultiToggle("drawMode", 0, variadic("faces")("fullWireframe"));
	panel.addMultiToggle("shading", 1, variadic("none")("lights")("shadows"));

    panel.addPanel("Calibration");
    panel.addToggle("loadCalibration", false);
	panel.addToggle("saveCalibration", false);
    
    panel.addPanel("Kinect");
	panel.addSlider("kinectPositionX", 0, -1000, 1000);
	panel.addSlider("kinectPositionY", 0, -1000, 1000);
	panel.addSlider("kinectPositionZ", 0, -1000, 1000);
    panel.addSlider("kinectRotationX", 0, -180, 180);
	panel.addSlider("kinectRotationY", 0, -180, 180);
	panel.addSlider("kinectRotationZ", 0, -180, 180);
    panel.addSlider("kinectScale", 1, 0, 2);
		
	panel.addPanel("Rendering");
	panel.addSlider("lineWidth", 2, 1, 8, true);
	panel.addSlider("screenPointSize", 2, 1, 16, true);
	panel.addSlider("selectedPointSize", 8, 1, 16, true);
	panel.addSlider("selectionRadius", 12, 1, 32);
	panel.addSlider("lightX", 200, -10000, 10000);
	panel.addSlider("lightY", 400, -10000, 10000);
	panel.addSlider("lightZ", -800, -10000, 10000);
	panel.addToggle("randomLighting", true);
	panel.addToggle("kinectLighting", false);
    panel.loadSettings("settings.xml");

}

void testApp::updatePointCloud() {
	int w = 640;
	int h = 480;
    kinectMesh.clear();
	kinectMesh.setMode(OF_PRIMITIVE_POINTS);
	int step = 4;
    
    bool kinectLighting = getb("kinectLighting");
    kincetClosestPoint = ofVec3f(0,0,10000);
    kinectOrigin = ofVec3f(getf("kinectPositionX"), getf("kinectPositionY"), getf("kinectPositionZ"));
    kinectRotation = ofVec3f(getf("kinectRotationX"), getf("kinectRotationY"), getf("kinectRotationZ"));
    kinectScale = getf("kinectScale");
    
    kincetClosestPoint.rotate(kinectRotation.x, kinectRotation.y, kinectRotation.z);
    kincetClosestPoint *= ofVec3f(kinectScale,kinectScale, kinectScale);
    kincetClosestPoint += kinectOrigin;
    
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > 0) {
                ofVec3f v = kinect.getWorldCoordinateAt(x, y) * ofVec3f(1,1,1);
                v.rotate(kinectRotation.x, kinectRotation.y, kinectRotation.z);
                v *= ofVec3f(kinectScale,kinectScale, kinectScale);
                v += kinectOrigin;
                if(kinectLighting && v.distance(kinectOrigin) < kincetClosestPoint.distance(kinectOrigin)) {
                    kincetClosestPoint = v;
                }
				kinectMesh.addColor(kinect.getColorAt(x,y));
				kinectMesh.addVertex(v);
			}
		}
	}
    
    float smoothFactor = 0.075;
    float invSmoothFactor = 1.0 - smoothFactor;
    
    if(getb("kinectLighting")){
        setf("lightX", (getf("lightX")*invSmoothFactor) + (smoothFactor*kincetClosestPoint.x));
        setf("lightY", (getf("lightY")*invSmoothFactor) + (smoothFactor*kincetClosestPoint.y));
        setf("lightZ", (getf("lightZ")*invSmoothFactor) + (smoothFactor*kincetClosestPoint.z));
    }
}

void testApp::drawPointCloud() {
	   
	glPointSize(3);
	ofPushMatrix();
	ofEnableDepthTest();
	kinectMesh.drawVertices();
    ofDrawSphere(ofVec3f(getf("lightX"), getf("lightY"), getf("lightZ")), 2);
    ofDisableDepthTest();
	ofPopMatrix();
    
    if (getb("setupMode")) {
        ofPushMatrix();
        ofTranslate(kinectOrigin);
        ofRotateX(kinectRotation.x);
        ofRotateY(kinectRotation.y);
        ofRotateZ(kinectRotation.z);
        ofScale(kinectScale, kinectScale, kinectScale);
        
        drawLabeledAxes(10);
        
        ofPopMatrix();
    }
    
}

void testApp::windowResized(int w, int h){
    topDisplay.updateRenderMode();
    bottomDisplay.updateRenderMode();
}

//--------------------------------------------------------------
void testApp::exit() {
	kinect.close();
}

