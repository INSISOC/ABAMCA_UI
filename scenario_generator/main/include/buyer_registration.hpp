#ifndef BUYER_REGISTRATION_HPP
#define BUYER_REGISTRATION_HPP

#include <random>
#include <iostream>
#include <vector>
#include <string>
#include "cadmium/modeling/devs/atomic.hpp"
#include "b_reg_info.hpp"
#include <fstream> // to create the output file 

using namespace cadmium;

struct buyer_registrationState {
    //double sigma; Same as requesting_time?
    float requesting_time;
    bool accepting;
    bool closed;
    std::vector<b_reg_info> b_reg;
    std::vector<int> pool_of_orders;

    //you can have as many state variables as you want/ need

    explicit buyer_registrationState(float _Req_timer): requesting_time(_Req_timer), accepting(true), closed(false){
    }
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const buyer_registrationState& state) {
    out  << "{requesting_time: " << state.requesting_time << ", accepting: " << state.accepting << ", closed: " << state.closed << "}";
    return out;
}
#endif

class buyer_registration : public Atomic<buyer_registrationState> {
    private:
    float REQUESTING_TIMER;
    
    public:
    Port<b_reg_info> b_r_in;
    Port<b_reg_info> b_r_out;
    Port<std::string> pool_of_orders_out;
    //Port<bool> req_finished_out;
    //Declare your ports here

    buyer_registration(const std::string id, float _Requesting_timer) : Atomic<buyer_registrationState>(id, buyer_registrationState(_Requesting_timer)), REQUESTING_TIMER(_Requesting_timer) {
        //Constructor of your atomic model. Initialize ports here.
        b_r_in = addInPort<b_reg_info>("b_r_in");
        b_r_out = addOutPort<b_reg_info>("b_r_out");
        pool_of_orders_out = addOutPort<std::string>("pool_of_orders_out");
        //req_finished_out = addOutPort<bool>("req_finished_out");
    }

    // internal transition
    void internalTransition(buyer_registrationState& state) const override {
        //your internal transition function goes here
        //state.sigma += 1;
        if (state.closed == false){
            state.closed = true;
        } else {
            state.closed = false;
            state.accepting = false;
            state.requesting_time = REQUESTING_TIMER;
        }
    }

    // external transition
    void externalTransition(buyer_registrationState& state, double e) const override {
        if(!b_r_in->empty()){

            auto& message_received = b_r_in->getBag().back();
            //b_reg_info message_received = b_r_in->getBag().back();
            std::cout << "Buyer message received is: " << message_received << std::endl;
            state.b_reg.push_back(message_received);

            state.pool_of_orders.push_back(message_received.order_id);

        }
        state.requesting_time -= e;
        //state.sigma -= e;    

    }
    
    
    // output function
    void output(const buyer_registrationState& state) const override {
        if(state.closed == true){ // and state.accepting == true
            for(int i=0; i<state.b_reg.size(); i++){
                b_reg_info out_buyer_message = {state.b_reg[i].buyer_id, state.b_reg[i].order_id, state.b_reg[i].initial_ppr, state.b_reg[i].reservation_pr};
                b_r_out->addMessage(out_buyer_message);
            }

            //req_finished_out->addMessage(state.closed);

            std::cout << "Pool of orders: ";
            for (int i=0; i<state.pool_of_orders.size()-1; i++)    {
                std::cout << state.pool_of_orders[i] << ", ";
            }
            std::cout << state.pool_of_orders.back() << std::endl;

            //write_to_csv or txt
            std::ofstream outFile;
            std::string fileName = "pool_of_orders.txt";
            outFile.open(fileName, std::ios::app);  // Open in append mode
            if (outFile.is_open()) {
                // Add headers if the file is open
                if (outFile.tellp() == 0) {
                    outFile << "Pool of orders: \n";
                }
                // Write values of order ids on a new line of the csv file
                for (int i=0; i<state.pool_of_orders.size()-1; i++)    {
                    outFile << state.pool_of_orders[i] << ", ";
                }
                outFile << state.pool_of_orders.back();
                outFile.close();
            } else {
                std::cerr << "Error when opening file" << std::endl;
            }
            
            pool_of_orders_out->addMessage(fileName); //MODIFY when I know what to do with this out port

            
        }
        //}
    }

    // time_advance function
    [[nodiscard]] double timeAdvance(const buyer_registrationState& state) const override {     
        if(state.closed == true){
            return 0.1;
        } else if (state.accepting == false){
            return std::numeric_limits<double>::infinity();
        } else {
            return state.requesting_time;
        }
        
        //return state.sigma;
    }
};

#endif