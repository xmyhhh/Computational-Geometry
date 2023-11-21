#pragma once
#include "common/typedef.h"
#include "common/helper.h"

void Intersection_01(std::vector<Line>& all_lines, std::vector <IntersectionResult>& intersectionResults) {
	//方法一：两两4次toleft测试

	for (int i = 0; i < all_lines.size(); i++) {
		for (int j = 0; j < all_lines.size(); j++) {
			if (i == j)
				continue;

			if (
				all_lines[i].p1 == all_lines[j].p1 ||
				all_lines[i].p2 == all_lines[j].p2 ||
				all_lines[i].p1 == all_lines[j].p2 ||
				all_lines[i].p2 == all_lines[j].p1
				)
				continue;

			bool res = true;
			res &= (
				(
					Area2(all_lines[i].p1, all_lines[i].p2, all_lines[j].p1) *
					Area2(all_lines[i].p1, all_lines[i].p2, all_lines[j].p2) < 0
					)
				&&
				(
					Area2(all_lines[j].p1, all_lines[j].p2, all_lines[i].p1) *
					Area2(all_lines[j].p1, all_lines[j].p2, all_lines[i].p2) < 0)
				);

			if (res) {
				intersectionResults.push_back(LineIntersectionCalulate(all_lines[i], all_lines[j]));
			}
		}
	}
}