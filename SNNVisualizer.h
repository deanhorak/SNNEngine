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

#pragma once

#include <GL/glut.h>
#include <opencv2/opencv.hpp> 
#include <vector>
#include <iostream>

class Brain; // forward declaration

class SNNVisualizer {

public:

    // Variables for controlling the scene
    float rotationX = 0.0f;
    float rotationY = 0.0f;
    float zoom = -5.0f;
    int lastMouseX, lastMouseY;
    bool leftButtonDown = false;
    bool rightButtonDown = false;

    SNNVisualizer(void);
    ~SNNVisualizer(void);

	int waitThread(void);
	int waitThreadWorker(void);
	void doWork(void);
	void start(void);
	void stop(void);

   	bool keepRunning;
    bool firstPass = true;


    static void display(void);
    static void idle(void);
    static void mouseButton(int button, int state, int x, int y);
    static void mouseMotion(int x, int y);
    static void displayImage(GLint x, GLint y, uint8_t *data);
    static void initVideoWriter(void);
    static void captureFrame(void); 
    static void reshape(int width, int height);
    static void keyboard(unsigned char key, int x, int y);
    static std::vector<std::string> split(std::string str, char delim);

    void initGL(void);
    void setupLighting(void);
    void initOpenGL(void);
    void computeInitialView(void);
    int startVisualizer(int argc, char **argv);
    Brain* thisBrain = nullptr;

       
};


