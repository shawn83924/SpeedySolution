//////////////////////////////////////////////////////////
// File Name: vectors.h
//
// Author : Little Chio
// Last Modified : 2002/7/8
// Last Modified : 2007/9/12
//
// Copyright (c) 2002, 2007, Gamania Inc. All Rights Reserved.
//////////////////////////////////////////////////////////

#ifndef __VECTORS_H__
#define __VECTORS_H__

#include <math.h>
#include "tchars.h"


#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__GNUC__)	// If using Microsoft or Borland or GNU C++ compiler, do the following.
	#pragma pack(push, 8)						// To confirm the alignment of the structure members is the same under any case. Otherwise, strange bugs will be resulted under multiple projects of different alignment.
												// (Please place this line after all other #include<>.)
#endif


//========================================
// define some constants and macros

#ifndef M_PI
#define M_PI  3.141592653f
#endif

//#ifndef cosf
//#define cosf(x)       (float)(cos(x))
//#define sinf(x)       (float)(sin(x))
//#define sqrtf(x)      (float)(sqrt(x))
//#define fmodf(x, y)   (float)(fmod(x, y))
//#endif

///////////////////////////////////////////////////////////////////
/// template Vector3 is used in vector calculations. It is in three
/// dimension, so it often represents the vector in 3D space.
/// @see Vector2
///////////////////////////////////////////////////////////////////
namespace DSAud
{
template<class tTYPE>
class Vector3
{
public:

      /** Constructor, without initialization. */
	  Vector3();

	  /** 
	   * Constructor, initialized with elements.
	   * @param a  It is assigned to x element.
	   * @param b  It is assigned to y element.
	   * @param c  It is assigned to z element.
	   */
      Vector3(tTYPE a, tTYPE b, tTYPE c);

      /** Add two vectors together. */
	  Vector3<tTYPE> operator+(const Vector3<tTYPE> &v) const;

	  /** Subtitude one vector with another vector. */
      Vector3<tTYPE> operator-(const Vector3<tTYPE> &v) const;

	  /** Multiply the vector by a scalar. */
      Vector3<tTYPE> operator*(tTYPE scalar) const;

	  /** Divide the vector by a scalar. */
      Vector3<tTYPE> operator/(tTYPE scalar) const;
      /** Multiply the vector by a scalar. */
/*	Note from Andre:
(refer to Stroustrup's "The C++ Programming Language", Section c.13.2 : Friends.)
A template class can have friends, but declare as:
friend Vector<tTYPE> operator*<>(tTYPE scalar, const Vector3<tTYPE> &v);
The <> after the name of the friend function is needed to make clear that the
friend is a template function. Without the <>, a non-template function would be
asumed. Friend do not affect the scope in which the template class is defined,
nor do they affect the scope in which the template is used. Instead, friend
functions and operators are found using a lookup base on their argument types.
Like a member function, a friend function is instantiated only if it is called.

Unfortunately, .NET2003 C++ won't complie!
*/
//	  friend Vector3<tTYPE> operator*(tTYPE scalar, const Vector3<tTYPE> &v);

	  /** Inner-cross two vectors. */
      tTYPE operator*(const Vector3<tTYPE> &v) const;

	  /** Change the sign of the vector. */
      Vector3<tTYPE> operator-() const;

	  /** Add-up the vector by another one. */
      Vector3<tTYPE>& operator+=(const Vector3<tTYPE> &v);

      /** Sub-up the vector by another one. */
      Vector3<tTYPE>& operator-=(const Vector3<tTYPE> &v);

	  /** Multiply-up the vector by a scalar. */
      Vector3<tTYPE>& operator *=(tTYPE scalar);

	  /** Divide-up the vector by a scalar. */
      Vector3<tTYPE>& operator /=(tTYPE scalar);

	  /** Check if the vector is same with another one. */
      bool operator ==(const Vector3<tTYPE> &v) const;

	  /** Check if the vector is different from another one. */
      bool operator !=(const Vector3<tTYPE> &v) const;

	  /** Outer-cross the vector by another vector. */
      Vector3<tTYPE> Cross(const Vector3<tTYPE> &v) const;

