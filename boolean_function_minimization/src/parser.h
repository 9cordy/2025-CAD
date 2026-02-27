#include<fstream>
#include<sstream>
#include<vector>
#include"espresso.h"
using namespace std;

#ifndef PARSER_H
#define PARSER_H
    void get_info(ifstream& fin,unsigned &bits,vector<unsigned> &on_set,vector<unsigned> &dc_set);
    void get_off_set(unsigned bits,vector<unsigned> &on_set,vector<unsigned> &dc_set,vector<unsigned> &off_set);
    void convert_on_set(unsigned bits,vector<unsigned> &on_set,vector<Implicant> &on_binary);
    void print_min_cover(ofstream &fout,unsigned bits,unordered_set<Implicant,Implicant_Hash> &min_cover);
    void print_info(vector<unsigned> &on_set,vector<unsigned> &dc_set,vector<unsigned> &off_set);
    void print_on_binary(vector<Implicant> &on_binary);
#endif