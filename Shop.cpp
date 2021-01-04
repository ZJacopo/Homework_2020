#include "Shop.h"


std::string EMPTY="this_cell_is_empty";

Shop::Shop(int lenght)
	:clients{0}, existing_queue{false}, exit_hhmm_queue{false}, MAX_CAP{lenght}, deg_window{0}, shop_is_open{true}
	{
		for(int i=0; i<MAX_CAP; ++i){
			in_shop.push_back("this_cell_is_empty");
		}
		
	}

bool Shop::sort_client(int size_q){
	
		bool enter_shop{true};
		std::unique_lock<std::mutex> mlock(mut_clients);
		//std::cout<<"fct sort_client \n";
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
			//std::cout<<"fct enter_client CLIENTE ENTRATO: "<<i<<"\n";
			i = MAX_CAP;
			increment_client();
			mlock.unlock();
		}
	}
	
}

void Shop::increment_client(){
	
	int cl=0;
	mut_clients.lock();
	clients++;
	cl=clients;
	//std::cout<<"fct increment_client: "<<clients<<"\n";
	mut_clients.unlock();
	
	not_empty.notify_one();//if we are here clients was updated so now is possible to notify not_empty
	if(cl==0 || cl==5 || cl==10 || cl==15){window.notify_one();}
}

std::string Shop::find_client(std::string code){
	
	std::string id_code = code.substr(0,9);
	std::string ret_str = EMPTY;
	//std::cout<<"cerco cliente \n";
	std::unique_lock<std::mutex> mlock(mut_shop);
	for(int i=0;i<MAX_CAP;++i){
		
		if((in_shop[i].substr(0,9)).compare(id_code)==0){
			ret_str = in_shop[i];
			in_shop[i] = EMPTY;
			//std::cout<<"trovato cliente, salvato e tolto dal vettore \n";
			i = MAX_CAP;
			mlock.unlock();
		}
	}
	
	return ret_str;
	
}

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
	bool is_hhmm_queue_empty;
	
	mut_hhmm_queue.lock();
	
		hhmm_info = hhmm_queue.front();
		hhmm_queue.pop();
		is_hhmm_queue_empty= hhmm_queue.empty();
		
	mut_hhmm_queue.unlock();
	
	not_empty.notify_one();// because if i'm here it means that there is someone in the shop
	
	if(is_hhmm_queue_empty){Shop::set_exit_queue(false);}
	
	return hhmm_info;
}

void Shop::is_someone_inside(){
	
	std::unique_lock<std::mutex> mlock(mut_clients);
	if(clients==0){
		//std::cout<<"2 bloccato su is_someone_inside \n";
		not_empty.wait(mlock);
	}
	
}

void Shop::decrement_client(){
	int cl=0;
	mut_clients.lock();
	clients--;
	cl = clients;
	mut_clients.unlock();
	
	if(cl==0 || cl==5 || cl==10 || cl==15){window.notify_one();}
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

bool Shop::get_is_open(){
	
	bool ret_is_open=false;
	
	mut_shop_is_open.lock();
	ret_is_open = shop_is_open;
	mut_shop_is_open.unlock();
	
	return ret_is_open;
}

void Shop::set_is_open(bool state){
	
	mut_shop_is_open.lock();
	shop_is_open = state;
	mut_shop_is_open.unlock();
	
	window.notify_one();
}


std::string Shop::set_value_window(){
	
	std::unique_lock<std::mutex> mlock(mut_shop_is_open);
	if(true){
		window.wait(mlock);
		int cl = get_clients();
		std::string reg_100="0%";
		if(cl>=1 && cl<5){reg_100="25%";}
		else if(cl>=5 && cl<10){reg_100="50%";}
			else if(cl>=10 && cl<15){reg_100="75%";}
				else if(cl==15){reg_100="100%";}
		mut_window.lock();
		deg_window = reg_100;
		mut_window.unlock();
		
		return reg_100;
		
	
	}	
}

void Shop::print_shop(){
	
	mut_shop.lock();
	for(int i=0; i<MAX_CAP; ++i){
			std::cout<<"in_shop pos["<<i<<"] = "<<in_shop[i]<<"\n";
		}
	mut_shop.unlock();
}