	  /** Get the length of the vector. */
      tTYPE Norm() const;
	  tTYPE NormSq() const;
	  tTYPE CabNorm() const;

	  /** Normalize the vector. */
      Vector3<tTYPE>& Normalize();

public:

      union
      {
            tTYPE x;   /**< Element x. */
            tTYPE r;
      };
       
      union
      {
            tTYPE y;   /**< Element y. */
            tTYPE g;
      };
        
      union 
      {
            tTYPE z;   /**< Element z. */
            tTYPE b;
      };

};


//===== template stream function for Vector3 =========
template<class tTYPE>
OutStream& operator<<(OutStream &os, const Vector3<tTYPE> &v)
{
	os << v.x << _T(" ") << v.y << _T(" ") << v.z;
	return os;
}

template<class tTYPE>
InStream& operator>>(InStream &is, Vector3<tTYPE> &v)
{
	is >> v.x >> v.y >> v.z;
	return is;
}

// use Vector to handle 3D transformation
typedef Vector3<float> Vector;
typedef Vector3<float> Vector3f;
typedef Vector3<int> Vector3i;


/*-----------------------------------------------------
      member functions of class Vector
--------------------------------------------------------*/

// Constructor : null
template<class tTYPE>
inline Vector3<tTYPE>::Vector3()
{}

// Constructor : setting values
template<class tTYPE>
inline Vector3<tTYPE>::Vector3(tTYPE a, tTYPE b, tTYPE c):
x(a), y(b), z(c)
{}

// Vector Addition
template<class tTYPE>
inline Vector3<tTYPE> Vector3<tTYPE>::operator+(const Vector3<tTYPE> &v) const
{
      return Vector3<tTYPE>(x + v.x, y + v.y, z + v.z);
}

// Vector Substraction
template<class tTYPE>
inline Vector3<tTYPE> Vector3<tTYPE>::operator-(const Vector3<tTYPE> &v) const
{
      return Vector3<tTYPE>(x - v.x, y - v.y, z - v.z);
}

// Vector-Scalar Multiplication
template<class tTYPE>
inline Vector3<tTYPE> Vector3<tTYPE>::operator*(tTYPE scalar) const
{
      return Vector3<tTYPE>(x*scalar, y*scalar, z*scalar);
}

// Vector-Scalar Division
template<class tTYPE>
inline Vector3<tTYPE> Vector3<tTYPE>::operator/(tTYPE scalar) const
{
      float s = 1.0f / scalar;
      return Vector3<tTYPE>(x*s, y*s, z*s);
}

template<class tTYPE>
inline Vector3<tTYPE> operator*(tTYPE scalar, const Vector3<tTYPE> &v)
{
      return Vector3<tTYPE>(v.x*scalar, v.y*scalar, v.z*scalar);
}

// Vector-Vector Multiplication (Dot Product)
template<class tTYPE>
inline tTYPE Vector3<tTYPE>::operator*(const Vector3<tTYPE> &v) const
{
      return x*v.x + y*v.y + z*v.z;
}

// Negative Vector
template<class tTYPE>
inline Vector3<tTYPE> Vector3<tTYPE>::operator-() const
{
      return Vector3<tTYPE>(-x, -y, -z);
}

// Vector Addition-up
template<class tTYPE>
inline Vector3<tTYPE>& Vector3<tTYPE>::operator+=(const Vector3<tTYPE> &v)
{
      x += v.x;
      y += v.y;
      z += v.z;

      return *this;
}

// Vector Substration-up
template<class tTYPE>
inline Vector3<tTYPE>& Vector3<tTYPE>::operator-=(const Vector3<tTYPE> &v)
{
      x -= v.x;
      y -= v.y;
      z -= v.z;

      return *this;
}

// Vector-Scalar Multiplication-up
template<class tTYPE>
inline Vector3<tTYPE>& Vector3<tTYPE>::operator *=(tTYPE scalar)
{
      x *= scalar;
      y *= scalar;
      z *= scalar;
      
      return *this;
}

// Vector-Scale Division-up
template<class tTYPE>
inline Vector3<tTYPE>& Vector3<tTYPE>::operator /=(tTYPE scalar)
{
      scalar = tTYPE(1) / scalar;
      return (*this) *= scalar;
}

// Check if the vector is same with another one. 
template<class tTYPE>
inline bool Vector3<tTYPE>::operator==(const Vector3<tTYPE> &v) const
{
      return x==v.x && y==v.y && z==v.z;
}

// Check if the vector is different from another one. 
template<class tTYPE>
inline bool Vector3<tTYPE>::operator!=(const Vector3<tTYPE> &v) const
{
      return x!=v.x || y!=v.y || z!=v.z;
}

// Cross Product
template<class tTYPE>
inline Vector3<tTYPE> Vector3<tTYPE>::Cross(const Vector3<tTYPE> &v) const
{
      return Vector3<tTYPE>(y*v.z - v.y*z, z*v.x - v.z*x, x*v.y - v.x*y);
}

// Vector Norm
template<class tTYPE>
inline tTYPE Vector3<tTYPE>::Norm() const
{
      return tTYPE( sqrt(x*x + y*y + z*z) );
}

// Vector NormSq
template<class tTYPE>
inline tTYPE Vector3<tTYPE>::NormSq() const
{
      return tTYPE( x*x + y*y + z*z );
}

// Vector CabNorm
template<class tTYPE>
inline tTYPE Vector3<tTYPE>::CabNorm() const
{
      return tTYPE( fabsf(x) + fabsf(y) + fabsf(z) );
}

// Vector Normalization
template<class tTYPE>
inline Vector3<tTYPE>& Vector3<tTYPE>::Normalize()
{
      return (*this) /= Norm();
}

// End of template Vector3 

///////////////////////////////////////////////////////////////////
/// template Vector2 is used in 2D vector calculations. It is in two
/// dimension, so it often represents the point in 2D space.
/// @see Vector3
///////////////////////////////////////////////////////////////////

template<class tTYPE>
class Vector2
{
public:
      
