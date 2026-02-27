#include<iostream>
#include<fstream>
#include <cstdint>
#include<vector>
#include<unordered_set>
#include<unordered_map>
#include<algorithm>
#include<chrono>
#include"source/parser.h"
#include"source/espresso.h"
using namespace std;
using Clock = std::chrono::high_resolution_clock;

int main(int argc,char *argv[])
{
    auto start=Clock::now(); 
    ifstream fin;
    ofstream fout;
    fin.open(argv[1]);
    unsigned bits;
    vector<unsigned> on_set,dc_set,off_set;
    get_info(fin,bits,on_set,dc_set);
    get_off_set(bits,on_set,dc_set,off_set);
    vector<Implicant> on_binary;
    convert_on_set(bits,on_set,on_binary);
    vector<Implicant> seeds=on_binary;
    Off_Set_Map off_map;
    build_off_map(bits,off_set,off_map);
    //espresso expand
    Implicant expand,pre_expand;
    unordered_set<Implicant,Implicant_Hash> min_cover;
    for(const Implicant& im:seeds) 
    {
        auto now=Clock::now();
        auto elapsed=chrono::duration_cast<chrono::seconds>(now-start).count();
        if(elapsed>=175)
            break;
        expand=im;
        for(int i=0;i<bits;i++) 
        {
            pre_expand=expand;
            expand_cover(i,expand);
            if(cover_off_set(expand,bits,off_map))
                expand=pre_expand;
        }
        min_cover.insert(expand);
    }
    fout.open(argv[2]);
    print_min_cover(fout,bits,min_cover);
    return 0;
}
    //espresso
    // Implicant expand,pre_expand;
    // vector<unsigned> expand_minterms;//decimal form of expanded cover
    // unordered_set<unsigned> covered_minterms;
    // unordered_set<Implicant,Implicant_Hash> min_cover;
    // for(Implicant im:on_binary)
    // {
    //     auto now=Clock::now();
    //     auto elapsed=chrono::duration_cast<chrono::seconds>(now-start).count();
    //     if(elapsed>=175)
    //         break;
    //     expand=im;
    //     for(unsigned i=0;i<bits;i++)
    //     {
    //         pre_expand=expand;
    //         expand_cover(i,bits,expand);
    //         get_expand_cover(bits,expand,expand_minterms);
    //         if(pass_expand_cover(off_set,expand_minterms))//check if cover off-set
    //             covered_minterms.insert(expand_minterms.begin(),expand_minterms.end());
    //         else
    //             expand=pre_expand;
    //         expand_minterms.clear();
    //     }
    //     on_set.erase(remove_if(on_set.begin(),on_set.end(),[&](unsigned m){return covered_minterms.count(m);}),on_set.end());
    //     on_binary.erase(remove_if(on_binary.begin(),on_binary.end(),[&](const Implicant &imp){return imp.care==((1u<<bits)-1)&& covered_minterms.count(imp.value);}),on_binary.end());
    //     min_cover.insert(expand);
    // }
    // min_cover.insert(on_binary.begin(),on_binary.end());
    // fout.open(argv[2]);
    // print_min_cover(fout,bits,min_cover);
    // return 0;
// }