/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  12/03/2019 11:17:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */

#include <cstdlib>
#include <time.h>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <boost/geometry.hpp>
#include <iostream>
#include "utils.h"

typedef boost::geometry::model::d2::point_xy<double> pt_t;
using namespace std;

#include "../poly2tri/poly2tri.h"
using namespace p2t;

void Init();
void ShutDown(int return_code);
void MainLoop(const double zoom);
void Draw(const double zoom);
void DrawMap(const double zoom);
void ConstrainedColor(bool constrain);
double StringToDouble(const std::string& s);
double Random(double (*fun)(double), double xmin, double xmax);
double Fun(double x);

/// Dude hole examples
vector<Point*> CreateHeadHole();
vector<Point*> CreateChestHole();

float rotate_y = 0,
      rotate_z = 0;
const float rotations_per_tick = .2;

/// Screen center x
double cx = 0.0;
/// Screen center y
double cy = 0.0;

/// Constrained triangles
vector<Triangle*> triangles;
/// Triangle map
list<Triangle*> map;
/// Polylines
vector< vector<Point*> > polylines;

/// Draw the entire triangle map?
bool draw_map = false;
/// Create a random distribution of points?
bool random_distribution = false;

template <class C> void FreeClear( C & cntr ) {
    for ( typename C::iterator it = cntr.begin();
            it != cntr.end(); ++it ) {
        delete * it;
    }
    cntr.clear();
}

const double L = 100;
int main(int argc, char* argv[]) {
    int num_points = 0;

    vector<p2t::Point*> polyline;

    // Create a simple bounding box
    polyline.push_back(new Point(0, 0));
    polyline.push_back(new Point(L, 0));
    polyline.push_back(new Point(L, L));
    polyline.push_back(new Point(0, L));
    cout << "Number of constrained edges = " << polyline.size() << endl;
    polylines.push_back(polyline);

    /*
     * Perform triangulation!
     */
    /*
     * STEP 1: Create CDT and add primary polyline
     * NOTE: polyline must be a simple polygon. The polyline's points
     * constitute constrained edges. No repeat points!!!
     */
    CDT* cdt = new CDT(polyline);
    /*
     * STEP 2: Add holes or Steiner points if necessary
     */

    for ( int i = 1; i < L-2; i+=96 ) {
        for ( int j = 1; j < L-2; j+=96 ) {
            vector<Point*> hole;
            hole.push_back(new Point(i, j));
            hole.push_back(new Point(i+1, j));
            hole.push_back(new Point(i+1, j+1));
            hole.push_back(new Point(i, j+1));
            cdt->AddHole(hole);
            polylines.push_back(hole);
            num_points+=hole.size();
        }
    }
    /*
     * STEP 3: Triangulate!
     */
    cdt->Triangulate();

    triangles = cdt->GetTriangles();
//  map = cdt->GetMap();

    cout << "Number of points = " << num_points << endl;
    cout << "Number of triangles = " << triangles.size() << endl;

    // Cleanup

    delete cdt;

    // Free points
    for ( int i = 0; i < polylines.size(); i++ ) {
        vector<Point*> poly = polylines[i];
        FreeClear(poly);
    }

    MsTimer tm;
    pt_t p1(0.1,0.2);
    double d;
    for(int i=0; i<10000; i++) {
        pt_t p2(i,i);
        //d = boost::geometry::distance(p1,p2);
        //d = boost::geometry::comparable_distance(p1,p2);
        d = (i-.1)*(i-.1) + (i-.2)*(i-.2);
        //d = pow(i-.1,2) + pow(i-.2,2);
    }
    cout<<"distance takes " << tm.elapsed()<<endl;

    return 0;
}

vector<Point*> CreateHeadHole() {
    vector<Point*> head_hole;
    head_hole.push_back(new Point(325, 437));
    head_hole.push_back(new Point(320, 423));
    head_hole.push_back(new Point(329, 413));
    head_hole.push_back(new Point(332, 423));

    return head_hole;
}

vector<Point*> CreateChestHole() {
    vector<Point*> chest_hole;
    chest_hole.push_back(new Point(320.72342, 480));
    chest_hole.push_back(new Point(338.90617, 465.96863));
    chest_hole.push_back(new Point(347.99754, 480.61584));
    chest_hole.push_back(new Point(329.8148, 510.41534));
    chest_hole.push_back(new Point(339.91632, 480.11077));
    chest_hole.push_back(new Point(334.86556, 478.09046));

    return chest_hole;
}

double StringToDouble(const std::string& s) {
    std::istringstream i(s);
    double x;
    if ( !(i >> x) )
        return 0;
    return x;
}

double Fun(double x) {
    return 2.5 + sin(10 * x) / x;
}

double Random(double (*fun)(double), double xmin = 0, double xmax = 1) {
    static double (*Fun)(double) = NULL, YMin, YMax;
    static bool First = true;

    // Initialises random generator for first call
    if ( First ) {
        First = false;
        srand((unsigned) time(NULL));
    }

    // Evaluates maximum of function
    if ( fun != Fun ) {
        Fun = fun;
        YMin = 0, YMax = Fun(xmin);
        for ( int iX = 1; iX < RAND_MAX; iX++ ) {
            double X = xmin + (xmax - xmin) * iX / RAND_MAX;
            double Y = Fun(X);
            YMax = Y > YMax ? Y : YMax;
        }
    }

    // Gets random values for X & Y
    double X = xmin + (xmax - xmin) * rand() / RAND_MAX;
    double Y = YMin + (YMax - YMin) * rand() / RAND_MAX;

    // Returns if valid and try again if not valid
    return Y < fun(X) ? X : Random(Fun, xmin, xmax);
}
