#ifndef SAS_STACK_H
#define SAS_STACK_H

#include <stdio.h>
#include <stdlib.h>


// A structure to represent a stack (hide it in impl later)
// For pointer items only
typedef struct _Stack
{
	int top;
	unsigned capacity;
	void** array;

}Stack;

// function to create a stack of given capacity. 
inline Stack* stackCreate(unsigned capacity)
{
	Stack* stack = (Stack*) malloc(sizeof(Stack));
	stack->capacity = capacity;
	stack->top = -1;
	stack->array = malloc(stack->capacity * sizeof(void*));
	return stack;
}

inline void stackDelete(Stack* stack)
{

	if (stack->array)
	{
		free(stack->array);
	}

	free(stack);

}

// Stack is full when top is equal to the last index
inline int stackIsFull(Stack* stack)
{
	return stack->top == stack->capacity - 1;
}

// Stack is empty when top is equal to -1
inline int stackIsEmpty(Stack* stack)
{
	return stack->top == -1;
}

// Function to add an item to stack. 
inline void stackPush(Stack* stack, void* item)
{
	if (stackIsFull(stack))
	{
		return;
	}
	
	stack->array[++stack->top] = item;

}

// Function to remove an item from stack.  It decreases top by 1
inline void* stackPop(Stack* stack)
{
	if (stackIsEmpty(stack))
	{
		return NULL;
	}

	return stack->array[stack->top--];
}
#endif // !SAS_STACK_H
