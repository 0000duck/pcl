/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2012-, Open Perception, Inc.
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the copyright holder(s) nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef PCL_RECOGNITION_RANSAC_BASED_AUX_H_
#define PCL_RECOGNITION_RANSAC_BASED_AUX_H_

#include <cmath>
#include <cstdlib>

namespace pcl
{
  namespace recognition
  {
    namespace aux
    {
      /** \brief Returns a random integer in [min, max] (including both min and max). This method uses rand() from
       * the C Standard General Utilities Library <cstdlib>. That's why it uses a seed to generate the integers, which
       * should be initialized to some distinctive value using srand(). */
      inline int
      getRandomInteger(int min, int max)
      {
        return static_cast<int>(static_cast<double>(min) + (static_cast<double>(rand())/static_cast<double>(RAND_MAX))*static_cast<double>(max-min) + 0.5);
      }

      /** \brief c = a + b */
      template <typename T> void
      vecSum3 (const T a[3], const T b[3], T c[3])
      {
        c[0] = a[0] + b[0];
        c[1] = a[1] + b[1];
        c[2] = a[2] + b[2];
      }

      /** \brief c = a - b */
      template <typename T> void
      vecDiff3 (const T a[3], const T b[3], T c[3])
      {
        c[0] = a[0] - b[0];
        c[1] = a[1] - b[1];
        c[2] = a[2] - b[2];
      }

      template <typename T> void
      vecCross3 (const T v1[3], const T v2[3], T out[3])
      {
        out[0] = v1[1]*v2[2] - v1[2]*v2[1];
        out[1] = v1[2]*v2[0] - v1[0]*v2[2];
        out[2] = v1[0]*v2[1] - v1[1]*v2[0];
      }

      /** \brief Returns the length of v. */
      template <typename T> T
      vecLength3 (const T v[3])
      {
        return (static_cast<T> (sqrt (v[0]*v[0] + v[1]*v[1] + v[2]*v[2])));
      }

      /** \brief Returns the Euclidean distance between a and b. */
      template <typename T> T
      vecDistance3 (const T a[3], const T b[3])
      {
        T l[3] = {a[0]-b[0], a[1]-b[1], a[2]-b[2]};
        return (vecLength3 (l));
      }

      /** \brief Returns the dot product a*b */
      template <typename T> T
      vecDot3 (const T a[3], const T b[3])
      {
        return (a[0]*b[0] + a[1]*b[1] + a[2]*b[2]);
      }

      /** \brief v = scalar*v. */
      template <typename T> void
      vecMult3 (T v[3], T scalar)
      {
        v[0] *= scalar;
        v[1] *= scalar;
        v[2] *= scalar;
      }

      /** \brief Normalize v */
      template <typename T> void
      vecNormalize3 (T v[3])
      {
        T inv_len = (static_cast<T> (1.0))/vecLength3 (v);
        v[0] *= inv_len;
        v[1] *= inv_len;
        v[2] *= inv_len;
      }

      /** \brief Returns the square length of v. */
      template <typename T> T
      vecSqrLength3 (const T v[3])
      {
        return (v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
      }

      /** Projects 'x' on the plane through 0 and with normal 'planeNormal' and saves the result in 'out'. */
      template <typename T> void
      projectOnPlane3 (const T x[3], const T planeNormal[3], T out[3])
      {
        T dot = vecDot3 (planeNormal, x);
        // Project 'x' on the plane normal
        T nproj[3] = {-dot*planeNormal[0], -dot*planeNormal[1], -dot*planeNormal[2]};
        vecSum3 (x, nproj, out);
      }

      /** \brief out = mat*v. 'm' is an 1D array of 9 elements treated as a 3x3 matrix (row major order). */
      template <typename T> void
      mult3x3(const T m[9], const T v[3], T out[3])
      {
      	out[0] = v[0]*m[0] + v[1]*m[1] + v[2]*m[2];
      	out[1] = v[0]*m[3] + v[1]*m[4] + v[2]*m[5];
      	out[2] = v[0]*m[6] + v[1]*m[7] + v[2]*m[8];
      }

      /** Let x, y, z be the columns of the matrix a = [x|y|z]. The method computes out = a*m.
        * Note that 'out' is a 1D array of 9 elements and the resulting matrix is stored in row
        * major order, i.e., the first matrix row is (out[0] out[1] out[2]), the second
        * (out[3] out[4] out[5]) and the third (out[6] out[7] out[8]). */
      template <typename T> void
      mult3x3 (const T x[3], const T y[3], const T z[3], const T m[3][3], T out[9])
      {
        out[0] = x[0]*m[0][0] + y[0]*m[1][0] + z[0]*m[2][0];
        out[1] = x[0]*m[0][1] + y[0]*m[1][1] + z[0]*m[2][1];
        out[2] = x[0]*m[0][2] + y[0]*m[1][2] + z[0]*m[2][2];

        out[3] = x[1]*m[0][0] + y[1]*m[1][0] + z[1]*m[2][0];
        out[4] = x[1]*m[0][1] + y[1]*m[1][1] + z[1]*m[2][1];
        out[5] = x[1]*m[0][2] + y[1]*m[1][2] + z[1]*m[2][2];

        out[6] = x[2]*m[0][0] + y[2]*m[1][0] + z[2]*m[2][0];
        out[7] = x[2]*m[0][1] + y[2]*m[1][1] + z[2]*m[2][1];
        out[8] = x[2]*m[0][2] + y[2]*m[1][2] + z[2]*m[2][2];
      }

      /** The first 9 elements of 't' are treated as a 3x3 matrix (row major order) and the last 3 as a translation.
        * First, 'p' is multiplied by that matrix and then translated. The result is saved in 'out'. */
      template<class T> void
      transform(const T t[12], const T p[3], T out[3])
      {
        out[0] = t[0]*p[0] + t[1]*p[1] + t[2]*p[2] + t[9];
        out[1] = t[3]*p[0] + t[4]*p[1] + t[5]*p[2] + t[10];
        out[2] = t[6]*p[0] + t[7]*p[1] + t[8]*p[2] + t[11];
      }
    }
  }
}

#endif // AUX_H_
