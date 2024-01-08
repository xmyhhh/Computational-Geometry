#pragma once

#include "common/typedef.h"
#include "common/helper.h"
#include "common/file/format_convert.h"
#include "common/memory.h"

namespace unwrap_01_datastruct {
    struct Edge;
    struct Tetrahedra;

    struct Vertex {
        cv::Point3d position;
        std::vector<Tetrahedra *> *connect_tetrahedra_array;
        std::vector<Edge *> *connect_edge_array;
        uint static_index;

        static Vertex *allocate_from_pool(MemoryPool *pool) {
            auto v = (Vertex *) pool->allocate();
            v->connect_tetrahedra_array = new std::vector<Tetrahedra *>();
            v->connect_edge_array = new std::vector<Edge *>();
            v->static_index = 0;
            return v;
        }

    };

    struct Face {
        Vertex *p1;
        Vertex *p2;
        Vertex *p3;
        Tetrahedra *disjoin_tet[2];//0 is ccw , 1 is cw
        Edge *disjoin_edge[3];
        bool is_boundary_face = false;
        bool draw_read = false;
        bool mark = false;


        static Face *allocate_from_pool(MemoryPool *pool, Vertex *_p1, Vertex *_p2, Vertex *_p3) {
            auto f = (Face *) pool->allocate();
            f->disjoin_tet[0] = nullptr;
            f->disjoin_tet[1] = nullptr;
            f->disjoin_edge[0] = nullptr;
            f->disjoin_edge[1] = nullptr;
            f->disjoin_edge[2] = nullptr;
            f->p1 = _p1;
            f->p2 = _p2;
            f->p3 = _p3;
            f->is_boundary_face = false;
            f->draw_read = false;
            f->mark = false;
            return f;
        }

        static Face *allocate_from_pool(MemoryPool *pool) {
            auto f = (Face *) pool->allocate();
            f->disjoin_tet[0] = nullptr;
            f->disjoin_tet[1] = nullptr;

            f->disjoin_edge[0] = nullptr;
            f->disjoin_edge[1] = nullptr;
            f->disjoin_edge[2] = nullptr;
            f->draw_read = false;
            f->mark = false;
            return f;
        }

        static bool is_disjoin_face(Face *f1, Face *f2) {
            bool b1 = f1->p1 == f2->p1 || f1->p1 == f2->p2 || f1->p1 == f2->p3;
            bool b2 = f1->p2 == f2->p1 || f1->p2 == f2->p2 || f1->p2 == f2->p3;
            bool b3 = f1->p3 == f2->p1 || f1->p3 == f2->p2 || f1->p3 == f2->p3;
            int i = 0;
            if (b1)
                i++;
            if (b2)
                i++;
            if (b3)
                i++;
            return i == 2;
        }

        static cv::Point3d get_face_normal(Face *f) {
            auto u = f->p2->position - f->p1->position;
            auto v = f->p3->position - f->p1->position;
            auto n = VectorNormal(cross(v, u));
            return n;

        }
    };

    struct Edge {
        Vertex *orig;
        Vertex *end;
        std::vector<Face *> *connect_face_array;
        //for draw debug
        bool draw_red = false;

        static Edge *allocate_from_pool(MemoryPool *pool, Vertex *_orig, Vertex *_end) {
            auto e = (Edge *) pool->allocate();
            e->orig = _orig;
            e->end = _end;
            e->draw_red = false;
            _orig->connect_edge_array->push_back(e);
            _end->connect_edge_array->push_back(e);
            e->connect_face_array = new std::vector<Face *>();
            return e;
        }

    };

    struct Tetrahedra {
        Vertex *p1;
        Vertex *p2;
        Vertex *p3;
        Vertex *p4;

        Tetrahedra *neighbors[4];
        Face *faces[4];
        bool mark;

        int depth;
        //for draw
        bool draw_red = false;


