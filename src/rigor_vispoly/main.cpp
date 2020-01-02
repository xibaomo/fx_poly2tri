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
#include <fstream>
using namespace std;
const double L = 220;
const double pitch = 44;
const double R = 14;
const int nv = 16;
const std::string foldername="polys/";
void createPolys(iht::PolygonArray& pa)
{
    for(int i=0; i < L-2*R; i+=pitch) {
        for(int j=0; j < L-2*R; j+=pitch) {
            iht::Polygon p;
            for(int k=0;k<nv;k++) {
                int x = i+R*cos(k*M_PI*2/nv);
                int y = j+R*sin(k*M_PI*2/nv);
                p.appendVertex(iht::Point{x,y});
            }
            p.appendVertex(iht::Point{i+R*cos(0*M_PI/nv),j+R*sin(0*M_PI/nv)});
            pa.appendPolygon(std::move(p));
        }
    }

    cout<<"Polygons: " << pa.size()<<endl;
}
void dumpPolygonArray(iht::PolygonArray& pa)
{
    ofstream ofs(foldername+"poly.cli");
    for(auto& poly : pa) {
        ofs<<"polygon:"<<endl;
        for(auto& pt : poly) {
            ofs<<pt.x<<","<<pt.y<<endl;
        }
    }
    ofs<<"polygon:"<<endl;
    ofs.close();
}
void dumpVisPoly(const std::string& fn, const std::vector<rig::Point>& vispoly)
{
    ofstream ofs(fn);
    ofs<<"X,Y"<<endl;
    for(auto& pt : vispoly) {
        ofs<<pt.x<<","<<pt.y<<endl;
    }
    ofs.close();
}
int main() {
    const double losRange = 100.;
    iht::PolygonArray pa;
    createPolys(pa);
    dumpPolygonArray(pa);
    rig::RigVisPoly rvp(&pa);
    rvp.setLosRange(losRange);
    rvp.convertPoly();

    int k=0;
    for(auto e : rvp.getAllEdges()) {
        Complex ev = e->dir_vec;
        Complex sv = -ev;
        vector<rig::Point> vis_poly;
        rvp.compVisPoly(*e->ps,sv,ev,vis_poly);

        const std::string fn = "vp"+to_string(k++)+"_"+to_string(e->ps->x)+"_"+to_string(e->ps->y)+".csv";
        dumpVisPoly(foldername+fn,vis_poly);
    }

    return 0;
}
