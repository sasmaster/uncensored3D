#include "scene.h"
#include "array.h"
#include <stdio.h>
void add_child(Spatial* parent,Spatial* child)
{
	if (parent->children == NULL)
	{
		parent->children = arrayCreate();
	}
	
	arrayAdd(parent->children, child);
	parent->num_children = arraySize(parent->children);
}

void remove_child(Spatial* parent,Spatial* child)
{
	for (unsigned int i = 0; i < parent->num_children; i++)
	{
	   Spatial* c =	(Spatial*)arrayGet(parent->children, 1);
	   if (child == c)
	   {
		   arrayRemove(parent->children, i);
		   break;
	   }
	}
}

void remove_all_children()
{

}
/**
* node name is limited to 19 chars
*/
void set_name(Spatial* node, const char* name)
{
	snprintf(node->name, 20, "%s", name);
}

