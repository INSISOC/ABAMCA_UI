#ifndef S_R_INFO_HPP
#define S_R_INFO_HPP

#include <random>
#include <iostream>
#include <vector>
#include <sstream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct s_reg_info {
    int seller_id;
    std::vector<int> bundle;
    float initial_apr;
    float prod_cost;

    //you can have as many state variables as you want/ need

    s_reg_info(int _seller_id, const std::vector<int>& _bundle, float _initial_apr, float _prod_cost): seller_id(_seller_id), bundle(_bundle), initial_apr(_initial_apr), prod_cost(_prod_cost) {
    }
    
    s_reg_info(): seller_id(-1), initial_apr(-1), prod_cost(-1) {}
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const s_reg_info& state) {
    out  << "{" << state.seller_id << ",[";
    for (int i=0; i<state.bundle.size()-1; i++){
        out << state.bundle[i] << ",";
    }
    out <<  state.bundle.back();
    out << "]," << state.initial_apr << "," << state.prod_cost << "}";
    return out;
}
#endif

std::istream& operator>> (std::istream& is, s_reg_info& info) {  //inline 
    std::string vector_str;
    // Read basic fields
    if (!(is >> info.seller_id >> vector_str >> info.initial_apr >> info.prod_cost))
        return is;

    // Parse the vector string "3,5,6"
    info.bundle.clear();
    std::stringstream ss(vector_str);
    std::string item;
    while (std::getline(ss, item, ',')) {
        try {
            info.bundle.push_back(std::stoi(item));
        } catch (...) {
            // ignore bad conversions (optional)
        }
    }

    return is;

}


#endif