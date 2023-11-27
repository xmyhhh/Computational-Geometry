
# Computing the n-dimensional Delaunay tessellation with application to Voronoi polytopes （D.F. Watson）

## 概念

### Dirichlet tessellation（Voronoi Diagram）
Suppose the positions of $n$ distinct points $P_1 ... P_n$ in the planeare given as data. We may give each point a territory(领地) that is the area of the plane nearer to it than to any other data point. The resulting territories will form a pattern of packed convex polygons covering the whole plane. This construct is known asthe Dirichlet tessellation of the points

* In a k dimensional Euclidean space the Delaunay triangles become simplexes with k + 1 data points as vertices. 
* Each vertex in the tessellation is where k + 1 territories meet and is the centre of the hypersphere passing through all the vertices of the associated simplex. 
* As before each contiguous pair of pointsis joined by a line that is an edge of some Delaunay simplexes.
* The territorial boundary shared by the contiguous point pair is a convex polygon lying in the k - 1 dimensional hyperplane that bisects that edge.

## 数据结构定义:
In k dimensions each vertex will have k + 1 forming pointsand k + 1 neighbouring vertices opposite them.
* Two lists, each of length three
    * one list holding the forming points of the vertex(Delaunay triangles)
    * the other holding the opposite neighbouring vertex

## 算法
如果能以上述方式记录结构，然后添加新的数据点并适当修改记录，那么任何数量的点都可以通过从简单结构开始并在其基础上进行网格划分和三角测量来实现。最明显的起始模式是由前 k + 1 个点构成的德劳内单纯形。这里唯一的限制是前 k + 1 个点不能全部位于所考虑的 k 维空间的超平面上
### Adding Point
假设我们要添加的点是Q
