#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include "Shop.h"

const int MAX_CAP=15;
const std::string EMPTY="cell_empty";
Shop shop(MAX_CAP,EMPTY);

//std::mutex mut_arr_queue;
//std::condition_variable arr_queue;

void enter_shop(){
	//std::cout<<"1 dentro la thread enter"<<"\n";
	
	std::ifstream rd_arrivals("arrive_shop.txt");
	std::queue<std::string> arrival_queue;
	std::string new_client;
	bool there_is_someone{true};
	bool allow_queue{false};
	bool other_entrance{true};
	int cl{0};
	
	
	while(there_is_someone){
		
		bool enter_shop;	
		//std::cout<<"1 divido il cliente tra negozio e coda \n";
		enter_shop = shop.sort_client(arrival_queue.size());
		
		if(getline(rd_arrivals,new_client)){
			
			if(enter_shop){
				shop.enter_client(new_client);
				shop.increment_client();
				std::cout<<"1 cliente va in negozio "<<new_client<<"\n";
			}
			else{
				arrival_queue.push(new_client); 
				shop.set_arrival_queue(true);
				 
				std::cout<<"1 cliente va in coda "<<arrival_queue.size()<<" "<<new_client<<"\n";
			}
		}
		else {other_entrance = false;}	
		//I'm sure that some clients are gone out so if there is a queue i can open the shop to it
		
		//std::cout<<"1 chiedo stato di esistenza della coda \n";
		
		cl = shop.get_clients();
		allow_queue = shop.get_exit_queue();
		//std::unique_lock<std::mutex> mlock(mut_arr_queue);
		
		if((arrival_queue.size()!= 0) && allow_queue && cl<MAX_CAP){
			//arr_queue.wait(mlock);
			//I'm sure that some clients are gone out so if there is a queue i can open the shop to it
			std::cout<<"1 la coda esiste \n";
			
			std::string hhmm_info;
			std::string new_id_queue;
			std::string new_client_queue;
			std::string allow_again;
			
			new_id_queue = (arrival_queue.front()).substr(0,9);
			arrival_queue.pop();
			if((arrival_queue.size()== 0)){shop.set_arrival_queue(false);}
			
			hhmm_info = shop.entrance_from_queue();
			
			new_client_queue = new_id_queue+" "+hhmm_info;
			
			shop.enter_client(new_client_queue);
			shop.increment_client();
			//std::cout<<"1 ho appena scritto sul file un cliente che era in coda"<<"\n";
		}
		
		if(!other_entrance && (arrival_queue.size()== 0)){there_is_someone = false;}
		//shop.print_shop();
	//std::cout<<"1 fine while grosso"<<"\n";
			
		
		
	}
	std::cout<<"1 FINITO \n";
}

void exit_shop(){
	//std::cout<<"2 dentro la thread exit"<<"\n";
	std::ifstream rd_exit("exit_shop.txt");
	std::ofstream wr_exit("out_shop.txt");
	std::queue<std::string> out_shop;
	
	std::string out_client;
	bool other_exits{true};
	bool find_new_exit{true};
	bool get_arr{false};
	
	std::getline(rd_exit,out_client);
	
	while(other_exits){
		
		shop.is_someone_inside();
		//std::cout<<"2 se sono qui, qualcuno è nel negozio \n";
		
		std::string find_it;
		
		find_it = shop.find_client(out_client);
		//ho cercato nel negozio e se ho trovato il cliente, allora in questo momento l'ho già tolto dal negozio, diciamo che è 
		//sulla soglia
		
		//qui devo capire se segnare il suo orario di uscita e renderlo disponibile alla coda
		//o farlo uscire e basta
		
		if(find_it.compare(EMPTY)!=0){
			std::cout<<"2 dentro c'è uno che deve uscire: "<<find_it<<" \n";
			
			get_arr = shop.get_arrival_queue();

			if(get_arr){
				//se esiste una coda allora devo caricare l'ora di uscita dentro la coda hhmm_queue
				shop.update_hhmm_queue(find_it.substr(10));
				shop.set_exit_queue(true);
				//std::cout<<"2 c'è coda quindi salvo l'ora del cliente uscente\n";
			}
			
			shop.decrement_client();
			find_new_exit = true;
			//std::cout<<"2 l'ho fatto uscire \n";
			
			int h_out = std::stoi(out_client.substr(10,12)); 
			int h_it = std::stoi(find_it.substr(10,12)); 
			int m_out = std::stoi(out_client.substr(13)); 
			int m_it = std::stoi(find_it.substr(13)); 
			
			int m_tot = (h_out*60)+(m_out)-(h_it*60)-(m_it);
			h_out = m_tot/60; 
			m_out = m_tot%60;
			
			wr_exit <<"client: "<<find_it.substr(0,9)<<" inside for h: "<<h_out<<" m: "<<m_out<<"\n";
			
		}
		
		//shop.print_shop();
		//std::cout<<"2 fine ciclo while \n";
		if(find_new_exit){
			
			if(getline(rd_exit,out_client)){find_new_exit = false; /*std::cout<<"2 dentro find \n";*/}
			else {other_exits=false; /*std::cout<<"2 FINITO TXT \n";*/}
			
		}
	}
	
	std::cout<<"2 FINITO  \n";
}

int main(){
	
	std::thread enter{enter_shop};
	std::thread exit{exit_shop};
	
	enter.join();
	exit.join();

return 0;
}
