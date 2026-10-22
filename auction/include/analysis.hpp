#ifndef CADMIUM_EXAMPLE_AUCTION_ANALYSYS_HPP_
#define CADMIUM_EXAMPLE_AUCTION_ANALYSYS_HPP_

#include <cadmium/modeling/devs/atomic.hpp>
#include <iostream>
#include <vector>
#include <assert.h>
#include "surplusinfo.hpp"

#include <fstream>  // to create the csv file containing the round number and the value of the objective function

namespace cadmium::example::auction {
	//! Class for representing the Analysis DEVS model state.

	//DECLARATION OF MODEL STATE BEGINS
	struct AnalysisState {
		//double sigma;  //!< 
		bool all_data_received;  //!< True/False variable to indicate the active/passive state of the model.
		double total_surplus_buyers;  //!< Sum of buyer surplus
		double total_surplus_sellers;  //!< Sum of seller surplus
		double total_surplus_auctioneer; // Auctioneer's surplus
		double total_surplus;  //!< Sum of buyer surplus + seller surplus + auctioneer surplus
		int received; // Count the number of messages received: social welfare must be calculated only when the information from ALL the agents have been received
		int round;

		//! Constructor function. 
		AnalysisState(): all_data_received(false), total_surplus_buyers(0), total_surplus_sellers(0), total_surplus_auctioneer(0), total_surplus(0), received(0), round(1) {}
	};

	/**
	 * Insertion operator for AnalysisState objects. 
	 * @param out output stream.
	 * @param s state to be represented in the output stream.
	 * @return output stream with S1 already inserted.
	 */
	std::ostream& operator<<(std::ostream& out, const AnalysisState& s) {
		out << "{" << s.round << "," << s.total_surplus_buyers << "," << s.total_surplus_sellers << "," << s.total_surplus_auctioneer << "," << s.total_surplus << "}";
		return out;
	}

	//DECLARATION OF MODEL STATE ENDS
	
	// CREAMOS LA CLASE PROTOTIPO (SELLO) Y DESPUÉS CREAMOS LAS INSTANCIAS DE LA CLASE. EL SELLO ES LA CLASE. CADA VEZ QUE USO EL SELLO ESTOY CREANDO UNA INSTANCIA.

	//! Atomic DEVS model of Analysis
	class Analysis : public Atomic<AnalysisState> { //! Atomic models MUST inherit from the cadmium::Atomic<S> class
	 private:
	 	int num_buyers;
		int num_sellers; 
		double max_social_welfare;                            
	 public:
		Port<Surplusinfo> in_buyer;       	 //!< Input Port for receiving the buyer surplus
		Port<Surplusinfo> in_seller;      	 //!< Input Port for receiving the seller surplus
		Port<Surplusinfo> in_auctioneer;     //!< Input Port for receiving the auctioneer surplus
		Port<double> out_total_surplus;	 	 //!< Output Port for sending the social welfare in the current round
		Port<double> out_buyer_surplus;		 //!< Output Port for sending the total buyer surplus in the current round
		Port<double> out_auctioneer_surplus; //!< Output Port for sending the auctioneer surplus in the current round
		Port<double> out_seller_surplus;     //!< Output Port for sending the total seller surplus in the current round
		Port<double> out_maximum_welfare;    //!< Output Port for sending the maximum theoretical social welfare

	
		/**
		 * Constructor function for Analysis DEVS model.
		 * @param id model ID.
		
		 */ 
			
		Analysis(const std::string& id, int _num_buyers, int _num_sellers, double _max_social_welfare): Atomic<AnalysisState>(id, AnalysisState()), num_buyers(_num_buyers), num_sellers(_num_sellers), max_social_welfare(_max_social_welfare){
			//num_buyers =_num_buyers; (es equivalente a lo anterior)
			in_buyer = addInPort<Surplusinfo>("in_buyer");
			in_seller = addInPort<Surplusinfo>("in_seller");
			in_auctioneer = addInPort<Surplusinfo>("in_auctioneer");
			out_total_surplus = addOutPort<double>("out_total_surplus");
			out_buyer_surplus = addOutPort<double>("out_buyer_surplus");
			out_seller_surplus = addOutPort<double>("out_seller_surplus");
			out_auctioneer_surplus = addOutPort<double>("out_auctioneer_surplus");
			out_maximum_welfare = addOutPort<double>("out_maximum_social_welfare");
		}

		/**
		 * Updates AnalysisState::all_data_received. If it is active (because all the date has been received), then it is passivated.
		 * @param s reference to the current analysis model state.
		 */
		void internalTransition(AnalysisState& s) const override {
			if (s.all_data_received == true)
			{
				s.all_data_received = false;
				s.received = 0;
				s.total_surplus_buyers = 0;
				s.total_surplus_sellers = 0;
				s.total_surplus = 0;
				s.total_surplus_auctioneer = 0;
				s.round += 1;

			}
			
		}