        static Tetrahedra *allocate_from_pool(MemoryPool *pool, Vertex *_p1, Vertex *_p2, Vertex *_p3, Vertex *_p4) {
            auto t = (Tetrahedra *) pool->allocate();
            t->p1 = _p1;
            t->p2 = _p2;
            t->p3 = _p3;
            t->p4 = _p4;
            t->neighbors[0] = nullptr;
            t->neighbors[1] = nullptr;
            t->neighbors[2] = nullptr;
            t->neighbors[3] = nullptr;

            t->faces[0] = nullptr;
            t->faces[1] = nullptr;
            t->faces[2] = nullptr;
            t->faces[3] = nullptr;

            t->mark = false;
            t->draw_red = false;

            t->p1->connect_tetrahedra_array->push_back(t);
            t->p2->connect_tetrahedra_array->push_back(t);
            t->p3->connect_tetrahedra_array->push_back(t);
            t->p4->connect_tetrahedra_array->push_back(t);

            return t;
        }

        Vertex *get_vtx(int i) {
            if (i == 0)
                return p1;
            if (i == 1)
                return p2;
            if (i == 2)
                return p3;
            if (i == 3)
                return p4;
            ASSERT(false);
            return nullptr;
        }

        static bool is_tetrahedra_disjoin(Tetrahedra *t1, Tetrahedra *t2) {
            int i = 0;
            if (t1->p1 == t2->p1 || t1->p1 == t2->p2 || t1->p1 == t2->p3 || t1->p1 == t2->p4)
                i++;
            if (t1->p2 == t2->p1 || t1->p2 == t2->p2 || t1->p2 == t2->p3 || t1->p2 == t2->p4)
                i++;
            if (t1->p3 == t2->p1 || t1->p3 == t2->p2 || t1->p3 == t2->p3 || t1->p3 == t2->p4)
                i++;
            if (t1->p4 == t2->p1 || t1->p4 == t2->p2 || t1->p4 == t2->p3 || t1->p4 == t2->p4)
                i++;

            return i == 3;

        }

        static int find_disjoin_tet_non_share_vtx_index_in_t2(Tetrahedra *t1, Tetrahedra *t2) {
            bool is_vtx1_share = 0, is_vtx2_share = 0, is_vtx3_share = 0, is_vtx4_share = 0;
            is_vtx1_share = (t2->p1 == t1->p1 || t2->p1 == t1->p2 || t2->p1 == t1->p3 || t2->p1 == t1->p4);
            is_vtx2_share = (t2->p2 == t1->p1 || t2->p2 == t1->p2 || t2->p2 == t1->p3 || t2->p2 == t1->p4);
            is_vtx3_share = (t2->p3 == t1->p1 || t2->p3 == t1->p2 || t2->p3 == t1->p3 || t2->p3 == t1->p4);
            is_vtx4_share = (t2->p4 == t1->p1 || t2->p4 == t1->p2 || t2->p4 == t1->p3 || t2->p4 == t1->p4);

            int i = 0;
            int value = 0;
            if (is_vtx1_share)
                i++;
            else
                value = 0;

            if (is_vtx2_share)
                i++;
            else
                value = 1;

            if (is_vtx3_share)
                i++;
            else
                value = 2;

            if (is_vtx4_share)
                i++;
            else
                value = 3;

            ASSERT(i == 3);

            return value;
        }

        static void bind_tet_and_face(Tetrahedra *t, Face *f) {
            bool b1 = t->p1 == f->p1 || t->p1 == f->p2 || t->p1 == f->p3;
            bool b2 = t->p2 == f->p1 || t->p2 == f->p2 || t->p2 == f->p3;
            bool b3 = t->p3 == f->p1 || t->p3 == f->p2 || t->p3 == f->p3;
            bool b4 = t->p4 == f->p1 || t->p4 == f->p2 || t->p4 == f->p3;
            int i = 0;
            if (b1)
                i++;
            if (b2)
                i++;
            if (b3)
                i++;
            if (b4)
                i++;
            ASSERT(i == 3);
            bool orient;
            if (!b1) {
                orient = ToLeft3D(f->p1->position, f->p2->position, f->p3->position, t->p1->position);
                t->faces[0] = f;
            }
            if (!b2) {
                orient = ToLeft3D(f->p1->position, f->p2->position, f->p3->position, t->p2->position);
                t->faces[1] = f;
            }
            if (!b3) {
                orient = ToLeft3D(f->p1->position, f->p2->position, f->p3->position, t->p3->position);
                t->faces[2] = f;
            }
            if (!b4) {
                orient = ToLeft3D(f->p1->position, f->p2->position, f->p3->position, t->p4->position);
                t->faces[3] = f;
            }
            orient ? f->disjoin_tet[0] = t : f->disjoin_tet[1] = t;
        }
    };

