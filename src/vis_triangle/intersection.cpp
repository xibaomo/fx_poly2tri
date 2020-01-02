/*
 * =====================================================================================
 *
 *       Filename:  intersection.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  12/01/2019 07:39:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */

#include "intersection.h"
#include <boost/geometry.hpp>
#include <iostream>
using namespace std;
typedef boost::geometry::model::d2::point_xy<double> pt_t;
typedef boost::geometry::model::segment<pt_t> seg_t;
int __comp_seg_intersection(const real_t s1sx, const real_t s1sy, const real_t s1ex, const real_t s1ey,
                            const real_t s2sx, const real_t s2sy, const real_t s2ex, const real_t s2ey,
                            real_t& x, real_t&y) {
    seg_t s1{{s1sx, s1sy}, {s1ex, s1ey}};
    seg_t s2{{s2sx, s2sy}, {s2ex, s2ey}};
    std::vector<pt_t> res;

    boost::geometry::intersection(s1, s2, res);

    if (res.empty())
        return 0;
    x = boost::geometry::get<0>(res[0]);
    y = boost::geometry::get<1>(res[0]);

    return 1;
    //MsTimer tm;
    //boost::geometry::distance(pt_t(s1sx,s1sy),pt_t(x,y));
    //cout<<"distance takes " << tm.elapsed()<<endl;

}

int comp_seg_intersection(const real_t s1sx, const real_t s1sy, const real_t s1ex, const real_t s1ey,
                          const real_t s2sx, const real_t s2sy, const real_t s2ex, const real_t s2ey,
                          real_t& x, real_t&y) {

    real_t x1 = s1sx;
    real_t dx1 = s1ex - s1sx;
    real_t y1 = s1sy;
    real_t dy1 = s1ey - s1sy;
    real_t x2 = s2sx;
    real_t dx2 = s2ex-s2sx;
    real_t y2 = s2sy;
    real_t dy2 = s2ey - s2sy;

    if (dx2*dy1-dx1*dy2==0) return 0;
    real_t t1 = -((-dy2*x1+dy2*x2+dx2*y1-dx2*y2)/(dx2*dy1-dx1*dy2));
    real_t t2 = -((-dy1*x1+dy1*x2+dx1*y1-dx1*y2)/(dx2*dy1-dx1*dy2));
    t1 = set_precision(t1);
    t2 = set_precision(t2);
    if (t1>=0 && t1 <= 1 && t2 >= 0 && t2 <= 1){
        x = x1 + t1 * dx1;
        y = y1 + t1 * dy1;
        return 1;
    }

    return 0;
}