      /** Constructor, without initialization. */
	  Vector2();

	  /** 
	   * Constructor, initialized with elements.
	   * @param a  It is assigned to x element.
	   * @param b  It is assigned to y element.
	   */
      Vector2(tTYPE a, tTYPE b);

      /** Add two vectors together. */
      Vector2<tTYPE> operator+(const Vector2<tTYPE> &v) const;

	  /** Subtitude one vector with another vector. */
      Vector2<tTYPE> operator-(const Vector2<tTYPE> &v) const;

	  /** Multiply the vector by a scalar. */
      Vector2<tTYPE> operator*(tTYPE scalar) const;
	  
	  /** Multiply the vector by a scalar. */
//      friend Vector2<tTYPE> operator*(tTYPE scalar, const Vector2<tTYPE> &v);

	  /** Divide the vector by a scalar. */
      tTYPE operator*(const Vector2<tTYPE> &v) const;

	  /** Change the sign of the vector. */
      Vector2<tTYPE> operator-() const;

	  /** Add-up the vector by another one. */
      Vector2<tTYPE>& operator+=(const Vector2<tTYPE> &v);

	  /** Sub-up the vector by another one. */
      Vector2<tTYPE>& operator-=(const Vector2<tTYPE> &v);

	  /** Multiply-up the vector by a scalar. */
      Vector2<tTYPE>& operator *=(tTYPE scalar);

	  /** Divide-up the vector by a scalar. */
      Vector2<tTYPE>& operator /=(tTYPE scalar);

	  /** Check if the vector is same with another one. */
      bool operator ==(const Vector2<tTYPE> &v) const;

	  /** Check if the vector is different from another one. */
      bool operator !=(const Vector2<tTYPE> &v) const;

	  /** Get the length of vector. */
      tTYPE Norm() const;

	  /** Normalize the vector. */
      Vector2<tTYPE>& Normalize();

public:
 
