#ifndef MAX_SW_HPP
#define MAX_SW_HPP

#include <random>
#include <iostream>
#include <vector>

#include <string>
#include <string.h>
#include <sstream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <assert.h>

#include "cadmium/modeling/devs/atomic.hpp"
#include "buyer_registration.hpp"
#include "seller_registration.hpp"
#include "b_reg_info.hpp"
#include "s_reg_info.hpp"

#include <fstream> // to create the output csv file 

using namespace cadmium;
using namespace std;

std::vector<std::vector<int>> create_matrix(const std::vector<b_reg_info> buyer_vector, const std::vector<s_reg_info> seller_vector){
    std::vector<std::vector<int>> binary_matrix;
    binary_matrix.resize(seller_vector.size());
        int flag;
        for (int i=0; i<seller_vector.size(); i++){
            for (int j=1; j<=buyer_vector.back().order_id; j++){
                flag = 0;
                for (int k : seller_vector[i].bundle){
                    if (k==j){
                        flag ++;
                    }
                }
                binary_matrix[i].push_back(flag);
            }
        }
    return binary_matrix;
}           

void parser_auction_data(std::string file_name, const std::vector<b_reg_info> buyer_vector, const std::vector<s_reg_info> seller_vector, const std::vector<std::vector<int>> item_matrix, double maximum_sw){
    std::ofstream outAuctionFile;
    outAuctionFile.open(file_name, std::ios::app);
    if (outAuctionFile.is_open()){
        // Add headers if the file is open
        if (outAuctionFile.tellp() == 0){
            outAuctionFile << "'Instance name:" << std::endl;
        }
        
        string str1, str2;
        str1 = to_string(buyer_vector.size());
        str2 = to_string(seller_vector.size());
        outAuctionFile << "'Instance_" << "C-" << str1 << "_P-" << str2 << "..." << std::endl;
        
        outAuctionFile << "'Instance Statistics " << "..." << std::endl;
        // I'm hardcoding one random number because I think this is not used in the auction DEVS model
        outAuctionFile << 512 << endl;

        outAuctionFile << "'Maximum theoretical social welfare" << endl;
        outAuctionFile << maximum_sw << endl;

        outAuctionFile << "'Customers reservation price" << std::endl;
        for (int i=0; i<buyer_vector.size()-1; i++){
            outAuctionFile << buyer_vector[i].reservation_pr << ";";
        }
        outAuctionFile << buyer_vector.back().reservation_pr << std::endl;;
        
        outAuctionFile << "'Customers initial purchase price" << std::endl;
        for (int i=0; i<buyer_vector.size()-1; i++){
            outAuctionFile << buyer_vector[i].initial_ppr << ";";
        }
        outAuctionFile << buyer_vector.back().initial_ppr << std::endl;

        outAuctionFile << "'Customers purchase price step" << endl;
        outAuctionFile << 5 << endl;
        
        outAuctionFile << "'Producers production cost" << std::endl;
        for (int i=0; i<seller_vector.size()-1; i++){
            outAuctionFile << seller_vector[i].prod_cost << ";";
        }
        outAuctionFile << seller_vector.back().prod_cost << std::endl;
        
        outAuctionFile << "'Producers initial ask price" << std::endl;
        for (int i=0; i<seller_vector.size()-1; i++){
            outAuctionFile << seller_vector[i].initial_apr << ";";
        }
        outAuctionFile << seller_vector.back().initial_apr << std::endl;

        outAuctionFile << "'Producers purchase price step" << endl;
        outAuctionFile << 5 << endl;

        outAuctionFile << "'Item matrix" << std::endl;
        for(int j=0; j<item_matrix.size(); j++){
            for(int k=0; k<item_matrix[0].size()-1; k++){
                outAuctionFile << item_matrix [j][k] << ";";
            }
            outAuctionFile << item_matrix[j].back() << std::endl;
        }

        outAuctionFile.close();
    }
}

struct max_swState {
    //double sigma;
    bool buyers_info_received;
    bool sellers_info_received;
    bool optimal_problem_solved;
    std::vector<b_reg_info> buyer_reg;
    std::vector<s_reg_info> seller_reg;
    vector<vector<int>> matrix;
    double max_theoretical_sw;
    //you can have as many state variables as you want/ need

