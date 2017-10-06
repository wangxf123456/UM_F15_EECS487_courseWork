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
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cstdlib>
#include <ctime>
#include <iostream>
using namespace std;

#include "xvec.h"
#include "mesh.h"

// the distance between two points needed
// to classify them as the same point
#define TOLERANCE 1E-10


// the maximum number of faces
// that can contain a vertex
#define MAX_FACES_PER_VERTEX 16


// the maximum number of edges that can contain
// a vertex. this number should be the same as
// the number of faces, but two faces may share
// an edge but not the actual vertices (that
// meaning that they have copies of the same
// vertex, which is needed if face-normals are
// used, opposed to vertex-normals)
#define MAX_EDGES_PER_VERTEX 2*MAX_FACES_PER_VERTEX


// since the mesh class only supports quads,
// a face will have 4 vertices and 4 edges
#define VERTICES_PER_FACE 4
#define EDGES_PER_FACE VERTICES_PER_FACE


// since the mesh will always have good topology
// and be closed, there will be two faces sharing
// every edge in the mesh
#define FACES_PER_EDGE 2


// average two vertices
Vertex 
averageVertices(Vertex a, Vertex b)
{
	Vertex result;
	
	// average position, normal, and color
	result.position = (a.position + b.position)/2.0f;
	result.normal = (a.normal + b.normal)/2.0f;
	result.color = (a.color + b.color)/2.0f;
	
	return result;
}


// average four vertices
Vertex 
averageVertices(Vertex a, Vertex b, Vertex c, Vertex d)
{
	return averageVertices(averageVertices(a,b),averageVertices(c,d));
}


