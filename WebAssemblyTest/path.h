#ifndef G_PATH_H
#define G_PATH_H

#include "linmath.h"

typedef struct Array Array;
 

typedef struct _Path
{

 	Array* path;
 
}Path;


void moveTo( Path*p,  vec2 coord);

void lineTo(Path*p, vec2 coord);

void quadraticTo(Path*p, vec2 coord);

void cubicTo(Path*p, vec2 coord);

void arcTo(Path*p, vec2 coord);

#endif // !G_PATH_H

 

