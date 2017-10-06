/*
 * Copyright (c) 2010 University of Michigan, Ann Arbor.
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
 * Author: Ari Grant (grantaa@umich.edu), Sugih Jamin (jamin@eecs.umich.edu)
 * 
 */

#ifndef  __XMAT_H__
#define  __XMAT_H__

#ifndef _NO_IOSTREAMS
#include <iostream>
#endif

#include <assert.h>
#include <math.h>

#include "xvec.h"

/* Simple matrix class, column-major, that is, the last 4 values are:
   tx, ty, tz, 1.0 */
template<int dim, class real_type> 
  class XMat
{
 public:
        
        
  XMat() { Identity(); }
        
  explicit XMat(real_type f)
  {
    for(int i=0; i<dim*dim; ++i)
      m_v[i] = f;
  }
        
  XMat(const XMat& c)
    {
      for(int i=0; i<dim*dim; ++i)
        m_v[i] = c.m_v[i];
    }
        
  explicit XMat(const real_type* a)
  {
    for(int i=0; i<dim*dim; ++i)
      m_v[i] = a[i];
  }
        
  template<class oreal_type> 
    explicit XMat(const XMat<dim, oreal_type>& c)
    {
      for(int i=0; i<dim*dim; ++i)
        m_v[i] = static_cast<oreal_type>(c(i));
    }
        
  template<int dim2> 
    explicit XMat(const XMat<dim2, real_type>& c)
    {
      for(int i=0; i<dim && i<dim2; ++i) {
        for(int j=0; j<dim && j<dim2; ++j) {
          (*this)(i,j) = c(i,j);
        }
      }
    }

  operator real_type*() { return &m_v[0]; }
        
  operator const real_type*() const { return &m_v[0]; }
        
  bool operator==(const XMat& c) const
  {
    for(int i=0; i<dim*dim; ++i)
      if(m_v[i]!=c.m_v[i])
        return false;
    return true;
  }
        
  bool operator!=(const XMat& c) const { return !((*this)==c); }
        
  XMat& operator=(const XMat& c)
    {
      for(int i=0; i<dim*dim; ++i)
        m_v[i] = c.m_v[i];
      return *this; 
    }

  XMat operator+(const XMat& c) const
  {
    XMat pt(*this);
    for(int i=0; i<dim*dim; ++i)
      pt[i] += c.m_v[i];
    return pt;
  }
        
  XMat operator-(const XMat& c) const
  {
    XMat pt(*this);
    for(int i=0; i<dim*dim; ++i)
      pt[i] -= c.m_v[i];
    return pt;
  }
        
  XMat operator*(real_type s) const
    {
      XMat pt(*this);
      for(int i=0; i<dim*dim; ++i)
        pt[i] *= s;
      return pt;
    }
        
  friend XMat operator*(real_type s, const XMat& c)
    {
      XMat pt(c);
      for(int i=0; i<dim*dim; ++i)
        pt[i] *= s;
      return pt;
    }
        
  template<int vDim>
  XVec<vDim, float> operator*(XVec<vDim, float> u) const
  {
    assert(dim==vDim);
    XVec<vDim, float> v(u);
    for(int i=0; i<dim; ++i)
      v(i) = row(i).dot(u);
    return v;
  }
        
  XMat operator/(real_type s) const
  {
    XMat pt(*this);
    for(int i=0; i<dim*dim; ++i)
      pt[i] /= s;
    return pt;
  }
        
  XMat& operator+=(const XMat& c)
    {
      for(int i=0; i<dim*dim; ++i)
        m_v[i] += c.m_v[i];
      return *this; 
    }
        
  XMat& operator-=(const XMat& c)
    {
      for(int i=0; i<dim*dim; ++i)
        m_v[i] -= c.m_v[i];
      return *this; 
    }
        
  XMat& operator*=(real_type s)
    {
      for(int i=0; i<dim*dim; ++i)
        m_v[i] *= s;
      return *this; 
    }
        
  XMat& operator/=(real_type s)
    {
      for(int i=0; i<dim*dim; ++i)
        m_v[i] /= s;
      return *this; 
    }
        
  XMat operator-() const
  {
    XMat pt(*this);
    for(int i=0; i<dim*dim; ++i)
      pt[i] = -pt[i];
    return pt;
  }
        
  XMat operator*(const XMat& c) const
    {
      XMat pt(0.0);
      /* row */
      for(int i=0; i<dim; ++i)
        {
          /* column */
          for(int j=0; j<dim; ++j)
            pt.m_v[dim*j+i] = row(i).dot(c.column(j));
                        
        }
      return pt;
    }
        
