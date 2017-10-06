/*
 * Copyright (c) 2009 University of Michigan, Ann Arbor.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of Michigan, Ann Arbor. The name of the University
 * may not be used to endorse or promote products derived from this
 * software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * Authors: Ari Grant
 *
*/
#ifndef __MESH_H__
#define __MESH_H__

#include <vector>
#include "xvec.h"

// the basic vertex object
typedef struct Vertex
{
	Vertex() : position(0.0f), normal(0.0f), color(0.0f) {}
	
	XVec3f position;
	XVec3f normal;
	XVec3f color;
	
} Vertex;

// four integers to lookup within a vector coordinate array
typedef XVec4ui Quad;

// the simple mesh class (supporting only quads)
class Mesh
{
public:
	Mesh() {}
	~Mesh() {}
	
	// perform subdivision on the mesh
	void subdivide();
	
	// draw the mesh
	void draw(bool withWireframe);
	
	// randomize the vertex locations
	void randomize();
	
	// become a cube
	void toCube();
	
private:
	// coordinate array of vertices
	std::vector<Vertex> _vertices;
	
	// the list of quads in the mesh
	std::vector<Quad> _quads;
	
	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
			// functions for subdivision
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
	
	
	// returns the center vertex of a given quad
	Vertex centerOfQuad(unsigned int quad);
	
	
	// returns true if the given face contains the
	// vertex and if so sets index to be which one
	// it is, so one of 0, 1, 2, or 3
	bool faceHasVertex(unsigned int face, XVec3f &vertex, unsigned int &index);
	
	
	// returns the bumber of faces/edges containing
	// the given vertex. the number of faces and
	// edges is guaranteed to be the same
	unsigned int edgesAndFacesForVertex(unsigned int vertex);
	
	
	// fills the array edges with a list of edges containing
	// the given vertex where maxEdges is the size of the array.
	// count will be the number of edges that were found
	void edgesWithVertex(XVec3f &v, unsigned int &count,
						 unsigned int *edges, unsigned int maxEdges);
	
	
	// fills the array faces with a list of faces containing
	// the given edge from v0 to v1 where maxaFaces is the
	// size of the array. count will be the number of faces
	// that were found
	void facesWithEdge(XVec3f &v0, XVec3f &v1, unsigned int &count,
					   unsigned int *faces, unsigned int maxFaces);
	
	
	// fills the array faces with a list of faces containing
	// the given vertex where maxFaces is the size of the array.
	// count will be the number of faces that were found
	void facesWithVertex(XVec3f &pt, unsigned int &count,
						 unsigned int *faces, unsigned int maxFaces);
	
	
	// averages all of the face centers of
	// faces containing the given vertex
	Vertex averageFacesAroundVertex(unsigned int vertex);
	
	
	// averages all of the edge midpoints of
	// edges containing the given vertex
	Vertex averageEdgesAroundVertex(unsigned int vertex);
	
	
	// returns the position of a given vertex by weighting
	// its position from its old location along with the
	// faces and edges containing it
	Vertex newVertexForSmooth(unsigned int vertex);
};

#endif /* __MESH_H__ */

