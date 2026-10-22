#ifndef AUCTION_SETUP_HPP
#define AUCTION_SETUP_HPP

#include <random>
#include <iostream>
#include <vector>
#include "cadmium/modeling/devs/atomic.hpp"
#include "buyer_registration.hpp"
#include "seller_registration.hpp"
#include "b_reg_info.hpp"
#include "s_reg_info.hpp"

#include <fstream> // to create the output csv file 

using namespace cadmium;

// std::vector<std::vector<int>> create_matrix(const std::vector<b_reg_info> buyer_vector, const std::vector<s_reg_info> seller_vector){
//     std::vector<std::vector<int>> binary_matrix;
//     binary_matrix.resize(seller_vector.size());
//         int flag;
//         for (int i=0; i<seller_vector.size(); i++){
//             for (int j=1; j<=buyer_vector.back().order_id; j++){
//                 flag = 0;
//                 for (int k : seller_vector[i].bundle){
//                     if (k==j){
//                         flag ++;
//                     }
//                 }
//                 binary_matrix[i].push_back(flag);
//             }
//         }
//     return binary_matrix;
// }           

// void parser_auction_data(const std::vector<b_reg_info> buyer_vector, const std::vector<s_reg_info> seller_vector, const std::vector<std::vector<int>> item_matrix){
//     std::ofstream outAuctionFile;
//     outAuctionFile.open("data_scenario.txt", std::ios::app);
//     if (outAuctionFile.is_open()){
//         // Add headers if the file is open
//         if (outAuctionFile.tellp() == 0){
//             outAuctionFile << "'Customers reservation price" << std::endl;
//         }
//         for (int i=0; i<buyer_vector.size()-1; i++){
//             outAuctionFile << buyer_vector[i].reservation_pr << ";";
//         }
//         outAuctionFile << buyer_vector.back().reservation_pr << std::endl;;
        
//         outAuctionFile << "'Customers initial purchase price" << std::endl;
//         for (int i=0; i<buyer_vector.size()-1; i++){
//             outAuctionFile << buyer_vector[i].initial_ppr << ";";
//         }
//         outAuctionFile << buyer_vector.back().initial_ppr << std::endl;
        
//         outAuctionFile << "'Producers production cost" << std::endl;
//         for (int i=0; i<seller_vector.size()-1; i++){
//             outAuctionFile << seller_vector[i].prod_cost << ";";
//         }
//         outAuctionFile << seller_vector.back().prod_cost << std::endl;
        
//         outAuctionFile << "'Producers initial ask price" << std::endl;
//         for (int i=0; i<seller_vector.size()-1; i++){
//             outAuctionFile << seller_vector[i].initial_apr << ";";
//         }
//         outAuctionFile << seller_vector.back().initial_apr << std::endl;

//         outAuctionFile << "'Item matrix" << std::endl;
//         for(int j=0; j<item_matrix.size(); j++){
//             for(int k=0; k<item_matrix[0].size()-1; k++){
//                 outAuctionFile << item_matrix [j][k] << ";";
//             }
//             outAuctionFile << item_matrix[j].back() << std::endl;
//         }

//         outAuctionFile.close();
//     }
// }

struct auction_setupState {
    //double sigma;
    bool activate;
    std::vector<b_reg_info> buyer_reg;
    std::vector<s_reg_info> seller_reg;
    //you can have as many state variables as you want/ need

    explicit auction_setupState(): activate(false) { //sigma(1)
    }
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const auction_setupState& state) {
    out  << "{" << state.activate << ",";
    for (b_reg_info item: state.buyer_reg){
        out << item << ",";
    }
    for (s_reg_info item: state.seller_reg){
        out << item << ",";
    }
    out << "}";
    return out;
}
#endif

class auction_setup : public Atomic<auction_setupState> {
    public:
    Port<b_reg_info> buy_in;
    Port<s_reg_info> sell_in;
    //Port<bool> scenario_generated;
    Port<b_reg_info> buy_out;
    Port<s_reg_info> sell_out;

    //Declare your ports here

    auction_setup(const std::string id) : Atomic<auction_setupState>(id, auction_setupState()) {
        //Constructor of your atomic model. Initialize ports here.
        buy_in = addInPort<b_reg_info>("buy_in");
        sell_in = addInPort<s_reg_info>("sell_in");
        //scenario_generated = addOutPort<bool>("scenario_generated");
        buy_out = addOutPort<b_reg_info>("buy_out");
        sell_out = addOutPort<s_reg_info>("sell_out");
    }

    // internal transition
    void internalTransition(auction_setupState& state) const override {
        //state.sigma += 1;
        if (state.activate == true){
            state.activate = false;
        }
    }

    // external transition
    void externalTransition(auction_setupState& state, double e) const override {
        if(!buy_in->empty()){
            for (auto& buyer_local: buy_in->getBag()){
                //std::cout << "buyer_local: " << buyer_loc.buyer_id << std::endl;
                b_reg_info new_buyer_reg (buyer_local.buyer_id, buyer_local.order_id, buyer_local.initial_ppr, buyer_local.reservation_pr);
                state.buyer_reg.push_back(new_buyer_reg);
                //std::cout << "B-" <<new_buyer_reg.buyer_id << ": {" << new_buyer_reg.order_id << "," << new_buyer_reg.initial_ppr << "," << new_buyer_reg.reservation_pr << "}" << std::endl;
            }
            
        }
        
        if(!sell_in->empty()){
            for (auto& seller_local: sell_in->getBag()){
                //std::cout << "buyer_local: " << buyer_loc.buyer_id << std::endl;
                s_reg_info new_seller_reg (seller_local.seller_id, seller_local.bundle, seller_local.initial_apr, seller_local.prod_cost);
                state.seller_reg.push_back(new_seller_reg);
                //std::cout << "S-" <<new_seller_reg.seller_id << std::endl;
                std::cout << "S-" <<new_seller_reg.seller_id << ": {[";
                for(int i=0; i<new_seller_reg.bundle.size(); i++){
                    std::cout << new_seller_reg.bundle[i]; }
                std::cout << "]," << new_seller_reg.initial_apr << "," << new_seller_reg.prod_cost << "}" << std::endl;
            }
            state.activate = true;
        }
        
    }
    
    
    // output function
    void output(const auction_setupState& state) const override {
        if (state.activate == true){
            for(int i=0; i<state.buyer_reg.size(); i++){
                b_reg_info out_buyer_message = {state.buyer_reg[i].buyer_id, state.buyer_reg[i].order_id, state.buyer_reg[i].initial_ppr, state.buyer_reg[i].reservation_pr};
                buy_out->addMessage(out_buyer_message);
            }

            for(int i=0; i<state.seller_reg.size(); i++){
                s_reg_info out_seller_message = {state.seller_reg[i].seller_id, state.seller_reg[i].bundle, state.seller_reg[i].initial_apr, state.seller_reg[i].prod_cost};
                sell_out->addMessage(out_seller_message);
            }

            // std::vector<std::vector<int>> matrix = create_matrix(state.buyer_reg, state.seller_reg);

            // parser_auction_data(state.buyer_reg, state.seller_reg, matrix);

            //scenario_generated->addMessage(state.activate);
        }
    }

    // time_advance function
    [[nodiscard]] double timeAdvance(const auction_setupState& state) const override {     
        if(state.activate == true) {
            return 0.1;
        } else {
            return std::numeric_limits<double>::infinity();
        }  
        //return state.sigma;
    }
};

#endif