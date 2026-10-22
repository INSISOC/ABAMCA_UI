#ifndef B_R_INFO_HPP
#define B_R_INFO_HPP

#include <random>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct b_reg_info {
    int buyer_id;
    int order_id;
    float initial_ppr;
    float reservation_pr;

    //you can have as many state variables as you want/ need

    b_reg_info(int _buyer_id, int _order_id, float _initial_ppr, float _reservation_pr): buyer_id(_buyer_id), order_id(_order_id), initial_ppr(_initial_ppr), reservation_pr(_reservation_pr) {
    }
    
    b_reg_info(): buyer_id(-1), order_id(-1), initial_ppr(-1), reservation_pr(-1) {}
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const b_reg_info& state) {
    out  << "{" << state.buyer_id << "," << state.order_id << "," << state.initial_ppr << "," << state.reservation_pr << "}";
    return out;
}
#endif

std::istream& operator>> (std::istream& in, b_reg_info& info) {
    in >> info.buyer_id >> info.order_id >> info.initial_ppr >> info.reservation_pr;
    return in;
}


#endif