      tTYPE x;   /**< Element x */
	  tTYPE y;   /**< Element y */
};

//===== template stream function for Vector2 =========
template<class tTYPE>
OutStream& operator<<(OutStream &os, const Vector2<tTYPE> &v)
{
	os << v.x << _T(" ") << v.y;
	return os;
}

template<class tTYPE>
InStream& operator>>(InStream &is, Vector2<tTYPE> &v)
{
	is >> v.x >> v.y;
	return is;
}

// use Vector2f to handle 2D transformation
typedef Vector2<float> Vector2f;
typedef Vector2<int> Vector2i;

/*-----------------------------------------------------
      member functions of template Vector2
--------------------------------------------------------*/
// Constructor : null
template<class tTYPE>
inline Vector2<tTYPE>::Vector2()
{}

// Constructor : setting values
template<class tTYPE>
inline Vector2<tTYPE>::Vector2(tTYPE a, tTYPE b):
x(a), y(b)
{}

// Vector Addition
template<class tTYPE>
inline Vector2<tTYPE> Vector2<tTYPE>::operator+(const Vector2<tTYPE> &v) const
{
      return Vector2<tTYPE>(x + v.x, y + v.y);
}

// Vector Substraction
template<class tTYPE>
inline Vector2<tTYPE> Vector2<tTYPE>::operator-(const Vector2<tTYPE> &v) const
{
      return Vector2<tTYPE>(x - v.x, y - v.y);
}

// Vector-Scalar Multiplication
template<class tTYPE>
inline Vector2<tTYPE> Vector2<tTYPE>::operator*(tTYPE scalar) const
{
      return Vector2<tTYPE>(x*scalar, y*scalar);
}

template<class tTYPE>
inline Vector2<tTYPE> operator*(tTYPE scalar, const Vector2<tTYPE> &v)
{
      return Vector2<tTYPE>(v.x*scalar, v.y*scalar);
}

// Vector-Vector Multiplication (Dot Product)
template<class tTYPE>
inline tTYPE Vector2<tTYPE>::operator*(const Vector2<tTYPE> &v) const
{
      return x*v.x + y*v.y;
}

// Negative Vector
template<class tTYPE>
inline Vector2<tTYPE> Vector2<tTYPE>::operator-() const
{
      return Vector2<tTYPE>(-x, -y);
}

// Vector Addition-up
template<class tTYPE>
inline Vector2<tTYPE>& Vector2<tTYPE>::operator+=(const Vector2<tTYPE> &v)
{
      x += v.x;
      y += v.y;

      return *this;
}

// Vector Substration-up
template<class tTYPE>
inline Vector2<tTYPE>& Vector2<tTYPE>::operator-=(const Vector2<tTYPE> &v)
{
      x -= v.x;
      y -= v.y;

      return *this;
}

// Vector-Scalar Multiplication-up
template<class tTYPE>
inline Vector2<tTYPE>& Vector2<tTYPE>::operator *=(tTYPE scalar)
{
      x *= scalar;
      y *= scalar;
      
      return *this;
}

// Vector-Scale Division-up
template<class tTYPE>
inline Vector2<tTYPE>& Vector2<tTYPE>::operator /=(tTYPE scalar)
{
      scalar = tTYPE(1) / scalar;
      return (*this) *= scalar;
}

// Check if the vector is same with another one. 
template<class tTYPE>
inline bool Vector2<tTYPE>::operator==(const Vector2<tTYPE> &v) const
{
      return x==v.x && y==v.y;
}

// Check if the vector is different from another one. 
template<class tTYPE>
inline bool Vector2<tTYPE>::operator!=(const Vector2<tTYPE> &v) const
{
      return x!=v.x || y!=v.y;
}

// Vector Norm
template<class tTYPE>
inline tTYPE Vector2<tTYPE>::Norm() const
{
      return tTYPE( sqrt((float)x*x + (float)y*y) );
}

// Vector Normalization
template<class tTYPE>
inline Vector2<tTYPE>& Vector2<tTYPE>::Normalize()
{
      return (*this) /= Norm();
}

}
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__GNUC__)	// If using Microsoft or Borland or GNU C++ compiler, do the following.
	#pragma pack(pop)							// Restore member alignment size. (Do not affect outside world)
#endif

#endif    // end of __VECTORS_H__
