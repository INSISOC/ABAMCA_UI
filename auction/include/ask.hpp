#ifndef CADMIUM_EXAMPLE_AUCTION_ASK_HPP_
#define CADMIUM_EXAMPLE_AUCTION_ASK_HPP_

#include <cadmium/modeling/devs/atomic.hpp>
#include <iostream>
#include "feedback.hpp"
#include "bidinfo.hpp"
#include "surplusinfo.hpp"

namespace cadmium::example::auction {
	//! Class for representing the Generator DEVS model state.
	struct AskState {
		//double sigma;  //!< 
		bool Notifyprice;  //!< True/False variable to indicate the active/passive state of the model.
		bool Notifysurplus;  //!< True/False variable to indicate whether or not to notify the seller's surplus
		double AskPr;  //!< True/False variable inputted from the auctioneer.
		double Surplus; //!< Seller surplus = ask price - production cost
		int Round;

		//! Constructor function. It sets all the attributes to 0.
		AskState(double x): Notifyprice(true), Notifysurplus(false), AskPr(x), Surplus(0), Round(1) {} 
	};

	/**
	 * Insertion operator for ask objects. It displays the value of the notifyprice, round, price, notifysurplus and surplus.
	 * @param out output stream.
	 * @param s state to be represented in the output stream.
	 * @return output stream with jobCount already inserted.
	 */
	std::ostream& operator<<(std::ostream& out, const AskState& s) {
		out << "{" << s.Notifyprice <<"," << s.Round << "," << s.AskPr << "," << s.Notifysurplus << "," << s.Surplus << "}";

		return out;
	}

	//! Atomic DEVS model of a Ask.
	class Ask : public Atomic<AskState> { //! Atomic models MUST inherit from the cadmium::Atomic<S> class
	 private:
		int ID_s;                            //!< Time to wait between Job generations.
		double PCost;
		double APrStep;
		double InitialAPr;
	 public:
		Port<Feedback> in;          //!< Input Port for receiving stop generating Job objects.
		Port<Bidinfo> out;
		Port<Surplusinfo> surpl; // 
		//BigPort<Job> outGenerated;  //!< Output Port for sending new Job objects to be processed.

		/**
		 * Constructor function for Generator DEVS model.
		 * @param id model ID.
		 * @param _ID Job generation period.
		 */
		Ask(const std::string& id, int _ID, double _PCost, double _APrStep, double _InitialAPr): Atomic<AskState>(id, AskState(_InitialAPr)), ID_s(_ID), PCost(_PCost), APrStep(_APrStep), InitialAPr(_InitialAPr){//, jobPeriod(jobPeriod) {
			in = addInPort<Feedback>("in");
			out = addOutPort<Bidinfo>("out");
			surpl = addOutPort<Surplusinfo>("surplus");
			//s.Notify = true;
			//s.PurPr = InitialPPr;
			//outGenerated = addOutBigPort<Job>("outGenerated");
		}

		/**
		 * Updates GeneratorState::clock and GeneratorState::sigma and increments GeneratorState::jobCount by one.
		 * @param s reference to the current generator model state.
		 */
		void internalTransition(AskState& s) const override {
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
		 * Updates GeneratorState::clock and GeneratorState::sigma.
		 * If it receives a true message via the Generator::inStop port, it passivates and stops generating Job objects.
		 * @param s reference to the current generator model state.
		 * @param e time elapsed since the last state transition function was triggered.
		 * @param x reference to the atomic model input port set.
		 */
		void externalTransition(AskState& s, double e) const override {
			//s.sigma = std::max(s.sigma - e, 0.);
			Feedback local_in(0, false);
			//Feedback local_in;
			local_in = in->getBag().back();
			s.Round +=1;
			//CRM - Calculate my surplus
			if(local_in.gotIt == false){
				s.Surplus = 0;
			}else{
				s.Surplus = s.AskPr - PCost;
			}
			s.Notifysurplus = true;
			//CRM - Update purchase price if I did not get the item

			if (local_in.gotIt == false && s.AskPr - APrStep >= PCost)
			{
				s.AskPr -= APrStep;
				s.Notifyprice = true;
			}
		}

		/**
		 * Sends a new Job that needs to be processed via the Generator::outGenerated port.
		 * @param s reference to the current generator model state.
		 * @param y reference to the atomic model output port set.
		 */
		void output(const AskState& s) const override {
			if (s.Notifyprice == true)
			{
				Bidinfo outmsg = {ID_s, s.AskPr};
				out->addMessage(outmsg);
				//out->addMessage(outmsg);
			}
			
			if(s.Notifysurplus == true)
			{
				Surplusinfo surp = {s.Round - 1, s.Surplus};
				surpl ->addMessage(surp);
			}
			// outGenerated->addMessage(Job(s.jobCount, s.clock + s.sigma)); // TODO we could also do this
		}

		/**
		 * It returns the value of GeneratorState::sigma.
		 * @param s reference to the current generator model state.
		 * @return the sigma value.
		 */
		[[nodiscard]] double timeAdvance(const AskState& s) const override {
			if (s.Notifyprice == true || s.Notifysurplus == true)
			{
				return 1;
			} else {
				return std::numeric_limits<double>::infinity();
			}
			// std::numeric_limits<double>::infinity();
		}
	};
}  //namespace cadmium::example::auction

#endif //CADMIUM_EXAMPLE_AUCTION_ASK_HPP_
