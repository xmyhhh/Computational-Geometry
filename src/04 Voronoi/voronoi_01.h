#pragma once

#include "common/typedef.h"
#include "common/helper.h"


void Voronoi_01(std::vector<cv::Point>& all_point, DECL::DECL& decl) {
	//Incremental Construction

	auto Incremental_construction = [](const cv::Point& site, DECL::DECL& decl) {
		
		//step 1: find the face that is closest to the new site, use kd_tree better
		double min_dis = Infinity;
		const DECL::Site* pSite;
		for (const auto& site_item : decl.site_list) {
			auto r = VectorLengthSqr(site_item.position, site);
			if (r < min_dis) {
				min_dis = r;
				pSite = &site_item;
			}
		}

		DECL::Face* closest_face = pSite->incident_face;

		//step 1: bisector b and Let {v, u} = b  Cell(q)
		HEEdge current = g[f].edge;//该面起点边
		HEEdge start = current;
		
		decl.InsertVertex(site);
		};

	for (const auto& site : all_point) {
		Incremental_construction(site, decl);
	}


}