    explicit max_swState(): buyers_info_received(false), sellers_info_received(false), optimal_problem_solved(false), max_theoretical_sw(0) { //sigma(1)
    }
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const max_swState& state) {
    out  << "{" << state.buyers_info_received << "," << state.sellers_info_received << "," << state.optimal_problem_solved << ",";
    for (b_reg_info item: state.buyer_reg){
        out << item << ",";
    }
    for (s_reg_info item: state.seller_reg){
        out << item << ",";
    }
    out << "}";
    // out << ",{";
    // for (int i=0; i<state.matrix.size(); i++){
    //     out << "[";
    //     for (int j=0; j<state.matrix[0].size()-1; j++){
    //         out << state.matrix[i][j] << ",";
    //     }
    //     out << state.matrix[i].back() << "]";
    //     if (i!=state.matrix.size()-1){
    //         out << ",";
    //     }
    // }
    // out << "}";
    return out;
}
#endif

class max_sw : public Atomic<max_swState> {
    private:
    //vector<vector<int>> matrix;
    //float calculation_timer = 3;

    std::string exec(const char* cmd) const {
        std::array<char, 128> buffer;
        std::string result;
        // std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
        using pipe_deleter = int (*)(FILE*);
        std::unique_ptr<FILE, pipe_deleter> pipe(popen(cmd, "r"), pclose);
        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        return result;
    }
    
    public:
    Port<b_reg_info> buy_in;
    Port<s_reg_info> sell_in;
    Port<bool> scenario_generated; //CAMBIAR A STRING

    //Declare your ports here

    max_sw(const std::string id) : Atomic<max_swState>(id, max_swState()) {
        //Constructor of your atomic model. Initialize ports here.
        buy_in = addInPort<b_reg_info>("buy_in");
        sell_in = addInPort<s_reg_info>("sell_in");
        scenario_generated = addOutPort<bool>("scenario_generated");
    }

