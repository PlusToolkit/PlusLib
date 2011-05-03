#include "segimpl.h"
// Thomas Kuiran Chen - retouched for ANSI-C++
#include <cmath>
//#include <math.h>
#include <climits>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

#define DOT_STEPS 4.0
#define DOT_RADIUS 6.0

void print_dots( Dot *dots, int ndots )
{
	for ( int d = 0; d < ndots; d++ ) {
		cout << dots[d].c << "\t" << dots[d].r << 
				"\t" << dots[d].intensity << endl;
	}
}

void print_lines( Line *lines, int nlines )
{
	for ( int l = 0; l < nlines; l++ ) {
		cout << lines[l].t << "\t" << lines[l].p << 
				"\t" << lines[l].b[0] << "\t" << lines[l].b[1] << 
				"\t" << lines[l].b[2] << "\t" << lines[l].length <<
				"\t" << lines[l].intensity << "\t" << lines[l].line_error << endl;
	}
}

void print_pair( LinePair *pair, Line *lines, Dot *dots )
{
	/* Output. */
	cout << "intensity: " << pair->intensity << endl;
	cout << "line_error: " << pair->line_error << endl;
	cout << "angle_diff: " << pair->angle_diff << endl;
	cout << "angle_conf: " << pair->angle_conf << endl;
	print_lines( lines+pair->l1, 1 );
	print_lines( lines+pair->l2, 1 );
	cout << "-----" << endl;
	for (int i=0; i<3; i++)
	{
		print_dots( dots+lines[pair->l1].b[i], 1 );
	}
	for (int i=0; i<3; i++)
	{
		print_dots( dots+lines[pair->l2].b[i], 1 );
	}
}

void SegImpl::draw_dots( pixel *image, Dot *dots, int ndots )
{
	for ( int d = 0; d < ndots; d++ ) {
		Dot *dot = dots+d;
		float row = dot->r;
		float col = dot->c;

		for ( float t = 0; t < 2*M_PI; t += M_PI/DOT_STEPS ) {
			uint r = (int)floor( row + cos(t) * DOT_RADIUS );
			uint c = (int)floor( col + sin(t)* DOT_RADIUS );

			if ( r >= 0 && r < rows && c >= 0 && c <= cols )
				image[r*cols+c] = UCHAR_MAX;
		}

		image[static_cast<int>(floor(row)*cols+floor(col))] = 0; 
	}
}

void SegImpl::draw_lines( pixel *image, Line *lines, int nlines )
{
	for ( int l = 0; l < nlines; l++ ) {
		Line *line = lines+l;

		float theta = line->t;
		float p = line->p;

		for (int i=0; i<3; i++)
		{
			draw_dots( image, dots+line->b[i], 1 );
		}		

		if ( theta < M_PI/4 || theta > 3*M_PI/4 ) {
			for ( uint y = 0; y < rows; y++ ) {
				// Thomas Kuiran Chen - retouched for ANSI-C++
				//float x = roundf(( p - y * sin(theta) ) / cos(theta));
				double x = floor( ( p - y * sin(theta) ) / cos(theta) + 0.5 );
				uint r = rows - y - 1;
				uint c = (uint)x;
				if ( c >= 0 && c < cols )
					image[r*cols+c] = UCHAR_MAX;
			}
		}
		else {
			for ( uint x = 0; x < cols; x++ ) {
				// Thomas Kuiran Chen - retouched for ANSI-C++
				//float y = roundf(( p - x * cos(theta) ) / sin(theta));
				double y = floor( ( p - x * cos(theta) ) / sin(theta) + 0.5 );
				uint r = rows - (uint)y - 1;
				uint c = x;
				if ( r >= 0 && r < rows )
					image[r*cols+c] = UCHAR_MAX;
			}
		}
	}
}

void SegImpl::draw_pair( pixel *image, LinePair *pair )
{
	/* Drawing on the original. */
	draw_lines( image, lines+pair->l1, 1 );
	draw_lines( image, lines+pair->l2, 1 );
}

void SegImpl::print_results( )
{
	cout << "===== DOTS =====" << endl;
	print_dots( dots, ndots );

	cout << "===== 3-POINT LINES =====" << endl;
	print_lines( lines, nlines );

	if ( npairs > 0 ) {
		cout << "===== PARALLEL PAIRS =====" << endl;
		for ( int p = 0; p < npairs; p++ ) {
			print_pair( pairs+p, lines, dots );
			cout << endl;
		}
	}
	else {
		cout << "ERROR: could not find any pair of parallel lines!" << endl;
	}
}

void SegImpl::draw_results( pixel *image )
{
	if ( npairs > 0 )
		draw_pair( image, pairs );
	else
	{
		cout << "ERROR: could not find the pair of the wires!  See other drawing outputs for more information!" << endl;
		draw_lines( image, lines, nlines );
		draw_dots( image, dots, ndots );
	}

}
