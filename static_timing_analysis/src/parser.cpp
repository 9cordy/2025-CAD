#include<iostream>
#include<fstream>
#include<sstream>
#include<array>
#include<vector>
#include<unordered_map>
#include<iomanip>
#include<algorithm> 
#include<cctype>    
#include<regex>
#include"parser.h"
#include"STA.h"
using namespace std;

void get_file_name(int argc,char *argv[],string &netlist_path,string &lib_path,string &pattern_path)
{
    for(int i=1;i<argc;i++)
    {
        string cur(argv[i]);
        if(cur.back()=='v')
            netlist_path=cur;
        else if(cur=="-l")
            lib_path=argv[++i];
        else if(cur=="-i")
            pattern_path=argv[++i];
    }
}

string remove_comments_lib(const string& code)
{
    string clean_code;
    clean_code.reserve(code.size());
    enum State{CODE,SINGLE_LINE_COMMENT,MULTI_LINE_COMMENT};
    State current_state=CODE;
    for(size_t i=0;i<code.size();i++)
    {
        char c=code[i];
        char next_c=(i+1<code.size())?code[i+1]:'\0';
        switch(current_state)
        {
            case CODE:
                if(c=='/') 
                {
                    if(next_c=='*') 
                    {
                        current_state=MULTI_LINE_COMMENT;
                        i++;
                    }
                    else if(next_c=='/') 
                    {
                        current_state=SINGLE_LINE_COMMENT;
                        i++;
                    } 
                    else
                        clean_code+=c;
                }
                else 
                    clean_code+=c;
                break;
            case MULTI_LINE_COMMENT:
                if(c=='*'&&next_c=='/')
                {
                    current_state=CODE;
                    i++;
                }
                break;
            case SINGLE_LINE_COMMENT:
                if(c=='\n')
                {
                    clean_code+=c;
                    current_state=CODE;
                }
                break;
        }
    }
    return clean_code;
}

void extract_doubles(const string& source,vector<double> &target) 
{
    regex float_pattern("[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?"); 
    smatch match;
    string cur_str=source;
    cur_str.erase(remove(cur_str.begin(),cur_str.end(),'\\'),cur_str.end());
    cur_str.erase(remove(cur_str.begin(),cur_str.end(),'"'),cur_str.end());
    while(regex_search(cur_str,match,float_pattern)) 
    {
        target.push_back(stod(match.str()));
        cur_str=match.suffix().str();
    }
}

