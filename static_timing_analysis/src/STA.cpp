#include<iostream>
#include<array>
#include<vector>
#include<queue>
#include<unordered_map>
#include<cfloat>
#include<algorithm>
#include"STA.h"
using namespace std;

#define OUTPUT_LOAD 0.03
#define WIRE_DELAY 0.005

Net::Net(int net_type,string net_name)
{
    type=net_type;
    name=net_name;
    input_cell=nullptr;
}

Cell::Cell(int cell_type,string cell_name)
{
    type=cell_type;
    name=cell_name;
    size_t i=cell_name.size();
    while(i>0&&isdigit(cell_name[i-1]))
        i--;
    num=stoi(cell_name.substr(i));
    in_degree=0;
    input_tran=0;
    output_tran=0;
    output_load=0;
    delay=0;
    arrival_time=0;
    input_net[0]=nullptr;
    input_net[1]=nullptr;
    prev_cell=nullptr;
    if(cell_type==0)//NAND
        controlling_value=0;
    else if(cell_type==1)//NOR
        controlling_value=1;
}

bool ascending(const Cell* const &cell_1,const Cell* const &cell_2)
{
    return cell_1->num<cell_2->num;
}

bool topological_sort(unordered_map<string,Net*> &nets,vector<Cell*> &cells,vector<Cell*> &sorted_cells)
{
    //from least in degree to most
    queue<Cell*> q;
    for(Cell* cell:cells)
    {
        for(Net* net:cell->input_net)
        {
            if(net==nullptr)
                break;
            if(net->type!=0)
                cell->in_degree++;
        }
        if(cell->in_degree==0)
        {
            q.push(cell);
            sorted_cells.push_back(cell);
        }
    }
    while(!q.empty())
    {
        Cell* front=q.front();
        q.pop();
        for(Cell* cell:front->output_net->output_cell)
        {
            cell->in_degree--;
            if(cell->in_degree==0)
            {
                q.push(cell);
                sorted_cells.push_back(cell);
            }
        }
    }
    if(sorted_cells.size()!=cells.size())
        return 0;
    return 1;
}

void calculate_load(vector<Cell*> &sorted_cells,Library &lib)
{
    for(Cell* cell:sorted_cells)
    {
        double cell_output_load=0;
        if(cell->output_net->type==1)
            cell_output_load+=OUTPUT_LOAD;//primary output
        for(Cell* out_cell:cell->output_net->output_cell)
        {
            int type=out_cell->type;
            if(type==2)//INV
                cell_output_load+=lib.libs[type]->input_cap[0];
            else//NAND, NOR
            {
                if(cell->output_net->name==out_cell->input_net[0]->name)
                    cell_output_load+=lib.libs[type]->input_cap[0];
                else
                    cell_output_load+=lib.libs[type]->input_cap[1];
            }
        }
        cell->output_load=cell_output_load;
    }
}

double interpolate(bool ex_1,bool ex_2,const double& cell_1,const double& cell_2,size_t C0,size_t C1,size_t T0,size_t T1,string cur_lut_type,vector<double> &LUT,Library& lib)
{
    vector<double> cur_index_1=lib.index_1[cur_lut_type];
    vector<double> cur_index_2=lib.index_2[cur_lut_type];
    size_t INDEX_1=cur_index_1.size();
    double value_1=LUT[T0*INDEX_1+C0];
    double value_2=LUT[T0*INDEX_1+C1];
    double value_3=LUT[T1*INDEX_1+C0];
    double value_4=LUT[T1*INDEX_1+C1];
    if(C0==C1)//value 1 and 3
        value_1=value_1+((cell_2-cur_index_2[T0])*(value_3-value_1)/(cur_index_2[T1]-cur_index_2[T0]));
    else if(T0==T1)//value 1 and 2
        value_1=value_1+((cell_1-cur_index_1[C0])*(value_2-value_1)/(cur_index_1[C1]-cur_index_1[C0]));
    else
    {
        value_1=value_1+((cell_1-cur_index_1[C0])*(value_2-value_1)/(cur_index_1[C1]-cur_index_1[C0]));
        value_3=value_3+((cell_1-cur_index_1[C0])*(value_4-value_3)/(cur_index_1[C1]-cur_index_1[C0]));
        value_1=value_1+((cell_2-cur_index_2[T0])*(value_3-value_1)/(cur_index_2[T1]-cur_index_2[T0]));
    }
    return value_1;
}

