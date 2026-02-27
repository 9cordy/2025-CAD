#include<sstream>
#include<array>
#include<vector>
#include<unordered_map>
using namespace std;

#ifndef STA_H
#define STA_H
    class Cell_Info//input capacitance, LUTs
    {
        public:
            array<double,2> input_cap;
            array<string,3> pin_type; 
            array<string,4> lut_type;
            unordered_map<string,double> input_capacitance;
            vector<double> cell_rise;//0
            vector<double> cell_fall;//1
            vector<double> rise_tran;//2
            vector<double> fall_tran;//3
            Cell_Info()
            {
                input_cap={0.0,0.0};
                pin_type={"","",""};
            }
    };
    class Library//index, cell type, etc.
    {
        public:
            string table_1;
            string table_2;
            unordered_map<string,vector<double>> index_1;//total_output_net_capacitance
            unordered_map<string,vector<double>> index_2;//input_transition_time
            array<Cell_Info*,3> libs;//0: NAND, 1: NOR, 2: INV
            Library() 
            {
                table_1="";
                table_2="";
                libs.fill(nullptr);
            }
    };
    class Cell;//declare cuz used by Net
    class Net
    {
        public:
            int type;//0: in, 1: out, 2: wire
            bool value;//0/1
            string name;
            Cell* input_cell;
            vector<Cell*> output_cell;
            Net();
            Net(int net_type,string net_name);
            ~Net();
    };
    class Cell
    {
        public:
            int type;//0: NAND, 1: NOR, 2: INV
            int in_degree;
            int num;//U??
            string name;
            double input_tran;
            double output_tran;
            double output_load;
            double delay;
            double arrival_time;
            array<Net*,2> input_net;
            Net* output_net;
            bool worst_case;
            bool value;
            bool controlling_value;
            Cell* prev_cell;
            Cell()
            {
                in_degree=0;
                input_tran=0;
                output_tran=0;
                output_load=0;
                delay=0;
                arrival_time=0;
                input_net[0]=nullptr;
                input_net[1]=nullptr;
                prev_cell=nullptr;
            }
            Cell(int cell_type,string cell_name);
            ~Cell();
    };
    bool ascending(const Cell* const &cell_1,const Cell* const &cell_2);
    bool topological_sort(unordered_map<string,Net*> &nets,vector<Cell*> &cells,vector<Cell*> &sorted_cells);
    void calculate_load(vector<Cell*> &sorted_cells,Library &lib);
    double interpolate(bool ex_1,bool ex_2,const double& cell_1,const double& cell_2,size_t C0,size_t C1,size_t T0,size_t T1,string cur_lut_type,vector<double> &LUT,Library& lib);
    double get_LUT_value(int cell_type,int table_num,const double& cell_index_1,const double& cell_index_2,Library &lib);
    void calculate_input_transition(Cell* &cell);
    void calculate_propagation_delay(vector<Cell*> &sorted_cells,Library &lib);
    void calculate_path(double &longest_delay,double &shortest_delay,vector<string> &longest_path,vector<string> &shortest_path,vector<Cell*> &output_cells);
    void calculate_input_transition_for_simulation(Cell* &cell);
    void simulate_pattern(int &input_num,vector<Cell*> &sorted_cells,unordered_map<string,Net*> &nets,vector<string> &pattern_order,vector<bool> &pattern,Library &lib);
#endif