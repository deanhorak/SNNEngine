/*
 * Proprietary License
 * 
 * Copyright (c) 2024-2025 Dean S Horak
 * All rights reserved.
 * 
 * This software is the confidential and proprietary information of Dean S Horak ("Proprietary Information").
 * You shall not disclose such Proprietary Information and shall use it only in accordance with the terms
 * of the license agreement you entered into with Dean S Horak.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are not permitted
 * without express written permission from Dean S Horak.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * If you have any questions about this license, please contact Your Name at dean.horak@gmail.com.
 */

#include <cmath>  // Include this at the top of your file
#include "Global.h"
#include "SNNVisualizer.h"

extern SNNVisualizer *snnVisualizer;

int windowWidth = 1920;
int windowHeight = 1080;

// Lighting properties
GLfloat lightPosition[] = { 1.0f, 1.0f, 1.0f, 0.0f };
GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat lightDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

// Material properties for neurons
GLfloat matAmbient[] = { 0.2f, 0.3f, 0.8f, 1.0f };
GLfloat matDiffuse[] = { 0.2f, 0.3f, 0.8f, 1.0f };
GLfloat matSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat matShininess[] = { 50.0f };

//static float centerX, centerY, centerZ;
static float lookAtX, lookAtY, lookAtZ;
static float cameraPosX, cameraPosY, cameraPosZ;  // Camera position
float cameraZoom = 10.0f;  // Initial distance of the camera from the scene

cv::VideoWriter videoWriter;
bool isCapturing = false;
bool showDendrites = false;
int videoSuffix = 0;

// Forward declaration
class VisNeuron;

// Synapse class represents a connection point between an axon and a dendrite
class VisSynapse {
public:
    long id;
    VisNeuron* from;  // Source neuron providing input via axon
    VisNeuron* to;    // Target neuron receiving input via dendrite

    VisSynapse(VisNeuron* from, VisNeuron* to) : from(from), to(to) {}
};

// Neuron class representing a neuron with axon and dendrites
class VisNeuron {
public:
    long id;
    float x, y, z;                  // Position in 3D space
    bool isFiring;                  // Whether the neuron is firing
    float rnotfiring,gnotfiring,bnotfiring;
    float rfiring,gfiring,bfiring;
    std::map<long, VisSynapse*> dendrites;  // Incoming connections (dendrites)

    VisNeuron(float x, float y, float z) : x(x), y(y), z(z), isFiring(false) {
        rfiring=1.0;
        gfiring=0.0;
        bfiring=0.0;

        rnotfiring=0.0;
        gnotfiring=1.0;
        bnotfiring=0.0;
    }

    // Fire the neuron
    void fire() {
        isFiring = true;
    }

    // Stop firing
    void stopFiring() {
        isFiring = false;
    }

    // Draw the neuron as a sphere with an axon line extending outward
    void drawNeuron() {
        // Draw the neuron body


        glPushMatrix();
        glTranslatef(x, y, z);  // Position the neuron
        //glColor3f(neuron->r, neuron->g, neuron->b);     // Set color (using neuron properties)
        glColor3f(isFiring ? rfiring : rnotfiring, isFiring ? gfiring : gnotfiring, isFiring ? bfiring : bnotfiring);  // Red if firing, green otherwise
        //glColor3f(0.8, 0.8, 0.8);    // grey  // Set color (using neuron properties)
        glutSolidSphere(0.01, 20, 20);  // Render the neuron as a solid 3D sphere
        glPopMatrix();

        if(showDendrites) {
            // Draw axon as a green line extending from the neuron
            glBegin(GL_LINES);
            glColor3f(isFiring ? rfiring : rnotfiring, isFiring ? gfiring : gnotfiring, isFiring ? bfiring : bnotfiring);  // Red if firing, green otherwise
            glVertex3f(x, y, z);
            glVertex3f(x + 0.1, y, z);  // Extend axon to a small distance
            glEnd();
        }
    }

    // Draw dendrites (incoming synapses) as yellow lines to the neuron
    void drawDendrites() {
        glColor3f(1.0, 0.0, 0.0);  // Red for active dendrites
        for (const auto& pair : dendrites) {
            VisSynapse *synapse = pair.second;
            if(synapse->to->isFiring && synapse->from->isFiring)
            {
                glBegin(GL_LINES);
                glVertex3f(synapse->from->x + 0.1, synapse->from->y, synapse->from->z);  // Axon end of the source neuron
                glVertex3f(x, y, z);  // Target neuron's position
                glEnd();
            }
        }
    }
};

