#include <iostream>
#include "pomelo.h"
#include "pomelocpp.h"
#include "test.h"
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
	
	Client client;
	PC_TEST_ASSERT(client.GetPcClient());
	
	client.Initialize();
    PC_TEST_ASSERT(pc_client_ex_data(client.GetPcClient()) == (void*)&client);
	PC_TEST_ASSERT(client.State() == PC_ST_INITED);

	client.AddEventHandler("connected",
		[](const char * eventName,const char * msg){
			std::cout<<"eventName is "<<eventName<<" 连接成功"<<std::endl;
	});
 
	client.Connect("127.0.0.1",3010);
	SLEEP(1);
	client.DoRequest(REQ_ROUTE,REQ_MSG,
		[&client](const Request & req,int rc,const char* resp){

			PC_TEST_ASSERT(rc == PC_RC_OK);
			PC_TEST_ASSERT(resp);

			printf("test get resp %s\n", resp);
			fflush(stdout);

			PC_TEST_ASSERT(req.Client() == &client);
			PC_TEST_ASSERT(!strcmp(req.Route(), REQ_ROUTE));
			PC_TEST_ASSERT(!strcmp(req.Message(), REQ_MSG));
			PC_TEST_ASSERT(req.TimeOut() == Client::TIMEOUT_DEFAULT);
	});

	SLEEP(1);

	client.DoNotify(NOTI_ROUTE,NOTI_MSG,
		[&client](const Notify & notify,int rc){
			PC_TEST_ASSERT(rc == PC_RC_OK);

			PC_TEST_ASSERT(notify.Client() == &client);
			PC_TEST_ASSERT(!strcmp(notify.Route(), NOTI_ROUTE));
			PC_TEST_ASSERT(!strcmp(notify.Message(), NOTI_MSG));
			PC_TEST_ASSERT(notify.TimeOut() == Client::TIMEOUT_DEFAULT);
	});

	SLEEP(10);
	client.DisConnect();

	SLEEP(1);
	client.ClearnUp();
	PC_TEST_ASSERT(client.State() == PC_ST_NOT_INITED);

	SLEEP(1);
	pomelo::LibClearUp();

	return 0;
}