  XMat& operator*=(const XMat& c)
    {   
      XMat pt(0.0);
      /* row */
      for(int i=0; i<dim; ++i)
        {
          /* column */
          for(int j=0; j<dim; ++j)
            pt.m_v[dim*j+i] = row(i).dot(c.column(j));
                        
        }
      *this = pt;
                
      return *this;
    }

  real_type& operator() (const int r, const int c) { return m_v[c*dim+r]; }
        
  real_type operator() (const int r, const int c) const { return m_v[c*dim+r]; }
        
  const real_type& ref() const { return m_v[0]; }
        
  XVec<dim, real_type> row(const int r) const
    {
      assert(r<dim && r>=0);
                
      XVec<dim, real_type> v;
      for(int i=0; i<dim; ++i)
        v[i] = m_v[dim*i+r];
                
      return v;
    }
        
  XVec<dim, real_type> column(const int c) const
    {
      assert(c<dim && c>=0);
                
      XVec<dim, real_type> v;
      for(int i=0; i<dim; ++i)
        v[i] = m_v[dim*c+i];
                
      return v;
    }
        
  template<int cDim, class cRealType>
    void setCol(const int c, XVec<cDim, cRealType> &v)
  {
    assert(c<dim && c>=0);
    for(int i=0; i<dim && i<cDim; ++i)
      m_v[dim*c+i] = v(i);
  }
        
  template<int cDim, class cRealType>
    void setRow(const int r, XVec<cDim, cRealType> &v)
  {
    assert(r<dim && r>=0);

    for(int i=0; i<dim && i<cDim; ++i)
      m_v[dim*i+r] = v(i);
  }
        
  XMat transpose() const
  {
    XMat pt(*this);
    for(int i=0; i<dim; ++i)
      {
        for(int j=0; j<i; ++j)
          {
            real_type temp = pt[dim*i+j];
            pt[dim*i+j] = pt[dim*j+i];
            pt[dim*j+i] = temp;
          }
      }
    return pt;
  }

  void Identity()
  {
    for( int i = 0; i < dim*dim; i++ )
      {
        if( i % (dim+1) == 0 )
          m_v[i] = 1;
        else
          m_v[i] = 0;
      }
  }
        
  /* return inverse of the matrix */
  XMat inverse() const
  {
    XMat inv(0.0);
    inv.Identity();

    XMat copy(*this);
                
    float div;
    int i, i1,j;
                
    /* systematically perform elementary row operations on A and A_inv so that
     * A becomes the identity matrix and A_inv becomes the inverse */
                
    for(i=0; i<dim; ++i)
      {
        /* Divide through by A[i][i] to get 1 on the diagonal */
        div = copy(i, i);
        if( fabsf(div) < 1e-15 )
          return XMat(*this);
        for(j=0; j<dim; ++j)
          {
            copy(i, j) /= div;
            inv(i, j) /= div;
          }
                        
        /* Subtract a factor times this row from other rows to get zeros above and below the 1 just created by division */
        for(i1=0; i1<dim; ++i1)
          {
            if(i1 == i) continue; /* skip the present row */
            div = copy(i1, i);
            for(j=0; j<dim; ++j)
              {
                copy(i1, j) -= div*copy(i, j);
                inv(i1, j) -= div*inv(i, j);
              }
          }
      }
                
    return inv;
  }
        
 protected:
  real_type  m_v[dim*dim];
};


#ifndef _NO_IOSTREAMS
template<int dim, class real_type> 
  std::ostream& operator<<( std::ostream& os, const XMat<dim, real_type>& c )
{       
  for(int i=0; i<dim; ++i) {
    for(int j=0; j<dim; ++j)
      //os << c(j,i) << " ";  // using the math row major convention
      os << c(i,j) << " ";    // using the C/C++/GLSL column major convention
    os << std::endl;
  }
  return os;
}

template<int dim, class real_type> 
  std::istream& operator>>( std::istream& is, XMat<dim, real_type>& f )
{
  for( int i = 0; i < dim; i++ )
    for( int j = 0; j < dim; j++ )
      is >> f(i,j);         // using the C/C++/GLSL column major convention
  return is;
}
 
#endif

typedef XMat<2, double> XMat2d;
typedef XMat<2, float> XMat2f;

typedef XMat<3, double> XMat3d;
typedef XMat<3, float> XMat3f;

typedef XMat<4, double> XMat4d;
typedef XMat<4, float> XMat4f;

#endif  // __XMAT_H__