// visbrain class representing the entire neural network
class VisBrain {
public:
    std::map<long,VisNeuron *> neurons;
    std::map<long,VisSynapse*> synapses;

    // Add a neuron to the visbrain
    void addNeuron(VisNeuron* neuron) {
        if(neuron==nullptr) {
            std::cout << "Bad neuron";
        } 
        else
        {
            neurons[neuron->id] = neuron; // .insert(std::make_pair(neuron->id,neuron));

        }
    }


    // Connect two neurons with a synapse from the axon of one to the dendrite of another
    void connect(VisNeuron* from, VisNeuron* to, long synapseId) {
        VisSynapse* synapse = new VisSynapse(from, to);
        if(from!=nullptr && to != nullptr) 
        {
            synapse->id = synapseId;
            from->isFiring = true;  // Example firing state
            to->dendrites[synapse->id] = synapse; // .push_back(synapse);  // Add to target neuron's dendrites
            synapses[synapse->id] = synapse; // .push_back(synapse);       // Store in visbrain's synapses
        }
    }

    // Update the state of neurons (for example, firing)
    void update() {
        // Randomly simulate firing (to be replaced by actual logic)
        for (const auto& pair : neurons) {
            VisNeuron *neuron = pair.second;
            Neuron *n = globalObject->neuronDB.getComponent(neuron->id);
            neuron->isFiring = n->isFiring();
//        for (VisNeuron* neuron : neurons) {
// dont do anything right now
//            neuron->isFiring = (rand() % 2 == 0);
        }
    }

    // Compute bounding box of all neurons for initial view calculations
    void computeBoundingBox(float &minX, float &maxX, float &minY, float &maxY, float &minZ, float &maxZ) {
        if (neurons.empty()) return;
        
        for (const auto& pair : neurons) {
            VisNeuron *neuron = pair.second;
            minX = maxX = neuron->x;
            minY = maxY = neuron->y;
            minZ = maxZ = neuron->z;
            break;
        }

        for (const auto &pair : neurons)
        {
            if (pair.second != nullptr)
            {
                VisNeuron *neuron = pair.second;
                //        for (VisNeuron* neuron : neurons) {
                if (neuron->x < minX)
                    minX = neuron->x;
                if (neuron->x > maxX)
                    maxX = neuron->x;
                if (neuron->y < minY)
                    minY = neuron->y;
                if (neuron->y > maxY)
                    maxY = neuron->y;
                if (neuron->z < minZ)
                    minZ = neuron->z;
                if (neuron->z > maxZ)
                    maxZ = neuron->z;
            }
        }
    }
};

void SNNVisualizer::initVideoWriter() {
        // Close any previous video writer
    if (videoWriter.isOpened()) {
        videoWriter.release();
    }
    // Define the codec and create a VideoWriter object
    std::stringstream ss;
    ss << ++videoSuffix;
    std::string videofilename = std::string(globalObject->getDBPath()) +  globalObject->getModelName() + "/output_" + ss.str() +".avi";
    videoWriter.open(videofilename, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, cv::Size(windowWidth, windowHeight));
    if (!videoWriter.isOpened()) {
        std::cerr << "Failed to open video writer" << std::endl;
        exit(1);
    }
}

void SNNVisualizer::captureFrame() {

    if(!isCapturing || !videoWriter.isOpened()) {
        return;
    }

    // Allocate a buffer to store the frame data
    int maxbufferX =1920;
    int maxbufferY =1080;
    std::vector<unsigned char> buffer(maxbufferX * maxbufferY * 3);

    // Read the pixels from the frame buffer
    glReadPixels(0, 0, windowWidth, windowHeight, GL_BGR, GL_UNSIGNED_BYTE, buffer.data());


    // Convert buffer to an OpenCV Mat, flipping vertically for OpenCV format
    cv::Mat frame(windowHeight, windowWidth, CV_8UC3, buffer.data());
    cv::flip(frame, frame, 0);  // Flip the frame vertically

    // Write the frame to the video file
    videoWriter.write(frame);
}

void SNNVisualizer::reshape(int width, int height) {
    // Update the global window dimensions
    windowWidth = width;
    windowHeight = height;

    // Reset the viewport and projection to match the new window size
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)width / height, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);

    // Reinitialize the video writer with the new dimensions
    if(isCapturing) {
        initVideoWriter();
    }
}


