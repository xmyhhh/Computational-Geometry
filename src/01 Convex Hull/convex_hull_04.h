#include "common/typedef.h"
#include "common/helper.h"

void ConvexHull_04(std::vector<cv::Point>& all_dots, std::vector<int>& extreme_dots_index) {
	//方法四：Grahma Scan

	class stack {
		int value[1000];
		int* pTop;
		int* pSecond;
		int stack_num = 0;
	public:
		stack() {
			pTop = value + 1;
			pSecond = value;
		}
		int Push(int a) {
			if (stack_num <= 999) {
				pTop++;
				pSecond++;

				*pTop = a;

				stack_num++;
				return a;
			}
			return 0;
		}
		int Pop() {
			if (stack_num >= 1) {
				stack_num--;
				pSecond--;
				return *(pTop--);
			}
			return 0;
		}
		int Size() {
			return stack_num;
		}
		int GetTop() {
			return *pTop;
		}
		int GetSecond() {
			return *pSecond;
		}
	};


	for (int i = 0; i < all_dots.size(); i++) {
		//mark as Non-EXTREME
		extreme_dots_index[i] = (false);
	}

	//1: find lowest then leftmost
	int LTL_index = 0;
	for (int i = 1; i < all_dots.size(); i++) {
		auto LTL = all_dots[LTL_index];
		if (LTL.y > all_dots[i].y) {
			LTL_index = i;
		}
		else if (LTL.y == all_dots[i].y && LTL.x > all_dots[i].x) {
			LTL_index = i;
		}
	}
	// put ltl to pos 0, and then sort by angle

	auto p = all_dots[LTL_index];
	all_dots[LTL_index] = all_dots[0];
	all_dots[0] = p;

	stack T;
	stack S;
	S.Push(0);
	S.Push(1);

	auto cross = [](cv::Point a, cv::Point b) {
		return a.x * b.y - a.y * b.x;
		};

	for (int i = 1; i < all_dots.size() - 1; i++) {
		for (int j = i + 1; j < all_dots.size(); j++) {
			if (!ToLeft(all_dots[0], all_dots[i],/*<*/ all_dots[j])) { //如果j不在i左边，交换ij
			//if (cross(all_dots[i] - all_dots[LTL_index], all_dots[j] - all_dots[LTL_index]) < 0) {
				//swep i and j
				auto p = all_dots[i];
				all_dots[i] = all_dots[j];
				all_dots[j] = p;
			}
		}

	}
	for (int i = all_dots.size() - 1; i >= 2; i--) {
		T.Push(i);
	}


	while (T.Size()) {
		auto a = S.GetTop();
		auto b = S.GetSecond();
		auto c = T.GetTop();
		if (ToLeft(all_dots[S.GetSecond()], all_dots[S.GetTop()], all_dots[T.GetTop()])) {
			S.Push(T.Pop());
		}
		else {
			S.Pop();
		}
	}

	//3: return res
	while (S.Size()) {
		extreme_dots_index[S.Pop()] = true;
	}

}