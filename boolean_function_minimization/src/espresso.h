#include <cstdint>
#include<vector>
#include<unordered_set>
#include<unordered_map>
#include<algorithm>
using namespace std;

#ifndef ESPRESSO_H
#define ESPRESSO_H
    struct Implicant
    {
        unsigned value;
        unsigned care;//1--01 =>10011
        bool operator==(const Implicant &o) const noexcept
        {
            return care==o.care&&(value&care)==(o.value&o.care);
        }
    };
    struct Implicant_Hash 
    {
        size_t operator()(const Implicant &x) const noexcept
        {
            unsigned maskedValue=x.value&x.care;
            return hash<unsigned>()(maskedValue)^(hash<unsigned>()(x.care)<<1);
        }
    };
    struct Off_Set_Map
    {
        vector<uint64_t> bits;
        unsigned nbits=0;
    };
    void build_off_map(unsigned bits,vector<unsigned> &off_set,Off_Set_Map &off_map);
    void expand_cover(int i,Implicant &expand);
    inline bool is_off_set(const Off_Set_Map& off_map, unsigned x);
    bool cover_off_set(const Implicant& c, unsigned bits, const Off_Set_Map& off_map);
    // void get_expand_cover(unsigned bits,Implicant &expand,vector<unsigned> &expand_minterms);
    // bool pass_expand_cover(vector<unsigned> &off_set,vector<unsigned> &expand_minterms);
#endif