    // internal transition
    void internalTransition(max_swState& state) const override {
        //state.sigma += 1;
        if (state.optimal_problem_solved == false){
            if (state.buyers_info_received == true && state.sellers_info_received == true){
                //SOLVE WDP
                //Create string to call the python file
                std::stringstream ss;
                std::string s = "python3 WDP_optimal.py ";
                // Reading matrix
                for (int i=0; i<state.matrix.size(); i++) {
                    if(i != 0)
                        ss << ":";
                    for (int j=0; j<state.matrix[0].size(); j++) {
                        if(j != 0)
                            ss << ",";
                        ss << state.matrix[i][j];
                    }
                }
                ss << " ";

                // Reading purchase prices
                for (int i=0; i<state.buyer_reg.size(); i++) {
                    if(i != 0)
                        ss << ",";
                    ss << state.buyer_reg[i].reservation_pr;
                }
                ss << " ";
                
                //Reading ask prices
                for (int i=0; i<state.seller_reg.size(); i++) {
                    if(i != 0)
                        ss << ",";
                    ss << state.seller_reg[i].prod_cost;
                }

                //After inserting all desired data, you can retrieve the entire content as a single std::string using the str() member function.
                s += ss.str();

                std::cout << s << endl;
                std::string str = s;  

                // Call the python file to run the solver and retrieve its output (and append it to the 'result' string)			
                std::string result = exec(str.c_str());
                cout << "\nRETURN\n";
                cout << result;
                cout << "END\n";
                
                // Find position of ':' using find()
                int pos = result.find(":");
                int pos2 = result.find(";");
                //int pos3 = result.find("Objective value =");

                // Copy substring after pos
                string sub1 = result.substr(pos + 1);
                string sub2 = sub1.substr(1,sub1.size()-1);
                string cons1 = result.substr(pos2+2);
                string cons2 = cons1.substr(0,cons1.size()-sub2.size()-2);
                //string objval1 = result.substr(pos3 + 18);
                //string objval2 = objval1.substr(0,objval1.size()-20);

                //cout << pos3 << endl;
                //string objval1 = result.substr(pos3 + 1);
                //cout << objval1 << endl;
                //string objval2 = objval.substr(17, 19);

                // *** POZA JUNE 2024 ***         

                // Find position of "Objective value = "
                size_t pos3 = result.find("Objective value = ");
                if (pos3 != std::string::npos) {
                // Extract substring containing the value of the objective function
                size_t end_pos = result.find("\n", pos3);
                std::string objective_value_str = result.substr(pos3 + 18, end_pos - pos3 - 18);
                // Convert the substring into a float number
                state.max_theoretical_sw = std::stof(objective_value_str);
                }
                
                // prints the result
                cout << "Variables string is:" << sub2;
                cout << "Constraints string is:" << cons2;
                //cout << "Objective function value: " << objective_function << endl;

                // convert string to vector of integers
                vector<int> variables;
                vector<int> constraints;

                // remove the square brackets at the beggining and end
                sub2 = sub2.substr(1, sub2.size() - 2);
                cons2 = cons2.substr(1, cons2.size() - 2);

                istringstream zz(sub2);
                istringstream xx(cons2);
                string token, token2;

                while(getline(zz, token, ',')) {
                    // convert the token to an integer and add it to the vector
                    variables.push_back(stoi(token));
                }

                // print the vector
                // for (int value : Variables) {
                //     cout << value << " ";
                // }
                // cout << endl;
                //cout << Variables << endl;

                while(getline(xx, token2, ',')) {
                    // convert the token to an integer and add it to the vector
                    constraints.push_back(stoi(token2));
                }

                // print the vector
                // for (int value2 : Constraints) {
                //     cout << value2 << " ";
                // }
                // cout << endl;
                        
                //cout << "Round:" << au.round <<endl;
                // cout << "Objective function value (AR): " << state.surplus << endl;

                state.optimal_problem_solved = true;
                
            }  
            // update sigma here if necessary
        } else {
            state.optimal_problem_solved = false;
            state.buyers_info_received = false;
            state.sellers_info_received = false;
        }
    } 
    // external transition
    void externalTransition(max_swState& state, double e) const override {
        if(!buy_in->empty()){
            for (auto& buyer_local: buy_in->getBag()){
                //std::cout << "buyer_local: " << buyer_loc.buyer_id << std::endl;
                b_reg_info new_buyer_reg (buyer_local.buyer_id, buyer_local.order_id, buyer_local.initial_ppr, buyer_local.reservation_pr);
                state.buyer_reg.push_back(new_buyer_reg);
                //std::cout << "B-" <<new_buyer_reg.buyer_id << ": {" << new_buyer_reg.order_id << "," << new_buyer_reg.initial_ppr << "," << new_buyer_reg.reservation_pr << "}" << std::endl;
                state.buyers_info_received = true;
            }
            
        }
        
        if(!sell_in->empty()){
            for (auto& seller_local: sell_in->getBag()){
                //std::cout << "buyer_local: " << buyer_loc.buyer_id << std::endl;
                s_reg_info new_seller_reg (seller_local.seller_id, seller_local.bundle, seller_local.initial_apr, seller_local.prod_cost);
                state.seller_reg.push_back(new_seller_reg);
            //     //std::cout << "S-" <<new_seller_reg.seller_id << std::endl;
            //     std::cout << "S-" <<new_seller_reg.seller_id << ": {[";
            //     for(int i=0; i<new_seller_reg.bundle.size(); i++){
            //         std::cout << new_seller_reg.bundle[i]; }
            //     std::cout << "]," << new_seller_reg.initial_apr << "," << new_seller_reg.prod_cost << "}" << std::endl;
            }
            state.matrix = create_matrix(state.buyer_reg, state.seller_reg);
            state.sellers_info_received = true;
            // state.activate = true;
        }
        
    }
    
    
    // output function
    void output(const max_swState& state) const override {
        if (state.optimal_problem_solved == true){
            std::string name_file = "data_auction.txt";
            parser_auction_data(name_file, state.buyer_reg, state.seller_reg, state.matrix, state.max_theoretical_sw); //AÑADIR MAX_SW y hardcodear steps
            scenario_generated->addMessage(state.optimal_problem_solved); //CAMBIAR POR NOMBRE DE TXT
        }
    }

    // time_advance function
    [[nodiscard]] double timeAdvance(const max_swState& state) const override {     
        // if(state.optimal_problem_solved == true) {
        if(state.optimal_problem_solved == false and state.buyers_info_received == true and state.sellers_info_received == true) {
            return 0.1;
        } else if (state.optimal_problem_solved == true and state.buyers_info_received == true and state.sellers_info_received == true){
            return 0.1;
        } else {
            return std::numeric_limits<double>::infinity();
        }  
        //return state.sigma;
    }
};

#endif