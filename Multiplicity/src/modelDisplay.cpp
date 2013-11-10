//
//  modelDisplay.cpp
//  Multiplicity
//
//  Created by Ole Kristensen on 09/11/13.
//
//


#include "modelDisplay.h"
#include "testApp.h"

using namespace ofxCv;
using namespace cv;

void modelDisplay::setb(string name, bool value) {
	panel.setValueB(name, value);
}
void modelDisplay::seti(string name, int value) {
	panel.setValueI(name, value);
}
void modelDisplay::setf(string name, float value) {
	panel.setValueF(name, value);
}
bool modelDisplay::getb(string name) {
	return panel.getValueB(name);
}
int modelDisplay::geti(string name) {
	return panel.getValueI(name);
}
float modelDisplay::getf(string name) {
	return panel.getValueF(name);
}

void modelDisplay::setup(string name, const ofRectangle &displayRect, string fileName, ofxAutoControlPanel * appPanel) {
    this->displayRect.set(displayRect);
    this->name = name;
    this->appPanel = appPanel;
    calibrationReady = false;
    setupMesh(fileName);
    setupControlPanel();
}

void modelDisplay::update(){
	if(getb("selectionMode")) {
		cam.enableMouseInput();
	} else {
        if (getb("setupMode")) {
            updateRenderMode();
        }
		cam.disableMouseInput();
	}
}

void modelDisplay::draw() {
    if(getb("loadCalibration")) {
		loadCalibration();
        updateRenderMode();
		setb("loadCalibration", false);
	}
	if(getb("saveCalibration")) {
		saveCalibration();
		setb("saveCalibration", false);
	}
	if(getb("selectionMode")) {
		drawSelectionMode();
	} else {
		drawRenderMode();
	}
}

void modelDisplay::setupMesh(string fileName) {
	model.loadModel(fileName);
	objectMesh = model.getMesh(0);
	int n = objectMesh.getNumVertices();
    for(int i = 0; i < n; i++) {
        objectMesh.addColor(ofColor::white);
        objectMesh.setNormal(i, objectMesh.getNormal(i).getScaled(-1));
    }
    objectPoints.resize(n);
	imagePoints.resize(n);
	referencePoints.resize(n, false);
	for(int i = 0; i < n; i++) {
		objectPoints[i] = toCv(objectMesh.getVertex(i));
	}
}

void modelDisplay::saveCalibration() {
    if (calibrationReady){
//	string dirName = "calibration-" + name + "-" + ofGetTimestampString() + "/";
	string dirName = "calibration-" + name + "/";
	ofDirectory dir(dirName);
	dir.create();
	
	FileStorage fs(ofToDataPath(dirName + "calibration-advanced.yml"), FileStorage::WRITE);
	
	Mat cameraMatrix = intrinsics.getCameraMatrix();
	fs << "cameraMatrix" << cameraMatrix;
	
	double focalLength = intrinsics.getFocalLength();
	fs << "focalLength" << focalLength;
	
	Point2d fov = intrinsics.getFov();
	fs << "fov" << fov;
	
	Point2d principalPoint = intrinsics.getPrincipalPoint();
	fs << "principalPoint" << principalPoint;
	
	cv::Size imageSize = intrinsics.getImageSize();
	fs << "imageSize" << imageSize;
	
	fs << "translationVector" << tvec;
	fs << "rotationVector" << rvec;
    
	Mat rotationMatrix;
	Rodrigues(rvec, rotationMatrix);
	fs << "rotationMatrix" << rotationMatrix;
	
	double rotationAngleRadians = norm(rvec, NORM_L2);
	double rotationAngleDegrees = ofRadToDeg(rotationAngleRadians);
	Mat rotationAxis = rvec / rotationAngleRadians;
	fs << "rotationAngleRadians" << rotationAngleRadians;
	fs << "rotationAngleDegrees" << rotationAngleDegrees;
	fs << "rotationAxis" << rotationAxis;
	
	ofVec3f axis(rotationAxis.at<double>(0), rotationAxis.at<double>(1), rotationAxis.at<double>(2));
	ofVec3f euler = ofQuaternion(rotationAngleDegrees, axis).getEuler();
	Mat eulerMat = (Mat_<double>(3,1) << euler.x, euler.y, euler.z);
	fs << "euler" << eulerMat;
	
	ofFile basic(ofToDataPath(dirName + "calibration-basic.txt"), ofFile::WriteOnly);
	ofVec3f position( tvec.at<double>(1), tvec.at<double>(2));
	basic << "position (in world units):" << endl;
	basic << "\tx: " << ofToString(tvec.at<double>(0), 2) << endl;
	basic << "\ty: " << ofToString(tvec.at<double>(1), 2) << endl;
	basic << "\tz: " << ofToString(tvec.at<double>(2), 2) << endl;
	basic << "axis-angle rotation (in degrees):" << endl;
	basic << "\taxis x: " << ofToString(axis.x, 2) << endl;
	basic << "\taxis y: " << ofToString(axis.y, 2) << endl;
	basic << "\taxis z: " << ofToString(axis.z, 2) << endl;
	basic << "\tangle: " << ofToString(rotationAngleDegrees, 2) << endl;
	basic << "euler rotation (in degrees):" << endl;
	basic << "\tx: " << ofToString(euler.x, 2) << endl;
	basic << "\ty: " << ofToString(euler.y, 2) << endl;
	basic << "\tz: " << ofToString(euler.z, 2) << endl;
	basic << "fov (in degrees):" << endl;
	basic << "\thorizontal: " << ofToString(fov.x, 2) << endl;
	basic << "\tvertical: " << ofToString(fov.y, 2) << endl;
	basic << "principal point (in screen units):" << endl;
	basic << "\tx: " << ofToString(principalPoint.x, 2) << endl;
	basic << "\ty: " << ofToString(principalPoint.y, 2) << endl;
	basic << "image size (in pixels):" << endl;
	basic << "\tx: " << ofToString(principalPoint.x, 2) << endl;
	basic << "\ty: " << ofToString(principalPoint.y, 2) << endl;
	
	saveMat(Mat(objectPoints), dirName + "objectPoints.yml");
	saveMat(Mat(imagePoints), dirName + "imagePoints.yml");
    }
}

