#ifndef CADMIUM_EXAMPLE_AUCTION_ALLOCATION_HPP_
#define CADMIUM_EXAMPLE_AUCTION_ALLOCATION_HPP_

#include <iostream>
#include <bidinfo.hpp>

namespace cadmium::example::auction {
	//! Bid messages. Buyer objects create new bids and send them to the Auctioneer.
	struct Allocation {
		Bidinfo bids(" ",-1.0);         //!< ID number of the bid.
		Bidinfo asks(" ",-1.0);  	//!< Purchase price for the current round.
		
		/**
		 * Constructor function for a Bid object.
		 * @param id  ID of the new bid/ask.
		 * @param InitialPPr As the first bid comes as an input, we generate the corresponding parameter. !!!I don't think this is done here
		 * @ref Purpr is initialized as the initial purchase price.
		 */
		Allocation(): id(), Price() {};
	};

	/**
	 * Insertion operator for Bid objects.
	 * @param out output stream.
	 * @param b bid to be represented in the output stream.
	 * @return output stream with the value of the bid already inserted.
	 */
	std::ostream& operator<<(std::ostream& out, const Allocation& al) {
		out << "{" << al.bids << "," << al.asks << "}";
		return out;
	}
}  //namespace cadmium::example::auction

#endif //CADMIUM_EXAMPLE_AUCTION_BIDINFO_HPP_