    struct PhysicalGroup_2D {
        std::vector<Face *> face_array;
        std::vector<Face *> boundary_face_array;
    };

    struct Unwrap {
        MemoryPool vertex_pool;
        MemoryPool edge_pool;
        MemoryPool face_pool;
        MemoryPool tetrahedra_pool;

        int size = 40;
        std::vector<PhysicalGroup_2D> phy_group_array;

        bool init_from_file(std::string path) {


            debug_cout("init_from_file");
            FILE *fp = fopen(path.c_str(), "r");
            if (fp == (FILE *) NULL) {
                //printf("File I/O Error:  Cannot create file %s.\n", vtk_file_path);
                return false;
            }

            char buffer[2048];
            char *bufferp;
            int line_count = 0;

            int nverts = 0, iverts = 0;
            int ntetrahedras = 0, itetrahedras = 0, itetrahedrasattr = 0;
            bool readattr = false;

            while ((bufferp = read_line(buffer, fp, &line_count)) != NULL) {
                if (nverts == 0) {
                    read_line(buffer, fp, &line_count); //Unstructured Grid
                    read_line(buffer, fp, &line_count); //ASCII
                    read_line(buffer, fp, &line_count); //DATASET UNSTRUCTURED_GRID
                    read_line(buffer, fp, &line_count); //POINTS xxxx double
                    sscanf(bufferp, "%*s %d %*s", &nverts);
                    if (nverts < 3) {
                        //printf("Syntax error reading header on line %d in file %s\n",
                        //	line_count, vtk_file_path);
                        fclose(fp);
                        return false;
                    }
                    vertex_pool.initializePool(sizeof(Vertex), nverts, 8, 32);
                } else if (nverts > iverts) {
                    auto v = Vertex::allocate_from_pool(&vertex_pool);

                    v->position.x = (double) strtod(bufferp, &bufferp) / 5;
                    v->position.y = (double) strtod(bufferp, &bufferp) / 5;
                    v->position.z = (double) strtod(bufferp, &bufferp) / 5;
                    v->static_index = iverts;

                    iverts++;
                } else if (ntetrahedras == 0) {
                    //CELLS 35186 175930

                    sscanf(bufferp, "%*s %d %*d", &ntetrahedras);
                    tetrahedra_pool.initializePool(sizeof(Tetrahedra), ntetrahedras * 1.2, 8, 32);

                } else if (ntetrahedras > itetrahedras) {
                    static int p0, p1, p2, p3;
                    sscanf(bufferp, "%*d %d %d %d %d",
                           &p0,
                           &p1,
                           &p2,
                           &p3
                    );

                    Tetrahedra::allocate_from_pool(&tetrahedra_pool, (Vertex *) vertex_pool[p0], (Vertex *) vertex_pool[p1], (Vertex *) vertex_pool[p2], (Vertex *) vertex_pool[p3]);

                    itetrahedras++;
                } else {
                    break;
                }
            }
            fclose(fp);

            update_tet_neightbors();
            create_face_and_edge();
            boundary_face_mark();
            return true;
        }

        void update_tet_neightbors() {
            debug_cout("update_tet_neightbors");
            for (int i = 0; i < tetrahedra_pool.size(); i++) {
                for (int j = 0; j < 4; j++) {
                    auto t1 = (Tetrahedra *) tetrahedra_pool[i];
                    t1->neighbors[j] = nullptr;
                }
            }

            for (int i = 0; i < tetrahedra_pool.size(); i++) {
                auto t1 = (Tetrahedra *) tetrahedra_pool[i];

                for (int j = 0; j < 4; j++) {
                    auto vtx = t1->get_vtx(j);
                    for (auto t2: *vtx->connect_tetrahedra_array) {
                        if (t1 == t2)
                            continue;

                        if (Tetrahedra::is_tetrahedra_disjoin(t1, t2)) {
                            //mark t1 t2 as neighbor
                            int k = Tetrahedra::find_disjoin_tet_non_share_vtx_index_in_t2(t1, t2);
                            t2->neighbors[k] = t1;

                            k = Tetrahedra::find_disjoin_tet_non_share_vtx_index_in_t2(t2, t1);
                            t1->neighbors[k] = t2;
                        }
                    }
                }


            }
        }

