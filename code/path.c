#include "path.h"

#include "array.h"

#include <stdio.h>
#include  <stdbool.h>

/**
* Loop blinn quadratic path triangle:
*  Start point   uv - 0.0,0.0
*  Control point uv - 0.5,0.0
*  End point     uv - 1.0,1.0
*/

static bool path_is_closed(Path* path)
{
	size_t size = kv_size(path->points);
	if (size < 6)
	{
		printf("the path contains a line segment only\n");
		return false;//this is line
	}
	vec2   p0 = { kv_A(path->points, 0),  kv_A(path->points, 1) };

	vec2   plast = { kv_A(path->points, size - 2),kv_A(path->points, size - 1) };

	return (p0[0] == plast[0] && p0[1] == plast[1]);
}

//cupic split methods courtecy of:
//https://github.com/atilimcetin/path-rendering/blob/master/ft-path.c

static void subdivide_cubic(const double c[8], double c1[8], double c2[8])
{
	double p1x = (c[0] + c[2]) / 2;
	double p1y = (c[1] + c[3]) / 2;
	double p2x = (c[2] + c[4]) / 2;
	double p2y = (c[3] + c[5]) / 2;
	double p3x = (c[4] + c[6]) / 2;
	double p3y = (c[5] + c[7]) / 2;
	double p4x = (p1x + p2x) / 2;
	double p4y = (p1y + p2y) / 2;
	double p5x = (p2x + p3x) / 2;
	double p5y = (p2y + p3y) / 2;
	double p6x = (p4x + p5x) / 2;
	double p6y = (p4y + p5y) / 2;

	double p0x = c[0];
	double p0y = c[1];
	double p7x = c[6];
	double p7y = c[7];

	c1[0] = p0x;
	c1[1] = p0y;
	c1[2] = p1x;
	c1[3] = p1y;
	c1[4] = p4x;
	c1[5] = p4y;
	c1[6] = p6x;
	c1[7] = p6y;

	c2[0] = p6x;
	c2[1] = p6y;
	c2[2] = p5x;
	c2[3] = p5y;
	c2[4] = p3x;
	c2[5] = p3y;
	c2[6] = p7x;
	c2[7] = p7y;
}

static void subdivide_cubic2(const double cin[8], double cout[16])
{
	subdivide_cubic(cin, cout, cout + 8);
}

static void subdivide_cubic4(const double cin[8], double cout[32])
{
	subdivide_cubic(cin, cout, cout + 16);
	subdivide_cubic2(cout, cout);
	subdivide_cubic2(cout + 16, cout + 16);
}

static void subdivide_cubic8(const double cin[8], double cout[64])
{
	subdivide_cubic(cin, cout, cout + 32);
	subdivide_cubic4(cout, cout);
	subdivide_cubic4(cout + 32, cout + 32);
}

static void cubic_to_quadratic(const double c[8], double q[6])
{
	q[0] = c[0];
	q[1] = c[1];
	q[2] = (3 * (c[2] + c[4]) - (c[0] + c[6])) / 4;
	q[3] = (3 * (c[3] + c[5]) - (c[1] + c[7])) / 4;
	q[4] = c[6];
	q[5] = c[7];
}


static void triangulate()
{

}


void initPath(Path* path)
{
	kv_init(path->points);
	//kv_init(path->curves);
	//kv_init(path->curve_coords);
	kv_resize(float, path->points, 50);
}

void destroyPath(Path* path)
{
	kv_destroy(path->points);
	//kv_destroy(path->curves);
	//kv_destroy(path->curve_coords);
}

void moveTo(Path*p, vec2 coord)
{
	if (kv_empty(p->commands))
	{
		kv_push(float, p->points, coord[0]);
		kv_push(float, p->points, coord[1]);
		kv_push(int, p->commands, MOVE_TO);
	}
	else
	{
		kv_a(float, p->points, 0) = coord[0];
		kv_a(float, p->points, 1) = coord[1];
	}
}

void lineTo(Path*p, vec2 coordA, vec2 coordB)
{
	//if there are not cmds,we must
	//insert move_to ,for which coordA is passed
	if (kv_empty(p->commands))
	{
		kv_push(float, p->points, coordA[0]);
		kv_push(float, p->points, coordA[1]);
		kv_push(int, p->commands, MOVE_TO);
	}

	kv_push(float, p->points, coordB[0]);
	kv_push(float, p->points, coordB[1]);
	kv_push(int, p->commands, LINE_TO);
}
void quadraticTo(Path*p, vec2 sp, vec2 cp, vec2 ep)
{

	if (kv_empty(p->commands))
	{
		kv_push(float, p->points, sp[0]);
		kv_push(float, p->points, sp[1]);
		kv_push(int, p->commands, MOVE_TO);
	}

	kv_push(float, p->points, cp[0]);
	kv_push(float, p->points, cp[1]);

	kv_push(float, p->points, ep[0]);
	kv_push(float, p->points, ep[1]);

	kv_push(int, p->commands, QUAD_TO);
}
void cubicTo(Path*p, vec2 sp, vec2 cp1, vec2 cp2, vec2 ep)
{
	int i;

	double cin[8] = { sp[0], sp[1], cp1[0], cp1[1], cp2[0], cp2[1], ep[0], ep[1] };
	double cout[64];
	subdivide_cubic8(cin, cout);

	if (kv_empty(p->commands))
	{
		kv_push(int, p->commands, MOVE_TO);
		kv_push(float, p->points, sp[0]);
		kv_push(float, p->points, sp[1]);
	}

	for (i = 0; i < 8; ++i)
	{
		double q[6];
		cubic_to_quadratic(cout + i * 8, q);
		kv_push(int, p->commands, QUAD_TO);
		kv_push(float, p->points, (float)q[2]);
		kv_push(float, p->points, (float)q[3]);
		kv_push(float, p->points, (float)q[4]);
		kv_push(float, p->points, (float)q[5]);
	}
}
void arcTo(Path*p, vec2 coord)
{

}
