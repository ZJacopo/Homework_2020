#include <string>
#include <fstream>
#include <iostream>

std::ofstream presence("arrive_shop.txt");

int main(){
presence.close();

std::string id{"ID"};
int id_num{0};
int f_m;
std::string fm;
int h{8};
int min{0};
const int max{100};


//l'ipotesi è che non possano entrare clienti nello stesso minuto
std::ofstream create_arrivals("arrive_shop.txt");
if(create_arrivals.is_open()){
	for(int i=0; i<max; ++i){ 
		id_num = rand() %89999 + 10000;
		f_m = rand() %2;
		if(f_m==0){fm="F";}
		else{fm="M";}
		min = min + (rand() %15 +1);
		if(min>=60){h=h+1; min = min-60;}
		
		if(h==19 && min<=29 && min>20){//le ore 19:29 sono un tributo alla canzone "Baffo Natale" di Eelst
			create_arrivals <<id<<id_num<<" "<<fm<<" "<<19<<":"<<29<<"\n";
			i = max;
		}
		else {
			std::string hh;
			std::string mm;
			if(h<10){hh = "0"+std::to_string(h);}
			else {hh=std::to_string(h);}
			if(min<10){mm = "0"+std::to_string(min);}
			else {mm=std::to_string(min);}
			create_arrivals <<id<<id_num<<" "<<fm<<" "<<hh<<":"<<mm<<"\n";
		}
	}
	
}
create_arrivals.close();

std::ifstream read_arrive("arrive_shop.txt");
std::ofstream temp("temp.txt");

std::string line;
std::string new_line;
int hh;
int mm;
int count{0};

//line.substr(pos,len); prende il valore 

while(getline(read_arrive, line)){
	count++;
	new_line = line.substr(0,10);
	hh = std::stoi(line.substr(10,12));
	mm = std::stoi(line.substr(13));
	if(hh==19){mm=mm+5;}
	else{mm = mm + rand() % 30 + 5;}
	
	if(mm >= 60){hh = hh + 1; mm = mm - 60;}
	
	std::string new_h;
	std::string new_m;
	if(hh<10){new_h = "0"+std::to_string(hh);}
		else {new_h=std::to_string(hh);}
	if(mm<10){new_m = "0"+std::to_string(mm);}
		else {new_m=std::to_string(mm);}
	
	temp <<new_line<<new_h<<":"<<new_m<<"\n";
	
	/*
	questa versione non mette lo zero, PERO' va messo perchè serve a rendere tutte le linee uguali
	
	if(mm > 60){hh = hh + 1; mm = mm - 60;}
	
	temp <<new_line<<hh<<":"<<mm<<"\n";
	*/
}

read_arrive.close();
temp.close();

std::ofstream write_exit("exit_shop.txt");
std::string ref_line;

int code_client;

for(int i=0;i<count;++i){
	
	std::ifstream temp2("temp.txt");
	std::ofstream ftemp1("ftemp.txt");
	
	while(getline(temp2,line))
	{ftemp1<<line<<"\n";} //cosi' ho una copia di temp
	temp2.close();
	ftemp1.close();
	
	std::ifstream ftemp("ftemp.txt");
	
	std::getline(ftemp,ref_line);
	int ref_code;
	int ref_h;
	int ref_m;
	
	while(getline(ftemp,line)){
		code_client = std::stoi(line.substr(2,7));
		hh = std::stoi(line.substr(10,12));
		mm = std::stoi(line.substr(13));
		
		ref_code = std::stoi(ref_line.substr(2,7));
		ref_h = std::stoi(ref_line.substr(10,12));
		ref_m = std::stoi(ref_line.substr(13));
		
		
		if(hh < ref_h){ref_line = line;}
			else if(hh==ref_h && mm<ref_m){ref_line = line;}
				else if(hh==ref_h && mm==ref_m && code_client<ref_code){ref_line = line;}
	}
	//alla fine di tutto sto giro ho ottenuto il cliente che è uscito prima o con il codice minore
	write_exit <<ref_line<<"\n";
	
	std::ifstream ftemp2("ftemp.txt");
	std::ofstream temp3("temp.txt");
	while(getline(ftemp2,line)){
		if(line.compare(ref_line)!=0)
		{
			temp3<<line<<"\n";
		}
	}
	ftemp2.close();
	
}
remove("ftemp.txt");
return 0;
}








