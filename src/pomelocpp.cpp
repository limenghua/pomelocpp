#include "pomelocpp.h"
#include <iostream>
#include <assert.h>

using namespace pomelo;

void pomelo::LibInit(int log_level /*= PC_LOG_DEBUG */)
{
    pc_lib_set_default_log_level(log_level); 
	pc_lib_init(NULL, NULL, NULL, NULL);
}

void pomelo::LibClearUp()
{
	pc_lib_cleanup();
}


Client::Client()
{
	pc_client = (pc_client_t*)malloc(pc_client_size());
	serialNumber =0;
	timeout = TIMEOUT_DEFAULT;
}
Client::~Client()
{
	free(pc_client);
}

int Client::Initialize()
{
	pc_client_config_t config = PC_CLIENT_CONFIG_DEFAULT;
	pc_client_init(pc_client, (void*)this, &config);

	eventHandleId = pc_client_add_ev_handler(pc_client, &Client::EventCallback, NULL, NULL);

	return 0;
};
int Client::ClearnUp()
{
	pc_client_rm_ev_handler(pc_client, eventHandleId);
	return pc_client_cleanup(pc_client);
}
int Client::Connect(const char * ip,int port)
{
	return pc_client_connect(pc_client, ip,port, NULL);
}
int Client::DisConnect()
{
	return pc_client_disconnect(pc_client);
}

void Client::RequestCallback(const pc_request_t* req, int rc, const char* resp)
{
	pc_client_t* pc_client = pc_request_client(req);
	Client * client = (Client* )pc_client_ex_data(pc_client);
	client->RequestResponse(Request(req),rc,resp);
}
void Client::NotifyCallback(const pc_notify_t* notify, int rc)
{
	pc_client_t* pc_client = pc_notify_client(notify);
	Client * client = (Client* )pc_client_ex_data(pc_client);
	client->NotifyResponse(Notify(notify),rc);
}

void Client::EventCallback(pc_client_t *client, int ev_type, void* ex_data,
						const char* arg1, const char* arg2)
{
	Client * owner = (Client *)pc_client_ex_data(client);
	assert(owner != NULL);

	switch(ev_type){
	case PC_EV_USER_DEFINED_PUSH:
		owner->EmitEvent(arg1,arg2);
		break;

	case PC_EV_CONNECTED:
		owner->EmitEvent("connected",NULL);
		break;

	case PC_EV_CONNECT_ERROR:
		owner->EmitEvent("connected_error",arg1);
		break;

	case PC_EV_CONNECT_FAILED:
		owner->EmitEvent("connected_failed",arg1);
		break;

	case PC_EV_DISCONNECT:
		owner->EmitEvent("disconnect",NULL);
		break;

	case PC_EV_KICKED_BY_SERVER:
		owner->EmitEvent("kicked",NULL);
		break;

	case PC_EV_UNEXPECTED_DISCONNECT:
		owner->EmitEvent("unexpected_disconnect",arg1);
		break;

	case PC_EV_PROTO_ERROR:
		owner->EmitEvent("proto_error",arg1);
		break;

	default:
		break;
	}
}

void Client::EmitEvent(const char * eventName,const char * msg)
{
	if(eventHandlers.find(eventName) != eventHandlers.end())
	{
		auto handlers = eventHandlers[eventName];
		for(auto it = handlers.begin();it != handlers.end();it++)
		if(*it){
			(*it)(msg);
		}
	}
}

void Client::RequestResponse(const Request & req,int rc,const char* msg)
{
	int key = req.GetId();
	if(requestHandlers.find(key) != requestHandlers.end())
	{
		auto callback = requestHandlers[key];
		if(callback){
			callback(req,rc,msg);
		}
		requestHandlers.erase(key);
	}
}

void Client::NotifyResponse(const Notify & notify,int rc)
{
	int key = notify.GetId();
	if(notifyHandlers.find(key) != notifyHandlers.end())
	{
		auto callback = notifyHandlers[key];
		if(callback){
			callback(notify,rc);
		}
		notifyHandlers.erase(key);
	}
}


