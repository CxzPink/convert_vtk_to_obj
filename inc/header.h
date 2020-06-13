#ifndef _CXZ_HEADER_H_
#define _CXZ_HEADER_H_

#include <string>
#include <vector>

namespace cxz
{
    struct coord
    {
        coord(const double x_,const double y_,const double z_);
        double x,y,z;
    };
    struct Tet
    {
        size_t index[4];
    };
    struct Tri
    {
        size_t index[3]; 
    };
    struct Edge
    {
        size_t index[2];
    };

    //path1 is the address of input vtk, path2 is the address of output obj
    int convert_vtk_to_obj(const std::string &path1,const std::string &path2); 
    // read vtk
    int read_vtk_10(const std::string &path, std::vector<coord> &points, std::vector<Tet> &tets);
    // get the surface of volume mesh 
    int generate_surface(const std::vector<Tet> &tets, std::vector<Tri> &triangles);
    // Delete the extra points
    int reindex_points(const std::vector<coord> &points, const std::vector<Tri> &triangles, std::vector<coord> &new_points, std::vector<Tri> &new_triangles);
    // write to obj
    int write_obj(const std::string &path,const std::vector<coord> &new_points, const std::vector<Tri> &new_triangles);

    bool judge_point_in_tet(const Tet &tet,const size_t &point);
    bool judge_edge_in_tet(const Tet &tet,const Edge &edge);
    bool judge_triangle_in_tet(const Tet &tet,const Tri &tri);
    bool judge_point_in_triangle(const Tri &tri,const size_t &point);
    bool judge_edge_in_triangle(const Tri &tri,const Edge &edge);
    bool judge_boundary_triangle(const std::vector<Tet> &tets,const Tri &tri);
    bool find_new_tri(const std::vector<Tet> &tets,const Edge &edge,const Edge &infor,Tri &tri,Edge &new_infor);
    void update_boundary_edge(std::vector<Edge> &boundary_edges,std::vector<Edge> &boundary_infor,const Tri &tri,const Edge &new_infor);
}

#endif