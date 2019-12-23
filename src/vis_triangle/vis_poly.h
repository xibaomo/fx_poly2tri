/*
 * =====================================================================================
 *
 *       Filename:  vis_poly.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  12/04/2019 12:19:51 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */

#ifndef  vis_poly_INC
#define  vis_poly_INC
#include <map>
#include <iostream>
#include <vector>
#include <complex>
#include <math.h>
#include "intersection.h"
#include "shapes.h"
#include "iht_poly.hpp"
namespace vis {
    using Point = p2t::Point;
    using Triangle = p2t::Triangle;
    typedef std::array<double, 4> Edge;
    struct EdgeComp {
        bool operator()(const Edge& e1, const Edge& e2) {
            if ( e1[0] != e2[0] ) {
                return e1[0] < e2[0];
            } else if (e1[1] != e2[1]) {
                return e1[1] < e2[1];
            } else if (e1[2] != e2[2]) {
                return e1[2] < e2[2];
            } else if (e1[3] != e2[3]){
                return e1[3] < e2[3];
            } else {
                return false;
            }
            return false;
        }
    };
    struct VertexAngle {
        Point* vtx;
        double polar_angle;
    };
    struct EdgeInfo {
        Point* opp_pts;
        Triangle* hostTriangle;
        bool isTrueEdge;
        std::pair<VertexAngle, VertexAngle> vtx_angles ; // sorted
    };

    class TriEdgeContainer {
   private:
       iht::PolygonArray* m_hostPolyArray;
       std::vector<Point*> m_allVertices;
       std::map<Edge, EdgeInfo, EdgeComp> m_edge2info;
       double m_losRange;
   public:
       TriEdgeContainer(iht::PolygonArray* pa) : m_hostPolyArray(pa) {m_losRange = 1000/0.05;; }
    };
}
#endif   /* ----- #ifndef vis_poly_INC  ----- */
