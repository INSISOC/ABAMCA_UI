#ifndef CADMIUM_EXAMPLE_AUCTION_SELLER_HPP_
#define CADMIUM_EXAMPLE_AUCTION_SELLER_HPP_

#include <cadmium/modeling/devs/coupled.hpp>
#include <cadmium/lib/iestream.hpp>

#include "filter.hpp"
#include "ask.hpp"	
#include "feedback.hpp"
#include "surplusinfo.hpp"

namespace cadmium::example::auction {
	//! Coupled DEVS model of the experimental frame.
	struct seller: public Coupled {
		Port<Feedback> in;   //!< 
		Port<Bidinfo> askValue;   //!< 
		Port<Surplusinfo> surplusValue;   //!<  This new port will cointain the seller's surplus in the current round
		/**
		 * Constructor function for the bid model.
		 * @param id ID of the bid model.
		 * @param jobPeriod Job generation period for the Generator model.
		 * @param obsTime time to wait by the Transducer before asking the Generator to stop creating Job objects.
		 */
		seller(const std::string& id, int ID_s, double PCost, double APrStep, double InitialAPr): Coupled(id) {
			in = addInPort<Feedback>("in");
			askValue = addOutPort<Bidinfo>("askValue");
			surplusValue = addOutPort<Surplusinfo>("surplusValue");

			auto filter = addComponent<Filter>("filter_s-"+std::to_string(ID_s), ID_s); // +ID_s
			auto ask = addComponent<Ask>("ask-"+std::to_string(ID_s), ID_s, PCost, APrStep, InitialAPr); //+ID_s
			//auto ies = addComponent<lib::IEStream<Feedback>>("file", filepath);

			addCoupling(in, filter->in);
			addCoupling(filter->out, ask->in);
			addCoupling(ask->out, askValue);
			addCoupling(ask->surpl, surplusValue);
		}
	};
}  //namespace cadmium::example::auction

#endif //CADMIUM_EXAMPLE_AUCTION_SELLER_HPP_
