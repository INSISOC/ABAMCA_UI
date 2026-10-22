#include <limits>
#include "include/auction_configuration.hpp"

/*
There are 3 macros defined at compile time that changes the behaviour of the simulation.
--> SIM_TIME: This macro, when defined, runs the simulation in simulation time. Else, the simulation runs at wall clock.
--> ESP_PLATFORM: When defined, the models are compiled for the ESP32 microcontroller. Else, compiles for Linux/ Windows
--> NO_LOGGING: When defined, prevents logging (maybe useful in embedded situations)
*/


#ifdef SIM_TIME
	#include "cadmium/simulation/root_coordinator.hpp"
#else
	#include "cadmium/simulation/rt_root_coordinator.hpp"
	#ifdef ESP_PLATFORM
		#include <cadmium/simulation/rt_clock/ESPclock.hpp>
	#else
		#include <cadmium/simulation/rt_clock/chrono.hpp>
	#endif
#endif

#ifndef NO_LOGGING
	#include "cadmium/simulation/logger/stdout.hpp"
	#include "cadmium/simulation/logger/csv.hpp"
#endif

using namespace cadmium;

extern "C" {
	#ifdef ESP_PLATFORM
		void app_main() //starting point for ESP32 code
	#else
		int main()		//starting point for simulation code
	#endif
	{
	
		auto model = std::make_shared<auction_configuration> ("auction_configuration top model", 5, 7);
		
		#ifdef SIM_TIME
			auto rootCoordinator = cadmium::RootCoordinator(model);
		#else
			#ifdef ESP_PLATFORM
				cadmium::ESPclock clock;
				auto rootCoordinator = cadmium::RealTimeRootCoordinator<cadmium::ESPclock<double>>(model, clock);
			#else
				cadmium::ChronoClock clock;
				auto rootCoordinator = cadmium::RealTimeRootCoordinator<cadmium::ChronoClock<std::chrono::steady_clock>>(model, clock);
			#endif
		#endif

		#ifndef NO_LOGGING
		//rootCoordinator.setLogger<STDOUTLogger>(";");
		rootCoordinator.setLogger<CSVLogger>("log_output.csv", ";");
		#endif

		remove("pool_of_orders.txt");
		remove("data_auction.txt");

		rootCoordinator.start();
		#ifdef ESP_PLATFORM
			rootCoordinator.simulate(std::numeric_limits<double>::infinity());
		#else
			rootCoordinator.simulate(23.0);
		#endif
		rootCoordinator.stop();	

		#ifndef ESP_PLATFORM
			return 0;
		#endif
	}
}