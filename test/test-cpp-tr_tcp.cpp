#include <iostream>
#include "pomelo.h"
#include "pomelocpp.h"
#include "test.h"
#include "json\json.h"
#include <windows.h>


#define SLEEP(x) Sleep(x * 1000);
using namespace pomelo;

#define REQ_ROUTE "connector.entryHandler.entry"
#define REQ_MSG "{\"name\": \"test\"}"

#define NOTI_ROUTE "test.testHandler.notify"
#define NOTI_MSG "{\"content\": \"test content\"}"


int main()
{
	pomelo::LibInit(PC_LOG_INFO);
	
	Client client;
	PC_TEST_ASSERT(client.GetPcClient());
	
	client.Initialize();
    PC_TEST_ASSERT(pc_client_ex_data(client.GetPcClient()) == (void*)&client);
	PC_TEST_ASSERT(client.State() == PC_ST_INITED);

	client.On("connected",
		[](const char * msg){
			std::cout<<" ���ӳɹ�"<<std::endl;
	});
 
	client.Connect("127.0.0.1",3010);
	SLEEP(1);

	Json::Value value;
	value["name"] = "test";
	std::string reqStr = value.toStyledString();

	client.DoRequest(REQ_ROUTE,reqStr.c_str(),
		[&client,reqStr](const Request & req,int rc,const char* resp){

			PC_TEST_ASSERT(rc == PC_RC_OK);
			PC_TEST_ASSERT(resp);

			printf("test get resp %s\n", resp);
			fflush(stdout);

			PC_TEST_ASSERT(req.Client() == &client);
			PC_TEST_ASSERT(!strcmp(req.Route(), REQ_ROUTE));
			PC_TEST_ASSERT(!strcmp(req.Message(), reqStr.c_str()));
			PC_TEST_ASSERT(req.TimeOut() == Client::TIMEOUT_DEFAULT);
	});

	SLEEP(1);

	value.clear();
	value["content"]="test content";
	std::string notifyStr = value.toStyledString();

	client.DoNotify(NOTI_ROUTE,notifyStr.c_str(),
		[&client,notifyStr](const Notify & notify,int rc){
			PC_TEST_ASSERT(rc == PC_RC_OK);

			PC_TEST_ASSERT(notify.Client() == &client);
			PC_TEST_ASSERT(!strcmp(notify.Route(), NOTI_ROUTE));
			PC_TEST_ASSERT(notifyStr == notify.Message());
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