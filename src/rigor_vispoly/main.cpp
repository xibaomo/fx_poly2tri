/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  12/21/2019 07:22:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */

#include "rig_vispoly.h"
#include <iostream>

using namespace std;
const double L = 160;
const double pitch = 44;
const double R = 14;
const int nv = 16;
void createPolys(iht::PolygonArray& pa)
{
    for(int i=0; i < L-2*R; i+=pitch) {
        for(int j=0; j < L-2*R; j+=pitch) {
            iht::Polygon p;
            for(int k=0;k<nv;k++) {
                int x = i+R*cos(k*M_PI/nv);
                int y = j+R*sin(k*M_PI/nv);
                p.appendVertex(iht::Point{x,y});
            }
            p.appendVertex(iht::Point{i+R*cos(0*M_PI/nv),j+R*sin(0*M_PI/nv)});
            pa.appendPolygon(std::move(p));
        }
    }

    cout<<"Polygons: " << pa.size()<<endl;
}
int main() {
    iht::PolygonArray pa;
    createPolys(pa);
    rig::RigVisPoly rvp(&pa);
    rvp.setLosRange(101.11);
    rvp.convertPoly();

    return 0;
}
