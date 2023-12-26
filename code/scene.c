#include "scene.h"
#include "array.h"
#include "stack.h"
void add_child(Spatial* parent, Spatial* child)
{
	if (parent->children == NULL)
	{
		parent->children = arrayCreate();
	}

	arrayAdd(parent->children, child);
	parent->num_children = arraySize(parent->children);
}

void remove_child(Spatial* parent, Spatial* child)
{
	for (unsigned int i = 0; i < parent->num_children; i++)
	{
		Spatial* c = (Spatial*)arrayGet(parent->children, 1);
		if (child == c)
		{
			arrayRemove(parent->children, i);
			break;
		}
	}
}

void remove_all_children(Spatial* root)
{
	//we use here non - recursive graph traversal
	//why? because it is non trivial
	//https://stackoverflow.com/questions/13092673/how-do-i-traverse-a-scene-graph-like-tree-structure?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa
	Stack* stack = stackCreate(50);//should be enough for single node level
	stackPush(stack, root);

	while (!stackIsEmpty(stack))
	{
		Spatial* currNode = stackPop(stack);

		for (unsigned int i = 0; i < currNode->num_children; i++)
		{
			stackPush(stack, arrayRemove(currNode->children, i));
		}

		currNode->num_children = 0;
	}

	//delete the stack:
	stackDelete(stack);
}
/**
* node name is limited to 19 chars
*/
void set_name(Spatial* node, const char* name)
{
	snprintf(node->name, 20, "%s", name);
}

