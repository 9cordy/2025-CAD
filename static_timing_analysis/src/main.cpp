#include<iostream>
#include<vector>
#include<unordered_map>
#include<iomanip>
#include<algorithm>
#include"parser.h"
#include"STA.h"
using namespace std;

int main(int argc,char *argv[])
{
    string netlist_path,lib_path,pattern_path;
    get_file_name(argc,argv,netlist_path,lib_path,pattern_path);
    string netlist_name,lib_name;
    int input_num=0;
    Library lib;
    get_lib(lib_path,lib_name,lib);
    // print_lib(lib,lib_name);
    unordered_map<string,Net*> nets;
    vector<Cell*> cells,output_cells;
    get_netlist(netlist_path,netlist_name,input_num,nets,cells,output_cells,lib);
    sort(cells.begin(),cells.end(),ascending);
    // print_netlist(nets,cells);
    // return 0;
    vector<string> pattern_order;
    vector<vector<bool>> patterns;
    get_pattern(pattern_path,input_num,pattern_order,patterns);
    // print_pattern(pattern_order,patterns);
    vector<Cell*> sorted_cells;//topological
    bool sorted=topological_sort(nets,cells,sorted_cells);
    if(!sorted)
        sorted_cells=cells;
    //step 1
    calculate_load(sorted_cells,lib);
    print_load(netlist_name,lib_name,cells);
    //step 2
    calculate_propagation_delay(sorted_cells,lib);
    print_delay(netlist_name,lib_name,cells);
    //step 3
    double longest_delay,shortest_delay;
    vector<string> longest_path,shortest_path;
    calculate_path(longest_delay,shortest_delay,longest_path,shortest_path,output_cells);
    print_path(netlist_name,lib_name,longest_delay,shortest_delay,longest_path,shortest_path);
    //step 4
    stringstream ss;
    ss.clear();
    for(vector<bool> pattern:patterns)
    {
        simulate_pattern(input_num,sorted_cells,nets,pattern_order,pattern,lib);
        get_gate_info(ss,cells);
    }
    print_gate_info(netlist_name,lib_name,ss);
    return 0;
}