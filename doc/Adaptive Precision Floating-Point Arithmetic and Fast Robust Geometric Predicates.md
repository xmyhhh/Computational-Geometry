# Adaptive Precision Floating-Point Arithmetic and Fast Robust Geometric Predicates

## Introduction
* an exact arithmetic library can be used and will yield a correct result, but exact arithmetic is slow; one would rather use it only when one really needs to.
* This article presents two techniques for writing fast implementations of extended precision calculations like these, and demonstrates them with implementations of four commonly used geometric predicates (本文介绍了编写类似扩展精度计算的快速实现的两种技术，并通过实现四种常用几何谓词对其进行了演示)
* The first technique is a suite of algorithms, several of them new, for performing arbitrary precision arithmetic. The method has its greatest advantage in computations that process values of extended but small precision (several hundred or thousand bits), and seems ideal for computational geometry and some numerical methods, where much benefit can be realized from a modest increase in precision.  (第一项技术是一套用于执行任意精度运算的算法，其中有几种是新算法。该方法的最大优势在于处理扩展但精度较小的数值（几百或几千比特），似乎非常适合计算几何和某些数值方法，因为在这些领域，精度的适度提高可以带来很多好处。)
* The second technique is a way to modify these algorithms so that they compute their result adaptively; they are quick in most circumstances, but are still slow when their results are prone to have high relative error. (第二种技术是对这些算法进行修改，使其能够自适应地计算结果；在大多数情况下，这些算法的计算速度很快，但当其结果容易产生较大的相对误差时，计算速度仍然很慢。)
* A third subject of this article is a demonstration of these techniques with implementations and performance measurements of four commonly used geometric predicates. An elaboration of each of these three topics follows( 本文的第三个主题是通过对四个常用几何谓词的实现和性能测量来演示这些技术。下面分别对这三个主题进行阐述)
* The algorithms described herein use floating-point hardware to perform extended precision floating point arithmetic, using the multiple-component approach. These algorithms, described in Section 2, work under the assumption that hardware arithmetic is performed in radix two with exact rounding. This assumption holds on processors compliant with the IEEE 754 floating-point standard. Proofs of the correctness of all algorithms are given. (本文介绍的算法采用多分量方法，使用浮点硬件执行扩展精度浮点运算。第 2 节中介绍的这些算法是在硬件运算以弧度二和精确四舍五入方式进行的假设条件下运行的。这一假设在符合 IEEE 754 浮点标准的处理器上成立。本文给出了所有算法的正确性证明。)


## Definitions
### Geometric algorithms may be divided into several classes with varying amounts of robustness: 
* exact algorithms, which are always correct; 
* robust algorithms, which are always correct for some perturbation of the input; 
* stable algorithms, for which the perturbation is small; 
* quasi-robust algorithms, whose results might be geometrically inconsistent, but nevertheless satisfy some weakened consistency criterion; 
* fragile algorithms, which are not guaranteed to produce any usable output at all. 


if two numbers have the same sign and differ by at most a factor of two, then their difference is computed with no roundoff