void get_lib(string lib_path,string &lib_name,Library& lib)
{
    ifstream fin;
    fin.open(lib_path);
    stringstream ss;
    ss<<fin.rdbuf();
    fin.close();
    string original_code=ss.str();
    string clean_code=remove_comments_lib(original_code);
    // cout<<clean_code<<endl;
    stringstream iss(clean_code); 
    string cur;
    smatch match;
    regex lib_name_pattern(R"REGEX(library\s*\(\s*"?([A-Za-z0-9_]+)"?\s*\))REGEX");
    regex lut_pattern(R"REGEX(lu_table_template\s*\(\s*"?([A-Za-z0-9_]+)"?\s*\))REGEX");
    regex cell_pattern("cell\\s*\\(([^\\s\\)]+)\\)");       
    regex pin_pattern("pin\\s*\\(\\s*\"?([A-Za-z0-9_]+)\"?\\s*\\)");       
    regex capacitance_pattern("capacitance\\s*:\\s*([0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?)"); 
    regex table_pattern(R"((cell_rise|cell_fall|rise_transition|fall_transition)\s*\(\s*([A-Za-z0-9_]+)\s*\))");
    regex index_pattern("(index_[12])\\s*\\([\"0-9,\\s\\.-]+\\)");
    while(getline(iss,cur))
    {
        //lib_name
        if(lib_name.empty()&&regex_search(cur,match,lib_name_pattern))
            lib_name=match[1].str();
        //lut & index
        else if(regex_search(cur,match,lut_pattern))
        {
            if(lib.table_1=="")
                lib.table_1=match[1].str();
            else
                lib.table_2=match[1].str();
        }
        else if(regex_search(cur,match,index_pattern))
        {
            string index_name=match[1].str();
            size_t start_pos=cur.find('('); 
            size_t end_pos=cur.find_last_of(')');
            if(start_pos!=string::npos&&end_pos!=string::npos &&end_pos>start_pos)
            {
                string value_str=cur.substr(start_pos+1,end_pos-start_pos-1);
                vector<double> index;
                extract_doubles(value_str,index);
                if(lib.table_2=="")
                {
                    if(index_name=="index_1"&&lib.index_1[lib.table_1].empty())
                        lib.index_1[lib.table_1]=index;
                    else if(index_name=="index_2"&&lib.index_2[lib.table_1].empty())
                        lib.index_2[lib.table_1]=index;
                }
                else
                {
                    if(index_name=="index_1")
                        lib.index_1[lib.table_2]=index;
                    else if(index_name=="index_2")
                        lib.index_2[lib.table_2]=index;
                }
            }
        }
        //cell
        else if(regex_search(cur,match,cell_pattern))
        {
            string curcur;
            Cell_Info* info=new Cell_Info;
            int count_cap=0;
            int count_LUT=0;
            if(cur.find("NAND")!=string::npos)
            {
                while(getline(iss,curcur))
                {
                    if(regex_search(curcur,match,pin_pattern))
                    {
                        string pin=match[1].str();
                        getline(iss,curcur);
                        if(curcur.find("output")!=string::npos)
                        {
                            info->pin_type[2]=pin;
                            continue;
                        }
                        getline(iss,curcur);
                        regex_search(curcur,match,capacitance_pattern);
                        if(count_cap==0)
                        {
                            info->pin_type[0]=pin;
                            info->input_cap[0]=stod(match[1].str());
                        }
                        else 
                        {
                            info->pin_type[1]=pin;
                            info->input_cap[1]=stod(match[1].str());
                        }
                        count_cap++;
                    }
                    else if(regex_search(curcur,match,table_pattern))
                    {
                        count_LUT++;
                        string table=match[1].str();
                        string cur_lut_type=match[2].str();
                        string table_value;
                        while(getline(iss,curcur))
                        {
                            if(curcur.back()=='}')
                                break;
                            table_value+=curcur;
                        }
                        if(table=="cell_rise")
                        {
                            extract_doubles(table_value,info->cell_rise);
                            info->lut_type[0]=cur_lut_type;
                        }
                        else if(table=="cell_fall")
                        {
                            extract_doubles(table_value,info->cell_fall);
                            info->lut_type[1]=cur_lut_type;
                        }
                        else if(table=="rise_transition")
                        {
                            extract_doubles(table_value,info->rise_tran);
                            info->lut_type[2]=cur_lut_type;
                        }
                        else if(table=="fall_transition")
                        {
                            extract_doubles(table_value,info->fall_tran);
                            info->lut_type[3]=cur_lut_type;
                        }
                    }
                    if(count_cap==2&&count_LUT==4)
                        break;
                }
                lib.libs[0]=info;
            }
            else if(cur.find("NOR")!=string::npos)
            {
                while(getline(iss,curcur))
                {
                    if(regex_search(curcur,match,pin_pattern))
                    {
                        string pin=match[1].str();
                        getline(iss,curcur);
                        if(curcur.find("output")!=string::npos)
                        {
                            info->pin_type[2]=pin;
                            continue;
                        }
                        getline(iss,curcur);
                        regex_search(curcur,match,capacitance_pattern);
                        if(count_cap==0)
                        {
                            info->pin_type[0]=pin;
                            info->input_cap[0]=stod(match[1].str());
                        }
                        else 
                        {
                            info->pin_type[1]=pin;
                            info->input_cap[1]=stod(match[1].str());
                        }
                        count_cap++;
                    }
                    else if(regex_search(curcur,match,table_pattern))
                    {
                        count_LUT++;
                        string table=match[1].str();
                        string cur_lut_type=match[2].str();
                        string table_value;
                        while(getline(iss,curcur))
                        {
                            if(curcur.back()=='}')
                                break;
                            table_value+=curcur;
                        }
                        if(table=="cell_rise")
                        {
                            extract_doubles(table_value,info->cell_rise);
                            info->lut_type[0]=cur_lut_type;
                        }
                        else if(table=="cell_fall")
                        {
                            extract_doubles(table_value,info->cell_fall);
                            info->lut_type[1]=cur_lut_type;
                        }
                        else if(table=="rise_transition")
                        {
                            extract_doubles(table_value,info->rise_tran);
                            info->lut_type[2]=cur_lut_type;
                        }
                        else if(table=="fall_transition")
                        {
                            extract_doubles(table_value,info->fall_tran);
                            info->lut_type[3]=cur_lut_type;
                        }
                    }
                    if(count_cap==2&&count_LUT==4)
                        break;
                }
                lib.libs[1]=info;
            }
            else if(cur.find("INV")!=string::npos)
            {
                while(getline(iss,curcur))
                {
                    if(regex_search(curcur,match,pin_pattern))
                    {
                        string pin=match[1].str();
                        getline(iss,curcur);
                        if(curcur.find("output")!=string::npos)
                        {
                            info->pin_type[2]=pin;
                            continue;
                        }
                        getline(iss,curcur);
                        regex_search(curcur,match,capacitance_pattern);
                        info->pin_type[0]=pin;
                        info->input_cap[0]=stod(match[1].str());
                        count_cap++;
                    }
                    else if(regex_search(curcur,match,table_pattern))
                    {
                        count_LUT++;
                        string table=match[1].str();
                        string cur_lut_type=match[2].str();
                        string table_value;
                        while(getline(iss,curcur))
                        {
                            if(curcur.back()=='}')
                                break;
                            table_value+=curcur;
                        }
                        if(table=="cell_rise")
                        {
                            extract_doubles(table_value,info->cell_rise);
                            info->lut_type[0]=cur_lut_type;
                        }
                        else if(table=="cell_fall")
                        {
                            extract_doubles(table_value,info->cell_fall);
                            info->lut_type[1]=cur_lut_type;
                        }
                        else if(table=="rise_transition")
                        {
                            extract_doubles(table_value,info->rise_tran);
                            info->lut_type[2]=cur_lut_type;
                        }
                        else if(table=="fall_transition")
                        {
                            extract_doubles(table_value,info->fall_tran);
                            info->lut_type[3]=cur_lut_type;
                        }
                    }
                    if(count_cap==1&&count_LUT==4)
                        break;
                }
                lib.libs[2]=info;
            }
        }
    }
}