double get_LUT_value(int cell_type,int table_num,const double& cell_index_1,const double& cell_index_2,Library &lib)
{
    string cur_lut_type=lib.libs[cell_type]->lut_type[table_num];
    vector<double> cur_index_1=lib.index_1[cur_lut_type];
    vector<double> cur_index_2=lib.index_2[cur_lut_type];
    size_t INDEX_1=cur_index_1.size();
    size_t INDEX_2=cur_index_2.size();
    size_t larger_index_value_1=INDEX_1;
    size_t larger_index_value_2=INDEX_2;
    bool exact_1=0;
    bool exact_2=0;
    bool ex_1=1;//extrapolate
    bool ex_2=1;
    for(size_t i=0;i<INDEX_1;i++)
    {
        if(cur_index_1[i]==cell_index_1)
        {
            larger_index_value_1=i;
            exact_1=1;
            ex_1=0;
            break;
        }
        if(cur_index_1[i]>cell_index_1)
        {
            larger_index_value_1=i;
            ex_1=0;
            break;
        }
    }
    for(size_t i=0;i<INDEX_2;i++)
    {
        if(cur_index_2[i]==cell_index_2)
        {
            larger_index_value_2=i;
            exact_2=1;
            ex_2=0;
            break;
        }
        if(cur_index_2[i]>cell_index_2)
        {
            larger_index_value_2=i;
            ex_2=0;
            break;
        }
    }
    vector<double> LUT;
    Cell_Info* info=lib.libs[cell_type];
    if(table_num==0)
        LUT=info->cell_rise;
    else if(table_num==1)
        LUT=info->cell_fall;
    else if(table_num==2)
        LUT=info->rise_tran;
    else if(table_num==3)
        LUT=info->fall_tran;
    //point in LUT!!
    if(exact_1&&exact_2)
        return LUT[larger_index_value_2*INDEX_1+larger_index_value_1];
    //no point in LUT :(
    size_t C0,C1,T0,T1;
    if(exact_1)
    {
        C0=larger_index_value_1;
        C1=larger_index_value_1;
    }
    else if(larger_index_value_1==0)
    {
        C0=0;
        C1=1;
    }
    else if(larger_index_value_1==INDEX_1)
    {
        C0=INDEX_1-2;
        C1=INDEX_1-1;
    }
    else
    {
        C0=larger_index_value_1-1;
        C1=larger_index_value_1;
    }
    if(exact_2)
    {
        C0=larger_index_value_2;
        C1=larger_index_value_2;
    }
    else if(larger_index_value_2==0)
    {
        T0=0;
        T1=1;
    }
    else if(larger_index_value_2==INDEX_2&&!exact_2)
    {
        T0=INDEX_2-2;
        T1=INDEX_2-1;
    }
    else
    {
        T0=larger_index_value_2-1;
        T1=larger_index_value_2;
    }
    return interpolate(ex_1,ex_2,cell_index_1,cell_index_2,C0,C1,T0,T1,cur_lut_type,LUT,lib);
}

