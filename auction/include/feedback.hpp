#ifndef CADMIUM_EXAMPLE_AUCTION_FEEDBACK_HPP_
#define CADMIUM_EXAMPLE_AUCTION_FEEDBACK_HPP_

#include <iostream>

namespace cadmium::example::auction {
	//! Feedback messages. Auctioneer objects create feedback messages after solving the WDP and sends them to the auction participants.
	struct Feedback {
		int id;                //!< ID number of the bid/ask.
		bool gotIt;  //!< If it is winning the current auction round or not.
		
		/**
		 * Constructor function for a Feedback object. As messages are generated once WDP is solved, @ref gotIt is set to False.
		 * @param id  ID of the new bid/ask.
		 */
		Feedback(int _id, bool _gotIt): id(_id), gotIt(_gotIt) {};
	};

	/**
	 * Insertion operator for Feedback objects.
	 * @param out output stream.
	 * @param f feeback message to be represented in the output stream.
	 * @return output stream with the value of the feedback message already inserted.
	 */
	std::ostream& operator<<(std::ostream& out, const Feedback& f) {
		out << "{" << f.id << "," << f.gotIt << "}";
		return out;
	}

	/*std::istream& operator>> (std::istream& is, Feedback& f) {
        is >> f.id >> f.gotIt;
        return is;
    }*/
}  //namespace cadmium::example::auction

#endif //CADMIUM_EXAMPLE_AUCTION_FEEDBACK_HPP_
