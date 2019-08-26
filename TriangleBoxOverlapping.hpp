#ifndef __TriangleBoxOverlapping_hpp__
#define __TriangleBoxOverlapping_hpp__

#include <cmath>
#include <glm/glm.hpp>

inline void findMinMax(float x0, float x1, float x2, float &min, float &max);

inline bool planeBoxOverlap(glm::vec3 normal, glm::vec3 vert, glm::vec3 maxbox);

/*======================== X-tests ========================*/
inline bool axisTestX01(float a, float b, float fa, float fb, const glm::vec3 &v0,
    const glm::vec3 &v2, const glm::vec3 &boxhalfsize, float &rad, float &min,
    float &max, float &p0, float &p2);

inline bool axisTestX2(float a, float b, float fa, float fb, const glm::vec3 &v0,
    const glm::vec3 &v1, const glm::vec3 &boxhalfsize, float &rad, float &min,
    float &max, float &p0, float &p1);

/*======================== Y-tests ========================*/
inline bool axisTestY02(float a, float b, float fa, float fb, const glm::vec3 &v0,
    const glm::vec3 &v2, const glm::vec3 &boxhalfsize, float &rad, float &min,
    float &max, float &p0, float &p2);

inline bool axisTestY1(float a, float b, float fa, float fb, const glm::vec3 &v0,
    const glm::vec3 &v1, const glm::vec3 &boxhalfsize, float &rad, float &min,
    float &max, float &p0, float &p1);

/*======================== Z-tests ========================*/
inline bool axisTestZ12(float a, float b, float fa, float fb, const glm::vec3 &v1,
    const glm::vec3 &v2, const glm::vec3 &boxhalfsize, float &rad, float &min,
    float &max, float &p1, float &p2);

inline bool axisTestZ0(float a, float b, float fa, float fb, const glm::vec3 &v0,
    const glm::vec3 &v1, const glm::vec3 &boxhalfsize, float &rad, float &min,
    float &max, float &p0, float &p1);

bool triBoxOverlap(glm::vec3 boxcenter, glm::vec3 boxhalfsize, glm::vec3 tv0, glm::vec3 tv1,
    glm::vec3 tv2);

#endif