#include "common/typedef.h"
#include "common/helper.h"

void ConvexHull_01(std::vector<cv::Point>& all_dots, std::vector<int>& extreme_dots_index) {
	//方法三：Jarvis March(also known as Gidt-Wrapping)

	for (size_t i = 0; i < all_dots.size(); i++) {
		//mark as Non-EXTREME
		extreme_dots_index.push_back(false);
	}

	//find lowest then leftmost

	cv::Point LTL = all_dots[0];
	for (size_t i = 1; i < all_dots.size(); i++) {
		
		if (LTL.y < all_dots[i].y) {
			LTL = all_dots[i];
		}
		else if(LTL.x > all_dots[i].x){
			LTL = all_dots[i];
		}
	}

	
}