void modelDisplay::loadCalibration() {
    
    // retrieve advanced calibration folder
/*
    string calibPath;
    ofFileDialogResult result = ofSystemLoadDialog("Select a calibration folder", true, ofToDataPath("", true));
    calibPath = result.getPath();
  */
    
    string calibPath = "calibration-" + name;
	ofDirectory dir(calibPath);
    
    calibPath = ofToDataPath(calibPath);
    
    try {

        // load objectPoints and imagePoints
    
    Mat objPointsMat, imgPointsMat;
    loadMat( objPointsMat, calibPath + "/objectPoints.yml");
    loadMat( imgPointsMat, calibPath + "/imagePoints.yml");
    
    int numVals;
    float x, y, z;
    cv::Point3f oP;
    
    const float* objVals = objPointsMat.ptr<float>(0);
    numVals = objPointsMat.cols * objPointsMat.rows;
    
    for(int i = 0; i < numVals; i+=3) {
        oP.x = objVals[i];
        oP.y = objVals[i+1];
        oP.z = objVals[i+2];
        objectPoints[i/3] = oP;
    }
    
    cv::Point2f iP;
    
    referencePoints.resize( (imgPointsMat.cols * imgPointsMat.rows ) / 2, false);
    
    const float* imgVals = imgPointsMat.ptr<float>(0);
    numVals = objPointsMat.cols * objPointsMat.rows;
    
    for(int i = 0; i < numVals; i+=2) {
        iP.x = imgVals[i];
        iP.y = imgVals[i+1];
        if(iP.x != 0 && iP.y != 0) {
            referencePoints[i/2] = true;
        }
        imagePoints[i/2] = iP;
    }
    
    
    // load the calibration-advanced yml
    
    FileStorage fs(ofToDataPath(calibPath + "/calibration-advanced.yml", true), FileStorage::READ);
    
    Mat cameraMatrix;
    Size2i imageSize;
    fs["cameraMatrix"] >> cameraMatrix;
    fs["imageSize"][0] >> imageSize.width;
    fs["imageSize"][1] >> imageSize.height;
    fs["rotationVector"] >> rvec;
    fs["translationVector"] >> tvec;
    
    intrinsics.setup(cameraMatrix, imageSize);
    modelMatrix = makeMatrix(rvec, tvec);

        calibrationReady = true;
        setb("selectionMode", false);

    } catch (exception) {
        calibrationReady = false;
    }

}

