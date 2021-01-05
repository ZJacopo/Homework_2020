#include <string>
using std::string;
#include <iostream>
#include <fstream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include "Shop.h"
#include "Rob_server.h"

std::mutex c_mutex;

Shop shop(15);
Rob_server rob_server;

void new_connection(int sock){
	string robot_state{"OFF"};
	std::cout<<"robot state: "<<robot_state<<"\n";
	
	//shop.stop_robot();
	int actual_cl;
	
	bool continue_to_operate{true};
	bool turn_off_robot{false};
	
	here:
	{
		shop.stop_robot();
		actual_cl = shop.get_clients();
		
		if(actual_cl >= 2){
			if(send(sock, "robot ON", 8, 0) == -1){std::cerr<<"send\n";}
			robot_state="ON";
			std::cout<<"robot state: "<<robot_state<<"\n";
		}
		else if(actual_cl <= 1){
			if(send(sock, "robot OFF", 9, 0) == -1){std::cerr<<"send\n";}
			robot_state="OFF";
			std::cout<<"robot state: "<<robot_state<<"\n";
		}
		
		bool turn_off_robot = shop.get_is_open();
		if(turn_off_robot){
			if(send(sock, "robot close", 11, 0) == -1){std::cerr<<"send\n";}
			robot_state="OFF and close";
			continue_to_operate=false;
			std::cout<<"robot state: "<<robot_state<<"\n";
		}
	}	
		
	if(continue_to_operate){goto here;}
	close(sock);
}

