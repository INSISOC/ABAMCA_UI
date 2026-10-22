#ifndef CADMIUM_EXAMPLE_AUCTION_BID_HPP_
#define CADMIUM_EXAMPLE_AUCTION_BID_HPP_

#include <cadmium/modeling/devs/atomic.hpp>
#include <iostream>
#include "feedback.hpp"
#include "bidinfo.hpp"
#include "surplusinfo.hpp"
#include <cmath>  // Needed for std::ceil (round up)

namespace cadmium::example::auction {
	//! Class for representing the Buyer DEVS model state.
	struct BidState {
		//double sigma;  //!< 
		bool Notifyprice;  //!< True/False variable to indicate the active/passive state of the model.
		bool Notifysurplus;  //!< True/False variable to indicate whether or not to notify the buyer's surplus
		double PurPr;  //!< Variable to set the purchase price of the bid.
		double Surplus; //!< Buyer surplus = reservation price - purchase price
		int Round;
		//! Constructor function. It sets all the attributes to 0.
		BidState(double x): Notifyprice(true), Notifysurplus(false), PurPr(x), Surplus(0), Round(1) {} 
	};

	/**
	 * Insertion operator for BuyerState objects. It only displays the value of Notify.
	 * @param out output stream.
	 * @param s state to be represented in the output stream.
	 * @return output stream with Notify already inserted.
	 */
	std::ostream& operator<<(std::ostream& out, const BidState& s) {
		out << "{" << s.Notifyprice <<"," << s.Round << "," << s.PurPr << "," << s.Notifysurplus << "," << s.Surplus << "}";
		return out;
	}

	//! Atomic DEVS model of a Buyer.
	class Bid : public Atomic<BidState> { //! Atomic models MUST inherit from the cadmium::Atomic<S> class
	 private:
		int ID_b;                            //!< 
		double RPr;
		double PPrStep;
		double InitialPPr;
	 public:
		Port<Feedback> in;         //!< Input Port for receiving the id of the item.
		Port<Bidinfo> out;			//!< Input Port for receiving the feedback from the auctioneer.
		Port<Surplusinfo> surpl;    // Output port to report surplus in the current round
		//BigPort<Job> outGenerated;  //!< Output Port for sending new Job objects to be processed.

		/**
		 * Constructor function for Generator DEVS model.
		 * @param id model ID.
		 * @param jobPeriod Job generation period.
		 */
		Bid(const std::string& id, int _ID, double _RPr, double _PPrStep, double _InitialPPr): Atomic<BidState>(id, BidState(_InitialPPr)), ID_b(_ID), RPr(_RPr), PPrStep(_PPrStep), InitialPPr(_InitialPPr){//, jobPeriod(jobPeriod) {
			in = addInPort<Feedback>("in");
			out = addOutPort<Bidinfo>("out");
			surpl = addOutPort<Surplusinfo>("surplus");
			//RPr = RPr_i;
			//PPrStep = PPrStep_i;
			//InitialPPr = InitialPPr_i;
			//s.Notify = true;
			//s.PurPr = _InitialPPr;
			//outGenerated = addOutBigPort<Job>("outGenerated");
		}

		/**
		 * Updates BuyerState::Notify. If it is active, then it is passivated.
		 * @param s reference to the current generator model state.
		 */
		void internalTransition(BidState& s) const override {
			if (s.Notifyprice == true)
			{
				s.Notifyprice = false;
			}
			if (s.Notifysurplus == true)
			{
				s.Notifysurplus = false;
			}
		}

		/**
		 * Updates BuyerState::Notify and BuyerState::PPrStep.
		 * If it receives a true message via the Buyer::inMsg port and the reservation priced is not reached, it activates and updates the PPr.
		 * @param s reference to the current buyer model state.
		 * @param e time elapsed since the last state transition function was triggered.
		 * @param x reference to the atomic model input port set.
		 */
		void externalTransition(BidState& s, double e) const override {
			//s.sigma = std::max(s.sigma - e, 0.);
			//s.PurPr = InitialPPr;
			Feedback local_in(0, false);
			//Feedback local_in;
			local_in = in->getBag().back();
			s.Round +=1;
			//CRM - Calculate my surplus
			if(local_in.gotIt == false){
				s.Surplus = 0;
			}else{
				s.Surplus = RPr - s.PurPr;
			}
			s.Notifysurplus = true;
			//CRM - Update purchase price if I did not get the item
			if (local_in.gotIt == false && s.PurPr + PPrStep <= RPr)
			{
				//July 2024: PPrStep is now the minimum bid increment
				// Original
				//s.PurPr += PPrStep;
				
				double bid_increment = PPrStep;
				
						
				//Inc = lambda · redondear_arriba {N· [reservation_price – current_bid] / lambda}
				//srand((unsigned int)time(0));
				//double random_number = (double)rand()/(double)RAND_MAX;
				//std::cout << "Random number: " << random_number << "\n";
				// Duda: cuando pongo s. y cuando el nombre de la variable directamente
				//double bid_increment = PPrStep * std::ceil(random_number*((RPr - s.PurPr)/PPrStep));
				//std::cout << "Bid increment: " << bid_increment << "\n";
				s.PurPr += bid_increment;
				
				s.Notifyprice = true;
			}
		}

		/**
		 * Sends new messages to the auctioneer via the Buyer::outItemId and Buyer::outPurPr ports.
		 * @param s reference to the current generator model state.
		 * @param y reference to the atomic model output port set.
		 */
		void output(const BidState& s) const override {
			if (s.Notifyprice == true)
			{
				Bidinfo outmsg = {ID_b, s.PurPr};
				out->addMessage(outmsg);
				
			}
			if(s.Notifysurplus == true){
				Surplusinfo surp = {s.Round - 1, s.Surplus};
				surpl ->addMessage(surp);
			}
			
			// outGenerated->addMessage(Job(s.jobCount, s.clock + s.sigma)); // TODO we could also do this
		}

		/**
		 * If the buyer is active, it returns a time advance of 1; otherwise it sets ta to infinity.
		 * @param s reference to the current generator model state.
		 * @return the sigma value.
		 */
		[[nodiscard]] double timeAdvance(const BidState& s) const override {
			if (s.Notifyprice == true || s.Notifysurplus == true)
			{
				return 1.0;
			} else {
				return std::numeric_limits<double>::infinity();
			}
			// std::numeric_limits<double>::infinity();
		}
	};
}  //namespace cadmium::example::auction

#endif //CADMIUM_EXAMPLE_AUCTION_BID_HPP_
