
# Computing the n-dimensional Delaunay tessellation with application to Voronoi polytopes （D.F. Watson）

## The Algorithm
### Theorietical
* Given any stochastic array of datapoints, there exists a unique Delaunay tessellation of those points. (Delaunay tessellation的唯一性)
* All ofthe old n-simplices whose circumspheres contain the new datapoint form a simplicial n-polytope, with all of the old datapoints, (of the intersected circumspheres), lying on its (n - 1)-dimensional boundary. These old n-simplices will be replaced.(那些旧的和新点共圆的n-simplices将和新点组成一个simplicial n-polytope)
* The new point is interior to the simplicial n-polytope and forms new n-simplices with each of the boundary (n - 1)-simplices （polytope表示多胞形）
* The circumspheres of these new n-simplices will not intersect any of the other data points because these other points all lie on circumspheres not intersected by the new point. The tessellation remains a Delaunay tessellation because all circumspheres remain empty.(Delaunay tessellation的空圆性) 
* The order in which the points are introduceddoes not affect the final configuration because that dependsonly on the number and position of the data points in the complete set(Delaunay tessellation的顺序无关性) 
* If an arbitrary point fails to intersect any of the circumspheresin a Delaunay tessellation then the existing tessellation is notaltered. However, new Delaunay n-simplices now exist between the arbitrary point and some of the (n - 1)-simplices on the boundary of the old structure（如果任意点未能与德劳内网格中的任何圆周相交，那么现有的网格并不会被改变，但是，在该任意点与旧结构边界上的一些 (n - 1) 叠加点之间，现在存在新的 n 个 Delaunay 叠加点）

### Operational Steps
* Stage 1：
    * For each data point to be introduced, sequentially search the list of (n + 1)-tuples to find all circumspheres that contain the new point. （对于每个新加的点，检查每一个共圆的情况）
    * This point must fall within some of the circum-spheres for they enclose the n-simplices which are space-filling. For each such intersected circumsphere, flag the (n + 1)-tuple to indicate deletion of the associated n-simplex. （这一点必须位于某些圆周球内，对于每一个这也的圆做一个删除标记，每一个圆和一个n-simplex关联）
    * The search is made by subtracting squared components of the new datapoint to circumcentre distance from the squared radius（删除是通过到圆心的距离和半径比较得到）. 
    * Since the data are ordered by the first component, a negative result to the first subtraction indicates a completed (n + 1)-tuple.

* Stage 2
    * Then each (n - 1)-face of the n-simplices whose circumspheres have been intersected, that is, each possible n-tuple of nuclei（原子核） from each (n + 1)-tuple, is saved on a temporary list. （经过stage 1以后，n-simplices的每个面，被保存在一个临时的列表）（这里的(n - 1)-face在二维下应该是指一条边？？）（each possible n-tuple of nuclei（原子核） from each (n + 1)-tuple这段话是在说从每一个 (n + 1)-tuple中每个可能的 n-tuple，例如在二维下，就是3个点里面选两个有三种可能？？）
    * If any (n - 1)-face is found to occur twice, both occurrences are dropped from the list since this face is shared by two adjacentn-simplices. （如果某个广义面出现两次，那么就从list上删除）
    * This means that it is interior to the simplicial n-polytope formed by all n-simplices whose circumspheres have been intersected by the new nucleus.（说明这个广义面是那些和新点共圆的n-simplices组成的单形多胞形的内部，这里单形多胞形表示由单形组成的多胞形） 
    * If the new nucleus falls within the limits of the expected accumulated truncation error of any circumsphere, the temporary list and the new nucleusare abandoned but alternate treatment such as recalculating thecircumcentre by a higher precision routine could be used here.（如果新核在任何圆周的预期累积截断误差范围内，则放弃临时列表和新核，但可以采用其他处理方法，例如用精度更高的例程重新计算圆心）
    * If such an ambiguity does not appear, new n-simplices are then formed with each of these singly occurring (n - 1)-faces and their circumcentres are calculated. （如果没有出现这种歧义，则用这些单个出现的 (n - 1) 个面组成新的 n 个简图，并计算它们的圆周率）（new n-simplices are then formed with each of these singly occurring (n - 1)-faces表示用新点和list中剩下的广义面组成新的n-simplices）
    * By maintaining a count of the n-simplices incident to each nucleus and decrementing foreach completed n-simplex, completed nuclei will show a zero count. 
    * Completed nuclei and (n + 1)-tuples are replaced as their indices appear on first-in-first-out stacks.


### Data Struct
* To implement this theory, the algorithm manipulates and maintains 
    * A list of(n + 1)-tuples of indices to the nuclei or datapoints. These (n + 1)-tuples represent Delaunay n-simplices. （二维下，使用list of 3-tuples存索引）
    * Each (n + 1)-tuple of indices is associated with 
        * an (n + 1)-tuple of real values being the coordinates of the simplicial circumcentre （圆的中心）
        * the square of the circumsphere radius （圆的半径）




