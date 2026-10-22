#ifndef SELLER_REGISTRATION_HPP
#define SELLER_REGISTRATION_HPP

#include <random>
#include <iostream>
#include <vector>
#include <string>
#include "cadmium/modeling/devs/atomic.hpp"
#include "s_reg_info.hpp"
//#include <fstream> // to create the output csv file 

using namespace cadmium;

struct seller_registrationState {
    //double sigma; Same as requesting_time?
    float asking_time;
    bool accepting;
    bool closed;
    std::vector<s_reg_info> s_reg;

    //you can have as many state variables as you want/ need

    explicit seller_registrationState(float _Ask_timer): asking_time(-1), accepting(false), closed(false){
    }
    // explicit seller_registrationState(float _Ask_timer): asking_time(_Ask_timer), accepting(false), closed(false){
    // }
    //explicit seller_registrationState(): asking_time(-1), accepting(false), closed(false){
    // }
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const seller_registrationState& state) {
    out  << "{asking_time: " << state.asking_time << ", accepting: " << state.accepting << ", closed: " << state.closed << "}";
    return out;
}
#endif

class seller_registration : public Atomic<seller_registrationState> {
    private:
    float ASKING_TIMER; 
    
    public:
    Port<s_reg_info> s_r_in;
    Port<s_reg_info> s_r_out;
    Port<std::string> start_asking_in;
    //Declare your ports here

    //seller_registration(const std::string id) : Atomic<seller_registrationState>(id, seller_registrationState()) {
    seller_registration(const std::string id, float _Asking_timer) : Atomic<seller_registrationState>(id, seller_registrationState(_Asking_timer)), ASKING_TIMER(_Asking_timer) {
        //Constructor of your atomic model. Initialize ports here.
        s_r_in = addInPort<s_reg_info>("s_r_in");
        s_r_out = addOutPort<s_reg_info>("s_r_out");
        start_asking_in = addInPort<std::string>("start_asking_in");
    }

    // internal transition
    void internalTransition(seller_registrationState& state) const override {
        //your internal transition function goes here
        //state.sigma += 1;
        if (state.closed == false){
            state.closed = true;
        } else {
            state.closed = false;
            state.accepting = false;
            state.asking_time = ASKING_TIMER;
        }
    }

    // external transition
    void externalTransition(seller_registrationState& state, double e) const override {
        if(!start_asking_in->empty()){
        //     auto& Asking_timer = start_asking_in->getBag().back();
        //     state.asking_time = Asking_timer
            state.asking_time = ASKING_TIMER;
            state.accepting = true;
        }
        
        if(!s_r_in->empty()){

            auto& message_received = s_r_in->getBag().back();
            //b_reg_info message_received = b_r_in->getBag().back();
            std::cout << "Seller message received is: " << message_received << std::endl;
            state.s_reg.push_back(message_received);

            //state.pool_of_orders.push_back(message_received.order_id);
            state.asking_time -= e;
        }
        
        //state.sigma -= e;    

    }
    
    
    // output function
    void output(const seller_registrationState& state) const override {
        if(state.closed == true){ // and state.accepting == true
            for(int i=0; i<state.s_reg.size(); i++){
                s_reg_info out_seller_message = {state.s_reg[i].seller_id, state.s_reg[i].bundle, state.s_reg[i].initial_apr, state.s_reg[i].prod_cost};
                s_r_out->addMessage(out_seller_message);
            }   
        }
    }

    // time_advance function
    [[nodiscard]] double timeAdvance(const seller_registrationState& state) const override {     
        if(state.closed == true){
            return 0.1;
        } else if (state.accepting == false){
            return std::numeric_limits<double>::infinity();
        } else {
            return state.asking_time;
        }
        
        //return state.sigma;
    }
};

#endif