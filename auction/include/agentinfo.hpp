#ifndef CADMIUM_EXAMPLE_AUCTION_AGENTINFO_HPP_
#define CADMIUM_EXAMPLE_AUCTION_AGENTINFO_HPP_

#include <iostream>

namespace cadmium::example::auction {
	//! Bid messages. Buyer objects create new bids and send them to the Auctioneer.
	struct Agentinfo {
		int id;         //!< ID number of the bid.
		double Price;  	//!< Purchase price for the current round.
        bool gotIt;
		
		/**
		 * Constructor function for a Bid object.
		 * @param id  ID of the new bid/ask.
		 * @param InitialPPr As the first bid comes as an input, we generate the corresponding parameter. 
		 * @ref Purpr is initialized as the initial purchase price.
		 */
		Agentinfo(int _id, double _Price): id(_id), Price(_Price), gotIt(false) {};
	};

	/**
	 * Insertion operator for Bid objects.
	 * @param out output stream.
	 * @param b bid to be represented in the output stream.
	 * @return output stream with the value of the bid already inserted.
	 */
	std::ostream& operator<<(std::ostream& out, const Agentinfo& a) {
		out << "{" << a.id << "," << a.Price << "," << a.gotIt <<"}";
		return out;
	}
}  //namespace cadmium::example::auction

#endif //CADMIUM_EXAMPLE_AUCTION_AGENTINFO_HPP_