        void create_face_and_edge() {
            debug_cout("create_face_and_egde");

            face_pool.initializePool(sizeof(Face), tetrahedra_pool.size() * 1.2, 8, 32);
            edge_pool.initializePool(sizeof(Face), tetrahedra_pool.size() * 3 * 1.2, 8, 32);

            for (int i = 0; i < tetrahedra_pool.size(); i++) {

                auto create_tet_face = [this](int tet_face_index, Tetrahedra *t) {
                    if (t->faces[tet_face_index] == nullptr) {
                        if (t->neighbors[tet_face_index] != nullptr) {
                            //find adjface_vtx_index
                            auto face_index = Tetrahedra::find_disjoin_tet_non_share_vtx_index_in_t2(t, t->neighbors[tet_face_index]);
                            auto face_in_neighbor = t->neighbors[tet_face_index]->faces[face_index];
                            if (face_in_neighbor != nullptr) {
                                Tetrahedra::bind_tet_and_face(t, face_in_neighbor);
                                return;
                            }
                        }

                        auto f = Face::allocate_from_pool(&face_pool);
                        if (tet_face_index == 0) {
                            f->p1 = t->p2;
                            f->p2 = t->p3;
                            f->p3 = t->p4;
                        } else if (tet_face_index == 1) {
                            f->p1 = t->p1;
                            f->p2 = t->p3;
                            f->p3 = t->p4;
                        } else if (tet_face_index == 2) {
                            f->p1 = t->p1;
                            f->p2 = t->p2;
                            f->p3 = t->p4;
                        } else if (tet_face_index == 3) {
                            f->p1 = t->p1;
                            f->p2 = t->p2;
                            f->p3 = t->p3;
                        } else {
                            ASSERT(false);
                        }
                        Tetrahedra::bind_tet_and_face(t, f);
                    }
                };

                auto t = (Tetrahedra *) tetrahedra_pool[i];

                //create face
                create_tet_face(0, t);
                create_tet_face(1, t);
                create_tet_face(2, t);
                create_tet_face(3, t);

                for (int j = 0; j < 4; j++) {
                    auto f = t->faces[j];

                    auto p1 = (Vertex *) f->p1;
                    auto p2 = (Vertex *) f->p2;
                    auto p3 = (Vertex *) f->p3;

                    auto find_edge = [](Vertex *p1, Vertex *p2, Edge *&res) -> bool {

                        for (auto e: *p1->connect_edge_array) {
                            if (e->orig == p2 || e->end == p2) {
                                res = e;
                                return true;
                            }
                        }

                        for (auto e: *p2->connect_edge_array) {
                            if (e->orig == p1 || e->end == p1) {
                                res = e;
                                return true;
                            }
                        }

                        return false;
                    };

                    Edge *e = nullptr;
                    if (!find_edge(p1, p2, e)) {
                        e = Edge::allocate_from_pool(&edge_pool, p1, p2);
                    }
                    f->disjoin_edge[0] = e;
                    e->connect_face_array->push_back(f);
                    if (!find_edge(p2, p3, e)) {
                        e = Edge::allocate_from_pool(&edge_pool, p2, p3);
                    }
                    f->disjoin_edge[1] = e;
                    e->connect_face_array->push_back(f);
                    if (!find_edge(p3, p1, e)) {
                        e = Edge::allocate_from_pool(&edge_pool, p3, p1);
                    }
                    f->disjoin_edge[2] = e;
                    e->connect_face_array->push_back(f);
                }

            }
        }

