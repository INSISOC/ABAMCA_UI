#ifndef __AUCTIONEER_HPP__
#define __AUCTIONEER_HPP__

#include <cadmium/modeling/devs/atomic.hpp>
#include <iostream>
//#include "input_data.hpp" // to read reservation prices and production costs from the input file. I don¡t need to include it again as it was already included in the main file
#include <vector>
#include <string>
#include <string.h>
#include <sstream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <assert.h>
#include "agentinfo.hpp"
#include "bidinfo.hpp"
#include "feedback.hpp"
//#include "calling_python.hpp"



namespace cadmium::example::auction {
	using namespace std;

	//! Class for representing the Generator DEVS model state.
	struct AuctioneerState {
		//double sigma;  //!< 
		int round;
		float time_to_calculate;  //!< .
		bool new_value;  //!< True/False variable inputted from the auctioneer.
		bool notify; 	 // True if we need to notify the allocation and surplus
		bool auction_over;		
		double surplus; // New state variable (surplus will be a new output)
		std::vector<Agentinfo> seller_ask;
		std::vector<Agentinfo> buyer_bid;
		
		// Default Constructor function
		AuctioneerState(): round(1), time_to_calculate(0), new_value(false), notify(false), auction_over(false), surplus (0) {
		} 
		// Another Constructor function
		AuctioneerState(float _time): round(1), time_to_calculate(_time), new_value(false), notify(false), auction_over(false), surplus (0) {
		}
		// AuctioneerState(float _time) {
		// 	AuctioneerState();
		// 	time_to_calculate = _time;	
		// }  
	};
	

	/**
	 * Insertion operator for GeneratorState objects. It only displays the value of jobCount.
	 * @param out output stream.
	 * @param s state to be represented in the output stream.
	 * @return output stream with jobCount already inserted.
	 */
	std::ostream& operator<<(std::ostream& out, const AuctioneerState& au) {
		out << "{" << au.time_to_calculate <<"," << au.round << "," << au.new_value <<"," << au.notify <<"," << au.auction_over <<"," << au.surplus <<",";
		for (Agentinfo item: au.seller_ask){ 
			out << item <<",";
		}
		for (Agentinfo item: au.buyer_bid){
			out << item ;
		}
		out << "}";
		return out;
	}

	//! Atomic DEVS model of a Job generator.
	class Auctioneer : public Atomic<AuctioneerState> { //! Atomic models MUST inherit from the cadmium::Atomic<S> class
	 private:
	 	std::vector<std::vector<int>> matrix;
		float round_timer;

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
		Port<Bidinfo> bid_in;          //!< Input Port for receiving stop generating Job objects.
		Port<Bidinfo> ask_in;
		Port<Feedback> bid_out;
		Port<Feedback> ask_out;
		Port<bool> allocation;
		Port<Surplusinfo> surpl;
		//BigPort<Job> outGenerated;  //!< Output Port for sending new Job objects to be processed.

		/**
		 * Constructor function for Generator DEVS model.
		 * @param id model ID.
		 * @param jobPeriod Job generation period.
		 */
		Auctioneer(const std::string& id, std::vector<std::vector<int>> _matrix, float _round_timer): Atomic<AuctioneerState>(id, AuctioneerState(_round_timer)), matrix(_matrix), round_timer(_round_timer) {
			bid_in = addInPort<Bidinfo>("bid_in");
			ask_in = addInPort<Bidinfo>("ask_in");
			bid_out = addOutPort<Feedback>("bid_out");
			ask_out = addOutPort<Feedback>("ask_out");
			allocation = addOutPort<bool>("allocation");
			surpl = addOutPort<Surplusinfo>("surplus");
		}

		/**
		 * Updates GeneratorState::clock and GeneratorState::sigma and increments GeneratorState::jobCount by one.
		 * @param s reference to the current generator model state.
		 */
		void internalTransition(AuctioneerState& au) const override {			
			if (au.notify == false) {	
				if (au.new_value == false) {
					au.auction_over = true;
					cout << "THIS IS THE FINAL ALLOCATION" << endl;
				} else {
					cout << "Round " << au.round << endl;
					// "Solve the WDP"

					// Input data: matrix, purchase_prices, ask_prices

					//Create string to call python file
					std::stringstream ss;
					std::string s = "python3 WDP.py ";
					// Reading matrix
					for (int i=0; i<matrix.size(); i++) {
						if(i != 0)
							ss << ":";
						for (int j=0; j<matrix[0].size(); j++) {
							if(j != 0)
								ss << ",";
							ss << matrix[i][j];
						}
					}
					ss << " ";

					// Reading purchase prices
					for (int i=0; i<au.buyer_bid.size(); i++) {
						if(i != 0)
							ss << ",";
						ss << au.buyer_bid[i].Price;
					}
					ss << " ";
					
					//Reading ask prices
					for (int i=0; i<au.seller_ask.size(); i++) {
						if(i != 0)
							ss << ",";
						ss << au.seller_ask[i].Price;
					}

					s += ss.str();

					std::cout << s << endl;
					std::string str = s;  

					// Call the python file to run the solver and retrieve its output				
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
    				au.surplus = std::stof(objective_value_str);
					}
					
					// prints the result
					cout << "Variables string is:" << sub2;
					cout << "Constraints string is:" << cons2;
					//cout << "Objective function value: " << objective_function << endl;

					// convert string to vector of integers
					vector<int> Variables;
					vector<int> Constraints;

					// remove the square brackets at the beggining and end
					sub2 = sub2.substr(1, sub2.size() - 2);
					cons2 = cons2.substr(1, cons2.size() - 2);

					istringstream zz(sub2);
					istringstream xx(cons2);
					string token, token2;

					while(getline(zz, token, ',')) {
						// convert the token to an integer and add it to the vector
						Variables.push_back(stoi(token));
					}

					// print the vector
					for (int value : Variables) {
						cout << value << " ";
					}
					cout << endl;
					//cout << Variables << endl;

					while(getline(xx, token2, ',')) {
						// convert the token to an integer and add it to the vector
						Constraints.push_back(stoi(token2));
					}

					// print the vector
					for (int value2 : Constraints) {
						cout << value2 << " ";
					}
					cout << endl;
							
					cout << "Round:" << au.round <<endl;
					cout << "Objective function value (AR): " << au.surplus << endl;			
					

					// Update of the gotIt for buyers and sellers

					for (int j=0; j<Variables.size(); j++) {
						if (Variables[j] == 1) {
							au.seller_ask[j].gotIt = true;
						} else {
							au.seller_ask[j].gotIt = false;
						}
					}

					for (int i=0; i<Constraints.size(); i++) {
						if (Constraints[i] == 1) {
							au.buyer_bid[i].gotIt = true;
						} else {
							au.buyer_bid[i].gotIt = false;
						}
					}

					// cout << "Objval = " << objval2 << endl;
					// if(count == au.buyer_bid.size() & zero > 0){
					// 	au.auction_over = true;
					// }
					
					au.notify = true;
					au.new_value = false;			
				}	
			} else  {
				au.notify = false;
				au.round +=1;
				au.time_to_calculate = round_timer;
			}
		}