void *get_in_addr(struct sockaddr *sa){
	
	if(sa->sa_family == AF_INET){
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int accept_loop(const char* port){
	
	int sock = rob_server.make_accept_sock(port);
	
	int new_sock = accept(sock, 0, 0);

	if(new_sock <0){
		std::cerr<<"server: filed on accept \n";
		exit(EXIT_FAILURE);
	}
	
	return new_sock;
}

string find_real_entrance(string entering_client, string exit_hhmm){
	 
	int new_id_hh = std::stoi(entering_client.substr(10,2));
	int new_id_mm = std::stoi(entering_client.substr(13));
	int exit_id_hh = std::stoi(exit_hhmm.substr(0,2));//hh:mm
	int exit_id_mm = std::stoi(exit_hhmm.substr(3));
	
	string ret_hhmm = std::to_string(new_id_hh)+":"+std::to_string(new_id_mm);
	
	if((new_id_hh > exit_id_hh) || ((new_id_hh == exit_id_hh)&&(new_id_mm > exit_id_mm))){
		ret_hhmm = std::to_string(new_id_hh)+":"+std::to_string(new_id_mm);
		}
		else {
			ret_hhmm = std::to_string(exit_id_hh)+":"+std::to_string(exit_id_mm);
			}

	return ret_hhmm;
}

void set_window(){
	
	bool window_online{true};
	bool this_shop_is_open{false};
	
	while(window_online){
		this_shop_is_open = shop.get_is_open();

		if(this_shop_is_open){

			string reg_100 = shop.set_value_window();
			c_mutex.lock();
			std::cout<<"window open: "<<reg_100<<"\n";
			c_mutex.unlock();
		}
		else{window_online = false;}
	}
	
	
}
void enter_shop(){
	
	std::ifstream rd_arrivals("arrive_shop.txt");
	std::queue<string> arrival_queue;				// here are queued all clients that arrives when shop is full
	
	string new_client;
	
	bool there_is_someone{true};
	bool allow_queue{false};
	bool other_entrance{true};
	
	int cl{0};					
	const int MAX_CAP=15;
		
	
	while(there_is_someone){
		
		if(getline(rd_arrivals,new_client) && other_entrance){
			
			bool enter_shop;
			enter_shop = shop.sort_client(arrival_queue.size());

			if(enter_shop){
				shop.enter_client(new_client);
				c_mutex.lock();
				std::cout<<"client enters from outside: "<<new_client<<"\n";
				c_mutex.unlock();
			}
			else{
				arrival_queue.push(new_client); 
				c_mutex.lock();
				std::cout<<"client enters in queue: "<<new_client<<"\n";
				c_mutex.unlock();
				shop.set_arrival_queue(true);
				
				}
		}
		else {other_entrance = false;}	
		
		cl = shop.get_clients();
		allow_queue = shop.get_exit_queue();

		
		if((arrival_queue.size()!= 0) && allow_queue && cl<MAX_CAP){
		
			string hhmm_info;
			string new_id_queue;
			string new_client_queue;
			
			hhmm_info = shop.entrance_from_queue();
			
			string hhmm_real = find_real_entrance(arrival_queue.front(), hhmm_info);
			
			new_id_queue = (arrival_queue.front()).substr(0,9);
			arrival_queue.pop();
			if((arrival_queue.size()== 0)){shop.set_arrival_queue(false);}
			
			new_client_queue = new_id_queue+" "+hhmm_real;

			c_mutex.lock();
			std::cout<<"client enters from queue: "<<new_client_queue<<"\n";
			c_mutex.unlock();

			shop.enter_client(new_client_queue);
		}

		if(!other_entrance && (arrival_queue.size()== 0)){there_is_someone = false;}
	
	}
}
void exit_shop(){
	
	std::ifstream rd_exit("exit_shop.txt");		
	std::ofstream wr_exit("out_shop.txt");		//here goes who exits and his permanence inside the shop
	std::ofstream wr_stat("stat_shop.txt");		//here goes all statistics analysis
	
	std::queue<string> out_shop;				//if there is a queue in entrance, here are saved hh:mm of exits
	
	const string EMPT="this_cell_is_empty";
	
	string out_client;
	
	bool someone_inside{true};
	bool need_new_client{true};
	bool get_arr{false};		//here i want to know if there is a queue entering the shop
	
	//all these int are used for the statistic analysis
	int hh_open{8};
	int count_tot_F{0};
	int count_tot_M{0};
	int count_F{0};
	int count_M{0};
	int perm_med_F{0};
	int perm_med_M{0};
	
	while(someone_inside){
		
		if(need_new_client){
			if(getline(rd_exit,out_client)){need_new_client = false;}
			else{someone_inside=false;}
		}
		
		if(someone_inside){
			shop.is_someone_inside();
			string find_it = shop.find_client(out_client);//if i find it, i cancel it from the shop and save in find_it
			
			if(find_it.compare(EMPT)!=0){
				get_arr = shop.get_arrival_queue();//ask for information about the existence of arrival queue

				if(get_arr){
					
					shop.update_hhmm_queue(out_client.substr(10));
					c_mutex.lock();
					std::cout<<"value of hh:mm exit saved in hhmm_queue: "<<out_client.substr(10)<<"\n";
					c_mutex.unlock();
					shop.set_exit_queue(true);
				}

				shop.decrement_client();
				need_new_client = true;
				
				int h_out = std::stoi(out_client.substr(10,2)); 
				int h_it = std::stoi(find_it.substr(10,2)); 
				int m_out = std::stoi(out_client.substr(13)); 
				int m_it = std::stoi(find_it.substr(13)); 
				
				int mm_tot = (h_out*60)+(m_out)-(h_it*60)-(m_it);
				int h_tot = mm_tot/60; 
				int m_tot = mm_tot%60;
				string hhmm_permanence = "h: "+std::to_string(h_tot)+" m: "+std::to_string(m_tot);

				c_mutex.lock();
				std::cout<<"client exits: "<<find_it.substr(0,9)<<" inside for "<<hhmm_permanence<<"\n";
				c_mutex.unlock();

				wr_exit <<"client: "<<find_it.substr(0,9)<<" inside for "<<hhmm_permanence<<"\n";
				//////////
				
				if((find_it.substr(8,1)).compare("F") == 0){
					count_F++;
					perm_med_F = perm_med_F + mm_tot;
				}
				else{
					count_M++;
					perm_med_M = perm_med_M + mm_tot;
				}
				
				if(h_it > hh_open){
					
					wr_stat<<"from: "<<hh_open<<":00 -> "<<h_it<<":00 \n"; 
					hh_open = h_out;
					int med_F;
					int med_M;
					if(count_F!=0){med_F = perm_med_F/count_F;}
					else {med_F = 0;}
					if(count_M!=0){med_M = perm_med_M/count_M;}
					else {med_M = 0;}
					
					wr_stat<<"count F: "<<count_F<<"      medium permanence: "<<med_F<<" [min]\n";
					wr_stat<<"count M: "<<count_M<<"      medium permanence: "<<med_M<<" [min]\n\n";
					
					count_tot_F = count_tot_F + count_F;
					count_tot_M = count_tot_M + count_M;
					count_F = 0;
					count_M = 0;
					perm_med_F = 0;
					perm_med_M = 0;
					
				}
			}
			
		}
		if(!someone_inside){
			wr_stat<<"Total F: "<<count_tot_F<<"      Total_M: "<<count_tot_M<<"\n";
		}
		
	}
	shop.set_is_open(false);
}
int main(int argc, char* argv[]){
	
	if(argc != 2){
		std::cerr<<"usage: ./server port \n";
		exit(EXIT_FAILURE);
	}
	
	int rob_ok = accept_loop(argv[1]);
	std::thread robot_(new_connection, rob_ok);
	
	std::thread enter(enter_shop);
	std::thread exit(exit_shop);
	std::thread window(set_window);
	
	robot_.join();
	enter.join();
	exit.join();
	window.join();
return 0;
}