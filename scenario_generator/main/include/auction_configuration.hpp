#ifndef AUCTION_CONFIGURATION_HPP
#define AUCTION_CONFIGURATION_HPP

#include "cadmium/modeling/devs/coupled.hpp"
#include "buyer_registration.hpp"
#include "seller_registration.hpp"
//#include "auction_setup.hpp"
#include "max_sw.hpp"

#include "b_reg_info.hpp"
#include "s_reg_info.hpp"

#include "cadmium/lib/iestream.hpp"          //iestream header

using namespace cadmium;

struct auction_configuration : public Coupled {
    //Port<b_reg_info> scenario_out; //Change the type: outputs a text file (or hard copy--for that, define a struct with output data for the scenario)
    Port<std::string> orders_requested_out;
    Port<b_reg_info> buyers_in;
    Port<s_reg_info> sellers_in;
    /**
     * Constructor function for the scenario_generator model.
     * @param id ID of the scenario_generator model.
     */
    auction_configuration(const std::string& id, float Requesting_timer, float Asking_timer) : Coupled(id) {
        //scenario_out = addOutPort<b_reg_info>("scenario_out"); //CHANGE AFTER TEST 1 (txt-like out format)??
        orders_requested_out = addOutPort<std::string>("orders_requested_out"); 

        buyers_in = addInPort<b_reg_info> ("b_in");
        sellers_in = addInPort<s_reg_info> ("s_in");
        
        
        auto buyer_registration_model = addComponent<buyer_registration>("buyer_registration model", Requesting_timer);
        auto seller_registration_model = addComponent<seller_registration>("seller_registration model", Asking_timer);

        //Remove comment comment for RT with files
        //auto buyers_file = addComponent<lib::IEStream<b_reg_info>>("buyers file", "/mnt/c/DOCUMENTS/ABAMCA/TopfolderDecember2025(Juan)/scenario_generator_rt-main/buyers_input.txt");   
        //auto sellers_file = addComponent<lib::IEStream<s_reg_info>>("sellers file", "/mnt/c/DOCUMENTS/ABAMCA/TopfolderDecember2025(Juan)/scenario_generator_rt-main/sellers_input.txt");   
        
        std::cout << "ok" <<std::endl;

        //auto auction_setup_model = addComponent<auction_setup>("auction_setup model");
        auto max_sw_model = addComponent<max_sw>("max_sw model");

        //Internal Couplings

        //Remove comment comment for RT with files
       // addCoupling(buyers_file->out, buyer_registration_model->b_r_in);
        //addCoupling(sellers_file->out, seller_registration_model->s_r_in);
        addCoupling(buyers_in, buyer_registration_model->b_r_in);
        addCoupling(sellers_in, seller_registration_model->s_r_in);


        addCoupling(buyer_registration_model->pool_of_orders_out, orders_requested_out);       
        addCoupling(buyer_registration_model->pool_of_orders_out, seller_registration_model->start_asking_in);
        //addCoupling(buyer_registration_model->b_r_out, auction_setup_model->buy_in); 
        //addCoupling(seller_registration_model->s_r_out, auction_setup_model->sell_in);
        addCoupling(buyer_registration_model->b_r_out, max_sw_model->buy_in); 
        addCoupling(seller_registration_model->s_r_out, max_sw_model->sell_in);

    }
};

#endif