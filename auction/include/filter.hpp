#ifndef CADMIUM_EXAMPLE_AUCTION_FILTER_HPP_
#define CADMIUM_EXAMPLE_AUCTION_FILTER_HPP_

#include <cadmium/modeling/devs/atomic.hpp>
#include <iostream>
#include <vector>
#include "feedback.hpp"


namespace cadmium::example::auction {
	//! Class for representing the Filter DEVS model state.
	struct FilterState {
		//double sigma;  //!< 
		bool S1;  //!< True/False variable to indicate the active/passive state of the model.
		bool gotIt;  //!< True/False variable inputted from the auctioneer.

		//! Constructor function. It sets all the attributes to 0.
		FilterState(): S1(false), gotIt() {}
	};

	/**
	 * Insertion operator for FilterState objects. It only displays the value of S1.
	 * @param out output stream.
	 * @param s state to be represented in the output stream.
	 * @return output stream with S1 already inserted.
	 */
	std::ostream& operator<<(std::ostream& out, const FilterState& s) {
		out << "{" << s.S1 <<"," << s.gotIt << "}";
		return out;
	}

	//! Atomic DEVS model of a Filter.
	class Filter : public Atomic<FilterState> { //! Atomic models MUST inherit from the cadmium::Atomic<S> class
	 private:
		int ID_f;                            //!< ID assigned to the filter instance.
	 public:
		Port<Feedback> in;         //!< Input Port for receiving the id of the item.
		Port<Feedback> out;		//!< Output Port for sending the id of the item.
		//BigPort<Job> outGenerated;  //!< Output Port for sending new Job objects to be processed.

		/**
		 * Constructor function for Filter DEVS model.
		 * @param id model ID.
		
		 */
		Filter(const std::string& id, int _ID): Atomic<FilterState>(id, FilterState()), ID_f(_ID){//, jobPeriod(jobPeriod) {
			in = addInPort<Feedback>("in");
			out = addOutPort<Feedback>("out");
			//S1 = false;
			//ID_f = id;
			//outGenerated = addOutBigPort<Job>("outGenerated");
		}

		/**
		 * Updates FilterState::S1. If it is active, then it is passivated.
		 * @param s reference to the current filter model state.
		 */
		void internalTransition(FilterState& s) const override {
			if (s.S1 == true)
			{
				s.S1 = false;
			}
			
			//s.sigma = std::numeric_limits<double>::infinity();
		}

		/**
		 * Updates FilterState::S1 and FilterState::gotIt.
		 * If it receives a true message via the Filter::inItemId port, it activates and reads the message of that input port.
		 * @param s reference to the current filter model state.
		 * @param e time elapsed since the last state transition function was triggered.
		 * @param x reference to the atomic model input port set.
		 */
		void externalTransition(FilterState& s, double e) const override {
			//s.sigma = std::max(s.sigma - e, 0.);
			Feedback local_in(0, false);
						
			if(!in->empty())
			{
				
				for (auto& msg: in->getBag()){
				/*
				int num_ag = in->getBag().size();
				std::cout << num_ag << "\n";
				
				for(int i=0; i<num_ag;i++){
					local_in = in->getBag().at(i);
					//auto& inp: in->getBag();
					//std::cout << inp << " - ";
					
					std::cout << ID_f << "-" << local_in.id << " - "<< local_in<< "\n";
				*/
					//std::cout << ID_f << "-" << msg.id << " - "<< msg << "\n";
					if (ID_f == msg.id)
					{
						s.gotIt = msg.gotIt;
						s.S1 = true;
					}
				}
				//std::cout << "\n";
			}
			
		}

		/**
		 * Sends new messages to the buyer/seller via the Filter::outItemId and Filter::outMsg ports.
		 * @param s reference to the current filter model state.
		 * @param y reference to the atomic model output port set.
		 */
		void output(const FilterState& s) const override {
			if (s.S1 == true)
			{
				Feedback outmsg = {ID_f, s.gotIt};
				out->addMessage(outmsg);
			}
			
			// outGenerated->addMessage(Job(s.jobCount, s.clock + s.sigma)); // TODO we could also do this
		}

		/**
		 * If the filter is active, it returns a time advance of 1; otherwise it sets ta to infinity.
		 * @param s reference to the current filter model state.
		 * @return the sigma value.
		 */
		[[nodiscard]] double timeAdvance(const FilterState& s) const override {
			if (s.S1 == true)
			{
				return 1.0;
			} else {
				return std::numeric_limits<double>::infinity();
			}
			// std::numeric_limits<double>::infinity();
		}
	};
}  //namespace cadmium::example::gpt

#endif //CADMIUM_EXAMPLE_GPT_FILTER_HPP_
