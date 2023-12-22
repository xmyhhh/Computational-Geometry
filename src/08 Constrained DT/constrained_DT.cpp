
#include "constrained_DT_01.h"
#include <common/vulkan/VulkanTools.h>


void constrained_DT() {

    int width = 2000;
    int height = 800;
    cv::Mat img = cv::Mat::zeros(cv::Size(width, height), CV_8UC3);

    std::vector<cv::Point> all_point;
    srand(13);


    using namespace CDT_01_datastruct;
    //load plsg
    PSLG plsg;

    plsg.offset = cv::Point2d(width / 8, height / 8);
    std::string plsg_path;
#define load_guita 1
    if (load_guita) {
        plsg.factor = 45;
        plsg_path = getAssetPath() + "guita.plsg";
    } else {
        plsg.factor = 10;
        plsg_path = getAssetPath() + "simple.plsg";
    }
    plsg.init_from_file(plsg_path);

    CDT cdt;
    CDT_01(plsg, cdt);

    //draw code begin
    debug_cout("\n*********start draw*********");

#define draw_plsg 1
//draw plsg
    draw_circle_origin_buttom_left(width, height, img, {100,100}, 2, BLUE, 2);
    if (draw_plsg) {
        for (size_t i = 0; i < plsg.boundary_array.size(); i++) {
            for (size_t j = 0; j < plsg.boundary_array[i].point_array.size(); j++) {
                draw_circle_origin_buttom_left(width, height, img, plsg.get_position(i, j), 2, RED, 2);

                if (j == 0) {
                    draw_line_origin_buttom_left(width, height, img, plsg.get_position(i, j),
                                                 plsg.get_position(i, plsg.boundary_array[i].point_array.size() - 1),
                                                 GREEN,
                                                 1);
                } else {
                    draw_line_origin_buttom_left(width, height, img, plsg.get_position(i, j),
                                                 plsg.get_position(i, j - 1),
                                                 GREEN, 1);
                }
            }
        }
        for (size_t i = 0; i < plsg.vertex_array.size(); i++) {
            draw_circle_origin_buttom_left(width, height, img, plsg.get_position(i), 2, RED, 2);
        }
    }

#define draw_cdt 0
//draw cdt
    if (draw_cdt) {
        for (size_t i = 0; i < cdt.number_of_constrained_edge; i++) {
            auto ce = cdt.constrained_edge_array[i];
            draw_line_origin_buttom_left(width, height, img, ce.orig->position, ce.end->position, GREEN, 1);
        }

        for (size_t i = 0; i < cdt.strip_array.size(); i++) {
            auto s = cdt.strip_array[i];
            if (s->region_left_x != -1) {
                draw_line_origin_buttom_left(width, height, img, cv::Point2d(s->region_left_x, 1000),
                                             cv::Point2d(s->region_left_x, -1000), GREEN, 1);
            }
            if (s->region_right_x != -1) {
                draw_line_origin_buttom_left(width, height, img, cv::Point2d(s->region_right_x, 1000),
                                             cv::Point2d(s->region_right_x, -1000), GREEN, 1);
            }

            for (size_t j = 0; j < s->region.size(); j++) {
                auto r = s->region[j];
                for (size_t k = 0; k < r->vertex_array.size(); k++) {
                    auto vtx = r->vertex_array[k];
                    draw_circle_origin_buttom_left(width, height, img, vtx->position, 2, RED, 2);
                }

            }
        }
    }


#define draw_strip 0
    if (draw_strip) {
        for (auto &strip: cdt.strip_array) {
            for (auto &region: strip->region) {
                if (region->infinity_vtx.buttom_left)
                    draw_circle_origin_buttom_left(width, height, img, region->infinity_vtx.buttom_left->position, 1,
                                                   WHITE, 2);
                if (region->infinity_vtx.buttom_right)
                    draw_circle_origin_buttom_left(width, height, img, region->infinity_vtx.buttom_right->position, 1,
                                                   WHITE, 2);
                if (region->infinity_vtx.top_left)
                    draw_circle_origin_buttom_left(width, height, img, region->infinity_vtx.top_left->position, 1,
                                                   WHITE, 2);
                if (region->infinity_vtx.top_right)
                    draw_circle_origin_buttom_left(width, height, img, region->infinity_vtx.top_right->position, 1,
                                                   WHITE, 2);
            }

        }
    }


#define Grid_Size_w 20
#define Grid_Size_h 8
#define Show_Grid 1
    {
        if (Show_Grid) {
            for (size_t i = 0; i < Grid_Size_w; i++) {
                line(img, cv::Point(width / Grid_Size_w * i, 0), cv::Point(width / Grid_Size_w * i, height), WHITE, 1);
            }
            for (size_t i = 0; i < Grid_Size_h; i++) {
                line(img, cv::Point(0, height / Grid_Size_h * i), cv::Point(width, height / Grid_Size_h * i), WHITE, 1);
            }
        }
    }


    imshow("CDT", img);
    cv::waitKey(0);
}