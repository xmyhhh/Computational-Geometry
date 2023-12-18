//Chew L P. Constrained delaunay triangulations[C]Proceedings of the third annual symposium on Computational geometry. 1987 215-222.


# Constrained delaunay triangulations （Chew L P）

## Definition of CDT
* Let G be a straight-line planargraph.A triangulation 
* T is a constrained Delaunay triangulation (CDT) of G if each edge of G is an edge of T and for each remaining edge of T there exists a circle C with the following properties
1) the endpoints of edge e are on the boundary of C, and
2) if any vertex v of G is in the interior of C then it can not be "seen" from at least one of the endpoints of e  (i.e., if you draw the line segments from v to each endpoint of e then at least one of the line segments crosses an edge of G). 

* 在CDT中，每条原图的边都是三角剖分的边，而且对于每一条剩余的边，都存在一个圆（记为C），满足以下两个条件：
1) 这条边的两个端点在圆的边界上。
2) 如果图中的某个顶点在这个圆的内部，那么这个顶点不能被从边的两个端点"看到"。也就是说，如果从这个顶点向边的两个端点画线段，至少有一条线段会穿过图中的另一条边。

## 2 types of edges
We distinguish 2 types of edges that appear in a CDT: 
* G-edges, prespecified edges that are forced upon us as part of G
* Delaunay edges, the remaining edges of the CDT.

## Algorithm
* by sorting the vertices of G by x-coordinate; then we use this information to divide the rectangle into vertical strips in such a way that there is exactly one vertex in each strip.
* Following the divide-and conquer paradigm, the CDT is calculated for each strip, adjacent strips are pasted together in pairs to form new strips, and the CDT is calculated for each such newly formed strip until the CDT for the entire G-containing rectangle has been built.
* Edges that cross a strip, edges with no endpoints within the strip, are, for the most part, ignored. Such an edge is of interest only if it interacts in some way with a vertex that lies within the strip.
* Each strip, then, is divided into regions by cross edges, G-edges that have no endpoint within the strip. We do not keep track of all of these regions (there could be O(n2) of them). Instead <b>we keep track of just the regions that contain one or more vertices</b>.
    * how to handle vertex-containing regions, initializing  them and keeping track of them as adjacent strips are combined(如何处理包含顶点的区域，初始化这些区域，并在合并相邻条带时跟踪这些区域)
        * Each initial strip has a single region containing its single vertex. To create these initial regions we <b>need to know the edge immediately above and the edge immediately below each vertex</b>.
        * Note that the edge immediately above (below) a vertex may be the top (bottom) edge of the rectangle that contains the entire graph G.
        * As we move from top to bottom in the combined strip, we start a new region whenever either strip starts a region, we continue the region as long as a region continues in either strip, we stop a region only when we reach a point where neither strip has a vertex-containing region.
    * how to stitch together CDTs as adjacent strips are combined.(如何在合并相邻条带时缝合 CDT)
        * To do this, we create imaginary vertices at infinity.
        * Each vertex-containing region has a set of 4 of these infinite vertices.
        * It is convenient to picture the infinite vertices for a region as if they are located at the 4 corners of the region;

    * Lemma 1. If we eliminate the infinite vertices of a region by removing the infinite vertices and their edges then the remaining vertices and edges form a CDT for the region.
    * Lemma 2. Let S1, and S2 be adjacent strips that are combined to make strip S and let T be a CDT that is in S. (S1 and S2 contain CDTs that may be altered in the process of making S.) If e is a Delaunay edge of T and both endpoints of e are in S1 then e is a Delaunay edge of a CDT in S1
    * <b>CDTs are combined by executing the following steps:</b>
        1) <b>eliminate infinite vertices along the boundary between the two strips.(消除boundary上的无限点)</b>, leaving a partial CDT in each half; Lemma 2 implies that the only new edges that need to be added to complete the CDT for ,the combined strip are edges that cross the boundary between the two strips.???

        2) <b>stitch the partial CDTs together</b>（拼接）; Consider a circle with points A and B on its boundary and with center well below AB. Change the circle by moving the center upward toward AB always keeping A and B on the boundary. Continue moving the center upward until the circle intersects the first point above AB that can be seen from both A and B. Call this point X. By definition, the edges AX and BX are Delaunay edges of the CDT.
            * how to Find the best candidate. 
                * AX is a Delaunay edge, if AX is a good candidate then circle ABX will not contain a vertex that can be seen from A and X.
                * Let AC be the next edge counterclockwise around A from AB. We test if AC is a good candidate by examining triangle ADC (if it exists) where AD is the next edge around A from AC. 
                    * If no such triangle exists (AC is on the edge of the partial CDT) or if AC is a G-edge then edge AC is automatically considered a good candidate.
                    * If triangle ADC does exist and AC is a Delaunay edge then we test to see if D is within circle ABC; if it is then AC is not a good candidate.
            * Each vertex containing region is divided into subregions by G-edges that cross the boundary of the strip. (每个包含顶点的区域都被穿过条带边界的 G 型边分割成若干个子区域)
            * We consider only those G-edges that have endpoints within the strip, <b>either G-vertices or the special infinite vertices.</b> (我们只考虑那些端点位于带状区域内的 G 边，即 G 顶点或特殊的无限顶点。)
            * New Delaunay edges that cross the boundary (by Lemma 2 these are the only new edges we need) can be found be completing the following process for each subregion.(对每个子区域完成以下过程，就能找到跨越边界的新德劳内边（根据定理 2，我们只需要这些新边）)

            ```c
                //Let A and B be the endpoints of the G-edge at the bottom of the current subregion.
                while(true){
                    1) Eliminate A-edges that can be shown to be illegal because of their interaction with B; 
                    2) Eliminate B-edges that can be shown to be illegal because of their interaction with A; 
                    3) Let C be a candidate where AC is the next edge counterclockwise around A from AB (if such an edge exists); 
                    4) Let D be a candidate where BD is the next edge  clockwise around B from BA (if such an edge exists):
                    break if no candidates exist;
                    5) Let X be the candidate that correspcnds to the lower of circles ABC and ABD; 
                    6)Add edge AX or BX as appropriate and call this new edge AB;
                }

                
            ```
        3) <b>eliminate any infinite vertices not stored at the region corners.（消除非corner处无限点）</b>


## 总结
There are two ideas that have been particularly important in reaching this time bound: 
1) as in [Ya84], the only cross edges that we keep track of are those that bound vertex-containing regions; 
2) infinite vertices are used so that partial CDTs are linked for efficient access.