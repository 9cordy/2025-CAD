#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include"parser.h"
using namespace std;

void get_info(ifstream& fin,unsigned &bits,vector<unsigned> &on_set,vector<unsigned> &dc_set)
{
    unsigned n;
    string s;
    stringstream ss;
    //bits
    getline(fin,s);
    ss.str(s);
    ss>>bits;
    ss.clear();
    //on-set
    getline(fin,s);
    ss.str(s);
    while(ss>>n)
        on_set.push_back(n);
    ss.clear();
    //dc-set
    getline(fin,s);
    ss.str(s);
    while(ss>>n)
        dc_set.push_back(n);
    ss.clear();
}

void get_off_set(unsigned bits,vector<unsigned> &on_set,vector<unsigned> &dc_set,vector<unsigned> &off_set)
{
    unsigned total=1<<bits;
    vector<bool> used(total,0);
    for(unsigned n:on_set)
        used[n]=1;
    for(unsigned n:dc_set)
        used[n]=1;
    for(int i=0;i<total;i++)
        if(!used[i])
            off_set.push_back(i);
}

void convert_on_set(unsigned bits,vector<unsigned> &on_set,vector<Implicant> &on_binary)
{
    for(auto it:on_set)
    {
        Implicant i;
        i.value=it;
        i.care=(1u<<(bits))-1;
        on_binary.push_back(i);
    }
}

void print_min_cover(ofstream &fout,unsigned bits,unordered_set<Implicant,Implicant_Hash> &min_cover)
{
    for(Implicant im:min_cover)
    {
        for(int i=bits-1;i>=0;i--) 
        {
            unsigned mask=1u<<i;
            if(im.care&mask)
            {
                if(im.value&mask)
                    fout<<'1';
                else
                    fout<<'0';
            }
            else
                fout<<'-';
        }
        fout<<endl;
    }
}

void print_info(vector<unsigned> &on_set,vector<unsigned> &dc_set,vector<unsigned> &off_set)
{
    for(int i=0;i<on_set.size();i++)
        cout<<on_set[i]<<" ";
    cout<<endl;
    for(int i=0;i<dc_set.size();i++)
        cout<<dc_set[i]<<" ";
    cout<<endl;
    for(int i=0;i<off_set.size();i++)
        cout<<off_set[i]<<" ";
    cout<<endl;
}

void print_on_binary(vector<Implicant> &on_binary)
{
    for(auto it:on_binary)
        cout<<it.value<<" "<<it.care<<endl;
}
