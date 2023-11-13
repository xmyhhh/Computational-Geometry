#include "common/typedef.h"
#include "common/helper.h"

void ConvexHull_03(std::vector<cv::Point>& all_dots, std::vector<int>& extreme_dots_index) {
	//方法三：Jarvis March(also known as Gidt-Wrapping)

	for (int i = 0; i < all_dots.size(); i++) {
		//mark as Non-EXTREME
		extreme_dots_index.push_back(false);
	}

	//1: find lowest then leftmost, LTL一定是极点？

	int LTL_index = 0;
	for (int i = 1; i < all_dots.size(); i++) {
		auto LTL = all_dots[LTL_index];
		if (LTL.y < all_dots[i].y) {
			LTL_index = i;
		}
		else if (LTL.y == all_dots[i].y && LTL.x > all_dots[i].x) {
			LTL_index = i;
		}
	}
	//2: find first EE and next ee……， 这个地方采用比较的思路，寻找更“右侧”的点

	int current_ee_index = LTL_index;


	do {
		int next_ee_index = 0;
		for (int i = 0; i < all_dots.size(); i++) {
			extreme_dots_index[current_ee_index] = true;
			if (i != next_ee_index && i != current_ee_index) {
				if (!ToLeft(all_dots[current_ee_index], all_dots[i], all_dots[next_ee_index])) {
					next_ee_index = i;
				}
			}
		}
		current_ee_index = next_ee_index;
	}

	while (current_ee_index != LTL_index);


}