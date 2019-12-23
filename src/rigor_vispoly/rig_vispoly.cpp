/*
 * =====================================================================================
 *
 *       Filename:  rig_vispoly.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  12/21/2019 07:08:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */

#include "rig_vispoly.h"
#include "vis_triangle/intersection.h"
#include <limits>
#include <algorithm>
#include <iostream>
#define EPS 1E-6
using namespace std;
namespace rig {
void RigVisPoly::convertPoly() {
    for ( auto& poly : *m_pa ) {
        Polyline* pln = new Polyline;
        for ( auto it = poly.begin(); it != poly.end(); it++ ) {
            auto itt = std::next(it);
            if ( itt == poly.end())
                continue;
            Point* pt = new Point(it->x, it->y);
            pln->push_back(pt);
        }
        m_allVertices.push_back(pln);
        for(auto it = pln->begin(); it!=pln->end(); it++) {
            auto itt = std::next(it);
            if (itt==pln->end())
                itt = pln->begin();
            Edge* e = new Edge();
            e->ps = *it;
            e->pe = *itt;
            e->dir_vec = Complex((*itt)->x-(*it)->x,(*itt)->y-(*it)->y);
            m_allEdges.push_back(e);
        }
    }

    cout<<"Edges: " << m_allEdges.size() << endl;
}
Point RigVisPoly::findNearestIntersection(const Segment& ray) {
    Point cs;
    Real min_dist2 = std::numeric_limits<Real>::max();
    Real csx,csy;
    for(auto e : m_allEdges) {
        int np = comp_seg_intersection(ray.first.x,ray.first.y,ray.second.x,ray.second.y,
                                       e->ps->x,e->ps->y,e->pe->x,e->pe->y,
                                       csx,csy);
        if (np==0)
            continue;
        Real dx = csx - ray.first.x;
        Real dy = csy - ray.first.y;
        Real dist2 = dx*dx+dy*dy;
        if (dist2 < min_dist2) {
            min_dist2 = dist2;
            cs = Point{csx,csy};
        }
    }
    return cs;
}
void RigVisPoly::compVisPoly(const Point& vp, const Complex& sv, const Complex& ev, std::vector<Point>& vis_poly) {
    for(auto pln : m_allVertices) {
        for(auto vtx : *pln) {
            Complex dir(vtx->x-vp.x,vtx->y-vp.y);
            Real ang = std::arg(dir);
            Real ap = ang + EPS;
            Real an = ang - EPS;
            Segment seg_pos = std::make_pair(vp,Point{vp.x+m_losRange*cos(ap),vp.y + m_losRange*sin(ap)});
            Segment seg_neg = std::make_pair(vp,Point{vp.x+m_losRange*cos(an),vp.y + m_losRange*sin(an)});
            Complex rp(cos(ap),sin(ap));
            Complex rn(cos(an),sin(an));
            if(cross(sv,rp)>=0 && cross(ev,rp)<=0) {
                //if (cross(sv,rp)==0 && dot(sv,rp)<0) goto jmp1;
                Point csp = findNearestIntersection(seg_pos);
                csp.dir_vec = Complex(csp.x-vp.x,csp.y-vp.y);
                if(std::abs(csp.dir_vec)<=m_losRange)
                    vis_poly.push_back(csp);
            }
            //jmp1:
            if (cross(sv,rn)>=0 && cross(ev,rn)<=0) {
                //if (cross(sv,rn)==0 && dot(sv,rn)<0) goto jmp2;
                Point csn = findNearestIntersection(seg_neg);
                csn.dir_vec = Complex(csn.x-vp.x,csn.y-vp.y);
                if (std::abs(csn.dir_vec)<=m_losRange)
                    vis_poly.push_back(csn);
            }
            //jmp2:;
        }
    }

    std::sort(vis_poly.begin(),vis_poly.end(),[](Point& a, Point& b) {
        Real cr = cross(a.dir_vec,b.dir_vec);
        if(cr>0)
            return true;
        return false;
    });
}
}
