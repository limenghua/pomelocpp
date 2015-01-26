#ifndef __POMELO_CPP_H
#define __POMELO_CPP_H
#include <hash_map>
#include <list>
#include <string>
#include <functional>

#include "pomelo.h"
#include "Notify.h"
#include "Request.h"

namespace pomelo
{
	/// 初始化客户端库，每个程序只调用一次
	void LibInit(int log_level = PC_LOG_DEBUG);
	
	/// 释放客户端库，每个程序只调用一次
	void LibClearUp();

	/*! \brief pomelo::Client 包装libpomelo2 中 pc_client_t 结构.\n
	 *  一个Client对象代表一个连接服务器的本地客户端.\n
	 *  利用C++ 11 支持lambda特性，对外提供类似node.js的异步调用语法.\n
	 */
	class Client
	{
	public:
		/// Request调用和Notify调用的默认超时时间.
		static const int TIMEOUT_DEFAULT = 3;

		Client();
		~Client();

		/*! \brief 初始化Client.
		* 包装 libpomelo2 pc_client_init()函数.Client在使用之前必须初始化.
		* \returns .
		*/
		int Initialize();

		/*! \brief 包装 libpomelo2 pc_client_cleanup()函数.
		* \returns pc_client_cleanup().
		*/
		int ClearnUp();

		/*! \brief 连接服务器.
		* \param ip 服务器端IP.
		* \param port 服务器端口号
		* \returns .
		*/
		int Connect(const char * ip,int port);

		/*! \brief 断开服务器端连接.
		* \returns Detail string at the specified index.
		*/
		int DisConnect();

		/*! \brief 增加事件处理器.
		* \param eventName 事件名称.
		* 服务器推送事件时为路由地址，系统事件为事件名\n
		* 系统事件包括以下类型：
		*	- "connected"
		*	- "connected_error"
		*	- "connected_failed"
		*	- "disconnect"
		*	- "kicked"
		*	- "unexpected_disconnect"
		*	- "proto_error"
		*	.
		* \param handler 事件响应对象，为一函数对象，事件发生后handler被调用
		* handler必须是可拷贝.\n
		* handler 函数签名:\n
		* \code void handler(const char * eventName,const char * msg) \endcode
		* Example:\n
		* \code
		* client.AddEventHandler("connected",[](const char * eventName,const char * msg){
		*		//some operation;
		* });
		* \endcode
		*/
		template<typename Function>
			void AddEventHandler(const char * eventName,Function handler);

		/// 功能与AddEventHandler相同，node.js写法
		template<typename Function>
			void On(const char *eventName,Function handler);

		/*! \brief Request请求.
		* \param route 请求路由地址.
		* \param msg 请求消息体
		* \param handler 请求响应处理对象，为一函数对象，服务器响应到来后handler被调用
		* handler必须是可拷贝.
		* handler 函数签名:
		* \code void handler(const Request & req,int rc,const char* msg) \endcode
		* Example:
		* \code
		* client.DoRequest("connector.entryHandler.entry","{\"name\": \"test\"}",
		*	[](const Request & req,int rc,const char* msg){
		*		//some operation;
		* });
		* \endcode
		*/
		template<typename Function>
			int DoRequest(const char * route,const char* msg,Function handler);

		/*! \brief Notify请求.
		* \param route 请求路由地址.
		* \param msg 请求消息体
		* \param handler 请求响应处理对象，为一函数对象，服务器响应到来后handler被调用
		* handler必须是可拷贝.\n
		* handler 函数签名:
		* \code void handler(const pc_notify_t* req, int rc) \endcode
		* Example:
		* \code
		* client.DoNotify("test.testHandler.notify","{\"content\": \"test content\"}",
		*	[](const Notify & notify,int rc){
		*		//some operation;
		* });
		* \endcode
		*/
		template<typename Function>
			int DoNotify(const char * route,const char* msg,Function handler);

		/*! \brief 包装.pc_client_state函数
		* \returns 客户端运行状态.\n
		* 状态码如下：
		*	- PC_ST_NOT_INITED
		*	- PC_ST_INITED
		*	- PC_ST_CONNECTING
		*	- PC_ST_CONNECTED
		*	- PC_ST_DISCONNECTING
		*	- PC_ST_UNKNOWN
		*	- PC_ST_COUNT
		*	.
		*/
		int State()
		{
			return pc_client_state(pc_client);
		}
		/*!
		* \returns libpomelo2 pc_client_t *指针.
		*/
		pc_client_t* GetPcClient()
		{
			return pc_client;
		}
	private:
		static void RequestCallback(const pc_request_t* req, int rc, const char* resp);
		static void NotifyCallback(const pc_notify_t* req, int rc);
		static void EventCallback(pc_client_t *client, int ev_type, void* ex_data,
								 const char* arg1, const char* arg2);

		void EmitEvent(const char * eventName,const char * msg);
		void RequestResponse(const Request & req,int rc,const char* msg);
		void NotifyResponse(const Notify & notify,int rc);

		int serialNumber;	
		int NextSerialNumber()
		{
			return serialNumber++;
		}

		pc_client_t* pc_client;	
		int timeout;
		int eventHandleId;

		typedef std::function<void (const char * ,const char*)> EventHandleFun;
		typedef std::function<void (const Request & req,int,const char*)> RequestHandleFun;
		typedef std::function<void (const Notify & notify,int rc)> NotifyHandleFun;
		std::hash_map<std::string,std::list<EventHandleFun>> eventHandlers;
		std::hash_map<int,RequestHandleFun> requestHandlers;
		std::hash_map<int,NotifyHandleFun> notifyHandlers;
	};
	typedef Client Pomelo;

	template<typename Function>
		void Client::AddEventHandler(const char * eventName,Function handler)
		{
			eventHandlers[eventName].push_back(handler);
		}
	template<typename Function>
		void Client::On(const char * eventName,Function handler)
		{
			AddEventHandler(eventName,handler);
		}

	template<typename Function>
		int Client::DoRequest(const char * route,const char* msg,Function handler)
		{
			int key = NextSerialNumber();
			requestHandlers[key] = handler;
			return pc_request_with_timeout(pc_client,route,msg, (void*)key,timeout,&Client::RequestCallback);
		};

	template<typename Function>
		int Client::DoNotify(const char * route,const char* msg,Function handler)
		{
			int key = NextSerialNumber();
			notifyHandlers[key] = handler;
			return pc_notify_with_timeout(pc_client,route,msg,(void*)key,timeout,&Client::NotifyCallback);
		}

}


#endif //__POMELO_CPP_H