#ifndef G_PATH_H
#define G_PATH_H

#include "linmath.h"

#include "vec.h"

typedef enum _PathCommand
{
	MOVE_TO,
	LINE_TO,
	QUAD_TO,
	CUBE_TO,
	ARC_TO
}PathCommand;

typedef struct _QuadraticCurve
{
	 vec2 curve[3];

}QuadraticCurve;

typedef struct _Path
{
	kvec_t(float) points;
	kvec_t(int) commands;
	//kvec_t(vec2) curves;
	//kvec_t(vec2) curve_coords;
	//kvec_t(unsigned short) curve_coords; 
}Path;

void initPath(Path* path);

void destroyPath(Path* path);


void moveTo( Path*p,  vec2 coord);

void lineTo(Path*p, vec2 coordA, vec2 coordB);

/**
* @param p  - path to work on
* @param sp - start point,which is normally last point
* of the previous segment
* @param cp - quadratic bezier control point
* @param ep - end point
*/
void quadraticTo(Path*p, vec2 sp, vec2 cp, vec2 ep);

void cubicTo(Path*p, vec2 sp, vec2 cp1, vec2 cp2, vec2 ep);

void arcTo(Path*p, vec2 coord);

#endif // !G_PATH_H

 

