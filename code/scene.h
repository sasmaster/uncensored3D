#ifndef G_SCENE_H
#define G_SCENE_H

#include "linmath.h"

typedef struct Array Array;

typedef struct _Spatial
{
	vec3           position;
	quat           rotation;
	vec3           scale;
	Array*         children;
	unsigned int   num_children;
	unsigned char  dirty;
	char           name[20];
}Spatial;


void add_child(Spatial* parent, Spatial* child);

void remove_child(Spatial* parent, Spatial* child);

void remove_all_children(Spatial* root);

void set_name(Spatial* node, const char* name);

/**
* x,y,z - angles in radians
*/
inline void rotate(Spatial* node,float x,float y ,float z)
{

}

inline void translate(Spatial* node, float x,float y,float z)
{

}

inline void scale(Spatial* node,float x,float y,float z)
{

}

#endif // !G_SCENE_H


