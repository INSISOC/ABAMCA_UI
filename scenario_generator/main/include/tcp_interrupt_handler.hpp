#ifndef TCP_INTERRUPT_HANDLER_HPP
#define TCP_INTERRUPT_HANDLER_HPP

#include "cadmium/simulation/rt_clock/interrupt_handler.hpp"
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <variant>
#include <assert.h>
#include <b_reg_info.hpp>
#include <s_reg_info.hpp>

using asyncPortType = std::variant<b_reg_info, s_reg_info>;

#define TCP_PORT 8080

class tcpInterruptHandler : public InterruptHandler<asyncPortType> {
    private:
        sockaddr_in serverAddress;
        int serverSocket;
        char buffer[1024];

    public:
        tcpInterruptHandler(): buffer({ 0 }) {
            // creating socket
            serverSocket = socket(AF_INET, SOCK_STREAM, 0);

            // specifying the address
            serverAddress.sin_family = AF_INET;
            serverAddress.sin_port = htons(TCP_PORT);
            serverAddress.sin_addr.s_addr = INADDR_ANY;

            int flags = -1;
            int err = -1;
            /* Set socket to non-blocking */ 
            if ((flags = fcntl(serverSocket, F_GETFL, 0)) < 0) { 
                /* Handle error */ 
                std::cout << "flags: " << flags << std::endl;
            } 

            if (err = fcntl(serverSocket, F_SETFL, flags | O_NONBLOCK) < 0) { 
                /* Handle error */ 
                std::cout << "err: " << err << std::endl;
            }

            // binding socket.
            bind(serverSocket, (struct sockaddr*)&serverAddress,
                sizeof(serverAddress));

            // listening to the assigned socket
            listen(serverSocket, 10);

        }

        bool ISRcb() {
            // accepting connection request
            int clientSocket = accept(serverSocket, nullptr, nullptr);
            int recvd = recv(clientSocket, buffer, sizeof(buffer), MSG_WAITALL);
            if(recvd <= 0) {
                return false;
            } else {
                return true;
            }
        }

        std::pair<asyncPortType, std::string> decodeISR() {

            //Process the buffer
            //string variable with all that information
            std::cout << "Arrival message:" << buffer << std::endl;

            //std::string msg(buffer);
            if (buffer[0] == 'B'){
                b_reg_info value_b = parse_b_reg_info(buffer);
                std::cout << "Departure message" << value_b << std::endl;
                 //Clears buffer
                std::fill_n(buffer, 1024, 0);  
                return std::make_pair(value_b, std::string("b_in"));
            }else if (buffer[0] == 'S'){
                s_reg_info value_s = parse_s_reg_info(buffer);
                std::cout << "Departure message" << value_s << std::endl;
                 //Clears buffer
                std::fill_n(buffer, 1024, 0);  
                return std::make_pair(value_s,  std::string("s_in"));
            }else{
                assert(true);
            }              
        }

        b_reg_info parse_b_reg_info(const std::string s) {
            int a, b, c, d;       
            std::string temp = "";
            int comma_counter = 0;
            std::cout<<"Inside buyer parser: " << s << std::endl;
        
            for(int i = 2; i < s.size(); i++) {
                if(s[i] != ',') {
                    temp += s[i];
                } else {
                    //std:: cout << "temp " << temp << std::endl;
                    switch(comma_counter) {
                        case 0:{
                            a = stoi(temp);
                            break;
                        }
                        case 1:{
                            b = stoi(temp);
                            break;
                        }
                        case 2:{
                            c = stoi(temp);
                            break;
                        }
                        case 3:{
                            d = stoi(temp);
                            break;
                        }
                    }
                    temp = "";
                    comma_counter++;
            }
            }   
            return b_reg_info(a, b, c, d);
        }

        s_reg_info parse_s_reg_info(const std::string s) {        
            int id, pp, pc;
            std::vector<int> ord;        
            std::string temp = "";
            int comma_counter = 0;

            std::cout<<"Inside Seller parser: " << s << std::endl;
        
            for(int i = 2; i < s.size(); i++) {
                if(s[i] != ',') {
                    temp += s[i];
                } else {
                    //std:: cout << "temp " << temp << std::endl;
                    switch(comma_counter) {
                        case 0:{
                            id = stoi(temp);
                            //std:: cout << "id " << id << std::endl;
                            break;
                        }
                        case 1:{
                            std::string temp2 = "";
                            //std:: cout << "case 1 temp " << temp << std::endl;
                            for(int e = 1; e < temp.size(); e++) {
                            //std:: cout << "case 1 temp[e] " << temp[e] << std::endl;
                                if(temp[e] != ';' && temp[e] != ']') {
                                    temp2 += temp[e];
                                    //std:: cout << "if temp2 " << temp2 << std::endl;
                                }else{
                                    //std:: cout << "else temp2 " << temp2 << std::endl;
                                    ord.push_back(stoi(temp2));
                                }    
                            }   
                            break;
                        }
                        case 2:{
                            pp = stoi(temp);
                            //std:: cout << "pp " << pp << std::endl;
                            break;
                        }
                        case 3:{
                            pc = stoi(temp);
                            //std:: cout << "pc " << pc << std::endl;
                            break;
                        }
                    }
                    temp = "";
                    comma_counter++;
                }
            }
            /*std::cout << "Leaving Seller parser" << std::endl;
            std::cout << id << " [" << std::endl;
            for (int i=0; i<ord.size()-1; i++){
                 std::cout << ord[i] << ",";
            }
             std::cout << "]" << pp << pc << std::endl;
             */
            return s_reg_info(id, ord, pp, pc);    
            
        }



};

#endif //TCP_INTERRUPT_HANDLER_HPP
