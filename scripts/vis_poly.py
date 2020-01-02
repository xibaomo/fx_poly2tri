#!/usr/bin/env python
import matplotlib.pyplot as plt
import pandas as pd
import sys, os
import pdb

def  plot_origin_polys():
    with open('polys/poly.cli','r') as f:
        x=[]
        y=[]
        for line in f:
            line = line.strip()
            if line.find('polygon:') >=0:
                if len(x) >0:
                    plt.fill(x, y, facecolor='none',edgecolor='b')
                x=[]
                y=[]
                continue
            xy = line.split(',')
            x.append(float(xy[0]))
            y.append(float(xy[1]))

    plt.axis('equal')

def plot_vis_poly(vpfile):
    df = pd.read_csv(vpfile)
    x = df.iloc[:, 0].values
    y = df.iloc[:, 1].values
    plt.fill(x, y, ls='-.',lw = 2)
    plt.plot(x, y, 'x')

    i1 = vpfile.find('_')
    i2 = vpfile.find('.csv')

    svp = vpfile[i1+1:i2].split('_')
    xp = float(svp[0])
    yp = float(svp[1])
    plt.plot(xp, yp, 'ro')

def disp_all_vispoly():
    fs = os.listdir('polys')
    for f in fs:
        if not f.endswith(".csv"):
            continue
        plot_origin_polys()
        print f
        plot_vis_poly("polys/"+f)
        plt.show()

if __name__ == "__main__":
    if len(sys.argv) == 1:
        disp_all_vispoly()
        sys.exit(0)

    vpfile = sys.argv[1]
    plot_origin_polys()
    plot_vis_poly(vpfile)
    plt.show()



