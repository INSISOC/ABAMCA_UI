#include <cadmium/simulation/logger/csv.hpp>
// #include <cadmium/simulation/root_coordinator.hpp>

//The following 3 include files helps with RT. These are the coordinator, cadmium clock and c++ clock
#include <cadmium/simulation/rt_root_coordinator.hpp>
#include <cadmium/simulation/rt_clock/chrono.hpp>
#include <chrono>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <limits>
#include "auction.hpp"
#include "input_data.hpp"
#include "auction_analysis.hpp"

using namespace cadmium::example::auction;

int main(int argc, char *argv[]) {    
    // First, we parse the arguments
    if (argc < 2) {
        std::cerr << "ERROR: not enough arguments" << endl;
        return -1;
    }
    int roundTime = std::stoi(argv[1]);
    if(roundTime < 1) {
        std::cerr << "ERROR: round time less than 1" << endl;
        return -1;
    }
    // We create the model and forward it to the root coordinator
    // NOTE THAT WE NEED A POINTER TO THE MODEL!
    string name_f = "data_auction.txt";
    vector<double> RPr, InitialPPr, PCost, InitialAPr;
    int PPrStep, APrStep;
    double maximum_social_welfare;
    vector<vector<int>> matrix;
    data_auction l_data = read_data(name_f);
    matrix = l_data._Matrix;
    std::vector<int> ID_b;
    for (int i=1; i<=matrix[0].size(); i++){
        ID_b.push_back(i);
    }
    RPr = l_data._RPr;
    InitialPPr = l_data._InitialPPr;
    PPrStep = l_data._PPrStep;
    maximum_social_welfare = l_data._maximum_social_welfare;
    std::vector<int> ID_s;
    for (int j=1; j<=matrix.size(); j++){
        ID_s.push_back(j);
    }
    PCost = l_data._PCost;
    InitialAPr = l_data._InitialAPr;
    APrStep = l_data._APrStep;
    matrix = l_data._Matrix;

    cout << "Maximum social welfare: " << endl;
    cout << maximum_social_welfare << endl;
    cout << "RPr: " << endl;
    for (int i=0; i<RPr.size(); i++){
        cout << RPr[i] << " ";
    }
    cout << endl;
    cout << "InitialPPr: " << endl;
    for (int i=0; i<InitialPPr.size(); i++){
        cout << InitialPPr[i] << " ";
    }
    cout << endl;
    cout << "PPrStep:" << endl;
    cout << PPrStep << endl;
    cout << "PCost: " << endl;
    for (int i=0; i<PCost.size(); i++){
        cout << PCost[i] << " ";
    }
    cout << endl;
    cout << "InitialAPr: " << endl;
    for (int i=0; i<InitialAPr.size(); i++){
        cout << InitialAPr[i] << " ";
    }
    cout << endl;
    cout << "APrStep:" << endl;
    cout << APrStep << endl;
    cout << "Items Matrix:" << endl;
    for(int j=0; j<matrix.size(); j++){
        for(int k=0; k<matrix[0].size(); k++){
            cout << matrix [j][k] << " ";
        }
        cout << endl;
    }
    
    //AGO 2024: maximum_social_welfare. Cascadear (revisar los constructores de los modelos que hay debajo para que recepcionen el valor, el orden de los parámetros matters)
    //AGO 2024: Modification of data_auction.txt and input_data.hpp to read the theoretical maximum social welfare
    
    // Modify the following line to include the compilation of the new auction_analysis model
    //auto model = std::make_shared<auction>("auction",  ID_b,RPr, PPrStep, InitialPPr ,ID_s ,PCost , APrStep, InitialAPr , matrix, roundTime); 
    //auto model = std::make_shared<auction_analysis>("auction_analysis",  ID_b,RPr, PPrStep, InitialPPr ,ID_s ,PCost , APrStep, InitialAPr , matrix, roundTime);
    auto model = std::make_shared<auction_analysis>("auction_analysis", maximum_social_welfare, ID_b,RPr, PPrStep, InitialPPr ,ID_s ,PCost , APrStep, InitialAPr , matrix, roundTime);


   // auto model = std::make_shared<auction>("auction",{"1","2","3"}, {40.0,30.0,20.0}, 5.0, {20.0,15.0,10.0}, {"1","2","3"}, {20.0,30.0,50.0}, 10.0, {30.0,50.0,70.0}, 4.0); 

    // Next, we create the coordinator and the logger
    // We can add a logger to the root coordinator
    // In this example, we use a CSV logger that uses ; to separate columns.
    // Simulation results will be stored in the log_gpt.csv file.
    
    // auto rootCoordinator = cadmium::RootCoordinator(model);

    //Instead of the virtualtime coordinator, we instantiate the real time coordinator with a clock
    cadmium::ChronoClock clock;
	auto rootCoordinator = cadmium::RealTimeRootCoordinator<cadmium::ChronoClock<std::chrono::steady_clock>>(model, clock);

    rootCoordinator.setLogger<cadmium::CSVLogger>("log_auction.csv", ";");

    // *** POZA JUNE 2024 ***
    remove("output.csv"); // delete previous simulations' output file before the current simulation starts

    // To start the simulation we 1) start, 2) simulate 3) stop.
    rootCoordinator.start();
    // in simulate method, we can select a maximum simulation time (in this case, infinity)
    rootCoordinator.simulate((double) 120);
    // rootCoordinator.simulate(std::numeric_limits<double>::infinity());
    rootCoordinator.stop();

    return 0;
}