string remove_comments_net(const string& code)
{
    string clean_code;
    clean_code.reserve(code.size());
    enum State{CODE,SINGLE_LINE_COMMENT,MULTI_LINE_COMMENT};
    State current_state=CODE;
    for(size_t i=0;i<code.size();i++)
    {
        char c=code[i];
        char next_c=(i+1<code.size())?code[i+1]:'\0';
        switch(current_state)
        {
            case CODE:
                if(c=='/') 
                {
                    if(next_c=='*') 
                    {
                        current_state=MULTI_LINE_COMMENT;
                        i++;
                    }
                    else if(next_c=='/') 
                    {
                        current_state=SINGLE_LINE_COMMENT;
                        i++;
                    } 
                    else
                        clean_code+=c;
                }
                else if(c=='('||c==')'||c==','||c== '.')
                    clean_code+=" ";
                else if(c==';')
                {
                    clean_code+=' ';
                    clean_code+=c;
                    clean_code+='\n';
                }
                else 
                    clean_code+=c;
                break;
            case MULTI_LINE_COMMENT:
                if(c=='*'&&next_c=='/')
                {
                    current_state=CODE;
                    i++;
                }
                break;
            case SINGLE_LINE_COMMENT:
                if(c=='\n')
                {
                    clean_code+=c;
                    current_state=CODE;
                }
                break;
        }
    }
    string cur;
    stringstream iss(clean_code);
    ostringstream oss;
    while(getline(iss,cur))
    {
        oss<<" "<<cur;
        if(cur.back()==';')
            oss<<endl;
    }
    return oss.str();
}

