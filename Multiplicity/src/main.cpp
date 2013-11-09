#include "testApp.h"
#include "ofAppGlutWindow.h"

int main() {
	ofAppGlutWindow window;
#ifdef TARGET_OSX
	window.setGlutDisplayString("rgba double samples>=8 depth");
#endif

    
	ofSetupOpenGL(&window, 800, 600, OF_WINDOW);
	ofRunApp(new testApp());
}
