.. _bspline_fitting:

Fitting trimmed B-splines to unordered point clouds
---------------------------------------------------

This tutorial explains how to run a B-spline fitting algorithm on a
point-cloud, to obtain a parametric surface representation.
The algorithm consists of the following steps:

* Choice of the parameters for B-spline surface fitting.

* Initialize B-spline by using the Principal Component Analysis (PCA). This
  assumes that the point-cloud has two main orientations, i.e. that it is roughly planar.

* Refinement and fitting.

* Choice of the parameters for B-spline curve fitting.

* Circular initialization of the B-spline curve. Here we assume that the point-cloud is
  compact, i.e. no separated clusters.

* Triangulation of the trimmed B-spline surface.

The algorithm applied to the frontal stanford bunny scan (204800 points):

.. raw:: html

  <iframe title="Trimmed B-spline surface fitting" width="480" height="390" src="http://youtu.be/trH2kWELvyw" frameborder="0" allowfullscreen></iframe>

Background
----------

Theoretical information on the algorithm can be found in this `report
<http://pointclouds.org/blog/trcs/moerwald/index.php>`_ and in my `PhD thesis
<http://users.acin.tuwien.ac.at/tmoerwald/?site=3>`_.


The code
--------

The cpp file used in this tutorial can be found in 
pcl/examples/surface/example_nurbs_fitting_surface.cpp

.. literalinclude:: ../../../../examples/surface/example_nurbs_fitting_surface.cpp
   :language: cpp
   :linenos:

The input file you can find at pcl/test/bunny.pcd

The explanation
---------------
Now, let's break down the code piece by piece. Lets start with the choice of the
parameters for B-spline surface fitting:


.. literalinclude:: ../../../../examples/surface/example_nurbs_fitting_surface.cpp
   :language: cpp
   :lines: 56-66

* *order* is the polynomial order of the B-spline surface.

* *refinement* is the number of refinement iterations, where for each iteration control-points
  are inserted, approximately doubeling the control points in each parametric direction 
  of the B-spline surface.

* *iterations* is the number of iterations that are performed after refinement is completed.

* *mesh_resoluation* the number of vertices in each parametric direction,
  used for triangulation of the B-spline surface.

* *interior_smoothness* is the smoothness of the surface interior.

* *interior_weight* is the weight for optimization for the surface interior.

* *boundary_smoothness* is the smoothness of the surface boundary.

* *boundary_weight* is the weight for optimization for the surface boundary.

Note, that the boundary in this case is not the trimming curve used later on.
The boundary can be used when a point-set exists that define the boundary. Those points
can be declared in *pcl::on_nurbs::NurbsDataSurface::boundary*. In this case, when the
*boundary_weight* is greater than 0.0, the algorithm tries to align the domain boundaries
to these points. In our example we are trimming the surface anyway, so there is no need
for aligning the boundary.   


.. literalinclude:: ../../../../examples/surface/example_nurbs_fitting_surface.cpp
   :language: cpp
   :lines: 68-72

The command *initNurbsPCABoundingBox* uses PCA to create a coordinate systems, where the principal
eigenvectors point into the direction of the maximum, middle and minimum extension of the point-cloud.
The center of the coordinate system is located at the mean of the points.
To estimate the extension of the B-spline surface domain, a bounding box is computed in the plane formed
by the maximum and middle eigenvectors. That bounding box is used to initialize the B-spline surface with
its minimum number of control points, according to the polynomial degree chosen.

The surface fitting class *pcl::on_nurbs::FittingSurface* is provided with the point data and the initial
B-spline.


.. literalinclude:: ../../../../examples/surface/example_nurbs_fitting_surface.cpp
   :language: cpp
   :lines: 74-80

The *on_nurbs* module allows easy conversion between the *ON_NurbsSurface* and the *PolygonMesh* class,
for visualization of the B-spline surfaces. Note that NURBS are a generalisation of B-splines,
and are therefore a valid container for B-splines, with all control-point weights = 1.


.. literalinclude:: ../../../../examples/surface/example_nurbs_fitting_surface.cpp
   :language: cpp
   :lines: 82-92

