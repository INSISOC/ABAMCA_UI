#ifndef CADMIUM_EXAMPLE_AUCTION_SURPLUSINFO_HPP_
#define CADMIUM_EXAMPLE_AUCTION_SURPLUSINFO_HPP_ 

#include <iostream>

namespace cadmium::example::auction {
	//! Bid messages. Buyer objects create new bids and send them to the Auctioneer.
	struct Surplusinfo {
		int round;         //!< round of the bid.
		double surplus;  	//!< Surplus
		
		/**
		 * Constructor function for a Bid object.
		 * @param _round  ID of the new bid/ask.
		 * @param _surplus As the first bid comes as an input, we generate the corresponding parameter. !!!I don't think this is done here
		 * @ref Purpr is initialized as the initial purchase price.
		 */
		Surplusinfo():round(0), surplus(0) {};
		Surplusinfo(int _round, double _surplus): round(_round), surplus(_surplus) {};
	};

	/**
	 * Insertion operator for Bid objects.
	 * @param out output stream.
	 * @param b bid to be represented in the output stream.
	 * @return output stream with the value of the bid already inserted.
	 */
	std::ostream& operator<<(std::ostream& out, const Surplusinfo& b) {
		out << "{" << b.round << "," << b.surplus << "}";
		return out;
	}
}  //namespace cadmium::example::auction

#endif //CADMIUM_EXAMPLE_AUCTION_SURPLUSINFO_HPP_ 
