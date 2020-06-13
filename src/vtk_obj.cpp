#include <fstream>
#include <sstream>
#include "../inc/header.h"

using namespace cxz;
using namespace std;

int cxz::convert_vtk_to_obj(const std::string &path1,const std::string &path2)
{
    std::vector<Tet> tets;
    std::vector<coord> points,new_points;
    std::vector<Tri> triangles,new_triangles;
    if(read_vtk_10(path1,points,tets)==1)
        return 1;
    if(generate_surface(tets,triangles)==1)
        return 1;
    if(reindex_points(points,triangles,new_points,new_triangles)==1)
        return 1;
    write_obj(path2,new_points,new_triangles);
    return 0;
}

int cxz::read_vtk_10(const std::string &path, std::vector<coord> &points, std::vector<Tet> &tets)
{
    points.clear();
    tets.clear();

	ifstream file(path);
	string line,temp_string;
    coord my_points(0,0,0);
    Tet tet;
    size_t type=0;
    size_t number=0;
    size_t temp_tet=0;

	while (getline(file, line)) {
		istringstream in(line);		
		in >> temp_string;
		if (temp_string == "POINTS") {
			in >> number;
			for (size_t i = 0; i < number; i++) {
				getline(file, line);
				in.clear();
				in.str(line);
				in >> my_points.x >> my_points.y >> my_points.z;
				points.push_back(my_points);
			}
		}
		if (temp_string == "CELLS") {
			in >> number;
			for (size_t i = 0; i < number; i++) {
				getline(file, line);
				in.clear();
				in.str(line);
				in >> temp_tet;
                if(temp_tet==4){
                    for(size_t i=0;i<4;i++){
                        in>>tet.index[i];
                    }
                    tets.push_back(tet);
                }
                else
                    return 1;
			}
		}
		if (temp_string == "CELL_TYPES") {
			in >> number;
			for (size_t i = 0; i < number; i++) {
				getline(file, line);
				in.clear();
				in.str(line);
				in >> type;
                if(type != 10)
                    return 1;
			}
		}
	}
	return 0;
}

int cxz::generate_surface(const std::vector<Tet> &tets, std::vector<Tri> &triangles)
{
    triangles.clear();
    size_t count=0;
    size_t init_tet_index,init_face_index;
    size_t tet_number=tets.size();
    bool flag=0;
    Tri temp_tri;
    Edge temp_edge;
    Edge temp_infor,new_infor;
    std::vector<Edge> boundary_edges;
    std::vector<Edge> boundary_infor;

    // Randomly find a triangle on the surface
    while(count<tet_number){
        init_tet_index=rand()%tet_number;
        count++;
        for(init_face_index=0;init_face_index<4;init_face_index++){
            flag=0;
            temp_tri.index[0]=tets[init_tet_index].index[(init_face_index+0)%4];
            temp_tri.index[1]=tets[init_tet_index].index[(init_face_index+1)%4];
            temp_tri.index[2]=tets[init_tet_index].index[(init_face_index+2)%4];
            if(judge_boundary_triangle(tets,temp_tri)){
                flag=1;
                break;
            }
        }
        if(flag==1)
            break;
    }
    if((init_tet_index==tet_number && init_face_index==4) || count==tet_number)
        return 1;    
    temp_infor.index[0]=init_tet_index;
    temp_infor.index[1]=init_face_index;    
    triangles.push_back(temp_tri);
    update_boundary_edge(boundary_edges,boundary_infor,temp_tri,temp_infor);

    // Extend the surface along the boundary edges
    size_t boundary_edge_number=boundary_edges.size();
    while(boundary_edge_number!=0){
        temp_edge=boundary_edges[boundary_edge_number-1];
        temp_infor=boundary_infor[boundary_edge_number-1];
        if(find_new_tri(tets,temp_edge,temp_infor,temp_tri,new_infor)){
            triangles.push_back(temp_tri);
            update_boundary_edge(boundary_edges,boundary_infor,temp_tri,new_infor);
        }
        else{
            boundary_edges.pop_back();
            boundary_infor.pop_back();
        }
        boundary_edge_number=boundary_edges.size();
    }
    return 0;
}

int cxz::reindex_points(const std::vector<coord> &points, const std::vector<Tri> &triangles, std::vector<coord> &new_points, std::vector<Tri> &new_triangles)
{
    new_points.clear();
    new_triangles.clear();
    
    size_t points_number=points.size();
    std::vector<size_t> index_change;
    std::vector<size_t> new_points_index;
    size_t temp;
    bool flag=0;
    Tri temp_tri;

    for(size_t i=0;i<points_number;i++){
        index_change.push_back(0);
    }
    for(auto i:triangles){
        for(size_t j=0;j<3;j++){
            temp=i.index[j];
            flag=0;
            for(auto k:new_points_index){
                if(k==temp){
                    flag=1;
                    break;
                }
            }
            if(flag==0){
                index_change[temp]=new_points_index.size();
                new_points_index.push_back(temp);
            }
        }
    }

    for(auto i:new_points_index){
        new_points.push_back(points[i]);
    }
    for(auto j:triangles){
        temp_tri.index[0]=index_change[j.index[0]];
        temp_tri.index[1]=index_change[j.index[1]];
        temp_tri.index[2]=index_change[j.index[2]]; 
        new_triangles.push_back(temp_tri);               
    }
    return 0;
}