        void boundary_face_mark() {
            debug_cout("boundary_face_mark");
            int boundary_face_num = 0;
            for (int i = 0; i < face_pool.size(); i++) {
                auto f = (Face *) face_pool[i];

                if (f->disjoin_tet[0] == nullptr || f->disjoin_tet[1] == nullptr) {
                    f->is_boundary_face = true;
                    boundary_face_num++;
                } else {
                    f->is_boundary_face = false;
                }

            }

            debug_cout("boundary_face_num: " + std::to_string(boundary_face_num), false);
            debug_cout("non-boundary_face_num: " + std::to_string(face_pool.size() - boundary_face_num), false);
        }

        VulkanDrawData toVulkanDrawData_Egde() {
            debug_cout("prepare vulkan data");
            VulkanDrawData vulkan_data;

            vulkan_data.numberOfPoint = vertex_pool.size();
            vulkan_data.points = (double *) malloc(vulkan_data.numberOfPoint * 3 * sizeof(double));

            //point
            for (int i = 0; i < vertex_pool.size(); i++) {
                auto vtx = (Vertex *) vertex_pool[i];
                vulkan_data.points[i * 3] = vtx->position.x;
                vulkan_data.points[i * 3 + 1] = -vtx->position.y;
                vulkan_data.points[i * 3 + 2] = vtx->position.z;
            }

            //edge
            vulkan_data.numberOfTriangle = edge_pool.size();

            vulkan_data.triangles = (int *) malloc(vulkan_data.numberOfTriangle * 3/*xyz*/ * sizeof(int));
            vulkan_data.triangleColoring = true;
            vulkan_data.triangleColors = (double *) malloc(vulkan_data.numberOfTriangle * 3/*rgb*/ * sizeof(double));

            for (int i = 0; i < vulkan_data.numberOfTriangle/* tetrahedra_pool.size() * 4 * 3 */; i++) {
                vulkan_data.triangleColors[i * 3] = 1;
                vulkan_data.triangleColors[i * 3 + 1] = 1;
                vulkan_data.triangleColors[i * 3 + 2] = 1;
            }

            for (int i = 0; i < edge_pool.size(); i++) {
                auto t = (Edge *) edge_pool[i];

                vulkan_data.triangles[i * 3] = t->orig->static_index;
                vulkan_data.triangles[i * 3 + 1] = t->orig->static_index;
                vulkan_data.triangles[i * 3 + 2] = t->end->static_index;

                vulkan_data.triangleColors[i * 3] = 1;
                vulkan_data.triangleColors[i * 3 + 1] = 0.8;
                vulkan_data.triangleColors[i * 3 + 2] = 1;
            }
            debug_cout("prepare vulkan data end");
            return vulkan_data;
        };

        VulkanDrawData toVulkanDrawData_Face() {
            debug_cout("prepare vulkan data");
            VulkanDrawData vulkan_data;

            vulkan_data.numberOfPoint = vertex_pool.size();
            vulkan_data.points = (double *) malloc(vulkan_data.numberOfPoint * 3 * sizeof(double));

            //point
            for (int i = 0; i < vertex_pool.size(); i++) {
                auto vtx = (Vertex *) vertex_pool[i];
                vulkan_data.points[i * 3] = vtx->position.x;
                vulkan_data.points[i * 3 + 1] = -vtx->position.y;
                vulkan_data.points[i * 3 + 2] = vtx->position.z;
            }

            //edge
            vulkan_data.numberOfTriangle = face_pool.size();

            vulkan_data.triangles = (int *) malloc(vulkan_data.numberOfTriangle * 3/*xyz*/ * sizeof(int));
            vulkan_data.triangleColoring = true;
            vulkan_data.triangleColors = (double *) malloc(vulkan_data.numberOfTriangle * 3/*rgb*/ * sizeof(double));

            for (int i = 0; i < vulkan_data.numberOfTriangle; i++) {

                vulkan_data.triangleColors[i * 3] = 1;
                vulkan_data.triangleColors[i * 3 + 1] = 1;
                vulkan_data.triangleColors[i * 3 + 2] = 1;
            }

            for (int i = 0; i < face_pool.size(); i++) {
                auto t = (Face *) face_pool[i];

                vulkan_data.triangles[i * 3] = t->p1->static_index;
                vulkan_data.triangles[i * 3 + 1] = t->p2->static_index;
                vulkan_data.triangles[i * 3 + 2] = t->p3->static_index;

                if (t->is_boundary_face) {
                    vulkan_data.triangleColors[i * 3] = 0.5;
                    vulkan_data.triangleColors[i * 3 + 1] = 0.5;
                    vulkan_data.triangleColors[i * 3 + 2] = 0.5;

                    if (t->draw_read) {
                        vulkan_data.triangleColors[i * 3] = 1;
                        vulkan_data.triangleColors[i * 3 + 1] = 0;
                        vulkan_data.triangleColors[i * 3 + 2] = 0;
                    }

                } else {
                    vulkan_data.triangleColors[i * 3] = 0.4;
                    vulkan_data.triangleColors[i * 3 + 1] = 0.4;
                    vulkan_data.triangleColors[i * 3 + 2] = 0.4;
                }
            }
            debug_cout("prepare vulkan data end");
            return vulkan_data;
        };