void SNNVisualizer::keyboard(unsigned char key, int x, int y)
{
    if(key=='v' || key=='V') {
        isCapturing = !isCapturing;

        if(isCapturing) {
            initVideoWriter();
            std::stringstream ss;
            ss << videoSuffix;
            std::cout << "Video Capture output_" << ss.str() << " started" << std::endl;
        } else {
            std::stringstream ss;
            ss << videoSuffix;
            std::cout << "Video Capture output_" << ss.str() << " Stopped" << std::endl;
            videoWriter.release();
        }
    } 
    else  if(key=='d' || key=='D') {  // D = display dendrites toggle
        showDendrites = !showDendrites;

    }
}

VisBrain visbrain;


void SNNVisualizer::doWork(void)
{
    int argc = 1;
    char parm[] = "TEST";
    char **argv = nullptr;
	startVisualizer(argc, argv);

}

int SNNVisualizer::waitThread(void)
{
    int ret = waitThreadWorker();
    return ret;
}

int SNNVisualizer::waitThreadWorker(void)
{

    pid_t tid = syscall(SYS_gettid);
    std::cout << "SNNVisualizer thread is " << tid << std::endl;

    doWork();
   
    return 0;
}

void SNNVisualizer::start(void)
{
    boost::thread t(&SNNVisualizer::waitThread, this);
    t.detach(); // Don't Wait for the new thread to finish execution
}

void SNNVisualizer::stop(void)
{
    keepRunning = false;
    firstPass = true;
}



SNNVisualizer::SNNVisualizer(void) {
// Global visbrain object
    //centerX = 0.0f, centerY = 0.0f, centerZ = 10.0f;
    lookAtX = 0.0f, lookAtY = 0.0f, lookAtZ = 0.0f;
    cameraPosX = 0.0f, cameraPosY = 0.0f, cameraPosZ = 10.0f;  // Camera position
    snnVisualizer = this;

}

SNNVisualizer::~SNNVisualizer(void) {
}

// Function to display text on the screen
void renderText(float x, float y, const std::string &text) {
    glRasterPos2f(x, y);  // Set text position in the 2D coordinate space
    for (char c : text) {
        // Use a smaller font for text rendering
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);
    }
}

std::vector<std::string> SNNVisualizer::split(std::string str, char delim)
{
    std::vector<std::string> tokens;
    std::string token;

    for(char ch : str) {
        if(ch == delim) {
            if(!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token += ch;
        }
    }

    if(!token.empty()) {
        tokens.push_back(token);
    }

    return tokens;

}

// OpenGL display function
void SNNVisualizer::display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

// Set up camera position and orientation
    // Apply zoom and rotation transformations
    gluLookAt(cameraPosX, cameraPosY, cameraZoom,  // Camera position
              lookAtX, lookAtY, lookAtZ,           // Look-at point
              0.0f, 1.0f, 0.0f);                   // Up direction
    //gluLookAt(snnVisualizer->centerX, snnVisualizer->centerY, snnVisualizer->zoom, snnVisualizer->centerX, snnVisualizer->centerY, snnVisualizer->centerZ, 0.0, 1.0, 0.0);
    glRotatef(snnVisualizer->rotationX, 1.0f, 0.0f, 0.0f);
    glRotatef(snnVisualizer->rotationY, 0.0f, 1.0f, 0.0f);
    
    // Draw all neurons and their connections
    
    for (const auto& pair : visbrain.neurons) {
        if(pair.second!=nullptr)
        {
            VisNeuron *neuron = pair.second;
            Neuron* realNeuron = globalObject->neuronDB.getComponent(neuron->id); 
            neuron->drawNeuron();
            if(showDendrites) {
                neuron->drawDendrites();
            }
        }
    }


    // Switch to orthographic projection for 2D overlay (text)
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);  // Set orthographic projection matching window size
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Convert camera coordinates to string
    std::ostringstream oss;
    oss << globalObject->getCurrentTimestamp() << ": ";
    oss << "Camera Position: (" << cameraPosX << ", " << cameraPosY << ", " << cameraPosZ << ")";
    oss << "  LookAt: (" << lookAtX << ", " << lookAtY << ", " << lookAtZ << ") Zoom: (" << cameraZoom << ") ";
    for(long regionId : globalObject->brain->regions)
    {
        Region *region = globalObject->regionDB.getComponent(regionId);
        oss << "  Region: " << region->name << " (" << region->getNeuronCount() << ") ";
    }
    if(isCapturing)
        oss << " Video Recording on";
    else        
        oss << " Video Recording off";
 
    // Display text in upper right corner
    glColor3f(1.0f, 1.0f, 1.0f);  // White text color
    // 570 is our starting point
    float startingYCoord = 570;
    float height = 12; // height for GLUT_BITMAP_HELVETICA_10 + 2 for space

    renderText(0, startingYCoord, oss.str());

    startingYCoord = startingYCoord - 30 ; // startingYCoord - height

    displayImage(0, startingYCoord, globalObject->lastBuffer);



    startingYCoord = startingYCoord - 30 ; // startingYCoord - 28+2

    std::ostringstream oss2;

    oss2 << "ApCount: " << globalObject->getTotalEvents();
    
    float ycoord = startingYCoord - height; // startingYCoord - height
    renderText(0, ycoord, oss2.str());

    
