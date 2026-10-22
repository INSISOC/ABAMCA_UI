#ifndef CADMIUM_EXAMPLE_AUCTION_BIDINFO_HPP_
#define CADMIUM_EXAMPLE_AUCTION_BIDINFO_HPP_

#include <iostream>

namespace cadmium::example::auction {
	//! Bid messages. Buyer objects create new bids and send them to the Auctioneer.
	struct Bidinfo {
		int id;         //!< ID number of the bid.
		double Price;  	//!< Purchase price for the current round.
		
		/**
		 * Constructor function for a Bid object.
		 * @param id  ID of the new bid/ask.
		 * @param InitialPPr As the first bid comes as an input, we generate the corresponding parameter. !!!I don't think this is done here
		 * @ref Purpr is initialized as the initial purchase price.
		 */
		Bidinfo(int _id, double _Price): id(_id), Price(_Price) {};
	};

	/**
	 * Insertion operator for Bid objects.
	 * @param out output stream.
	 * @param b bid to be represented in the output stream.
	 * @return output stream with the value of the bid already inserted.
	 */
	std::ostream& operator<<(std::ostream& out, const Bidinfo& b) {
		out << "{" << b.id << "," << b.Price << "}";
		return out;
	}
}  //namespace cadmium::example::auction

#endif //CADMIUM_EXAMPLE_AUCTION_BIDINFO_HPP_
