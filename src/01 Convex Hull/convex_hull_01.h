#include "common/typedef.h"
#include "common/helper.h"

void ConvexHull_01(std::vector<cv::Point>& all_dots, std::vector<int>& extreme_dots_index) {
	//方法一：三角形内部测试判断极点（可行，但是效率低）（n^4）
	for (size_t i = 0; i < all_dots.size(); i++) {
		//mark as EXTREME
		extreme_dots_index.push_back(true);
	}

	for (size_t i = 0; i < all_dots.size(); i++) {
		for (size_t j = 0; j < all_dots.size(); j++) {
			if (i == j)
				continue;
			for (size_t k = 0; k < all_dots.size(); k++) {
				if (j == k || i == k)
					continue;
				//now we get triangle as index (i,j,k)
				//for each point
				for (size_t point_index = 0; point_index < all_dots.size(); point_index++) {
					if (point_index == i || point_index == j || point_index == k)
						continue;
					if (InTriangleTest(all_dots[i], all_dots[j], all_dots[k], all_dots[point_index]))
						extreme_dots_index[point_index] = false;

				}
			}
		}
	}
}