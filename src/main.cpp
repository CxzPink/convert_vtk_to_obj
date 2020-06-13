#include <iostream>
#include "../inc/header.h"

using namespace cxz;
using namespace std;

int main(int argc,char *argv[])
{
    string s1="sphere.vtk";
    string s2="sphere.obj";
    convert_vtk_to_obj(s1,s2);
    return 0;
}