        VulkanDrawData toVulkanDrawData_Tet() {
            debug_cout("prepare vulkan data");
            VulkanDrawData vulkan_data;

            vulkan_data.numberOfPoint = vertex_pool.size();
            vulkan_data.points = (double *) malloc(vulkan_data.numberOfPoint * 3 * sizeof(double));

            //point
            for (int i = 0; i < vertex_pool.size(); i++) {
                auto vtx = (Vertex *) vertex_pool[i];
                vulkan_data.points[i * 3] = vtx->position.x;
                vulkan_data.points[i * 3 + 1] = -vtx->position.y;
                vulkan_data.points[i * 3 + 2] = vtx->position.z;
            }

            //edge
            vulkan_data.numberOfTriangle += tetrahedra_pool.size() * 4;

            vulkan_data.triangles = (int *) malloc(vulkan_data.numberOfTriangle * 3/*xyz*/ * sizeof(int));
            vulkan_data.triangleColoring = true;
            vulkan_data.triangleColors = (double *) malloc(vulkan_data.numberOfTriangle * 3/*rgb*/ * sizeof(double));

            for (int i = 0; i < vulkan_data.numberOfTriangle/* tetrahedra_pool.size() * 4 * 3 */; i++) {
                vulkan_data.triangleColors[i * 3] = 1;
                vulkan_data.triangleColors[i * 3 + 1] = 1;
                vulkan_data.triangleColors[i * 3 + 2] = 1;
            }

            for (int i = 0; i < tetrahedra_pool.size(); i++) {
                auto t = (Tetrahedra *) tetrahedra_pool[i];

                vulkan_data.triangles[i * 3 * 4] = t->p1->static_index;
                vulkan_data.triangles[i * 3 * 4 + 1] = t->p2->static_index;
                vulkan_data.triangles[i * 3 * 4 + 2] = t->p3->static_index;

                vulkan_data.triangles[i * 3 * 4 + 3] = t->p1->static_index;
                vulkan_data.triangles[i * 3 * 4 + 4] = t->p2->static_index;
                vulkan_data.triangles[i * 3 * 4 + 5] = t->p4->static_index;

                vulkan_data.triangles[i * 3 * 4 + 6] = t->p1->static_index;
                vulkan_data.triangles[i * 3 * 4 + 7] = t->p3->static_index;
                vulkan_data.triangles[i * 3 * 4 + 8] = t->p4->static_index;

                vulkan_data.triangles[i * 3 * 4 + 9] = t->p2->static_index;
                vulkan_data.triangles[i * 3 * 4 + 10] = t->p3->static_index;
                vulkan_data.triangles[i * 3 * 4 + 11] = t->p4->static_index;

                bool is_interior_tet = (t->neighbors[0] != nullptr) && (t->neighbors[1] != nullptr) && (t->neighbors[2] != nullptr) && (t->neighbors[3] != nullptr);

                if (!is_interior_tet) {
                    vulkan_data.triangleColors[i * 3 * 4] = 1;
                    vulkan_data.triangleColors[i * 3 * 4 + 1] = 0;
                    vulkan_data.triangleColors[i * 3 * 4 + 2] = 0;

                    vulkan_data.triangleColors[i * 3 * 4 + 3] = 1;
                    vulkan_data.triangleColors[i * 3 * 4 + 4] = 0;
                    vulkan_data.triangleColors[i * 3 * 4 + 5] = 0;

                    vulkan_data.triangleColors[i * 3 * 4 + 6] = 1;
                    vulkan_data.triangleColors[i * 3 * 4 + 7] = 0;
                    vulkan_data.triangleColors[i * 3 * 4 + 8] = 0;

                    vulkan_data.triangleColors[i * 3 * 4 + 9] = 1;
                    vulkan_data.triangleColors[i * 3 * 4 + 10] = 0;
                    vulkan_data.triangleColors[i * 3 * 4 + 11] = 0;
                } else {
                    vulkan_data.triangleColors[i * 3 * 4] = 0.4;
                    vulkan_data.triangleColors[i * 3 * 4 + 1] = 0.4;
                    vulkan_data.triangleColors[i * 3 * 4 + 2] = 0.4;

                    vulkan_data.triangleColors[i * 3 * 4 + 3] = 0.4;
                    vulkan_data.triangleColors[i * 3 * 4 + 4] = 0.4;
                    vulkan_data.triangleColors[i * 3 * 4 + 5] = 0.4;

                    vulkan_data.triangleColors[i * 3 * 4 + 6] = 0.4;
                    vulkan_data.triangleColors[i * 3 * 4 + 7] = 0.4;
                    vulkan_data.triangleColors[i * 3 * 4 + 8] = 0.4;

                    vulkan_data.triangleColors[i * 3 * 4 + 9] = 0.4;
                    vulkan_data.triangleColors[i * 3 * 4 + 10] = 0.4;
                    vulkan_data.triangleColors[i * 3 * 4 + 11] = 0.4;
                }

            }
            debug_cout("prepare vulkan data end");
            return vulkan_data;
        };


    };


}

