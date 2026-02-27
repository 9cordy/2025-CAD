#include<vector>
#include<unordered_set>
#include<unordered_map>
#include<algorithm>
#include"espresso.h"
using namespace std;

void build_off_map(unsigned bits,vector<unsigned> &off_set,Off_Set_Map &off_map)
{
    const size_t n=(1u<<bits);
    off_map.nbits=bits;
    off_map.bits.assign((n+63)/64,0);
    for(unsigned x:off_set)
    {
        size_t word=x>>6;//x/64
        unsigned bit=x&63;//x%64 
        off_map.bits[word]|=(1ull<<bit); 
    }
}

void expand_cover(int i,Implicant &expand)
{
    unsigned expanding_bit=~(1u<<i);
    expand.care=expand.care&(expanding_bit);
}

inline bool is_off_set(const Off_Set_Map& off_map,unsigned x) 
{
    size_t word=x>>6;//x/64
    unsigned bit=x&63;//x%64 
    return (off_map.bits[word]&(1ull<<bit))!=0;
}

bool cover_off_set(const Implicant& expand, unsigned bits, const Off_Set_Map& off_map)
{
    unsigned free_mask=(~expand.care)&((1u<<bits)-1);
    unsigned base=expand.value&expand.care;
    unsigned subset=free_mask;
    while(1)
    {
        unsigned m=base|subset;
        if(is_off_set(off_map,m))
            return 1;
        if(subset==0)
            break;
        subset=(subset-1)&free_mask; 
    }
    return 0;  
}

// void get_expand_cover(unsigned bits,Implicant &expand,vector<unsigned> &expand_minterms)
// {
//     unsigned free_mask=(~expand.care)&((1u<<bits)-1);
//     unsigned base=expand.value&expand.care;
//     unsigned subset=free_mask;
//     while(1)
//     {
//         expand_minterms.push_back(base|subset);
//         if(subset==0)
//             break;
//         subset=(subset-1)&free_mask;
//     }
// }

// bool pass_expand_cover(vector<unsigned> &off_set,vector<unsigned> &expand_minterms)
// {
//     sort(expand_minterms.begin(),expand_minterms.end());
//     auto it0=off_set.begin();
//     auto it1=expand_minterms.begin();
//     while(it0!=off_set.end()&&it1!=expand_minterms.end())
//     {
//         if(*it0==*it1)
//             return 0;
//         if(*it0<*it1)
//             it0++;
//         else
//             it1++;
//     }
//     return 1;
// }


