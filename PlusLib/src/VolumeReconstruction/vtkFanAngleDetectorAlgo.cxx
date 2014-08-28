/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtkFanAngleDetectorAlgo.h"

#include "vtkMath.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkFanAngleDetectorAlgo);

//----------------------------------------------------------------------------
vtkFanAngleDetectorAlgo::vtkFanAngleDetectorAlgo()
{
  this->Image=NULL;

  this->MaxFanAnglesDeg[0]=0.0;
  this->MaxFanAnglesDeg[1]=0.0;
  this->DetectedFanAnglesDeg[0]=0.0;
  this->DetectedFanAnglesDeg[1]=0.0;
  this->IsFrameEmpty=true;

  this->ClipRectangleOrigin[0] = 0;
  this->ClipRectangleOrigin[1] = 0;
  this->ClipRectangleSize[0] = 0;
  this->ClipRectangleSize[1] = 0;

  this->FanOrigin[0] = 0.0;
  this->FanOrigin[1] = 0.0;
  this->FanRadiusStart = 0.0;
  this->FanRadiusStop = 500.0;
}

//----------------------------------------------------------------------------
vtkFanAngleDetectorAlgo::~vtkFanAngleDetectorAlgo()
{
  SetImage(NULL);
}

//----------------------------------------------------------------------------
void vtkFanAngleDetectorAlgo::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkFanAngleDetectorAlgo::Update()
{
  double presetAngleLeft = this->MaxFanAnglesDeg[0];
  double presetAngleRight = this->MaxFanAnglesDeg[1];
  int xOrigin = this->FanOrigin[0];
  int yOrigin = this->FanOrigin[1];
  vtkImageData* frameImage = this->Image;

	int nRadii = 3;
	double testRadius1 = 320;
	double testRadius2 = 380;
	double testRadius3 = 440; //480
	double angularResolution = 1;
	double thetaIncrement1 = angularResolution/testRadius1 * (180 / vtkMath::Pi()); 
	double thetaIncrement2 = angularResolution/testRadius2 * (180 / vtkMath::Pi()); 
	double thetaIncrement3 = angularResolution/testRadius3 * (180 / vtkMath::Pi()); 

	// Angle Detection Parameters
	double nPix  = 20;
	double threshold = 33; //26
	double buffer = 0; //degrees

	// Indecies and definitions 
	int left = 0;
	int right = 1;
	int test1 = 0;
	int test2 = 1;
	int test3 = 2;
	int bad = 0;
	int good = 1;

	// create and initialize vectors of test thetas
	std::vector<double> testTheta1;
	std::vector<double> testTheta2;
	std::vector<double> testTheta3;
	testTheta1.push_back(presetAngleLeft);
	testTheta2.push_back(presetAngleLeft);
	testTheta3.push_back(presetAngleLeft);

	// initialize test (x,y) coordinates based on the first test angle
	int testX1 = vtkMath::Round(xOrigin + testRadius1*sin(vtkMath::RadiansFromDegrees(testTheta1[0])));
	int testX2 = vtkMath::Round(xOrigin + testRadius2*sin(vtkMath::RadiansFromDegrees(testTheta2[0])));
	int testX3 = vtkMath::Round(xOrigin + testRadius3*sin(vtkMath::RadiansFromDegrees(testTheta3[0])));
	int testY1 = vtkMath::Round(yOrigin + testRadius1*cos(vtkMath::RadiansFromDegrees(testTheta1[0])));
	int testY2 = vtkMath::Round(yOrigin + testRadius2*cos(vtkMath::RadiansFromDegrees(testTheta2[0])));
	int testY3 = vtkMath::Round(yOrigin + testRadius3*cos(vtkMath::RadiansFromDegrees(testTheta3[0])));

	// print screens for debugging
	// std::cout << testX1 << std::endl;
	// std::cout << testY1 << std::endl;
	// std::cout << frameImage->GetScalarTypeAsString() << std::endl;

	// create and initialize testValue vectors
	std::vector<unsigned char> testValue1; 
	std::vector<unsigned char> testValue2;
	std::vector<unsigned char> testValue3;
	testValue1.push_back(*static_cast<unsigned char*>(frameImage->GetScalarPointer(testX1,testY1,0)));
	testValue2.push_back(*static_cast<unsigned char*>(frameImage->GetScalarPointer(testX2,testY2,0)));
	testValue3.push_back(*static_cast<unsigned char*>(frameImage->GetScalarPointer(testX2,testY2,0)));

	// run three while loops to populate the testValue vectors. Note that test x's and y's are not being retained.
	int i = 0;

	while (testTheta1[i] + thetaIncrement1 <= presetAngleRight) {

		testTheta1.push_back(testTheta1[i] + thetaIncrement1);
		testX1 = vtkMath::Round(xOrigin + testRadius1*sin(vtkMath::RadiansFromDegrees(testTheta1[i+1])));
		testY1 = vtkMath::Round(yOrigin + testRadius1*cos(vtkMath::RadiansFromDegrees(testTheta1[i+1])));
		testValue1.push_back(*static_cast<unsigned char*>(frameImage->GetScalarPointer(testX1,testY1,0)));
		//std::cout << (int) testValue1[i] << std::endl;
		i++;

	}

	i = 0;

	while (testTheta2[i] + thetaIncrement2 <= presetAngleRight) {

		testTheta2.push_back(testTheta2[i] + thetaIncrement2);
		testX2 = vtkMath::Round(xOrigin + testRadius2*sin(vtkMath::RadiansFromDegrees(testTheta2[i+1])));
		testY2 = vtkMath::Round(yOrigin + testRadius2*cos(vtkMath::RadiansFromDegrees(testTheta2[i+1])));
		testValue2.push_back(*static_cast<unsigned char*>(frameImage->GetScalarPointer(testX2,testY2,0)));
		i++;

	}

	i = 0;

	while (testTheta3[i] + thetaIncrement3 <= presetAngleRight) {

		testTheta3.push_back(testTheta3[i] + thetaIncrement3);
		testX3 = vtkMath::Round(xOrigin + testRadius3*sin(vtkMath::RadiansFromDegrees(testTheta3[i+1])));
		testY3 = vtkMath::Round(yOrigin + testRadius3*cos(vtkMath::RadiansFromDegrees(testTheta3[i+1])));
		testValue3.push_back(*static_cast<unsigned char*>(frameImage->GetScalarPointer(testX3,testY3,0)));
		i++;

	}

	// Print lines for debugging
	// std::cout << testTheta1.size() << std::endl;
	// std::cout << testTheta2.size() << std::endl;
	// std::cout << testTheta3.size() << std::endl;
	// Find the limiting fan angles

	// create fan angles vector and initialize values to zero
	std::vector< std::vector<double> > fanAngles(nRadii, std::vector<double>(2));

	for (int i = 0; i < nRadii; i++) {

		for (int j = 0; j < 2; j++) {

			fanAngles[i][j] = 0;

		}
	}

	// variables for use in the loop
	int nTheta1 = testTheta1.size();
	int nTheta2 = testTheta2.size();
	int nTheta3 = testTheta3.size();

	// create and initialize state variable
	int state1 = bad;
	int state2 = bad;
	int state3 = bad;

	// create and initialize angle log
	std::vector<double> left_log_1;
	std::vector<double> right_log_1;
	std::vector<double> left_log_2;
	std::vector<double> right_log_2;
	std::vector<double> left_log_3;
	std::vector<double> right_log_3;

	left_log_1.push_back(presetAngleRight);
	left_log_2.push_back(presetAngleRight);
	left_log_3.push_back(presetAngleRight);
	right_log_1.push_back(presetAngleRight);
	right_log_2.push_back(presetAngleRight);
	right_log_3.push_back(presetAngleRight);

	int j = nPix;

	while (true) {

		if (j <= nTheta1) {

			double testCount = 0;

			for (int k = 0; k < nPix; k++) {

				if (testValue1[j - nPix + k] >= threshold) {testCount++;}

			}

			if (state1 == bad && testCount >= nPix/2.0) {

				left_log_1[left_log_1.size() - 1] = testTheta1[j - nPix];
				state1 = good;

			} else if (state1 == good && testCount <= nPix/2.0 ) {

				right_log_1[right_log_1.size() - 1] = testTheta1[j - 1];
				left_log_1.push_back(presetAngleRight);
				right_log_1.push_back(presetAngleRight);
				state1 = bad;

			}



		}

		if (j <= nTheta2) {

			double testCount = 0;

			for (int k = 0; k < nPix; k++) {

				if (testValue2[j - nPix + k] >= threshold) {testCount++;}

			}

			if (state2 == bad && testCount >= nPix/2.0) {

				left_log_2[left_log_2.size() - 1] = testTheta2[j - nPix];
				state2 = good;

			} else if (state2 == good && testCount <= nPix/2.0 ) {

				right_log_2[right_log_2.size() - 1] = testTheta2[j - 1];
				left_log_2.push_back(presetAngleRight);
				right_log_2.push_back(presetAngleRight);
				state2 = bad;

			}



		}

		if (j <= nTheta3) {

			double testCount = 0;

			for (int k = 0; k < nPix; k++) {

				if (testValue3[j - nPix + k] >= threshold) {testCount++;}

			}

			if (state3 == bad && testCount >= nPix/2.0) {

				left_log_3[left_log_3.size() - 1] = testTheta3[j - nPix];
				state3 = good;

			} else if (state3 == good && testCount <= nPix/2.0 ) {

				right_log_3[right_log_3.size() - 1] = testTheta3[j - 1];
				left_log_3.push_back(presetAngleRight);
				right_log_3.push_back(presetAngleRight);
				state3 = bad;

			}


		} else {

			break;

		}

		j++; 

	}

	std::cout << "Completed the detection loop?!" << std::endl;

	// pick the largest continuous area of pixel value for each test radius and send it to fanAngles

	fanAngles[test1][left] = left_log_1[0];
	fanAngles[test1][right] = right_log_1[0];

	for (int k = 0; k < left_log_1.size(); k++) {

		if ( (right_log_1[k] - left_log_1[k]) > (fanAngles[test1][right] - fanAngles[test1][left]) ) {

			fanAngles[test1][left] = left_log_1[k];
			fanAngles[test1][right] = right_log_1[k];

		}

	}

	// std::cout << "log 1" << std::endl;
	// std::cout << left_log_1 << std::endl;
	// std::cout << right_log_1 << std::endl;

	fanAngles[test2][left] = left_log_2[0];
	fanAngles[test2][right] = right_log_2[0];

	for (int k = 0; k < left_log_2.size(); k++) {

		if ( (right_log_2[k] - left_log_2[k]) > (fanAngles[test2][right] - fanAngles[test2][left]) ) {

			fanAngles[test2][left] = left_log_2[k];
			fanAngles[test2][right] = right_log_2[k];

		}

	}

	// std::cout << "log 2" << std::endl;
	// std::cout << left_log_2 << std::endl;
	// std::cout << right_log_2 << std::endl;

	fanAngles[test3][left] = left_log_3[0];
	fanAngles[test3][right] = right_log_3[0];

	for (int k = 0; k < left_log_3.size(); k++) {

		if ( (right_log_3[k] - left_log_3[k]) > (fanAngles[test3][right] - fanAngles[test3][left]) ) {

			fanAngles[test3][left] = left_log_3[k];
			fanAngles[test3][right] = right_log_3[k];

		}

	}


	// std::cout << "log 3" << std::endl;
	// std::cout << left_log_3 << std::endl;
	// std::cout << right_log_3 << std::endl;

	// search for the broadest of the three angular windows and set them as the outputs
	// broadest in case there's a ventricle or something
	double outputAngleLeft = fanAngles[test1][left];
	double outputAngleRight = fanAngles[test1][right];

	for (int ii = 1; ii < nRadii; ii++) {

		if ( (fanAngles[ii][right] - fanAngles[ii][left]) < (outputAngleRight - outputAngleLeft) ) {

			outputAngleLeft = fanAngles[ii][left];
			outputAngleRight = fanAngles[ii][right];

		}

	}

	// set the blank frame case (i.e. both angles = presetRightAngle) to be 0 degrees for consistency
	if (outputAngleLeft == presetAngleRight && outputAngleRight == presetAngleRight) {

		outputAngleLeft = 0;
		outputAngleRight = 0;

	}

	// apply a buffer if necessary 
	if (outputAngleLeft != 0) {outputAngleLeft = outputAngleLeft - buffer;}
	if (outputAngleRight != 0) {outputAngleRight = outputAngleRight + buffer;}

  this->DetectedFanAnglesDeg[0]=outputAngleLeft;
  this->DetectedFanAnglesDeg[1]=outputAngleRight;
}
