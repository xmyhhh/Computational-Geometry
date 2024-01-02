# Meshing Piecewise Linear Complexes by Constrained Delaunay Tetrahedralizations (Hang Si and Klaus G¨artner)


## Define
### Local degeneracy
* Definition Let D be the Delaunay tetrahedralization of the vertices of X, t is a tetrahedron in D and t' is an adjacent tetrahedron of t (sharing a face with t), V is the set of vertices of t, t' . If all vertices of V lie on a common sphere, V is called a local degeneracy of X.
*  Theorem 2. If D contains no local degeneracy and contains all segments of X, then the CDT of X exists.

## The Algorithm
* Let the initial PLC be X0,The CDT is constructed by the following consecutive steps:
    1) Construct an initial Delaunay tetrahedralization D0 of the vertices of X0
    2) Recover the segments of X0 in D0 by incrementally inserting points on missing segments, update X0 → X1 and D0 → D1 with the newly inserted points respectively.
    3) Remove the local degeneracies(局部退化) in X1 by either perturbing vertices or inserting new vertices, update X1 → X2 and D1 → D2 with the newly inserted points respectively.
    4) Recover the subfaces of X2 in D2 by a cavity retetrahedralization method.


### Facet Recovery
* Let T be a CDT of X2. Generally, D2 = T because some subfaces of T are non-Delaunay faces and penetrated by edges of D2
* At initialization, let T(0) = D2; add all missing subfaces into a queue Q. The algorithm starts to recover the subfaces in Q until Q is empty.
* At step i(i ≤ m), several missing subfaces are recovered together. We define a <b>missing region Ω</b> to be a set of coplanar subfaces of X2
    * all subfaces of Ω belong to one facet of X2
    * the boundary edges of Ω are edges of T(i)
    * the internal edges of Ω are missing in T(i)
