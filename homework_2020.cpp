#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include "Shop.h"

std::mutex c_mutex;

Shop shop(15);

void set_window(){
	
	bool window_online{true};
	bool this_shop_is_open{false};
	
	while(window_online){
		this_shop_is_open = shop.get_is_open();
		
		if(this_shop_is_open){
			
			std::string reg_100 = shop.set_value_window();
			c_mutex.lock();
			std::cout<<"window open: "<<reg_100<<"\n";
			c_mutex.unlock();
		}
		else{window_online = false;}
	}
	
	
	
	//shop.set_wndw();
}

void enter_shop(){
	
	std::ifstream rd_arrivals("arrive_shop.txt");
	std::queue<std::string> arrival_queue;
	std::string new_client;
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
		
			std::string hhmm_info;
			std::string new_id_queue;
			std::string new_client_queue;
			std::string allow_again;
						
			new_id_queue = (arrival_queue.front()).substr(0,9);
			arrival_queue.pop();
			if((arrival_queue.size()== 0)){shop.set_arrival_queue(false);}
			
			hhmm_info = shop.entrance_from_queue();
			
			new_client_queue = new_id_queue+" "+hhmm_info;
			
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
	std::ofstream wr_exit("out_shop.txt");
	std::queue<std::string> out_shop;
	const std::string EMPT="this_cell_is_empty";
	
	std::string out_client;
	
	bool someone_inside{true};
	bool need_new_client{true};
	bool get_arr{false};
	
	while(someone_inside){
		
		if(need_new_client){
			if(getline(rd_exit,out_client)){need_new_client = false;}
			else{someone_inside=false;}
		}
		
		if(someone_inside){
			shop.is_someone_inside();
			std::string find_it = shop.find_client(out_client);//if i find it, i cancel it from the shop and save in find_it
			
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
				
				int h_out = std::stoi(out_client.substr(10,12)); 
				int h_it = std::stoi(find_it.substr(10,12)); 
				int m_out = std::stoi(out_client.substr(13)); 
				int m_it = std::stoi(find_it.substr(13)); 
				
				int m_tot = (h_out*60)+(m_out)-(h_it*60)-(m_it);
				h_out = m_tot/60; 
				m_out = m_tot%60;
				std::string hhmm_permanence = "h: "+std::to_string(h_out)+" m: "+std::to_string(m_out);
				
				c_mutex.lock();
				std::cout<<"client exits: "<<find_it.substr(0,9)<<" inside for "<<hhmm_permanence<<"\n";
				c_mutex.unlock();

				wr_exit <<"client: "<<find_it.substr(0,9)<<" inside for "<<hhmm_permanence<<"\n";
			}
		}
		
	}
	shop.set_is_open(false);
}

int main(){
	//shop.print_shop();
	
	std::thread enter(enter_shop);
	std::thread exit(exit_shop);
	std::thread window(set_window);
	
	enter.join();
	exit.join();
	window.join();

return 0;
}
