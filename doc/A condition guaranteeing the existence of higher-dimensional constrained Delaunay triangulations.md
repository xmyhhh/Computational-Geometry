
# A condition guaranteeing the existence of higher-dimensional constrained Delaunay triangulations （Shewchuk）

## Abstract
* X has a d-dimensional constrained Delaunay triangulation if each k-dimensional constraining facet in X with k <= d - 2 is a union of strongly Delaunay k-simplices(如果d维PLC X中的每一个k维单纯形都是强Delaunay的，则X有CDT)
*  If the bounding segment of these facets are subdivided so that the subsegments are strongly Delaunay, then a constrained tetrahedralization exists.(如果对这些面的边界段进行细分，使子段具有很强的 Delaunay 特性，那么就会出现受约束的四面体化。)


## Definitions
### ridge-protected
* A PLC X is said to be ridge-protected if each constraining simplex in X of dimension d - 2 or less is strongly Delaunay.(如果plc中的每个约束单纯形都是强Delaunay的，该plc被称为ridge-protected) (二维要求点是强Delaunay的，三维要求线段/点是强Delaunay的)
* (The main result of this paper)if X is ridge-protected, and if no d + 2 vertices of X lie on a common sphere, then the constrained Delaunay d-simplices defined on the vertices of X collectively form a triangulation of X. This triangulation is called the constrained Dehnay triangulation of X.（如果plc是ridge-protected，那么这个plc的cdt存在）
* The condition that X be ridre-protected holds trivially in two dimensions (hence the success of two-dimensional CDTs), but not in three or more.（二维情况下，ridre-protected显然成立）
* It is not possible to place the vertices of Schiinhardt’s polyhedron so that all three of its reflex edges are Jtrongly Delaunay (though any two may be)（解释：无论怎么放置席因哈特多面体的三个顶点，三条反射边不可能都是强Delaunay的）

### ridge-protected Testing
* Testing whether a PLC is ridge-protected is straightforward. 
* Form the Delaunay triangulation of the vertices of the PLC. 
* If a constraining simples s is missing from the triangulation, then s is not strongly Delaunay. Otherwise, s is Delaunay; 
* Why is it useful to know that ridge-protected PLCs have CDTs’?  Although a given PLC X may not be ridge-protected, it can be made ridge-protected by splitting simplices that are not strongly  Delaunay into smaller simplices, with the insertion of additional vertices

### Algorithms for Constructing CDTs
* Starting with one constrained Delaunay d-simplex, a naive implementation of the foregoing gift-wrapping algorithm might construct each additional d-simples by considering all of the vertices as candidates and testing the visibility of each one against every facet. (从一个受约束的 Delaunay d-simplex 开始，上述礼品包装算法的简单实现方法可能是将所有顶点视为候选顶点，并根据每个面测试每个顶点的可见性，从而构建出每个额外的 d-simplex 。)
* the fastest way to form the CDT of a ridge-protected PLC  is to use the best available algorithm to find an unconstrained Delaunay triangulation of the input vertices, then recover the (d - 1)-facets one by one.(形成有山脊保护的 PLC CDT 的最快方法是使用现有的最佳算法找到输入顶点的无约束 Delaunay 三角剖分，然后逐个恢复 (d - 1)- 面。)
* Each (d - l)-facet f may be recovered by deleting the d-simplices  whose interiors it intersects, then retriangulating the polytopes now  left empty on each side of f.(每个 (d - l)-facet f 都可以通过删除其内部相交的 d 个简单面，然后对 f 的每一侧空着的多边形进行重切分来恢复。)
* It is easy to show that all of the aimplices not thus deleted are still constrained Delaunay. (很容易证明，所有没有被删除的目的矩阵仍然是受约束的 Delaunay。)
* Since a CDT of the new configuration exists, each empty polytope can be triangulated with constrained Delaunay simplices. (由于存在新配置的 CDT，每个空多面体都可以用受约束的 Delaunay 仿真进行三角化。)
* If these polytopcs are typically small, the performance of the algorithm used to triangulate them is not critical, and gift-wrapping will suffice. (如果这些多面体通常较小，用于三角剖分的算法的性能并不重要，礼品包装就足够了。)
* Vertices may be incrementally inserted into and deleted from a CDT just like nn ordinary Delaunay triangulation, so long as the underlying PLC (which changes incrementally with the triangulation) remnins ridge-protected. (只要底层 PLC（随三角剖分的增量而变化）仍然受到脊保护，就可以像普通的 Delaunay 三角剖分一样，在 CDT 中逐步插入和删除顶点。)
* When a vertex is inserted, the simplices that are no longer constrained Delaunay are deleted. ( 插入顶点后，不再受 Delaunay 约束的简约将被删除。)
* When a vertex is deleted, the simplices that contain it are deleted. In either case, the resulting polytopal hole is retriangulated to complete the new  CDT. (删除顶点时，包含该顶点的简面也会被删除。在这两种情况下，所产生的多边形洞都要重新进行三角剖分，以完成新的 CDT。)
* As with facet recovery, the existence of a CDT of the entire underlying PLC ensures that a CDT of the hole can be produced. (与切面恢复一样，整个底层 PLC CDT 的存在确保了孔 CDT 的产生。)
* Hence, the best approach to triangulating a PLC might be to start with a Dclnunny triangulation of the vertices of the (d - 1)-facets, then recover the (d - 1)-facets themselves, and then finally insert the remaining vertices incrementally (因此，对 PLC 进行三角剖分的最佳方法可能是先对 (d - 1)- 面板的顶点进行 Dclnunny 三角剖分，然后恢复 (d - 1)- 面板本身，最后逐步插入其余顶点。)

* The ability to incrementally insert and delete vertices is also useful for mesh generation, especially in circumstances where the constrnined Delaunny property can be used to establish provable properties of the meshing algorithm. (增量插入和删除顶点的功能对网格生成也很有用，尤其是在可以利用 Constrnined Delaunny 属性建立网格算法可证明属性的情况下。)
* Unfortunately, subsets of d + 2 or more cospherical vertices can cause real difficulties for gift-wrapping. （遗憾的是，d + 2 或更多球面顶点的子集会给礼品包装造成实际困难）
* A gift-wrapping algorithm may make dceisions that are mutually inconsistent, and find itself unable to complete the triangulation. For an example affecting unconstruined Delaunay tetrahedralizations in E^3, imagine a huge vertex set that includes six cospherical vertices whose surroundings have been inadvertently tetrahedralizd so as to form a hollow space shaped like SchUnhardt’s polyhedron. （礼品包装算法可能会做出相互不一致的决定，并发现自己无法完成三角剖分。举个影响 E^3 中未经规则化的 Delaunay 四面体化的例子，想象一个巨大的顶点集，其中包括六个球形顶点，它们的周围无意中被四面体化，从而形成了一个形状类似 SchUnhardt 多面体的空心空间。）
* This problem can be solved by using symbolic perturbation to simulntc general position, thereby ensuring that all decisions made by gift-wrapping are mutually consistent. Additionally, cospherical or nenrly-cosphericnl vertices create the need for exact arithmetic when performing the insphere tests associated with Delaunay trinngulntlon（这个问题可以通过使用符号扰动来模拟一般位置来解决，从而确保礼品包装所做的所有决定都是相互一致的。此外，在进行与 Delaunay trinngulntlon 相关的空间检验时，球面顶点或近似球面顶点需要进行精确运算）