		/**
		 * Updates GeneratorState::clock and GeneratorState::sigma.
		 * If it receives a true message via the Generator::inStop port, it passivates and stops generating Job objects.
		 * @param s reference to the current generator model state.
		 * @param e time elapsed since the last state transition function was triggered.
		 * @param x reference to the atomic model input port set.
		 */
		void externalTransition(AuctioneerState& au, double e) const override {
			// for the multi-agent case it will be a vector of structs
			if (! bid_in->empty()){
				for (auto& buyer_local: bid_in->getBag()){
					//Mirar si el ID esta en la lista de buyers					
					bool flag = false;
					int index = 0;
					for(int i=0; i<au.buyer_bid.size(); i++){
						if(au.buyer_bid[i].id == buyer_local.id){
							flag = true;
							index = i;
							break;
						}
					}
					//Si lo encuentra, actualizar el price					
					if(flag){
						au.buyer_bid[index].Price = buyer_local.Price;
					} else {
						//Si el ID no esta en la lista, lo añado (añado ID + precio)
						Agentinfo new_buyer (buyer_local.id, buyer_local.Price);
						au.buyer_bid.push_back(new_buyer);
					}
					//if (au.round == 0){
					//	std::cout << "Buyer_bid size: " << au.buyer_bid.size() << "\n";
					//}
				}
							
			}
			
			if (! ask_in->empty()){
				
				for (auto& seller_local: ask_in->getBag()){
					bool flag = false;
					int index = 0;
					for(int i=0; i<au.seller_ask.size(); i++){
						if(au.seller_ask[i].id == seller_local.id){
							flag = true;
							index = i;
						}
					}
					if(flag){
						au.seller_ask[index].Price = seller_local.Price;
					} else {
						Agentinfo new_seller (seller_local.id, seller_local.Price);
						au.seller_ask.push_back(new_seller);
					}
					//if (au.round == 0){
					//	std::cout << "Seller_ask size: " << au.seller_ask.size() << "\n";
					//}
				}
			}
			au.new_value = true;
			au.time_to_calculate -= e;
			assert(au.time_to_calculate >= 0 && "Issue with time advance in the external");	
			//printf("Time %f\n", au.time_to_calculate);
			
		}

		/**
		 * Sends a new Job that needs to be processed via the Generator::outGenerated port.
		 * @param s reference to the current generator model state.
		 * @param y reference to the atomic model output port set.
		 */
		void output(const AuctioneerState& au) const override {
			if (au.notify == true)			{
				for (int i=0; i<au.buyer_bid.size(); i++){
					Feedback out_bid = {au.buyer_bid[i].id, au.buyer_bid[i].gotIt};
					bid_out->addMessage(out_bid);
				}
				for (int i=0; i<au.seller_ask.size(); i++){
					Feedback out_ask = {au.seller_ask[i].id, au.seller_ask[i].gotIt};
					ask_out->addMessage(out_ask);
				}
				//if(au.auction_over == true){
				//	allocation->addMessage(au.auction_over);
				//}
				//TODO - QUITAR PUERTO ALLOCATION O QUE INFORME DE LA ASSIGNACION
				Surplusinfo surp = {au.round, au.surplus};
				surpl ->addMessage(surp);
			}
			if(au.auction_over == true){std::cout << "se acabó" << endl;}
		}

		/**
		 * It returns the value of GeneratorState::sigma.
		 * @param s reference to the current generator model state.
		 * @return the sigma value.
		 */
		[[nodiscard]] double timeAdvance(const AuctioneerState& au) const override {
			if (au.notify == true)	{
				return 0;
			} else if (au.auction_over == true){
				return std::numeric_limits<double>::infinity();
			}else{
				return au.time_to_calculate;
			}
		}
	};
}  //namespace cadmium::example::auction

#endif //__AUCTIONEER_HPP__
