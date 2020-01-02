/*
 * =====================================================================================
 *
 *       Filename:  rig_vispoly.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  12/20/2019 11:05:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */

#ifndef  _RIG_VISPOLY_H_
#define  _RIG_VISPOLY_H_

#include "vis_triangle/intersection.h"
#include "vis_triangle/iht_poly.hpp"
#include "shapes.h"
#include <complex>
#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>
namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef double Real;
typedef std::complex<Real> Complex;
typedef boost::geometry::model::d2::point_xy<Real> bg_pt;
typedef boost::geometry::model::segment<bg_pt> bg_seg;
typedef bg::model::box<bg_pt> bg_box;
typedef std::pair<bg_seg, unsigned> bg_node;
typedef bgi::rtree<bg_node, bgi::quadratic<16>> bg_rtree;
namespace rig {
using Point = p2t::Point;
typedef std::vector<Point*> Polyline;

inline Real cross(const Complex& a, const Complex& b) {
    return a.real()*b.imag() - a.imag()*b.real();
}
inline Real dot(const Complex& a, const Complex& b) {
    return a.real()*b.real() + a.imag()*b.imag();
}

inline Real sqDistPt2Seg(const Point& pt, const Point& ps, const Point& pe) {
    Complex seg_vec (pe.x-ps.x, pe.y-ps.y);
    Real len = std::abs(seg_vec);
    seg_vec = seg_vec / len;
    Complex sp (pt.x-ps.x, pt.y-ps.y);
    Real proj = dot(sp, seg_vec);
    if ( proj>=len ) {
        Complex v(pe.x-pt.x, pe.y-pt.y);
        return dot(v, v);
    }
    if ( proj <= 0 ) {
        return dot(sp, sp);
    }

    Real pp = cross(seg_vec, sp);
    return pp*pp;
}

typedef std::pair<Point, Point> Segment;
inline int comp_seg_circle_intersection(const Segment& seg, const Point& pt, const Real R,
                                        Real& x1, Real& y1, Real& x2, Real& y2)
{
    const Real eps=1e-10;
    Real dx = seg.second.x - seg.first.x;
    Real dy = seg.second.y - seg.first.y;
    Real xc1 = pt.x - seg.first.x;
    Real yc1 = pt.y - seg.first.y;
    Real delta = dy*dy*(R - xc1)* (R + xc1) + 2*dx*dy* xc1*yc1 + dx*dx* (R - yc1)*(R + yc1);
    if (delta<=0) return 0;

    Real t1 = (1./(dx*dx + dy*dy))* (dx*xc1 + dy*yc1 - sqrt(delta));
    Real t2 = (1./(dx*dx + dy*dy))* (dx*xc1 + dy*yc1 + sqrt(delta));

    if (t1>=-eps && t1 <=1+eps && t2>=-eps && t2<=1+eps) {
        // 2 intersections
        x1 = seg.first.x+dx*t1;
        y1 = seg.first.y+dy*t1;
        x2 = seg.first.x+dx*t2;
        y2 = seg.first.y+dy*t2;
        return 2;
    } else if (t1>=-eps && t1 <=1+eps) {
        x1 = seg.first.x+dx*t1;
        y1 = seg.first.y+dy*t1;
        return 1;
    } else if (t2>=-eps && t2<=1+eps) {
        x1 = seg.first.x+dx*t2;
        y1 = seg.first.y+dy*t2;
        return 1;
    } else {
        return 0;
    }

    return -1;
}
struct Edge {
    Point* ps;
    Point* pe;
    Complex dir_vec;
};

struct PointCmp{
    bool operator()(const Point& a, const Point& b) {
        if (a.x!=b.x) return a.x < b.x;
        else {
            return a.y < b.y;
        }
        //return false;
    }
};
class RigVisPoly {
    iht::PolygonArray* m_pa;
    std::vector<Polyline*> m_allVertices;
    std::vector<Edge*>  m_allEdges;
    std::vector<Segment> m_localSegs;
    std::set<Point,PointCmp>     m_localVertices;
    Real m_losRange, m_losRange2;
  public:
    RigVisPoly(iht::PolygonArray* pa) : m_pa(pa), m_losRange(-1) {;}
    virtual ~RigVisPoly() {
        for ( auto pln : m_allVertices ) {
            for ( auto p : *pln ) {
                if ( p )
                    delete p;
            }
            delete pln;
        }
        for ( auto p : m_allEdges ) {
            if ( p )
                delete p;
        }
    }
    std::vector<Polyline*>& getVertices() {
        return m_allVertices;
    }
    std::vector<Edge*>& getAllEdges() {
        return m_allEdges;
    }
    void setLosRange(Real rng) {
        m_losRange = rng;
        m_losRange2 = rng*rng;
    }
    void convertPoly();

    // Extract segments no farther than m_losRange from the view point
    void extractLocalSegments(const Point& viewpoint);
    Point findNearestIntersection(const Segment& ray);
    void compVisPoly(const Point& viewpoint, const Complex& sv, const Complex& ev, std::vector<Point>& vis_poly);
};
}
#endif   /* ----- #ifndef _RIG_VISPOLY_H_  ----- */
