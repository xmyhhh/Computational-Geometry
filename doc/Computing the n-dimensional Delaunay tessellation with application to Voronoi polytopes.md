
# Computing the n-dimensional Delaunay tessellation with application to Voronoi polytopes （D.F. Watson）

## The Algorithm
### Theorietical
* Given any stochastic array of datapoints, there exists a unique Delaunay tessellation of those points. (Delaunay tessellation的唯一性)
* A new data point may be introduced to any Delaunay tessellation by observing which circumspheres are intersected. old n-simplices whose circumspheres contain the new datapoint will be replaced.(逐一添加，替换旧的)
The circumspheres of these new n-simplices will not intersectany of the other data points because these other points all lie on circumspheres not intersected by the new point. The tessellation remains a Delaunay tessellation because all circumspheres remain empty.(Delaunay tessellation的空圆性) 
* The order in which the points are introduceddoes not affect the final configuration because that dependsonly on the number and position of the data points in the complete set(Delaunay tessellation的顺序无关性) 
* If an arbitrary point fails to intersect any of the circumspheresin a Delaunay tessellation then the existing tessellation is notaltered. However, new Delaunay n-simplices now exist between the arbitrary point and some of the (n - 1)-simplices on the boundary of the old structure（如果任意点未能与德劳内网格中的任何圆周相交，那么现有的网格并不会被改变，但是，在该任意点与旧结构边界上的一些 (n - 1) 叠加点之间，现在存在新的 n 个 Delaunay 叠加点）
### Data Struct
* To implement this theory, the algorithm manipulates and maintains 
    * A list of(n + 1)-tuples of indices to the nuclei or datapoints. These (n + 1)-tuples represent Delaunay n-simplices. 
    * Each (n + 1)-tuple of indices is associated with an (n + 1)-tuple of real values being the coordinates of the simplicial circumcentre and the square of the circumsphere radius.