void modelDisplay::setupControlPanel() {
	panel.setup();
    panel.ofxControlPanel::setup(name, displayRect.x+10, displayRect.y+10, 250, 400);
    panel.setXMLFilename("settings-" + name + ".xml" );
    panel.setPosition(displayRect.x+10, displayRect.y+10);
	panel.addPanel("Interaction");

    panel.addToggle("setupMode", false);

	panel.addToggle("loadCalibration", false);
	panel.addToggle("saveCalibration", false);
		
	panel.addPanel("Calibration");
	panel.addSlider("aov", 80, 50, 100);
	panel.addToggle("CV_CALIB_FIX_ASPECT_RATIO", true);
	panel.addToggle("CV_CALIB_FIX_K1", true);
	panel.addToggle("CV_CALIB_FIX_K2", true);
	panel.addToggle("CV_CALIB_FIX_K3", true);
	panel.addToggle("CV_CALIB_ZERO_TANGENT_DIST", true);
	panel.addToggle("CV_CALIB_FIX_PRINCIPAL_POINT", false);
		
	panel.addPanel("Internal");
	panel.addToggle("validShader", true);
	panel.addToggle("selectionMode", true);
	panel.addToggle("hoverSelected", false);
	panel.addSlider("hoverChoice", 0, 0, objectPoints.size(), true);
	panel.addToggle("selected", false);
	panel.addToggle("dragging", false);
	panel.addToggle("arrowing", false);
	panel.addSlider("selectionChoice", 0, 0, objectPoints.size(), true);
	panel.addSlider("slowLerpRate", .001, 0, .01);
	panel.addSlider("fastLerpRate", 1, 0, 1);
    panel.setMinimized(true);
}

void modelDisplay::updateRenderMode() {
	// generate camera matrix given aov guess
	float aov = getf("aov");
	Size2i imageSize(ofGetWidth(), ofGetHeight());
	float f = imageSize.width * ofDegToRad(aov); // i think this is wrong, but it's optimized out anyway
	Point2f c = Point2f(imageSize) * (1. / 2);
	Mat1d cameraMatrix = (Mat1d(3, 3) <<
                          f, 0, c.x,
                          0, f, c.y,
                          0, 0, 1);
    
	// generate flags
#define getFlag(flag) (panel.getValueB((#flag)) ? flag : 0)
	int flags =
    CV_CALIB_USE_INTRINSIC_GUESS |
    getFlag(CV_CALIB_FIX_PRINCIPAL_POINT) |
    getFlag(CV_CALIB_FIX_ASPECT_RATIO) |
    getFlag(CV_CALIB_FIX_K1) |
    getFlag(CV_CALIB_FIX_K2) |
    getFlag(CV_CALIB_FIX_K3) |
    getFlag(CV_CALIB_ZERO_TANGENT_DIST);
	
	vector<Mat> rvecs, tvecs;
	Mat distCoeffs;
	vector<vector<Point3f> > referenceObjectPoints(1);
	vector<vector<Point2f> > referenceImagePoints(1);
	int n = referencePoints.size();
	for(int i = 0; i < n; i++) {
		if(referencePoints[i]) {
			referenceObjectPoints[0].push_back(objectPoints[i]);
			referenceImagePoints[0].push_back(imagePoints[i]);
		}
	}
	const static int minPoints = 4;
	if(referenceObjectPoints[0].size() >= minPoints) {
		calibrateCamera(referenceObjectPoints, referenceImagePoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs, flags);
		rvec = rvecs[0];
		tvec = tvecs[0];
		intrinsics.setup(cameraMatrix, imageSize);
		modelMatrix = makeMatrix(rvec, tvec);
		calibrationReady = true;
	} else {
		calibrationReady = false;
	}
}

