#ifndef CADMIUM_EXAMPLE_AUCTION_AUCTION_HPP_
#define CADMIUM_EXAMPLE_AUCTION_AUCTION_HPP_

#include <cadmium/modeling/devs/coupled.hpp>
#include <cadmium/lib/iestream.hpp>
#include <vector>

#include "buyer.hpp"
#include "seller.hpp"
#include "auctioneer.hpp"
//#include "feedback.hpp"

namespace cadmium::example::auction {
	//! Coupled DEVS model of the experimental frame.
	struct auction: public Coupled {
		//Port<Feedback> in;   //!< 
		Port<bool> alloc_out;   //!< 
		Port<Surplusinfo> actioneersurpl;
		Port<Surplusinfo> buyersurpl;
		Port<Surplusinfo> sellersurpl;

		/**
		 * Constructor function for the bid model.
		 * @param id ID of the bid model.
		 * @param jobPeriod Job generation period for the Generator model.
		 * @param obsTime time to wait by the Transducer before asking the Generator to stop creating Job objects.
		 */
		auction(const std::string id, std::vector<int> ID_b, std::vector<double> RPr, double PPrStep, std::vector<double> InitialPPr, std::vector<int> ID_s, std::vector<double> PCost, double APrStep, std::vector<double> InitialAPr, std::vector<std::vector<int>> matrix, float round_timer): Coupled(id) {
			//in = addInPort<Feedback>("in");
			alloc_out = addOutPort<bool>("alloc_out");
			buyersurpl = addOutPort<Surplusinfo>("buyer_surp");
			sellersurpl = addOutPort<Surplusinfo>("seller_surp");
			actioneersurpl = addOutPort<Surplusinfo>("actioneer_surp");
			
			auto auctioneer = addComponent<Auctioneer>("auctioneer", matrix, round_timer);
			addCoupling(auctioneer->allocation, alloc_out);	
			addCoupling(auctioneer->surpl, actioneersurpl);	

			for (int i=0; i<ID_b.size(); i++){ 
				auto buyers = addComponent<buyer>("buyer"+ std::to_string(ID_b[i]), ID_b[i], RPr[i], PPrStep, InitialPPr[i]); // +ID_b[i]
				addCoupling(buyers->bidValue, auctioneer->bid_in);
				addCoupling(auctioneer->bid_out, buyers->in);
				addCoupling(buyers->surplusValue, buyersurpl);
			}

			
			for (int i=0; i<ID_s.size(); i++){
				auto sellers = addComponent<seller>("seller"+ std::to_string(ID_s[i]), ID_s[i], PCost[i], APrStep, InitialAPr[i]); // +ID_s[i]		
				addCoupling(sellers->askValue, auctioneer->ask_in);			
				addCoupling(auctioneer->ask_out, sellers->in);
				addCoupling(sellers->surplusValue, sellersurpl);
			}
	
		}
	};
}  //namespace cadmium::example::auction

#endif //CADMIUM_EXAMPLE_AUCTION_AUCTION_HPP_
