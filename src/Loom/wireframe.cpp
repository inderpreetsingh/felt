/*
    This file is part of the FElt finite element analysis package.
    Copyright (C) 1993-2000 Jason I. Gobat and Darren C. Atkinson

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/****************************************************************************
 *
 * File:         wireframe.c
 *
 * Description:  
 *
 *****************************************************************************/

# include <vector>
# include <stdio.h>
# include "cvector1.hpp"
# include "wireframe.hpp"
# include "problem.h"
# include "error.h"
# include "draw.hpp"

# define min(a,b) ((a) < (b) ? (a) : (b))
# define max(a,b) ((a) > (b) ? (a) : (b))

void WriteWireframeFile (char *filename, double mag, 
                         double xrot, double yrot, double zrot, double zsc)
{
    FILE       *fp;
    unsigned	x, y;
    unsigned	i, j, k;
    unsigned	n1, n2;
    double	zplane;
    char	draw3d;
    unsigned    count;	

    fp = fopen(filename, "w");
    if (fp == NULL) 
       Fatal ("temporary file error -> wireframe");

    const unsigned ne = problem.elements.size();
    const Element *e = problem.elements.c_ptr1();

    draw3d = 0;
    zplane = e [1] -> node [1] -> z;

  
    std::vector<cvector1c> adjacency(problem.nodes.size()+1);
   
    for (i = 1 ; i <= problem.nodes.size() ; i++)
        adjacency[i] = cvector1c(i, 0);

    count = 0;

    for (i = 1; i <= ne ; i++) {
       if (e [i] -> definition -> shapenodes == 2) {
          y = max(e [i] -> node [1] -> number, e [i] -> node [2] -> number);
          x = min(e [i] -> node [1] -> number, e [i] -> node [2] -> number);

          if (!adjacency [y][x]) {
             adjacency [y][x] = 1;
             count ++;
          }

          if (!draw3d && (e [i] -> node [1] -> z != zplane || e [i] -> node [2] -> z != zplane))
             draw3d = 1;
       }
       else {
          for (j = 1 ; j <= e [i] -> definition -> shapenodes ; j++) {

             n1 = j;
             if (j == e [i] -> definition -> shapenodes) 
                n2 = 1;
             else 
                n2 = j+ 1;
            
             y = max(e [i] -> node [n1] -> number, e [i] -> node [n2] -> number);
             x = min(e [i] -> node [n1] -> number, e [i] -> node [n2] -> number);

             if (!adjacency [y][x]) {
                adjacency [y][x] = 1;
                count ++;
             }
         
             if (!draw3d && e [i] -> node[j] -> z != zplane) 
                draw3d = 1;
          }
       }
    }

    // waste the first one, it's not actually allocated so it's ok.
    std::vector< cvector1<Node> > cnxtable(count+1);
    
    for (i = 1 ; i <= count ; i++)
        cnxtable[i] = cvector1<Node>(2);

    k = 1;
    for (i = 1 ; i <= problem.nodes.size() ; i++) {
       for (j = 1 ; j <= i ; j++) {

          if (adjacency [i][j]) {
             cnxtable [k][1] = problem.nodes [i];
             cnxtable [k][2] = problem.nodes [j];
             k ++;
          }
       }
    }
   
    if (draw3d)
       WriteWireframe3D (fp, cnxtable, mag, xrot, yrot, zrot, zsc);
    else
       WriteWireframe2D (fp, cnxtable, mag);

   fclose(fp);

   return;
}