/******************************** */
    for (const auto& pair : visbrain.neurons) {
        // 700000000 - 700000009
        long thisNeuronId = pair.first;

        if(thisNeuronId >= 700000000 && thisNeuronId <= 700000009)
        { 
            Neuron *neuron = globalObject->neuronDB.getComponent(thisNeuronId);
            float potential = neuron->getMembranePotential();
            float threshold = neuron->threshold;

            oss2.str("");
            oss2 << neuron->id << " : " << potential << " : " << threshold;

            ycoord -= height; // startingYCoord - height
            renderText(0, ycoord, oss2.str());
        }
    }

    // Restore perspective projection
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glutSwapBuffers();

    captureFrame();

    snnVisualizer->firstPass = false;

}

//////////////////////////////////////////////////////
void SNNVisualizer::displayImage(GLint x, GLint y, uint8_t *data) {
    //glClear(GL_COLOR_BUFFER_BIT);

    unsigned char myBuffer[28*28];
    unsigned char rotated[28*28];

    for(int i=0;i<28;i++) {
        for(int j=0;j<28;j++) {
            myBuffer[j*28+i] = data[i*28+j];
        }
    }

    // rotate 270 degrees clockwise
    for (int i = 0; i < 28; ++i) {
        for (int j = 0; j < 28; ++j) {
            rotated[(27 - j)*28+i] = myBuffer[i*28+j];
        }
    }
    
    // Set up the pixel storage mode
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    // Draw the grayscale image data at the center
    glRasterPos2i(x, y);  // Set position 
    glDrawPixels(28, 28, GL_LUMINANCE, GL_UNSIGNED_BYTE, rotated);
    
    glFlush();
}

/*
void SNNVisualizer::display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Set the camera position
    gluLookAt(0.0, 0.0, 10.0,  // Eye position
              0.0, 0.0, 0.0,   // Look-at position
              0.0, 1.0, 0.0);  // Up vector


// Render neurons as 3D spheres
//    for (const auto& neuron : neurons) {
    for (const auto& pair : visbrain.neurons) {
        VisNeuron *neuron = pair.second;
        glPushMatrix();

        glTranslatef(neuron->x, neuron->y, neuron->z);  // Position the neuron
        //glColor3f(neuron->r, neuron->g, neuron->b);     // Set color (using neuron properties)
        glColor3f(0.8, 0.8, 0.8);    // grey  // Set color (using neuron properties)

        glutSolidSphere(0.2, 20, 20);  // Render the neuron as a solid 3D sphere

        glPopMatrix();
    }

    glutSwapBuffers();
}
*/

// OpenGL idle function to update the neural network and refresh display
void SNNVisualizer::idle(void) {
    visbrain.update();
    glutPostRedisplay();
}

// Mouse button callback
void SNNVisualizer::mouseButton(int button, int state, int x, int y) {
    // Handle mouse button presses for left and right buttons
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            snnVisualizer->leftButtonDown = true;
            snnVisualizer->lastMouseX = x;
            snnVisualizer->lastMouseY = y;
        } else {
            snnVisualizer->leftButtonDown = false;
        }
    } else if (button == GLUT_RIGHT_BUTTON) {
        if (state == GLUT_DOWN) {
            snnVisualizer->rightButtonDown = true;
            snnVisualizer->lastMouseX = x;
            snnVisualizer->lastMouseY = y;
        } else {
            snnVisualizer->rightButtonDown = false;
        }
    }

    // Use mouse wheel for zoom functionality
    if (button == 3) {          // Scroll up (zoom in)
        cameraZoom -= 0.05f;     // Decrease the zoom distance
        if (cameraZoom < -100.0f)  // Set minimum zoom limit
            cameraZoom = -100.0f;
        glutPostRedisplay();
    } else if (button == 4) {   // Scroll down (zoom out)
        cameraZoom += 0.05f;     // Increase the zoom distance
        if (cameraZoom > 100.0f) // Set maximum zoom limit
            cameraZoom = 100.0f;
        glutPostRedisplay();
    }
}