// perform subdivision on the mesh
void Mesh::subdivide()
{
	// new temporary vectors to build the subdivided mesh;
	// if any problems occurs during the subdivide, then the
	// original mesh with remain intact
	std::vector<Vertex> newVertices;
	std::vector<Quad> newQuads;

	// to create the new vertices
	Vertex v;
	
	// for every face in the mesh
	for( unsigned int i = 0; i < _quads.size(); ++i )
	{
		// get the face center
		Vertex faceCenter = centerOfQuad(i);
		
		// add the new vertex to the mesh
		newVertices.push_back(faceCenter);
		
		// get the current quad
		Quad currentQuad = _quads[i];
				
		// for each edge of the face
		for( unsigned int j = 0; j < EDGES_PER_FACE; ++j )
		{
			// get the two vertex coordinates of the
			// edge, this will be indices (0,1), (1,2),
			// (2,3), or (3,0) in the given quad
			unsigned int k = (j+1) % EDGES_PER_FACE;
			unsigned int vertex0 = currentQuad(j);
			unsigned int vertex1 = currentQuad(k);
			
			// get the actual edge positions
			XVec3f v0 = _vertices[vertex0].position;
			XVec3f v1 = _vertices[vertex1].position;
			
			// find the other face with this edge
			// the number of faces that will be found
			unsigned int faceCount = 0;
			
			// the buffer that will contain the indices
			// of the faces that contain the edge from
			// v0 t0 v1
			unsigned int faces[FACES_PER_EDGE];
			facesWithEdge(v0, v1, faceCount, faces, FACES_PER_EDGE);
			
			// if the edge is not shared by two faces,
			// stop the subdivision, the mesh stinks!
			if( faceCount != 2 )
				return;
			
			// get the face that shares this edge
			//	that is not this face
			unsigned otherFace = faces[0];
			if( otherFace == i )
				otherFace = faces[1];
			
			// get the center of the other face
			Vertex otherFaceCenter = centerOfQuad(otherFace);
			
			// TASK 0 //
			// currently the new edge center is the average of the endpoints
			// change it to be the average of the two endpoints and the cneters
			// of the two faces containing the edge. all of what you need is
			// already prepared, but you should look around and see how it is done
			Vertex edgeCenter = averageVertices(_vertices[_quads[i](j)],_vertices[_quads[i](k)],
												faceCenter, otherFaceCenter);

			// add the new averaged edge center to the mesh
			newVertices.push_back(edgeCenter);
			
			// update one end of the edge. the other end will
			// be taken care of by the other edges, since they
			// form a closed loop (quad)
			Vertex updatedVertex = newVertexForSmooth(vertex1);
			
			// add the updated vertex to the mesh
			newVertices.push_back(updatedVertex);
		}
		
		// add the four new faces to the mesh
		//
		// the new vertices are added as:
		//
		//     add face center vertex
		//     for each edge:
		//         add vertex at center of edge
		//         add vertex at end of edge
		//
		// thus the new vertices array will have,
		// as the last nine vertices, those that
		// make four new faces. let n be the number
		// of vertices im the array at this point;
		// then n-9,n-8,...,n-2, and n-1 will form
		// a shape with topology shown below
		//
		//       (n-3)   (n-4)   (n-5)    
		//         *-------*-------*
		//         |       |       |
		//         |       |       |
		//         |       |(n-9)  |
		//   (n-2) *-------*-------* (n-6)
		//         |       |       |
		//         |       |       |
		//         |       |       |
		//         *-------*-------*
		//       (n-1)   (n-8)   (n-7)
		//
		// so, add the four new faces
		unsigned int nVerts = newVertices.size();
		newQuads.push_back(Quad(nVerts-9, nVerts-8, nVerts-7, nVerts-6));
		newQuads.push_back(Quad(nVerts-9, nVerts-6, nVerts-5, nVerts-4));
		newQuads.push_back(Quad(nVerts-9, nVerts-4, nVerts-3, nVerts-2));
		newQuads.push_back(Quad(nVerts-9, nVerts-2, nVerts-1, nVerts-8));
	}
	
	// clear the mesh vertices and quads
	_vertices.clear();
	_quads.clear();
	
	// set the mesh to be the new subdivided version
	_vertices = newVertices;
	_quads = newQuads;

  return;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
		// functions for subdivision
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //


// returns the center vertex of a given quad
Vertex 
Mesh::centerOfQuad(unsigned int quad)
{
	// average the four vertices of the given quad
	return averageVertices(_vertices[_quads[quad](0)],
						   _vertices[_quads[quad](1)],
						   _vertices[_quads[quad](2)],
						   _vertices[_quads[quad](3)]);
}


// returns true if the given face contains the
// vertex and if so sets index to be which one
// it is, so one of 0, 1, 2, or 3
bool 
Mesh::faceHasVertex(unsigned int face, XVec3f &vertex, unsigned int &index)
{
	// for each vertex in the face
    for( unsigned int i = 0; i < VERTICES_PER_FACE; i++ )
		// check if it is in the same location as vertex
		if( _vertices[_quads[face](i)].position.dist(vertex) < TOLERANCE )
		{
			//if so, record the index, and return true
			index = i;
			return true;
		}
	// this face does not contain the vertex
	return false;
	
}


// fills the array edges with a list of edges containing
// the given vertex where maxEdges is the size of the array.
// count will be the number of edges that were found
void 
Mesh::edgesWithVertex(XVec3f &v, unsigned int &count, unsigned int *edges, unsigned int maxEdges)
{
	// no edges found yet
	count = 0;
	
	// to record which index of a face is the vertex
	unsigned int index;
	
	// for every face
	for( unsigned int m = 0; m < _quads.size(); ++m )
	{
		// see if the face contains the vertex
		if( faceHasVertex(m, v, index) )
		{
			// record the edge from v to the right
			// if it has not already been found
			bool edgeAlreadyFound = false;
			for( unsigned int s = 0; s < count; ++s )
			{
				// check if the two vertices are both in the same location as the vertices in the edge
				if( (_vertices[edges[2*s]].position.dist(_vertices[_quads[m](index)].position) < TOLERANCE
					 && _vertices[edges[2*s+1]].position.dist(_vertices[_quads[m]((index+1) % 4)].position) < TOLERANCE)
				   ||
				    (_vertices[edges[2*s+1]].position.dist(_vertices[_quads[m](index)].position) < TOLERANCE
					 && _vertices[edges[2*s]].position.dist(_vertices[_quads[m]((index+1) % 4)].position) < TOLERANCE) )
					edgeAlreadyFound = true;
			}
			if( !edgeAlreadyFound )
			{
				edges[2*count] = _quads[m](index);
				edges[2*count+1] = _quads[m]((index+1) % 4);
				count++;
			}
			
			// record the edge from v to the left
			// if it has not already been found
			edgeAlreadyFound = false;
			for( unsigned int s = 0; s < count; ++s )
			{
				// check if the two vertices are both in the same location as the vertices in the edge
				if( (_vertices[edges[2*s]].position.dist(_vertices[_quads[m](index)].position) < TOLERANCE
					 && _vertices[edges[2*s+1]].position.dist(_vertices[_quads[m]((index-1) % 4)].position) < TOLERANCE)
				   ||
				   (_vertices[edges[2*s+1]].position.dist(_vertices[_quads[m](index)].position) < TOLERANCE
					&& _vertices[edges[2*s]].position.dist(_vertices[_quads[m]((index-1) % 4)].position) < TOLERANCE) )
					edgeAlreadyFound = true;
			}
			if( !edgeAlreadyFound )
			{
				edges[2*count] = _quads[m](index);
				edges[2*count+1] = _quads[m]((index-1) % 4);
				count++;
			}
			
			// if the buffer is full, stop
			if( count >= maxEdges - 2 )
				return;
		}
	}

  return;
}

// fills the array faces with a list of faces containing
// the given edge from v0 to v1 where maxaFaces is the
// size of the array. count will be the number of faces
// that were found
void 
Mesh::facesWithEdge(XVec3f &v0, XVec3f &v1, unsigned int &count, unsigned int *faces, unsigned int maxFaces)
{
	// no faces found yet
	count = 0;
	
	// to record which index of a face is the vertex
	unsigned int dummy;
	
	// for every face
	for( unsigned int m = 0; m < _quads.size(); ++m )
	{
		// check if the face contains both vertices
		if( faceHasVertex(m, v0, dummy) && faceHasVertex(m, v1, dummy) )
		{
			// if so, record the face
			faces[count] = m;
			count++;
			
			// if the buffer is full, stop
			if( count == maxFaces )
				return;
		}
	}

  return;
}


// fills the array faces with a list of faces containing
// the given vertex where maxFaces is the size of the array.
// count will be the number of faces that were found
void 
Mesh::facesWithVertex(XVec3f &v, unsigned int &count,
						   unsigned int *faces, unsigned int maxFaces)
{
	// no faces found yet
	count = 0;
	
	// to record which index of a face is the vertex
	unsigned int dummy;
	
	// for every face
	for( unsigned int m = 0; m < _quads.size(); ++m )
	{
		// check if the face contains the vertex
		if( faceHasVertex(m, v, dummy) )
		{
			// if so, record the face
			faces[count] = m;
			count++;
			
			// if the buffer is full, stop
			if( count == maxFaces )
				return;
		}
	}

  return;
}


// averages all of the face centers of
// faces containing the given vertex
Vertex 
Mesh::averageFacesAroundVertex(unsigned int vertex)
{	
	Vertex result;
	
	// get the vertex position
	XVec3f vert = _vertices[vertex].position;
	
	// the number of faces that will be found
	unsigned int count = 0;
	
	// the buffer to contain the face indices
	static unsigned int faces[MAX_FACES_PER_VERTEX];
	
	// fill the buffer with the face indices
	facesWithVertex(vert, count, faces, MAX_FACES_PER_VERTEX);
 // TASK 2 //
 // set result to be the average of the face centers
 // of all of the faces that were found
	for (int i = 0; i < count; i++) {
		result.position += centerOfQuad(faces[i]).position;
		result.normal += centerOfQuad(faces[i]).normal;
		result.color += centerOfQuad(faces[i]).color;
	}
	result.position /= (float)count;
	result.normal /= (float)count;
	result.color /= (float)count;
	return result;
}

// averages all of the edge midpoints of
// edges containing the given vertex
Vertex 
Mesh::averageEdgesAroundVertex(unsigned int vertex)
{	
	Vertex result;
	
	// get the vertex position
	XVec3f vert = _vertices[vertex].position;
	
	// the number of edges that will be found
	unsigned int count = 0;
	
	// the buffer that will contain the edges found
	static unsigned int edges[MAX_EDGES_PER_VERTEX] = { 0 };
	
	// fill the buffer with vertex index-pairs
	edgesWithVertex(vert, count, edges, MAX_EDGES_PER_VERTEX);
	// TASK 4 //
	// set result to be the average of the midpoints of
	// all of the edges that were found.
	// the buffer will NOT contain duplicate edges

	for (int i = 0; i < count * 2; i++) {
		result.position += _vertices[edges[i]].position;
		result.normal += _vertices[edges[i]].normal;
		result.color += _vertices[edges[i]].color;
	}
	cout << endl;
	result.position /= (float)(count * 2);
	result.normal /= (float)(count * 2);
	result.color /= (float)(count * 2);
	return result;
}


// returns the bumber of faces/edges containing
// the given vertex. the number of faces and
// edges is guaranteed to be the same
unsigned int 
Mesh::edgesAndFacesForVertex(unsigned int vertex)
{
	unsigned int count = 0;
	XVec3f v = _vertices[vertex].position;
	unsigned int dummy;
	for( unsigned int m = 0; m < _quads.size(); ++m )
	{
		if( faceHasVertex(m, v, dummy) )
			count++;
	}

	return count;
}


// returns the position of a given vertex by weighting
// its position from its old location along with the
// faces and edges containing it
Vertex 
Mesh::newVertexForSmooth(unsigned int vertex)
{
	// get the number of edges/faces containing the vertex
	unsigned int count = edgesAndFacesForVertex(vertex);

	// get the average location of the face
	// centers that contain the given vertex
	Vertex averageFaces = averageFacesAroundVertex(vertex);
	
	// get the average location of the edge
	// midpoints that contain the given vertex
	Vertex averageEdges = averageEdgesAroundVertex(vertex);
	
	// get the old location of the vertex
	Vertex oldVertex = _vertices[vertex];
	
	// the new vertex
	// TASK 1 //
	// set result to be the average of the face centers
 
	Vertex result = averageFaces;

	// TASK 3 //
	// set result to be the average of the edge midpoints
 
 	result = averageEdges;
	// TASK 5 //
	// set result to be a weighted combination of averageFaces,
	// averageEdges, and oldVertex. You will have to set the
	// position, normal, and color separately.
	// Vertex result = oldVertex;

 	result.position = (averageFaces.position + 2 * averageEdges.position 
 						+ (count - 3) * oldVertex.position) / count;
 	result.normal = (averageFaces.normal + 2 * averageEdges.normal 
 						+ (count - 3) * oldVertex.normal) / count;
 	result.color = (averageFaces.color + 2 * averageEdges.color 
 						+ (count - 3) * oldVertex.color) / count;
	return result;
}


// draw the mesh
void 
Mesh::draw(bool withWireframe)
{
	// if there is no mesh, get out!
	if( _vertices.size() == 0 || _quads.size() == 0 )
		return;

	// enable arrays for vertex positions, normals, and colors.
	// because the std::vector is tightly packed, the vertoces
	// array can be passed in with a stride offset simply as
	// size of an individual vertex
	//
	// stare at the next six lines carefully and understand
	// what it is that they are doing. this is a very common
	// use since it allows one to have a vertex object, an
	// array of them, and a simple mechanism to send the
	// entire batch to the OpenGL pipeline
	glEnableClientState(GL_VERTEX_ARRAY);   
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (float *)&_vertices[0]);
		
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, sizeof(Vertex), (float *)&(_vertices[0].normal));
		
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, sizeof(Vertex), (float *)(_vertices[0].color));
	
	
	// with the vertex positions, normals, and colors arrays
	// setup, draw the quads using the coordinates 4-tuples
	// stored in the _quads array
	glDrawElements(GL_QUADS, (GLint)_quads.size()*4, GL_UNSIGNED_INT, (unsigned int *)&_quads[0]);
	
	
	// draw the wireframe if requested
	if( withWireframe )
	{
		// save the polygon and enable state
		glPushAttrib(GL_POLYGON_BIT | GL_ENABLE_BIT);
		
		// set the polygon draw mode to line, for wireframe
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		
		// turn off lighting so wireframe is flat-colored
		glDisable(GL_LIGHTING);
		
		// set a polygon offset to avoid z-fighting
		glPolygonOffset(-1.0f, -5.0f);
		glEnable(GL_POLYGON_OFFSET_LINE);

		// draw the wireframe with a dark gray
		glDisableClientState(GL_COLOR_ARRAY);
		glColor3f(0.3f, 0.3f, 0.3f);
		
		// draw the wireframe
		glDrawElements(GL_QUADS, (GLint)_quads.size()*4, GL_UNSIGNED_INT, (unsigned int *)&_quads[0]);
		
		// return the polygon and enable state
		glPopAttrib();
	}
	
	// disable vertex arrays
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

  return;
}

