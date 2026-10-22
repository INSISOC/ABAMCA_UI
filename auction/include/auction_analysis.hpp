#ifndef CADMIUM_EXAMPLE_AUCTION_AUCTION_ANALYSIS_HPP_
#define CADMIUM_EXAMPLE_AUCTION_AUCTION_ANALYSIS_HPP_

#include <cadmium/modeling/devs/coupled.hpp>
#include <cadmium/lib/iestream.hpp>
#include <vector>

#include "auction.hpp" // We include the two models. Remember to modify the main_auction. cpp file to add the compilation of this auction_analysis coupled model
#include "analysis.hpp"

namespace cadmium::example::auction {
	//! Coupled DEVS model of the experimental frame.
	struct auction_analysis: public Coupled {
		
		Port<bool> auction_finished;   
		Port<double> social_welfare;
		Port<double> total_buyer_surplus;
		Port<double> total_seller_surplus;
		Port<double> total_auctioneer_surplus;
		Port<double> maximum_theoretical_social_welfare;
		
		/**
		 * Constructor function for the auction analysis model.
		 * @param id ID of the bid model.
		 * @param jobPeriod Job generation period for the Generator model.
		 * @param obsTime time to wait by the Transducer before asking the Generator to stop creating Job objects.
		 */
		
		//AGO2024: added maximum_social_welfare 
		auction_analysis(const std::string id, double maximum_social_welfare, std::vector<int> ID_b, std::vector<double> RPr, double PPrStep, std::vector<double> InitialPPr, std::vector<int> ID_s, std::vector<double> PCost, double APrStep, std::vector<double> InitialAPr, std::vector<std::vector<int>> matrix, float round_timer): Coupled(id) {
			
			auction_finished = addOutPort<bool>("auction_finished");
			social_welfare = addOutPort<double>("social_welfare");
			total_buyer_surplus = addOutPort<double>("total_buyer_surplus"); 
			total_seller_surplus = addOutPort<double>("total_seller_surplus");
			total_auctioneer_surplus = addOutPort<double>("total_auctioneer_surplus");
			maximum_theoretical_social_welfare = addOutPort<double>("maximum_social_welfare");
			
			//AGO2024: added maximum_social_welfare
			auto analysis = addComponent<Analysis>("analysis", ID_b.size(), ID_s.size(),maximum_social_welfare); //Fijarme en el constructor de cada uno de los dos modelos
			//Analysis(const std::string& id, int _num_buyers, int _num_sellers): Atomic<AnalyisState>(id, AnalysisState()), num_buyers(_num_buyers), num_sellers(_num_sellers){

			// Para decidir qué modelo creamos: fixed_step o el nuevo, según un nuevo parámetro..

			//if parámetro = fixed_step --> creamos el fixedstep (el actual) 
			//auto auction_comp =addComponent<auction_fixed_step>("auction_fixed_step",  ID_b,RPr, PPrStep, InitialPPr ,ID_s ,PCost , APrStep, InitialAPr , matrix, round_timer);  
			//else... el otro

			auto auction_comp =addComponent<auction>("auction",  ID_b,RPr, PPrStep, InitialPPr ,ID_s ,PCost , APrStep, InitialAPr , matrix, round_timer);  
			//auction(const std::string id, std::vector<int> ID_b, std::vector<double> RPr, double PPrStep, std::vector<double> InitialPPr, std::vector<int> ID_s, std::vector<double> PCost, double APrStep, std::vector<double> InitialAPr, std::vector<std::vector<int>> matrix, float round_timer): Coupled(id) {


			addCoupling(auction_comp->alloc_out, auction_finished);	
			addCoupling(analysis->out_total_surplus, social_welfare);	
			addCoupling(analysis->out_buyer_surplus, total_buyer_surplus);
			addCoupling(analysis->out_seller_surplus, total_seller_surplus);
			addCoupling(analysis->out_auctioneer_surplus, total_auctioneer_surplus);
			addCoupling(analysis->out_maximum_welfare, maximum_theoretical_social_welfare);
			
			addCoupling(auction_comp->buyersurpl, analysis->in_buyer);
			addCoupling(auction_comp->sellersurpl, analysis->in_seller);	
			addCoupling(auction_comp->actioneersurpl, analysis->in_auctioneer);		

		}

	};
}  //namespace cadmium::example::auction

#endif //CADMIUM_EXAMPLE_AUCTION_AUCTION_ANALYSIS_HPP_