void SNNVisualizer::mouseMotion(int x, int y) {
    // Calculate the change in mouse position
    float dx = x - snnVisualizer->lastMouseX;
    float dy = y - snnVisualizer->lastMouseY;

    if (snnVisualizer->leftButtonDown) {
        // Orbit around the lookAt point
        float orbitSpeed = 0.1f;  // Adjust speed of orbiting
        snnVisualizer->rotationX += dy * orbitSpeed;
        snnVisualizer->rotationY += dx * orbitSpeed;

        // Convert rotationX and rotationY from degrees to radians
        float rotationXRad = snnVisualizer->rotationX * M_PI / 180.0f;
        float rotationYRad = snnVisualizer->rotationY * M_PI / 180.0f;

        // Calculate the new camera position in spherical coordinates
        float radius = cameraZoom;  // Distance from camera to lookAt point
        float camX = lookAtX + radius * cos(rotationYRad) * cos(rotationXRad);
        float camY = lookAtY + radius * sin(rotationXRad);
        float camZ = lookAtZ + radius * sin(rotationYRad) * cos(rotationXRad);

        cameraPosX = camX;
        cameraPosY = camY;
        cameraPosZ = camZ;
    }

    if (snnVisualizer->rightButtonDown) {
        // Pan the view by moving the lookAt point
        float panSpeed = 0.01f;  // Adjust pan speed
        lookAtX -= dx * panSpeed;
        lookAtY += dy * panSpeed;
        
        // Update the camera position to reflect the new lookAt point
        cameraPosX -= dx * panSpeed;
        cameraPosY += dy * panSpeed;
    }

    // Update last mouse position
    snnVisualizer->lastMouseX = x;
    snnVisualizer->lastMouseY = y;

    // Redraw the scene
    glutPostRedisplay();
}


void SNNVisualizer::initGL() {
    glEnable(GL_DEPTH_TEST);              // Enable depth testing for 3D
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.33, 0.1, 100.0);  // 45° field of view, aspect ratio 4:3, near and far planes
    glMatrixMode(GL_MODELVIEW);

    setupLighting();  // Initialize lighting configuration
}

