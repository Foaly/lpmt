#ifndef UTIL_INCLUDE
#define UTIL_INCLUDE

#include "ofTypes.h"

class Util
{
public:
    static const float EPSILON;

    static bool pointInTriangle2D(ofPoint vertex1, ofPoint vertex2, ofPoint vertex3, ofPoint point);
    static ofPoint scalePointToPixel(const ofPoint& point);

};

#endif // UTIL_INCLUDE