At this point of the code we have a B-spline surface with minimal number of control points.
Typically they are not enough to represent finer details of the underlying geometry 
of the point-cloud. However, if we increase the control-points to our desired level of detail and
subsequently fit the refined B-spline, we run into problems. For robust fitting B-spline surfaces
the rule is: 
"The higher the degree of freedom of the B-spline surface, the closer we have to be to the points to be approximated".

This is the reason why we iteratively increase the degree of freedom by refinement in both directions (line 85-86),
and fit the B-spline surface to the point-cloud, getting closer to the final solution.


.. literalinclude:: ../../../../examples/surface/example_nurbs_fitting_surface.cpp
   :language: cpp
   :lines: 94-102

After we reached the final level of refinement, the surface is further fitted to the point-cloud
for a pleasing end result.

Now that we have the surface fitted to the point-cloud, we want to cut off the overlapping regions of the surface.
To achiev this we project the point-cloud into the parametric domain using the closest points to the B-spline surface.
In this domain of R^2 we perform the weighted B-spline curve fitting, that creates a closed trimming curve that approximately
contains all the points.

.. literalinclude:: ../../../../examples/surface/example_nurbs_fitting_surface.cpp
   :language: cpp
   :lines: 107-120

The topic of curve fitting goes a bit deeper into the thematics of B-splines. Here we assume that you are
familiar with the concept of B-splines, knot vectors, control-points, and so forthe.
Please consider the curve being split into supporting regions which is bound by consecutive knots.
Also note that points that are inside and outside the curve are distinguished.

* *addCPsAccuracy* the distance of the supporting region of the curve to the closest data points has to be below
  this value, otherwise a control point is inserted.

* *addCPsIteration* inner iterations without inserting control points.

* *maxCPs* the maximum total number of control-points.

* *accuracy* the average fitting accuracy of the curve, w.r.t. the supporting regions.

* *iterations* maximum number of iterations performed.

* *closest_point_resolution* number of control points that must lie within each supporting region. (0 turns this constraint off)

* *closest_point_weight* weight for fitting the curve to its closest points.

* *closest_point_sigma2* threshold for closest points (disregard points that are further away from the curve).

* *interior_sigma2* threshold for interior points (disregard points that are further away from and lie within the curve).

* *smooth_concavity* value that leads to inward bending of the curve (0 = no bending; <0 inward bending; >0 outward bending).

* *smoothness* weight of smoothness term.


.. literalinclude:: ../../../../examples/surface/example_nurbs_fitting_surface.cpp
   :language: cpp
   :lines: 122-127

The curve is initialised using a minimum number of control points to reprecent a circle, with the center located
at the mean of the point-cloud and the radius of the maximum distance of a point to the center.
Please note that in line 126 interior weighting is enabled.


.. literalinclude:: ../../../../examples/surface/example_nurbs_fitting_surface.cpp
   :language: cpp
   :lines: 129-133

Similar to the surface fitting approach, the curve is iteratively fitted and refined, as shown in the video.
Note how the curve tends to bend inwards at regions where it is not supported by any points.


.. literalinclude:: ../../../../examples/surface/example_nurbs_fitting_surface.cpp
   :language: cpp
   :lines: 136-142

After the curve fitting terminated, our geometric representation consists of a B-spline surface and a closed
B-spline curved, defined within the parametric domain of the B-spline surface. This is called trimmed B-spline surface.
In line 140 we can use the trimmed B-spline to create a triangular mesh.
When running this example and switch to wireframe mode (w), you will notice that the triangles are ordered in
a rectangular way, which is a result of the rectangular domain of the surface.


Compiling and running the program
---------------------------------

Add the following lines to your CMakeLists.txt file:

.. TODO literalinclude:: sources/greedy_projection/CMakeLists.txt
.. TODO   :language: cmake
.. TODO   :linenos:   

After you have made the executable, you can run it. Simply do::

  $ ./pcl_example_nurbs_fitting_surface ../../test/bunny.pcd

Saving and viewing the result
-----------------------------

* Saving as triangle mesh into a vtk file
  TODO

* Saving as OpenNURBS file
  TODO