void SNNVisualizer::setupLighting() {
    glEnable(GL_LIGHTING);      // Enable lighting
    glEnable(GL_LIGHT0);        // Enable light source 0

    GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat lightPosition[] = { 2.0f, 5.0f, 5.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glEnable(GL_COLOR_MATERIAL);  // Allows object colors to reflect light
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}


// OpenGL initialization function
void SNNVisualizer::initOpenGL(void) {
    glClearColor(0.0, 0.0, 0.0, 1.0);  // Black background
    glEnable(GL_DEPTH_TEST);           // Enable depth testing
}

// Function to compute the initial zoom and center the neurons
void SNNVisualizer::computeInitialView(void) {
    float minX, maxX, minY, maxY, minZ, maxZ;
    visbrain.computeBoundingBox(minX, maxX, minY, maxY, minZ, maxZ);

    cameraPosX = (minX + maxX) / 2.0f;
    cameraPosY = (minY + maxY) / 2.0f;
    cameraPosZ = (minZ + maxZ) / 2.0f;

    float maxDistance = std::max(std::max(maxX - minX, maxY - minY), maxZ - minZ);
    zoom = -2.0f * maxDistance;  // Adjust the zoom based on the neuron spread

    //////// initial
    cameraPosX = 2.29974;initVideoWriter();  // Initialize the video writer
    lookAtX = -0.19;
    lookAtY = -1.06;
    lookAtZ = 0;


    zoom = 4.1998;
}

// Main function
int SNNVisualizer::startVisualizer(int argc, char **argv) {


    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Neural Network Visualization");

    //initOpenGL();
    initGL();

    initVideoWriter();  // Initialize the video writer

    int neuronCount = 0;
    float scaleFactor = 2.0f / 10000.0f;
    for(size_t iRegIx =0; iRegIx < thisBrain->regions.size(); iRegIx++)
    {
        long regionId = thisBrain->regions[iRegIx];
        Region *reg = globalObject->regionDB.getComponent(regionId);
        for (size_t iNuc = 0; iNuc < reg->nuclei.size(); iNuc++)
        {
            long nucleusId = reg->nuclei[iNuc];
            Nucleus *nucleus = globalObject->nucleusDB.getComponent(nucleusId);
            for (size_t iCol = 0; iCol < nucleus->columns.size(); iCol++)
            {
                long columnId = nucleus->columns[iCol];
                Column *column = globalObject->columnDB.getComponent(columnId);
                for (size_t iLay = 0; iLay < column->layers.size(); iLay++)
                {
                    long layerId = column->layers[iLay];
                    Layer *layer = globalObject->layerDB.getComponent(layerId);
                    for (size_t iClu = 0; iClu < layer->clusters.size(); iClu++)
                    {
                        long clusterId = layer->clusters[iClu];
                        Cluster *cluster = globalObject->clusterDB.getComponent(clusterId);
                        for (size_t iNeu = 0; iNeu < cluster->neurons.size(); iNeu++)
                        {
                            long neuronId = cluster->neurons[iNeu];
                            Neuron *neuron = globalObject->neuronDB.getComponent(neuronId);

                            float x = neuron->location.x * scaleFactor;
                            float y = neuron->location.y * scaleFactor;
                            float z = neuron->location.z * scaleFactor;
                            VisNeuron* vn = new VisNeuron(x,y,z);
                            vn->id = neuron->id;
                            if(iRegIx==0) {
                                vn->rfiring = 1.0;
                                vn->gfiring = 0.0;
                                vn->bfiring = 0.0;
                                vn->rnotfiring = 0.0;
                                vn->gnotfiring = 1.0;
                                vn->bnotfiring = 0.0;
                            } else if(iRegIx==1) {
                                vn->rfiring = 1.0;
                                vn->gfiring = 0.0;
                                vn->bfiring = 0.0;
                                vn->rnotfiring = 0.0;
                                vn->gnotfiring = 1.0;
                                vn->bnotfiring = 0.5;
                            } else {
                                vn->rfiring = 1.0;
                                vn->gfiring = 0.0;
                                vn->bfiring = 0.0;
                                vn->rnotfiring = 0.0;
                                vn->gnotfiring = 1.0;
                                vn->bnotfiring = 1.0;
                            }
/*
                            //
                            if(iRegIx==1 && iNeu==0 && iClu==0) {
                                vn->rnotfiring = 1.0;
                                vn->gnotfiring = 1.0;
                                vn->bnotfiring = 1.0;
                            }

                            if(iRegIx==1 && iNeu==0 && iClu==1) {
                                vn->rnotfiring = 0.0;
                                vn->gnotfiring = 0.0;
                                vn->bnotfiring = 1.0;
                            }
*/
                            visbrain.addNeuron(vn);
                            neuronCount++;
                        }
                    }
                }
            }
        }
    }

    for (const auto &pair : visbrain.neurons)
    {
        if (pair.second != nullptr)
        {
            VisNeuron *fromNeuron = pair.second;
            Neuron *neuron = globalObject->neuronDB.getComponent(fromNeuron->id);
            std::vector<long> *dendriteIds = neuron->getDendrites();
            for (size_t iy = 0; iy < dendriteIds->size(); iy++)
            {
                long dendriteId = (*dendriteIds)[iy];
                Dendrite *dendrite = globalObject->dendriteDB.getComponent(dendriteId);
                long connectedToNeuron = dendrite->getPreSynapticNeuronId();

                VisNeuron *toNeuron = visbrain.neurons[connectedToNeuron];
                visbrain.connect(fromNeuron, toNeuron, dendrite->getSynapseId());
            }
        }
    }

    std::cout << "Number of neurons to visualize: " << neuronCount << std::endl;

    computeInitialView();

    glutDisplayFunc(display);
    glutIdleFunc(idle);             // Idle function to update the state of neurons
    glutMouseFunc(mouseButton);     // Mouse button callback
    glutMotionFunc(mouseMotion);    // Mouse motion callback
    glutReshapeFunc(reshape);       // reshape  callback
    glutKeyboardFunc(keyboard);    // keyboard callback

    gluLookAt(cameraPosX, cameraPosY, cameraPosZ,  // Camera position
              lookAtX, lookAtY, lookAtZ,           // Look-at point
              0.0f, 1.0f, 0.0f);                   // Up direction


    glutMainLoop();

    videoWriter.release();  // Release the video writer when done
}
