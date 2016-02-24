#include <stdio.h>
#include <math.h>


static int Calculate_Area_Perimeter_Triangle(double ax, double ay, double bx, double by, double cx, double cy, double *perimeter, double *area)
{
    double A = sqrt((double)(bx-ax) * (bx-ax) + (by-ay) * (by-ay));
    double B = sqrt((double)(bx-cx) * (bx-cx) + (by-cy) * (by-cy));
    double C = sqrt((double)(ax-cx) * (ax-cx) + (ay-cy) * (ay-cy));
    double height = 0;

    // Heron's formula for area calculation
    // area = sqrt( s * (s-a) * (s-b) * (s-c))
    double s = (A + B + C) / 2;

    *perimeter = A + B + C;

    *area = sqrt( s * (s-A) * (s-B) * (s-C));

    // area = 1/2 * base * height
    // if side A is base, then height
    height = (*area * 2) / A;

    return 1;
}

static double		my_util_triangle_area	(double ax, double ay, double bx, double by, double cx, double cy) {

    double A = sqrt((double)(bx-ax) * (bx-ax) + (by-ay) * (by-ay));
    double B = sqrt((double)(bx-cx) * (bx-cx) + (by-cy) * (by-cy));
    double C = sqrt((double)(ax-cx) * (ax-cx) + (ay-cy) * (ay-cy));
    double height = 0;

    // Heron's formula for area calculation
    // area = sqrt( s * (s-a) * (s-b) * (s-c))
    double s = (A + B + C) / 2;
    double area = sqrt( s * (s-A) * (s-B) * (s-C));
	return area;
}

static void main()
{
    double m1, c1, m2, c2;
    double ax, ay, bx, by, cx, cy;
    double perimeter, area;
    double angleA, angleB, angleC;
    int type = 0;
    double total = 0;

	ax = 28;
	ay = 7;
	bx = 272;
	by = 39;
	cx = 272;
	cy = 33;

    Calculate_Area_Perimeter_Triangle(ax, ay, bx, by, cx, cy, &perimeter, &area);

    printf("\nPerimeter: %.4f", perimeter);
    printf("\nArea:  %.4f", area);

    printf("\nArea:  %.4f", my_util_triangle_area(ax, ay, bx, by, cx, cy));
    printf("\n");
}