void modelDisplay::drawLabeledPoint(int label, ofVec2f position, ofColor color, ofColor bg, ofColor fg) {
	glPushAttrib(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	//glEnable(GL_DEPTH_TEST);
	ofVec2f tooltipOffset(5, -25);
	ofSetColor(color);
	float w = displayRect.width;
	float h = displayRect.height;
	ofSetLineWidth(1.5);
	ofLine(position - ofVec2f(w,0), position + ofVec2f(w,0));
	ofLine(position - ofVec2f(0,h), position + ofVec2f(0,h));
	ofCircle(position, appPanel->getValueF("selectedPointSize"));
	drawHighlightString(ofToString(label), position + tooltipOffset, bg, fg);
	glPopAttrib();
}

void modelDisplay::drawSelectionMode() {
    
    testApp * theApp = (testApp*)ofGetAppPtr();
    
    glScissor(displayRect.x,
              displayRect.y,
              displayRect.width,
              displayRect.height);
    
    glEnable(GL_SCISSOR_TEST);

    ofViewport(displayRect);
	ofSetColor(255);
	cam.begin();
	render();
    theApp->drawPointCloud();
	if(getb("setupMode")) {
		imageMesh = getProjectedMesh(objectMesh);
	}
	cam.end();

    ofViewport();
	if(getb("setupMode")) {
		// draw all points cyan small
		glPointSize(appPanel->getValueF("screenPointSize"));
		glEnable(GL_POINT_SMOOTH);
		ofSetColor(cyanPrint);
		imageMesh.drawVertices();
        
		// draw all reference points cyan
		int n = referencePoints.size();
		for(int i = 0; i < n; i++) {
			if(referencePoints[i]) {
				drawLabeledPoint(i, imageMesh.getVertex(i), cyanPrint);
			}
		}
		
		// check to see if anything is selected
		// draw hover point magenta
		int choice;
		float distance;
		ofVec3f selected = getClosestPointOnMesh(imageMesh, ofGetAppPtr()->mouseX, ofGetAppPtr()->mouseY, &choice, &distance);
		if(!ofGetMousePressed() && distance < appPanel->getValueF("selectionRadius")) {
			seti("hoverChoice", choice);
			setb("hoverSelected", true);
			drawLabeledPoint(choice, selected, magentaPrint);
		} else {
			setb("hoverSelected", false);
		}
		
		// draw selected point yellow
		if(getb("selected")) {
			int choice = geti("selectionChoice");
			ofVec2f selected = imageMesh.getVertex(choice);
			drawLabeledPoint(choice, selected, yellowPrint, ofColor::white, ofColor::black);
		}
	}

    glDisable(GL_SCISSOR_TEST);

}

void modelDisplay::drawRenderMode() {
    
    glScissor(displayRect.x,
              displayRect.y,
              displayRect.width,
              displayRect.height);
    
    glEnable(GL_SCISSOR_TEST);

    glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
    
	if(calibrationReady) {
		intrinsics.loadProjectionMatrix(10, 2000);
		applyMatrix(modelMatrix);
        render();
        if(getb("setupMode")) {
			imageMesh = getProjectedMesh(objectMesh);
		}
    }
    
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	if(getb("setupMode")) {
		// draw all reference points cyan
		int n = referencePoints.size();
		for(int i = 0; i < n; i++) {
			if(referencePoints[i]) {
				drawLabeledPoint(i, toOf(imagePoints[i]), cyanPrint);
			}
		}
		
		// move points that need to be dragged
		// draw selected yellow
		int choice = geti("selectionChoice");
		if(getb("selected")) {
			referencePoints[choice] = true;
			Point2f& cur = imagePoints[choice];
			if(cur == Point2f()) {
				if(calibrationReady) {
					cur = toCv(ofVec2f(imageMesh.getVertex(choice)));
				} else {
					cur = Point2f(ofGetAppPtr()->mouseX, ofGetAppPtr()->mouseY);
				}
			}
		}
		if(getb("dragging")) {
			Point2f& cur = imagePoints[choice];
			float rate = ofGetMousePressed(0) ? getf("slowLerpRate") : getf("fastLerpRate");
			cur = Point2f(ofLerp(cur.x, ofGetAppPtr()->mouseX, rate), ofLerp(cur.y, ofGetAppPtr()->mouseY, rate));
			drawLabeledPoint(choice, toOf(cur), yellowPrint, ofColor::white, ofColor::black);
			ofSetColor(ofColor::black);
			ofRect(toOf(cur), 1, 1);
		} else if(getb("arrowing")) {
			Point2f& cur = imagePoints[choice];
			drawLabeledPoint(choice, toOf(cur), yellowPrint, ofColor::white, ofColor::black);
			ofSetColor(ofColor::black);
			ofRect(toOf(cur), 1, 1);
        } else {
			// check to see if anything is selected
			// draw hover magenta
			float distance;
			ofVec2f selected = toOf(getClosestPoint(imagePoints, ofGetAppPtr()->mouseX, ofGetAppPtr()->mouseY, &choice, &distance));
			if(!ofGetMousePressed() && referencePoints[choice] && distance < appPanel->getValueF("selectionRadius")) {
				seti("hoverChoice", choice);
				setb("hoverSelected", true);
				drawLabeledPoint(choice, selected, magentaPrint);
			} else {
				setb("hoverSelected", false);
			}
		}
	}
    glDisable(GL_SCISSOR_TEST);

}

void modelDisplay::render() {

    testApp * theApp = (testApp*)ofGetAppPtr();

	ofPushStyle();
    ofEnableSmoothing();
    ofEnableDepthTest();

    ofSetLineWidth(appPanel->getValueI("lineWidth"));
    
    int shading = appPanel->getValueI("shading");
	bool useLights = shading == 1;
	bool useShadows = shading == 2;
	bool useShader = shading == 3;
    
	if(useLights) {
		theApp->light.enable();
		glShadeModel(GL_SMOOTH);
        ofEnableLighting();
		glEnable(GL_NORMALIZE);
    }
    
    if(useShadows){
        cam.end();
        glEnable( GL_DEPTH_TEST );
        ofDisableAlphaBlending();
        
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        glClear( GL_DEPTH_BUFFER_BIT );

        
        theApp->m_shadowLight.lookAt( ofVec3f(0.0,0.0,0.0) );
        theApp->m_shadowLight.enable();

        // render linear depth buffer from light view
        theApp->m_shadowLight.beginShadowMap();
        model.drawFaces();
        theApp->m_shadowLight.endShadowMap();
        
        // render final scene
        
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        theApp->m_shader.begin();
        
        theApp->m_shadowLight.bindShadowMapTexture(0); // bind shadow map texture to unit 0
        theApp->m_shader.setUniform1i("u_ShadowMap", 0); // set uniform to unit 0
        theApp->m_shader.setUniform1f("u_LinearDepthConstant", theApp->m_shadowLight.getLinearDepthScalar()); // set near/far linear scalar
        theApp->m_shader.setUniformMatrix4f("u_ShadowTransMatrix", theApp->m_shadowLight.getShadowMatrix(cam)); // specify our shadow matrix
        
        cam.begin();
        
        theApp->m_shadowLight.enable();
        objectMesh.draw();
        theApp->m_shadowLight.disable();
        
        cam.end();
        
        theApp->m_shadowLight.unbindShadowMapTexture();
        
        theApp->m_shader.end();
    } else {
    
    ofSetColor(255);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(GL_DEPTH_TEST);

    
	if(useShader) {
		ofFile fragFile("shader.frag"), vertFile("shader.vert");
		Poco::Timestamp fragTimestamp = fragFile.getPocoFile().getLastModified();
		Poco::Timestamp vertTimestamp = vertFile.getPocoFile().getLastModified();
		if(fragTimestamp != theApp->lastFragTimestamp || vertTimestamp != theApp->lastVertTimestamp) {
			bool validShader = theApp->shader.load("shader");
			appPanel->setValueB("validShader", validShader);
		}
		theApp->lastFragTimestamp = fragTimestamp;
		theApp->lastVertTimestamp = vertTimestamp;
		
		theApp->shader.begin();
		theApp->shader.setUniform1f("elapsedTime", ofGetElapsedTimef());
		theApp->shader.end();
	}

    ofColor transparentBlack(0, 0, 0, 0);
	switch(appPanel->getValueI("drawMode")) {
		case 0: // faces
            if(useShader) theApp->shader.begin();
            glDisable(GL_CULL_FACE);
            objectMesh.drawFaces();
            if(useShader) theApp->shader.end();
			break;
		case 1: // fullWireframe
            if(useShader) theApp->shader.begin();
			objectMesh.drawWireframe();
            if(useShader) theApp->shader.end();
			break;
    }
        
    }
    
	glPopAttrib();
    if(useLights){
        theApp->light.disable();
        ofDisableLighting();
    }
    if (useShadows) {
        theApp->m_shadowLight.disable();
        theApp->m_shadowLight.debugShadowMap();
        ofDisableLighting();
    }

    if(getb("setupMode")){
        drawLabeledAxes(10);
    }
    
	ofPopStyle();
}