void calculate_input_transition(Cell* &cell)
{
    if(cell->type==2)//INV
    {
        Net* net=cell->input_net[0];
        if(net->type==0)
        {
            cell->input_tran=0;
            cell->arrival_time=0;
        }
        else
        {
            Cell* prev=net->input_cell;
            cell->input_tran=prev->output_tran;
            cell->arrival_time=prev->arrival_time+prev->delay+WIRE_DELAY;
            cell->prev_cell=prev;
        }
    }
    else//NAND, NOR
    {
        Net* net_0=cell->input_net[0];
        Net* net_1=cell->input_net[1];
        if(net_0->type==0&&net_1->type==0)//both primary input
        {
            cell->input_tran=0;
            cell->arrival_time=0;
        }
        else if(net_0->type==0)//net 0 is primary input
        {
            Cell* prev=net_1->input_cell;
            cell->input_tran=prev->output_tran;
            cell->arrival_time=prev->arrival_time+prev->delay+WIRE_DELAY;
            cell->prev_cell=prev;
        }
        else if(net_1->type==0)//net 1 is primary input
        {
            Cell* prev=net_0->input_cell;
            cell->input_tran=prev->output_tran;
            cell->arrival_time=prev->arrival_time+prev->delay+WIRE_DELAY;
            cell->prev_cell=prev;
        }
        else
        {
            Cell* prev_0=net_0->input_cell;
            Cell* prev_1=net_1->input_cell;
            double arrivel_time_0=prev_0->arrival_time+prev_0->delay+WIRE_DELAY;
            double arrivel_time_1=prev_1->arrival_time+prev_1->delay+WIRE_DELAY;
            if(arrivel_time_0>arrivel_time_1)
            {
                cell->input_tran=prev_0->output_tran;
                cell->arrival_time=arrivel_time_0;
                cell->prev_cell=prev_0;
            }
            else
            {
                cell->input_tran=prev_1->output_tran;
                cell->arrival_time=arrivel_time_1;
                cell->prev_cell=prev_1;
            }
        }
    }
}

void calculate_propagation_delay(vector<Cell*> &sorted_cells,Library &lib)
{
    for(Cell* cell:sorted_cells)
    {
        calculate_input_transition(cell);
        //get output tran of primary input cells to calcuulate input tran of other cells
        double rise_delay=get_LUT_value(cell->type,0,cell->output_load,cell->input_tran,lib);
        double fall_delay=get_LUT_value(cell->type,1,cell->output_load,cell->input_tran,lib);
        if(rise_delay>fall_delay)
        {
            cell->worst_case=1;//output=1, output transition time is defined by rising time
            cell->delay=rise_delay;
            cell->output_tran=get_LUT_value(cell->type,2,cell->output_load,cell->input_tran,lib);
        }
        else
        {
            cell->worst_case=0;//output=1, output transition time is defined by falling time
            cell->delay=fall_delay;
            cell->output_tran=get_LUT_value(cell->type,3,cell->output_load,cell->input_tran,lib);
        }
    }
}

void calculate_path(double &longest_delay,double &shortest_delay,vector<string> &longest_path,vector<string> &shortest_path,vector<Cell*> &output_cells)
{
    longest_delay=DBL_MIN;
    shortest_delay=DBL_MAX;
    Cell* longest_cell=nullptr;
    Cell* shortest_cell=nullptr;
    for(Cell* cell:output_cells)
    {
        double current_delay=cell->arrival_time+cell->delay;
        if(current_delay>longest_delay)
        {
            longest_delay=current_delay;
            longest_cell=cell;
        }
        if(current_delay<shortest_delay)
        {
            shortest_delay=current_delay;
            shortest_cell=cell;
        }
    }
    //get longest path
    while(longest_cell!=nullptr)
    {
        longest_path.push_back(longest_cell->output_net->name);
        if(longest_cell->prev_cell==nullptr)
            longest_path.push_back(longest_cell->input_net[0]->name);
        longest_cell=longest_cell->prev_cell;
    }
    //get shortest path
    while(shortest_cell!=nullptr)
    {
        shortest_path.push_back(shortest_cell->output_net->name);
        if(shortest_cell->prev_cell==nullptr)
            shortest_path.push_back(shortest_cell->input_net[0]->name);
        shortest_cell=shortest_cell->prev_cell;
    }
    reverse(longest_path.begin(),longest_path.end());
    reverse(shortest_path.begin(),shortest_path.end());
}

