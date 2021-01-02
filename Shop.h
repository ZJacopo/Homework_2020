#ifndef SHOP_H
#define SHOP_H

#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>

class Shop{
	
	public:
		Shop(int lenght, std::string word);
		
		bool sort_client(int size_q);
		void enter_client(std::string code);
		std::string find_client(std::string code);
		bool get_arrival_queue();
		void set_arrival_queue(bool state);
		int get_clients();
		std::string entrance_from_queue();
		void increment_client();
		
		bool get_exit_queue();
		void set_exit_queue(bool state);
		void is_someone_inside();
		void decrement_client();
		void update_hhmm_queue(std::string hhmm);		
		
		const std::string EMPTY="cell_empty";
		void print_shop();
		
	private:
		
		const int MAX_CAP;
		std::vector<std::string> in_shop;
		std::queue<std::string> hhmm_queue;//this queue will contain exit hh:mm of clients, it will be used to change the queue entrance 
		int clients;
		bool existing_queue;
		bool exit_hhmm_queue;
		
		std::mutex mut_shop;
		std::mutex mut_hhmm_queue;
		std::mutex mut_clients;
		std::mutex mut_existing;
		std::mutex mut_exit_hhmm;
		
		
		
		std::condition_variable not_empty;
};



#endif 