		/**
		 * @param s reference to the current filter model state.
		 * @param e time elapsed since the last state transition function was triggered.
		 * @param x reference to the atomic model input port set.
		 */
		void externalTransition(AnalysisState& s, double e) const override {
			
			if (! in_buyer->empty()){
				for (auto& buyer_local: in_buyer->getBag()){ //the bag (array) may contain one or more elements (i.e., messages). We iterate over all the elements.
				                                             // We use auto but we alreday know that messages will be of the type surplusinfo

					//std::cout << "Analysis' round: " << s.round << endl;
					//std::cout << "Buyer's round: " << buyer_local.round << endl;
					if(buyer_local.round == s.round){
						s.received +=1;
						s.total_surplus_buyers += buyer_local.surplus;
						s.total_surplus += buyer_local.surplus;
	
					}else  {
					assert(false && "Buyer: my model should not get here - external error, wrong round");
					}
				}
							
			}

			if (! in_seller->empty()){
				for (auto& seller_local: in_seller->getBag()){ //en la bolsa (array) puedo tener uno o varios elementos (mensajes)
					//std::cout << "Analysis' round: " << s.round << endl;
					//std::cout << "Seller's round: " << seller_local.round << endl;
					
					if(seller_local.round == s.round){
						s.received +=1;
						s.total_surplus_sellers += seller_local.surplus;
						s.total_surplus += seller_local.surplus;
					}
					else  {
						assert(false && "Seller: my model should not get here - external error, wrong round");
					}
				}
							
			}

			if (! in_auctioneer->empty()){
					Surplusinfo auctioneer_local(0,0); //we create this variable as the received info is not a basic variable (int, etc). Creamos objeto de tipo surplus y le damos los valores por defecto (ver definición de surplusinfo)
					auctioneer_local = in_auctioneer->getBag().back(); //cogemos el último porque asumimos que solo va a haber uno.
					//std::cout << "Auctioneer's round: " << auctioneer_local.round << endl;
					//std::cout << "Auctioneer surplus: " << auctioneer_local.surplus << endl;
					if(auctioneer_local.round == s.round){
						s.received +=1;
						s.total_surplus_auctioneer = auctioneer_local.surplus;
						s.total_surplus += auctioneer_local.surplus;
					}
					else  {
							assert(false && "Auctioneer: my model should not get here - external error, wrong round");
							}
							
			}


			if(s.received == num_buyers + num_sellers+ 1){
				s.all_data_received = true;
			}				
			
		}

		/**
		 * Sends output message with information about the social welfare
		 * @param s reference to the current filter model state.
		 * @param y reference to the atomic model output port set.
		 */
		void output(const AnalysisState& s) const override {
			if (s.all_data_received == true)
			{
				out_total_surplus->addMessage(s.total_surplus);
				out_buyer_surplus->addMessage(s.total_surplus_buyers);	
				out_seller_surplus->addMessage(s.total_surplus_sellers);
				out_auctioneer_surplus->addMessage(s.total_surplus_auctioneer);
				out_maximum_welfare->addMessage(max_social_welfare);

				//write_to_csv(s.round, s.total_surplus_buyers, s.total_surplus_sellers, s.total_surplus);
				ofstream outputFile;
  	  			outputFile.open("output.csv", ios::app);  // Open in append mode
   	 			if (outputFile.is_open()) {
   	    		 	// Add headers if the file is open
   	    		 	if (outputFile.tellp() == 0) {
   	     		   	outputFile << "Round number;Buyers' surplus;Seller's surplus;Auctioneer's surplus;Social Welfare;Maximum social welfare;Efficiency\n";
   	    	 		}
   	     			// Write values of count and objective_function on a new line of the csv file
   	     			outputFile << s.round << ";" << s.total_surplus_buyers << ";" << s.total_surplus_sellers << ";" << s.total_surplus_auctioneer << ";" << s.total_surplus << ";" << max_social_welfare << ";" << (s.total_surplus / max_social_welfare)*100 << "\n";
			 	    outputFile.close();
  	  				} else {
  	      				cerr << "Error when opening file" << endl;
			 	  	}
		
			}
			
		}

		/**
		 * If the analysis is active, it returns a time advance of 0.5; otherwise it sets ta to infinity.
		 * @param s reference to the current filter model state.
		 * @return the sigma value.
		 */
		[[nodiscard]] double timeAdvance(const AnalysisState& s) const override {
			if (s.all_data_received == true)
			{
				return 0.5;
			} else {
				return std::numeric_limits<double>::infinity();
			}
			// std::numeric_limits<double>::infinity();
		}
	};

			

}  //namespace cadmium::example::gpt

#endif //CADMIUM_EXAMPLE_GPT_FILTER_HPP_