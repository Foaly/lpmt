#include "quad.h"
#include <dirent.h>

//--------------------------------------------------------------
void quad::allocateFbo(int w, int h)
{
    settings.width = w;
    settings.height = h;
    quadFbo.allocate(settings);
    maskFbo.allocate(settings);
    targetFbo.allocate(settings);
}

// adds a new point to the mask point vector
// the given point has to be in non-normalized window pixel coordinates
void quad::maskAddPoint(ofPoint point)
{
    const ofPoint warpedPoint = getWarpedPoint(point);
    const ofPoint normalizedPoint(warpedPoint.x / ofGetWidth(), warpedPoint.y / ofGetHeight());

    m_maskPoints.push_back(normalizedPoint);
}

ofPoint quad::getWarpedPoint(ofPoint point)
{
    return findWarpedPoint(src, dst, point);
}



//---------------------------------------------------------------
void quad::loadImageFromFile(string imgName, string imgPath)
{
    ofFile image(imgPath);
    imgBg = true;
    img.loadImage(image);
    bgImg = imgPath;
    loadedImg = imgName;
}


//---------------------------------------------------------------
void quad::loadVideoFromFile(string videoName, string videoPath)
{
    //ofFile movie(videoPath);
    videoBg = true;
    bgVideo = videoPath;
    loadedVideo = videoName;
    if (video.isLoaded())
    {
        video.closeMovie();
    }
    video.loadMovie(videoPath);
    videoWidth = video.width;
    videoHeight = video.height;

    video.play();
    loadedVideo = videoName;
}

// a func for reading a dir content to a vector of strings
int quad::getdir (string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL)
    {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL)
    {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

void quad::setupCamera()
{
    camWidth = cams[camNumber].width;
    camHeight = cams[camNumber].height;

}