int cxz::write_obj(const std::string &path,const std::vector<coord> &new_points, const std::vector<Tri> &new_triangles)
{
    ofstream file;
	file.open(path);
    for (auto i : new_points)
		file <<"v "<< i.x << " " <<  i.y << " " <<  i.z << endl;
	for (auto i : new_triangles) 
		file <<"f "<< i.index[0]+1 << " " <<  i.index[1]+1 << " " <<  i.index[2]+1 << endl;
	file.close();
    return 0;
}


cxz::coord::coord(const double x_,const double y_,const double z_)
{
    x=x_;
    y=y_;
    z=z_;
}

bool cxz::judge_point_in_tet(const Tet &tet,const size_t &point)
{
    if(point==tet.index[0] || point==tet.index[1] || point==tet.index[2] || point==tet.index[3])
        return 1;
    return 0;
}

bool cxz::judge_edge_in_tet(const Tet &tet,const Edge &edge)
{
    if(judge_point_in_tet(tet,edge.index[0]) && judge_point_in_tet(tet,edge.index[1]))
        return 1;
    return 0;
}

bool cxz::judge_triangle_in_tet(const Tet &tet,const Tri &tri)
{
    if(judge_point_in_tet(tet,tri.index[0]) && judge_point_in_tet(tet,tri.index[1]) && judge_point_in_tet(tet,tri.index[2]))
        return 1;
    return 0;
}

bool cxz::judge_point_in_triangle(const Tri &tri,const size_t &point)
{
    if(point==tri.index[0] || point==tri.index[1] || point==tri.index[2])
        return 1;
    return 0;
}

bool cxz::judge_edge_in_triangle(const Tri &tri,const Edge &edge)
{
    if(judge_point_in_triangle(tri,edge.index[0]) && judge_point_in_triangle(tri,edge.index[1]))
        return 1;
    return 0; 
}

bool cxz::judge_boundary_triangle(const std::vector<Tet> &tets,const Tri &tri)
{
    size_t count=0;
    for(auto i:tets){
        if(judge_triangle_in_tet(i,tri))
           count++;
    }
    if(count==1)
        return 1;
    return 0;
}

bool cxz::find_new_tri(const std::vector<Tet> &tets,const Edge &edge,const Edge &infor,Tri &tri,Edge &new_infor)
{
    size_t number=tets.size();
    Tri temp_tri;
    for(size_t i=0;i<number;i++){
        if(judge_edge_in_tet(tets[i],edge)){
            for(size_t j=0;j<4;j++){
                if((i!=infor.index[0] || j!=infor.index[1]) && tets[i].index[(j+3)%4]!=edge.index[0] && tets[i].index[(j+3)%4]!=edge.index[1]){
                    temp_tri.index[0]=tets[i].index[(j+0)%4];
                    temp_tri.index[1]=tets[i].index[(j+1)%4];
                    temp_tri.index[2]=tets[i].index[(j+2)%4];
                    if(judge_boundary_triangle(tets,temp_tri)){
                        tri=temp_tri;
                        new_infor.index[0]=i;
                        new_infor.index[1]=j;
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

void cxz::update_boundary_edge(std::vector<Edge> &boundary_edges,std::vector<Edge> &boundary_infor,const Tri &tri,const Edge &new_infor)
{
    std::vector<Edge> new_edges;
    std::vector<Edge> new_boundary_infor;
    Edge temp_edge;
    size_t edge_number=boundary_edges.size();
    bool flag;

    for(size_t i=0;i<edge_number;i++){
        if(!judge_edge_in_triangle(tri,boundary_edges[i])){
            new_edges.push_back(boundary_edges[i]);
            new_boundary_infor.push_back(boundary_infor[i]);
        }
    }

    for(size_t i=0;i<3;i++){
        temp_edge.index[0]=tri.index[i%3];
        temp_edge.index[1]=tri.index[(i+1)%3];
        flag=0;
        for(auto j:boundary_edges){
            if((j.index[0]==temp_edge.index[0] && j.index[1]==temp_edge.index[1]) || (j.index[1]==temp_edge.index[0] && j.index[0]==temp_edge.index[1])){
                flag=1;
                break;
            }
        }
        if(flag==0){
            new_edges.push_back(temp_edge);
            new_boundary_infor.push_back(new_infor);
        }
    }
    boundary_edges=new_edges;
    boundary_infor=new_boundary_infor;
}