// return a pseudorandom number in [0,1]
inline float 
frand() 
{ 
  return rand()/(float)RAND_MAX; 
}

// randomize the vertices in the mesh
void 
Mesh::randomize()
{	
	// for every vertex
	for( unsigned int i = 0; i < _vertices.size(); ++i )
	{
		// get the vertex
		XVec3f currentVertex = _vertices[i].position;
		
		// make some random numbers
		float a = 0.1f*frand()-0.05f;
		float b = 0.1f*sinf(a);
		float c = 0.1f*sinf(b)*a+b;
		float y = currentVertex.y();
		float y2 = y*y;
		
		// create a funky displacement
		XVec3f disp = XVec3f(a,b,c)
			+ currentVertex * y2 * 0.03f * (0.5f + a)
			+ XVec3f(0.05f*sinf(10.0f*y), 0.05f*cosf(currentVertex.x()+y), 0.0f);
		
		// find any vertex at the same location and displace it as well
		for( unsigned int j = 0; j < _vertices.size(); ++j ) {
			if( _vertices[j].position.dist(currentVertex) < TOLERANCE && i != j )
				_vertices[j].position += disp;
		}
		
		// displace the vertex
		_vertices[i].position += disp;
	}

  return;
}

// create the mesh and set it to be a cube
void 
Mesh::toCube()
{
	// clear out the old data	
	_vertices.clear();
	_quads.clear();
	
	// seed rand for randomization of vertices */
	srand(4);
	
	// used to build vertices and then
	// add them to the mesh
	Vertex v;
	
	// initialize the mesh to a cube
	
	
	// front
	v.color = XVec3f(1.0f, 0.0f, 0.8f);
	v.normal = XVec3f(0.0f, 0.0f, 1.0f);
	
	v.position = XVec3f(1.0f, 1.0f, 1.0f);
	_vertices.push_back(v);
	
	v.position = XVec3f(-1.0f, 1.0f, 1.0f);
	_vertices.push_back(v);
	
	v.position = XVec3f(-1.0f, -1.0f, 1.0f);
	_vertices.push_back(v);
	
	v.position = XVec3f(1.0f, -1.0f, 1.0f);
	_vertices.push_back(v);
	
	_quads.push_back(Quad(0,1,2,3));
	
	
	// back
	v.normal = XVec3f(0.0f, 0.0f, -1.0f);
	
	v.position = XVec3f(1.0f, 1.0f, -1.0f);
	_vertices.push_back(v);
	
	v.position = XVec3f(1.0f, -1.0f, -1.0f);
	_vertices.push_back(v);
	
	v.position = XVec3f(-1.0f, -1.0f, -1.0f);
	_vertices.push_back(v);
	
	v.position = XVec3f(-1.0f, 1.0f, -1.0f);
	_vertices.push_back(v);
	
	_quads.push_back(Quad(4,5,6,7));
	
	// left
	v.color = XVec3f(0.0f, 0.8f, 1.0f);
	v.normal = XVec3f(-1.0f, 0.0f, 0.0f);
	
	v.position = XVec3f(-1.0f, 1.0f, 1.0f);
	_vertices.push_back(v);
	
	v.position = XVec3f(-1.0f, 1.0f, -1.0f);
	_vertices.push_back(v);
	
	v.position = XVec3f(-1.0f, -1.0f, -1.0f);
	_vertices.push_back(v);
	
	v.position = XVec3f(-1.0f, -1.0f, 1.0f);
	_vertices.push_back(v);
	
	_quads.push_back(Quad(8,9,10,11));
	
	
	// right
	v.normal = XVec3f(1.0f, 0.0f, 0.0f);
	
	v.position = XVec3f(1.0f, 1.0f, 1.0f);
	_vertices.push_back(v);
	
	v.position = XVec3f(1.0f, -1.0f, 1.0f);
	_vertices.push_back(v);
	
	v.position = XVec3f(1.0f, -1.0f, -1.0f);
	_vertices.push_back(v);
	
	v.position = XVec3f(1.0f, 1.0f, -1.0f);
	_vertices.push_back(v);
	
	_quads.push_back(Quad(12,13,14,15));
	
	
	// top
	v.color = XVec3f(0.8f, 1.0f, 0.0f);
	v.normal = XVec3f(0.0f, 1.0f, 0.0f);
	
	v.position = XVec3f(1.0f, 1.0f, 1.0f);
	_vertices.push_back(v);
	
	v.position = XVec3f(1.0f, 1.0f, -1.0f);
	_vertices.push_back(v);
	
	v.position = XVec3f(-1.0f, 1.0f, -1.0f);
	_vertices.push_back(v);
	
	v.position = XVec3f(-1.0f, 1.0f, 1.0f);
	_vertices.push_back(v);
	
	_quads.push_back(Quad(16,17,18,19));
	
	
	// bottom
	v.normal = XVec3f(0.0f, -1.0f, 0.0f);
	
	v.position = XVec3f(1.0f, -1.0f, 1.0f);
	_vertices.push_back(v);
	
	v.position = XVec3f(-1.0f, -1.0f, 1.0f);
	_vertices.push_back(v);
	
	v.position = XVec3f(-1.0f, -1.0f, -1.0f);
	_vertices.push_back(v);
	
	v.position = XVec3f(1.0f, -1.0f, -1.0f);
	_vertices.push_back(v);
	
	_quads.push_back(Quad(20,21,22,23));

  return;
}