void get_netlist(string netlist_path,string &netlist_name,int &input_num,unordered_map<string,Net*> &nets,vector<Cell*> &cells,vector<Cell*> &output_cells,Library& lib)
{
    size_t pos=netlist_path.find_last_of("/\\");
    if(pos==string::npos)
        netlist_name=netlist_path;
    else
        netlist_name=netlist_path.substr(pos+1);
    netlist_name.erase(netlist_name.length()-2);
    ifstream fin;
    fin.open(netlist_path);
    stringstream ss;
    ss<<fin.rdbuf();
    fin.close();
    string original_code=ss.str();
    string clean_code=remove_comments_net(original_code);
    // cout<<clean_code<<endl;
    stringstream ss_code(clean_code);
    string cur;
    string op;
    string cell_name,net_name;
    while(getline(ss_code,cur))
    {
        ss.clear();
        ss.str(cur);
        ss>>op;
        if(op[0]=='i'||op[0]=='o'||op[0]=='w')//input, output, wire
        {
            while(ss>>net_name)
            {
                if(net_name==";")
                    break;
                if(op[0]=='i')
                {
                    Net* net=new Net(0,net_name);
                    nets[net_name]=net;
                    input_num++;
                }
                else if(op[0]=='o')
                {
                    Net* net=new Net(1,net_name);
                    nets[net_name]=net;
                }
                else
                {
                    Net* net=new Net(2,net_name);
                    nets[net_name]=net;
                }
            }                 
        }
        else if(op[1]=='A'||op[1]=='O'||op[1]=='N')//NAND, NOR, INV
        {
            int cell_type;
            if(op[1]=='A')
                cell_type=0;
            else if(op[1]=='O')
                cell_type=1;
            else if(op[1]=='N')
                cell_type=2;
            ss>>cell_name;
            Cell* cell=new Cell(cell_type,cell_name);
            array<string,3> pin=lib.libs[cell_type]->pin_type;
            while(ss>>cur)
            {
                if(cur==";")
                    break;
                ss>>net_name;
                Net* net=nets[net_name];
                if(cur==pin[0])
                {   
                    cell->input_net[0]=net;
                    net->output_cell.push_back(cell);
                }
                else if(cur==pin[1])
                {
                    cell->input_net[1]=net;
                    net->output_cell.push_back(cell);
                }
                else if(cur==pin[2])
                {
                    cell->output_net=net;
                    net->input_cell=cell;
                    if(net->type==1)
                        output_cells.push_back(cell);
                }
            }
            cells.push_back(cell);
        }
    }
}

void get_pattern(string pattern_path,int input_num,vector<string> &pattern_order,vector<vector<bool>> &patterns)
{
    ifstream fin;
    fin.open(pattern_path);
    string cur;
    fin>>cur;
    for(int i=0;i<input_num;i++)
    {
        fin>>cur;
        if(cur.back()==',')
            cur.erase(cur.size()-1,1);
        pattern_order.push_back(cur);
    }
    getline(fin,cur);
    stringstream ss;
    while(getline(fin,cur))
    {
        vector<bool> pattern;
        if(cur==".end")
            break;
        ss.clear();
        ss.str(cur);
        for(int i=0;i<input_num;i++)
        {
            bool value;
            ss>>value;
            pattern.push_back(value);
        }
        patterns.push_back(pattern);
        pattern.clear();
    }
}

void print_load(string netlist_name,string lib_name,vector<Cell*> &cells)
{
    ofstream fout;
    fout.open(lib_name+"_"+netlist_name+"_load.txt");
    for(Cell* cell:cells)
        fout<<cell->name<<" "<<fixed<<setprecision(6)<<cell->output_load<<endl;
    fout.close();
}

void print_delay(string netlist_name,string lib_name,vector<Cell*> &cells)
{
    ofstream fout;
    fout.open(lib_name+"_"+netlist_name+"_delay.txt");
    for(Cell* cell:cells)
        fout<<cell->name<<" "<<cell->worst_case<<" "<<fixed<<setprecision(6)<<cell->delay<<" "<<cell->output_tran<<endl;
    fout.close();
}

void print_path(string netlist_name,string lib_name,double &longest_delay,double &shortest_delay,vector<string> &longest_path,vector<string> &shortest_path)
{
    ofstream fout;
    fout.open(lib_name+"_"+netlist_name+"_path.txt");
    fout<<"Longest delay = "<<fixed<<setprecision(6)<<longest_delay<<", the path is: ";
    for(string s:longest_path)
    {
        fout<<s;
        if(s!=longest_path.back())
            fout<<" -> ";
        else
            fout<<endl;
    }
    fout<<"Shortest delay = "<<fixed<<setprecision(6)<<shortest_delay<<", the path is: ";
    for(string s:shortest_path)
    {
        fout<<s;
        if(s!=shortest_path.back())
            fout<<" -> ";
        else
            fout<<endl;
    }
    fout.close();
}

