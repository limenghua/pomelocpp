#include <iostream>
#include "pomelo.h"
#include "pomelocpp.h"
#include <windows.h>
#define SLEEP(x) Sleep(x * 1000);
using namespace pomelo;

#define REQ_ROUTE "connector.entryHandler.entry"
#define REQ_MSG "{\"name\": \"test\"}"

#define NOTI_ROUTE "test.testHandler.notify"
#define NOTI_MSG "{\"content\": \"test content\"}"


int main()
{
	pomelo::LibInit();
	
	Pomelo pomelo;
	pomelo.Initialize();

	pomelo.AddEventHandler("connected",
		[](const char * eventName,const char * msg){
			std::cout<<"eventName is "<<eventName<<" 连接成功"<<std::endl;
	});
 
	pomelo.Connect("127.0.0.1",3010);
	SLEEP(1);
	pomelo.DoRequest(REQ_ROUTE,REQ_MSG,
		[](const Request & req,int rc,const char* resp){
			std::cout<<"________________test request return data_________________"<<std::endl;
			std::cout<<"response:"<<resp<<std::endl;
			std::cout<<"return code:"<<rc<<std::endl;
			std::cout<<"request route:"<<req.Route()<<std::endl;
			std::cout<<"request msg:"<<req.Message()<<std::endl;
			std::cout<<"____________________end__________________________________"<<std::endl;
	});

	SLEEP(1);

	pomelo.DoNotify(NOTI_ROUTE,NOTI_MSG,
		[](const Notify & notify,int rc){
			std::cout<<"_______________________________________________________"<<std::endl;
			std::cout<<"test notify respond,rc:"<<rc<<std::endl;
			std::cout<<"notify router:"<<notify.Route()<<std::endl;
			std::cout<<"notify msg:"<<notify.Message()<<std::endl;
			std::cout<<"_______________________________________________________"<<std::endl;
	});
	SLEEP(1);


	SLEEP(10);

	pomelo.DisConnect();

	SLEEP(1);

	pomelo.ClearnUp();
	
	SLEEP(1);
	pomelo::LibClearUp();

	return 0;
}