void Unwrap(unwrap_01_datastruct::Unwrap &uw) {
    debug_cout("begin Unwrap");
    using namespace unwrap_01_datastruct;
    //Step 1: Get mesh center
    cv::Point3d center(0);
    auto uw_vtx_len = uw.vertex_pool.size();
    for (int i = 0; i < uw.vertex_pool.size(); i++) {
        auto v = (Vertex *) uw.vertex_pool[i];
        center += cv::Point3d(v->position.x / uw_vtx_len, v->position.y / uw_vtx_len, v->position.z / uw_vtx_len);
    }

    //find six tri
    debug_cout("find six tri");
    Face *six_direction_center_face[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};//x+-;y+-;z+-
    const cv::Point3d center_offset{0, 0, 0};
    const cv::Point3d center_rotation{-50, 0, 0};
    const double len = 5000;
    cv::Point3d axis_x = Rotate3d({1, 0, 0}, center_rotation.x, center_rotation.y, center_rotation.z);
    cv::Point3d axis_y = Rotate3d({0, 1, 0}, center_rotation.x, center_rotation.y, center_rotation.z);
    cv::Point3d axis_z = VectorNormal(cross(axis_x, axis_y));

    for (int i = 0; i < uw.face_pool.size(); i++) {
        auto f = (Face *) uw.face_pool[i];

        if (f->is_boundary_face) {
            auto res0 = RayIntersectionCalulate_Triangle({f->p1->position, f->p2->position, f->p3->position}, {center, center + axis_x * len});
            if (res0.intersect) {
                f->draw_read = true;
                six_direction_center_face[0] = f;
            }

            auto res1 = RayIntersectionCalulate_Triangle({f->p1->position, f->p2->position, f->p3->position}, {center, center - axis_x * len});
            if (res1.intersect) {
                f->draw_read = true;
                six_direction_center_face[1] = f;
            }

            auto res2 = RayIntersectionCalulate_Triangle({f->p1->position, f->p2->position, f->p3->position}, {center, center + axis_y * len});
            if (res2.intersect) {
                f->draw_read = true;
                six_direction_center_face[2] = f;
            }

            auto res3 = RayIntersectionCalulate_Triangle({f->p1->position, f->p2->position, f->p3->position}, {center, center - axis_y * len});
            if (res3.intersect) {
                f->draw_read = true;
                six_direction_center_face[3] = f;
            }

            auto res4 = RayIntersectionCalulate_Triangle({f->p1->position, f->p2->position, f->p3->position}, {center, center + axis_z * len});
            if (res4.intersect) {
                f->draw_read = true;
                six_direction_center_face[4] = f;
            }

            auto res5 = RayIntersectionCalulate_Triangle({f->p1->position, f->p2->position, f->p3->position}, {center, center - axis_z * len});
            if (res5.intersect) {
                f->draw_read = true;
                six_direction_center_face[5] = f;
            }

        }
    }
    {
        ASSERT(six_direction_center_face[0] != nullptr);
        ASSERT(six_direction_center_face[1] != nullptr);
        ASSERT(six_direction_center_face[2] != nullptr);
        ASSERT(six_direction_center_face[3] != nullptr);
        ASSERT(six_direction_center_face[4] != nullptr);
        ASSERT(six_direction_center_face[5] != nullptr);
    }
    debug_cout("begin grow up");
    uw.phy_group_array.reserve(6);
    uw.phy_group_array[0].boundary_face_array.push_back(six_direction_center_face[0]);
    uw.phy_group_array[1].boundary_face_array.push_back(six_direction_center_face[1]);
    uw.phy_group_array[2].boundary_face_array.push_back(six_direction_center_face[2]);
    uw.phy_group_array[3].boundary_face_array.push_back(six_direction_center_face[3]);
    uw.phy_group_array[4].boundary_face_array.push_back(six_direction_center_face[4]);
    uw.phy_group_array[5].boundary_face_array.push_back(six_direction_center_face[5]);



    //auto grow
    auto physicalGroup_2D_grow = [](PhysicalGroup_2D &phy_group) {

        auto find_boundary_adj_face = [](Face *f, int index) -> Face * {
            for (auto connect_f: *f->disjoin_edge[index]->connect_face_array) {
                if (connect_f->is_boundary_face && connect_f != f)
                    return connect_f;
            }
            return nullptr;
        };
        auto is_face_grow_able = [](Face *f, Face *face_to_grow) {
            ASSERT(Face::is_disjoin_face(f, face_to_grow));
            auto n1 = Face::get_face_normal(f);
            auto n2 = Face::get_face_normal(face_to_grow);
            static const double limit = std::cos(45);
            return fabs(dot(n1, n2)) > limit;
        };

        if (phy_group.boundary_face_array.size() == 0)
            return false;
        std::vector<Face *> new_add_face_array;

        bool new_face_added = false;
        for (auto f: phy_group.boundary_face_array) {
            auto adj_f0 = find_boundary_adj_face(f, 0);
            if (adj_f0->mark == false && is_face_grow_able(f, adj_f0)) {
                new_add_face_array.push_back(adj_f0);
                adj_f0->mark = true;
                new_face_added = true;
            }

            auto adj_f1 = find_boundary_adj_face(f, 1);
            if (adj_f1->mark == false && is_face_grow_able(f, adj_f1)) {
                new_add_face_array.push_back(adj_f1);
                adj_f1->mark = true;
                new_face_added = true;
            }

            auto adj_f2 = find_boundary_adj_face(f, 2);
            if (adj_f2->mark == false && is_face_grow_able(f, adj_f2)) {
                new_add_face_array.push_back(adj_f2);
                adj_f2->mark = true;
                new_face_added = true;
            }


        }

        for (auto f: phy_group.boundary_face_array) {
            phy_group.face_array.push_back(f);
        }
        phy_group.boundary_face_array.clear();

        for (auto f: new_add_face_array) {
            phy_group.boundary_face_array.push_back(f);
        }

        return new_face_added;

    };
    bool do_next = false;
    do {
        debug_cout("do_next Unwrap");
        do_next |= physicalGroup_2D_grow(uw.phy_group_array[0]);
        do_next |= physicalGroup_2D_grow(uw.phy_group_array[1]);
        do_next |= physicalGroup_2D_grow(uw.phy_group_array[2]);
        do_next |= physicalGroup_2D_grow(uw.phy_group_array[3]);
        do_next |= physicalGroup_2D_grow(uw.phy_group_array[4]);
        do_next |= physicalGroup_2D_grow(uw.phy_group_array[5]);

    } while (do_next);


    debug_cout("end Unwrap");
}