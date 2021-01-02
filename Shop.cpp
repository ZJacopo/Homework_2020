#include "Shop.h"


const std::string EMPTY="cell_empty";

Shop::Shop(int lenght, std::string word)
	:clients{0}, existing_queue{false}, exit_hhmm_queue{false}, MAX_CAP{lenght}
	{
		for(int i=0; i<MAX_CAP; ++i){
			in_shop.push_back(EMPTY);
		}
		
	}

bool Shop::sort_client(int size_q){
	
		bool enter_shop{true};
		std::unique_lock<std::mutex> mlock(mut_clients);
		std::cout<<"fct sort_client \n";
		if(clients==MAX_CAP || size_q!=0){
			enter_shop = false; 
			mlock.unlock();
			}
		
		return enter_shop;	
}

void Shop::enter_client(std::string code){
	
	
	std::unique_lock<std::mutex> mlock(mut_shop);
	
	for(int i=0; i<MAX_CAP;++i){
		if(in_shop[i].compare(EMPTY)==0){
			in_shop[i]=code;
			std::cout<<"fct enter_client CLIENTE ENTRATO: "<<i<<"\n";
			i = MAX_CAP;
			mlock.unlock();
		}
	}
	
}

void Shop::increment_client(){
	
	mut_clients.lock();
	clients++;
	std::cout<<"fct increment_client: "<<clients<<"\n";
	mut_clients.unlock();
	
	not_empty.notify_one();//if we are here clients was updated so now is possible to notify not_empty	
}

std::string Shop::find_client(std::string code){
	
	std::string id_code = code.substr(0,9);
	std::string ret_str = EMPTY;
	
	std::unique_lock<std::mutex> mlock(mut_shop);
	for(int i=0;i<MAX_CAP;++i){
		
		if((in_shop[i].substr(0,9)).compare(id_code)==0){
			ret_str = in_shop[i];
			in_shop[i] = EMPTY;
			i = MAX_CAP;
			mlock.unlock();
		}
	}
	
	return ret_str;
	
}//this section is protected from unique_lock to here 

void Shop::set_exit_queue(bool state){
	
	mut_exit_hhmm.lock();
	exit_hhmm_queue = state;
	mut_exit_hhmm.unlock();
	
}

void Shop::set_arrival_queue(bool state){
	
	mut_existing.lock();
	existing_queue = state;
	mut_existing.unlock();
	
}

bool Shop::get_exit_queue(){
	
	bool ret_ex;
	
	mut_exit_hhmm.lock();
	
	ret_ex = exit_hhmm_queue;
	mut_exit_hhmm.unlock();
	
	return ret_ex;
}

bool Shop::get_arrival_queue(){
	
	bool ret_arr;
	
	mut_existing.lock();
	
	ret_arr = existing_queue;
	mut_existing.unlock();
	
	return ret_arr;
	
}

std::string Shop::entrance_from_queue(){
	
	std::string hhmm_info;
	
	mut_hhmm_queue.lock();
	
		hhmm_info = hhmm_queue.front();
		hhmm_queue.pop();
		
	mut_hhmm_queue.unlock();
	
	not_empty.notify_one();// because if i'm here it means that there is someone in the shop
	return hhmm_info;
}

void Shop::is_someone_inside(){
	
	std::unique_lock<std::mutex> mlock(mut_clients);
	if(clients==0){
		
		not_empty.wait(mlock);
	}
	
}

void Shop::decrement_client(){
	
	mut_clients.lock();
	clients--;
	mut_clients.unlock();
}

void Shop::update_hhmm_queue(std::string hhmm){
	
	mut_hhmm_queue.lock();
	hhmm_queue.push(hhmm);
	mut_hhmm_queue.unlock();
}

int Shop::get_clients(){
	int ret_cl;
	
	mut_clients.lock();
	ret_cl = clients;
	mut_clients.unlock();

	return ret_cl;
}

void Shop::print_shop(){
	
	mut_shop.lock();
	for(int i=0; i<MAX_CAP; ++i){
			std::cout<<"in_shop pos["<<i<<"] = "<<in_shop[i]<<"\n";
		}
	mut_shop.unlock();
}