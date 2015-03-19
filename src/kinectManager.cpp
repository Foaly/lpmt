#include "kinectManager.h"



//---------------------------------------------------------
//This is called in the setup
bool kinectManager::setup()
{

    kinectAngle = 0;

    // enable depth->rgb image calibration
    //kinect.setRegistration(true);

    kinect.init(false,false,true);
    //kinect.init(true); // shows infrared instead of RGB video image
    //kinect.init(false, false); // disable video image (faster fps)

    // set the default size to a small power of two texture, so if there is no kinect we can allocate small textures
    // to avoid the error about copying unallocated textures when copying the kinect manager
    int width = 32;
    int height = 32;

    if(kinect.open())
    {
        width = kinect.width;
        height = kinect.height;
    }

    //grayImage.allocate(width, height, OF_IMAGE_GRAYSCALE);
    grayImage.allocate(width, height);
    grayThreshNear.allocate(width, height);
    grayThreshFar.allocate(width, height);
    //thDepthImage.allocate(width, height, OF_IMAGE_GRAYSCALE);
    thDepthImage.allocate(width, height);

    kinectOn = kinect.isConnected();

    return kinectOn;

}

//---------------------------------------------------------
void kinectManager::update()
{

    kinect.update();
    if(kinect.isFrameNew())
    {

        // load grayscale depth image from the kinect source
        grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        //grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height, OF_IMAGE_GRAYSCALE, false);
        if (kinectAngle != kinect.getCurrentCameraTiltAngle() && kinectAngle != kinect.getTargetCameraTiltAngle())
        {
            kinect.setCameraTiltAngle(kinectAngle);
        }

    }

}

//---------------------------------------------------------
ofxCvGrayscaleImage kinectManager::getThresholdDepthImage(int nearDepthTh, int farDepthTh, int blurVal)
{

    int nearThreshold = nearDepthTh;
    int farThreshold = farDepthTh;

    //grayImage.dilate();
    //grayImage.erode();

    grayThreshNear = grayImage;
    grayThreshFar = grayImage;
    thDepthImage = grayImage;
    grayThreshNear.threshold(nearThreshold, true);
    grayThreshFar.threshold(farThreshold);
    cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), thDepthImage.getCvImage(), NULL);

    thDepthImage.flagImageChanged();

    // blur filter
    thDepthImage.blur(blurVal);


    // morphology filter
    thDepthImage.erode();
    thDepthImage.dilate();
    //thDepthImage.blurGaussian(blurVal);



    return thDepthImage;

}


