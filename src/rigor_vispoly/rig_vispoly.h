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
typedef double Real;
typedef std::complex<Real> Complex;
namespace rig {
    using Point = p2t::Point;
    typedef std::vector<Point*> Polyline;

    inline Real cross(const Complex& a, const Complex& b) {
        return a.real()*b.imag() - a.imag()*b.real();
    }
    inline Real dot(const Complex& a, const Complex& b) {
        return a.real()*b.real() + a.imag()*b.imag();
    }
    struct Edge {
        Point* ps;
        Point* pe;
        Complex dir_vec;
    };

    typedef std::pair<Point,Point> Segment;

    class RigVisPoly {
        iht::PolygonArray* m_pa;
        std::vector<Polyline*> m_allVertices;
        std::vector<Edge*>  m_allEdges;
        Real m_losRange;
   public:
        RigVisPoly(iht::PolygonArray* pa) : m_pa(pa),m_losRange(-1) {;}
        virtual ~RigVisPoly() {
            for ( auto pln : m_allVertices ) {
                for ( auto p : *pln ) {
                    if ( p) delete p;
                }
                delete pln;
            }
            for ( auto p : m_allEdges ) {
                if ( p) delete p;
            }
        }
        std::vector<Polyline*>& getVertices() { return m_allVertices; }
        void setLosRange(Real rng) { m_losRange = rng; }
        void convertPoly();

        Point findNearestIntersection(const Segment& ray);
        void compVisPoly(const Point& viewpoint, const Complex& sv, const Complex& ev, std::vector<Point>& vis_poly);
    };
}
#endif   /* ----- #ifndef _RIG_VISPOLY_H_  ----- */
