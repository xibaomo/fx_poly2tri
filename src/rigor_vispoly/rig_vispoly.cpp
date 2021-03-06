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
    Real min_dist2 = m_losRange2;
    Complex vec(ray.second.x-ray.first.x,ray.second.y-ray.first.y);
    vec=vec/std::abs(vec);
    Real csx,csy;
    csx = vec.real()*m_losRange + ray.first.x;
    csy = vec.imag()*m_losRange + ray.first.y;
    cs = Point{csx,csy};
    for(auto& seg  : m_localSegs) {
        int np = comp_seg_intersection(ray.first.x,ray.first.y,ray.second.x,ray.second.y,
                                       seg.first.x,seg.first.y,seg.second.x,seg.second.y,
                                       //e->ps->x,e->ps->y,e->pe->x,e->pe->y,
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

void
RigVisPoly::extractLocalSegments(const Point& vp) {
    m_localSegs.clear();
    m_localVertices.clear();
    for(auto e : m_allEdges) {
        Real dist2 = sqDistPt2Seg(vp,*e->ps,*e->pe);
        Complex vs(e->ps->x-vp.x,e->ps->y-vp.y);
        Complex ve(e->pe->x-vp.x,e->pe->y-vp.y);
        Real ds = dot(vs,vs);
        Real de = dot(ve,ve);

        dist2=set_precision(dist2);
        ds =   set_precision(ds);
        de = set_precision(de);
        if (dist2 >= m_losRange2)
            continue;
        if (dist2 <EPS+100)
            continue;
        if (ds <= m_losRange2 && de<=m_losRange2) {
            // no intersection
            m_localSegs.emplace_back(std::make_pair(*e->ps,*e->pe));
            continue;
        }
        // segment intersects circle
        Segment seg = std::make_pair(*e->ps,*e->pe);
        Real x1,y1,x2,y2;
        int np = comp_seg_circle_intersection(seg,vp,m_losRange,x1,y1,x2,y2);
        if (np==0)
            continue;
        if (np==2) {
            Segment s = std::make_pair(Point{x1,y1},Point{x2,y2});
            m_localSegs.emplace_back(s);
        }
        if (np==1) {
            Point cs{x1,y1};
            Segment s;
            if (ds<de) {
                s = std::make_pair(cs,*e->ps);
            } else {
                s = std::make_pair(cs,*e->pe);
            }
            m_localSegs.emplace_back(s);
        }
    }
    for(auto& seg : m_localSegs) {
        m_localVertices.insert(seg.first);
        m_localVertices.insert(seg.second);
    }

}
void RigVisPoly::compVisPoly(const Point& vp, const Complex& sv, const Complex& ev, std::vector<Point>& vis_poly) {
    m_localSegs.clear();
    extractLocalSegments(vp);
    for(auto& vtx : m_localVertices) {
        //for(auto vtx : *pln) {
        Complex dir(vtx.x-vp.x,vtx.y-vp.y);
        if (vp.x==93 && vp.y==100 )
            cout<<endl;
        if(dir==Complex(0,0))
            continue;
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

            Real d = std::abs(csp.dir_vec);
            d= set_precision(d);
            if(d<=m_losRange)
                vis_poly.push_back(csp);
        }
        //jmp1:
        if (cross(sv,rn)>=0 && cross(ev,rn)<=0) {
            //if (cross(sv,rn)==0 && dot(sv,rn)<0) goto jmp2;
            Point csn = findNearestIntersection(seg_neg);
            csn.dir_vec = Complex(csn.x-vp.x,csn.y-vp.y);

            Real d = std::abs(csn.dir_vec);
            d = set_precision(d);
            if (d<=m_losRange)
                vis_poly.push_back(csn);
        }
    }
    // sv direction
    Complex svv = sv/std::abs(sv);
    Segment seg = std::make_pair(vp,Point{vp.x+svv.real()*m_losRange*2,vp.y+svv.imag()*m_losRange*2});
    Point cs = findNearestIntersection(seg);
    cs.dir_vec = Complex(cs.x-vp.x,cs.y-vp.y);
    Real d = std::abs(cs.dir_vec);
    d = set_precision(d);
    if (d<=m_losRange)
        vis_poly.push_back(cs);

    // ev direction
    Complex evv = ev/std::abs(ev);
    seg = std::make_pair(vp,Point{vp.x+evv.real()*m_losRange*2,vp.y+evv.imag()*m_losRange*2});
    cs =findNearestIntersection(seg);
    cs.dir_vec = Complex(cs.x-vp.x,cs.y-vp.y);
    d = std::abs(cs.dir_vec);
    d = set_precision(d);
    if(d<=m_losRange)
        vis_poly.push_back(cs);


    std::sort(vis_poly.begin(),vis_poly.end(),[](Point& a, Point& b) {
        Real cr = cross(a.dir_vec,b.dir_vec);
        if(cr>0)
            return true;
        return false;
    });
}
}
