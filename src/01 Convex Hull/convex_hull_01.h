#include "common/typedef.h"
#include "common/helper.h"

void ConvexHull_02(std::vector<cv::Point>& all_dots, std::vector<int>& extreme_dots_index) {
	//方法二：判断每条边是不是极边（n^3）
	for (size_t i = 0; i < all_dots.size(); i++) {
		//mark as EXTREME
		extreme_dots_index.push_back(false);
	}

	for (size_t i = 0; i < all_dots.size(); i++) {
		for (size_t j = 0; j < all_dots.size(); j++) {
			if (i == j)
				continue;
			//check edge is extreme edge
			{
				bool left_empty = true, right_empty = true;
				for (size_t k = 0; k < all_dots.size(); k++) {
					if (k == j|| k == i)
						continue;

					if (!left_empty && !right_empty) {
						continue;
					}
					ToLeft(all_dots[i], all_dots[j], all_dots[k]) ? left_empty = false : right_empty = false;
				}
				if (left_empty || right_empty) {
					extreme_dots_index[i] = true;
					extreme_dots_index[j] = true;
				}
			}
		}
	}
}