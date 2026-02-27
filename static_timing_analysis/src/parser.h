#include<iostream>
#include<vector>
#include<unordered_map>
#include"STA.h"
using namespace std;

#ifndef PARSER_H
#define PARSER_H
    void get_file_name(int argc,char *argv[],string &netlist_path,string &lib_path,string &pattern_path);
    string remove_comments_net(const string& code);
    void get_netlist(string netlist_path,string &netlist_name,int &input_num,unordered_map<string,Net*> &nets,vector<Cell*> &cells,vector<Cell*> &output_cells,Library& lib);
    string remove_comments_lib(const string& code);
    void extract_doubles(const string& source,double* target,size_t count);
    void get_lib(string lib_path,string &lib_name,Library& lib);
    void get_pattern(string pattern_path,int input_num,vector<string> &pattern_order,vector<vector<bool>> &patterns);
    void print_load(string netlist_name,string lib_name,vector<Cell*> &cells);
    void print_delay(string netlist_name,string lib_name,vector<Cell*> &cells);
    void print_path(string netlist_name,string lib_name,double &longest_delay,double &shortest_delay,vector<string> &longest_path,vector<string> &shortest_path);
    void get_gate_info(stringstream &ss,vector<Cell*> &cells);
    void print_gate_info(string netlist_name,string lib_name,stringstream &ss);
    void print_netlist(unordered_map<string,Net*> &nets,vector<Cell*> &cells);
    void print_array(const vector<double>& arr,const string& label);
    void print_array49(const vector<double>& arr,const string& label);
    void print_lib(const Library& lib,const string& lib_name);
    void print_pattern(vector<string> &pattern_order,vector<vector<bool>> &patterns);
#endif