void get_gate_info(stringstream &ss,vector<Cell*> &cells)
{
    for(Cell* cell:cells)
        ss<<cell->name<<" "<<cell->value<<" "<<fixed<<setprecision(6)<<cell->delay<<" "<<cell->output_tran<<endl;
    ss<<endl;
}

void print_gate_info(string netlist_name,string lib_name,stringstream &ss)
{
    ofstream fout;
    fout.open(lib_name+"_"+netlist_name+"_gate_info.txt");
    fout<<ss.str();
    fout.close();
}

void print_netlist(unordered_map<string,Net*> &nets,vector<Cell*> &cells)
{
    for(auto it=nets.begin();it!=nets.end();it++)
    {
        Net* net=it->second;
        if(net->input_cell!=nullptr)
            cout<<net->name<<" "<<net->type<<" "<<net->input_cell->name<<endl;
        else
            cout<<net->name<<" "<<net->type<<" (No Input Cell)"<<endl;
        cout<<"Output Cells: ";
        for(Cell* cell:net->output_cell)
            cout<<cell->name<<" ";
        cout<<endl;
    }
    // cout<<"d";char c;cin>>c;
    for(Cell* cell:cells)
    {
        cout<<cell->name<<" "<<cell->type<<" "<<cell->output_net->name<<endl<<"Input Nets: ";
        if(cell->input_net[0]!=nullptr)
            cout<<cell->input_net[0]->name;
        else
            cout<<"(N/A)";
        if(cell->input_net[1]!=nullptr) 
            cout<<", "<<cell->input_net[1]->name;
        cout<<endl;
    }
}

void print_array(const vector<double>& arr,const string& label) 
{
    cout<<label<<": ";
    for(double v:arr)
        cout<<v<<" ";
    cout<<endl;
}

void print_array49(const vector<double>& arr,const string& label)
{
    cout<<label<<": ";
    for(int i=0;i<49;i++)
    {
        cout<<arr[i]<<" ";
        if(i%7==6)
            cout<<endl;
    }
    cout<<endl;
}

void print_lib(const Library& lib,const string& lib_name)
{
    cout<<"=============================="<<endl;
    cout<<"Library Name: "<<lib_name<<endl;
    cout<<"------------------------------"<<endl;
    for(auto it=lib.index_1.begin();it!=lib.index_1.end();it++)
    {
        cout<<it->first<<endl;
        print_array(it->second,"index_1 (total_output_net_capacitance)");
    }
    for(auto it=lib.index_2.begin();it!=lib.index_2.end();it++)
    {
        cout<<it->first<<endl;
        print_array(it->second,"index_2 (input_transition_time)");
    }
    cout<<"------------------------------"<<endl;
    const char* cell_names[3] = {"NAND","NOR","INV"};
    for (int i = 0; i < 3; i++) 
    {
        cout<<"["<<cell_names[i]<<"]"<<endl;
        if(lib.libs[i]==nullptr) 
        {
            cout<<"(No data found)"<<endl;
            continue;
        }
        const Cell_Info* c=lib.libs[i];
        cout<<"Input Capacitance: "<<c->input_cap[0];
        if(c->input_cap[1]!=0)
            cout<<", "<<c->input_cap[1];
        cout<<endl;

        print_array49(c->cell_rise,"cell_rise");
        print_array49(c->cell_fall,"cell_fall");
        print_array49(c->rise_tran,"rise_tran");
        print_array49(c->fall_tran,"fall_tran");
        cout<<"------------------------------"<<endl;
    }
    cout<<"=============================="<<endl;
}

void print_pattern(vector<string> &pattern_order,vector<vector<bool>> &patterns)
{
    for(string order:pattern_order)
        cout<<order<<" ";
    cout<<endl;
    for(vector<bool> pattern:patterns)
    {
        for(bool b:pattern)
            cout<<b<<" ";
        cout<<endl;
    }
}