void calculate_input_transition_for_simulation(Cell* &cell)
{
    if(cell->type==2)
    {
        Net* net=cell->input_net[0];
        if(net->type==0)
        {
            cell->input_tran=0;
            cell->arrival_time=0;
        }
        else
        {
            Cell* prev=net->input_cell;
            cell->input_tran=prev->output_tran;
            cell->arrival_time=prev->arrival_time+prev->delay+WIRE_DELAY;
        }
        cell->value=(!net->value);
    }
    else
    {
        Net* net_0=cell->input_net[0];
        Net* net_1=cell->input_net[1];
        if(net_0->type==0&&net_1->type==0)//both primary input
        {
            cell->input_tran=0;
            cell->arrival_time=0;
        }
        else if(net_0->type==0)//net 0 is primary input
        {
            if(net_0->value==cell->controlling_value)
            {
                cell->input_tran=0;
                cell->arrival_time=0;
            }
            else//if primary input is not controlling value, it has to wait
            {
                Cell* prev=net_1->input_cell;
                cell->input_tran=prev->output_tran;
                cell->arrival_time=prev->arrival_time+prev->delay+WIRE_DELAY;
            }
        }
        else if(net_1->type==0)//net 1 is primary input
        {
            if(net_1->value==cell->controlling_value)
            {
                cell->input_tran=0;
                cell->arrival_time=0;
            }
            else
            {
                Cell* prev=net_0->input_cell;
                cell->input_tran=prev->output_tran;
                cell->arrival_time=prev->arrival_time+prev->delay+WIRE_DELAY;
            }
        }
        else
        {
            if(net_0->value==cell->controlling_value&&net_1->value==cell->controlling_value)
            {
                Cell* prev_0=net_0->input_cell;
                Cell* prev_1=net_1->input_cell;
                double arrivel_time_0=prev_0->arrival_time+prev_0->delay+WIRE_DELAY;
                double arrivel_time_1=prev_1->arrival_time+prev_1->delay+WIRE_DELAY;
                if(arrivel_time_0<arrivel_time_1)//get faster
                {
                    cell->input_tran=prev_0->output_tran;
                    cell->arrival_time=arrivel_time_0;
                }
                else
                {
                    cell->input_tran=prev_1->output_tran;
                    cell->arrival_time=arrivel_time_1;
                }
            }
            else if(net_0->value==cell->controlling_value)
            {
                Cell* prev=net_0->input_cell;
                cell->input_tran=prev->output_tran;
                cell->arrival_time=prev->arrival_time+prev->delay+WIRE_DELAY;
            }
            else if(net_1->value==cell->controlling_value)
            {
                Cell* prev=net_1->input_cell;
                cell->input_tran=prev->output_tran;
                cell->arrival_time=prev->arrival_time+prev->delay+WIRE_DELAY;
            }
            else
            {
                Cell* prev_0=net_0->input_cell;
                Cell* prev_1=net_1->input_cell;
                double arrivel_time_0=prev_0->arrival_time+prev_0->delay+WIRE_DELAY;
                double arrivel_time_1=prev_1->arrival_time+prev_1->delay+WIRE_DELAY;
                if(arrivel_time_0>arrivel_time_1)//get slower
                {
                    cell->input_tran=prev_0->output_tran;
                    cell->arrival_time=arrivel_time_0;
                }
                else
                {
                    cell->input_tran=prev_1->output_tran;
                    cell->arrival_time=arrivel_time_1;
                }
            }
        }
        if(cell->type==0)//NAND
            cell->value=!(net_0->value&&net_1->value);
        else if(cell->type==1)
            cell->value=!(net_0->value||net_1->value);
    }
}

void simulate_pattern(int &input_num,vector<Cell*> &sorted_cells,unordered_map<string,Net*> &nets,vector<string> &pattern_order,vector<bool> &pattern,Library &lib)
{
    for(int i=0;i<input_num;i++)
        nets[pattern_order[i]]->value=pattern[i];
    for(Cell* cell:sorted_cells)
    {
        calculate_input_transition_for_simulation(cell);
        if(cell->value==1)
        {
            cell->delay=get_LUT_value(cell->type,0,cell->output_load,cell->input_tran,lib);
            cell->output_tran=get_LUT_value(cell->type,2,cell->output_load,cell->input_tran,lib);
            cell->output_net->value=1;
        }
        else
        {
            cell->delay=get_LUT_value(cell->type,1,cell->output_load,cell->input_tran,lib);
            cell->output_tran=get_LUT_value(cell->type,3,cell->output_load,cell->input_tran,lib);            
            cell->output_net->value=0;
        }
    }
}