#ifndef __POMELO_CPP_H
#define __POMELO_CPP_H
#include <hash_map>
#include <string>
#include <functional>

#include "pomelo.h"
#include "Notify.h"
#include "Request.h"

namespace pomelo
{
	
	void LibInit();
	void LibClearUp();

	/*! \brief pomelo::Client 包装libpomelo2 中 pc_client_t 结构.\n
	 *  一个Client对象代表一个连接服务器的本地客户端.\n
	 *  利用C++ 11 支持lambda特性，对外提供类似node.js的异步调用语法.\n
	 *  Example:\n
	 * \code
	 * pomelo::LibInit();
	 * Client client;
	 * client.Initialize();
	 * client.AddEventHandler("connected",
	 *	[](const char * eventName,const char * msg){
	 * 		std::cout<<"eventName is "<<eventName<<" 连接成功"<<std::endl;
	 * }); 
	 * client.Connect("127.0.0.1",3010);
	 * SLEEP(1);
	 * client.DoRequest(REQ_ROUTE,REQ_MSG,
	 * 	[](const Request & req,int rc,const char* resp){
	 * 		//do some operation
	 * });
	 * client.DoNotify(NOTI_ROUTE,NOTI_MSG,
	 * 	[](const Notify & notify,int rc){
	 * 		//do some operation
	 * });
	 * client.DisConnect();
	 * client.ClearnUp();
	 * pomelo::LibClearUp();
	 * \endcode
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
		* 系统事件包括以下类型：\n
		* -"connected"\n
		* -"connected_error"\n
		* -"connected_failed"\n
		* -"disconnect"\n
		* -"kicked"\n
		* -"unexpected_disconnect"\n
		* -"proto_error"\n
		* \param handler 事件响应对象，为一函数对象，事件发生后handler被调用
		* handler必须是可拷贝.\n
		* handler 函数签名:\n
		* \code void handler(const char * eventName,const char * msg) /endcode
		* Example:\n
		* \code
		* client.AddEventHandler("connected",[](const char * eventName,const char * msg){
		*		//some operation;
		* });
		* \endcode
		*/
		template<typename Function>
			void AddEventHandler(const char * eventName,Function handler);

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
		* 状态码：
		* \code
		* #define PC_ST_NOT_INITED 0
		* #define PC_ST_INITED 1
		* #define PC_ST_CONNECTING 2
		* #define PC_ST_CONNECTED 3
		* #define PC_ST_DISCONNECTING 4
		* #define PC_ST_UNKNOWN 5
		* #define PC_ST_COUNT 6
		* \endcode
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
		std::hash_map<std::string,EventHandleFun> eventHandlers;
		std::hash_map<int,RequestHandleFun> requestHandlers;
		std::hash_map<int,NotifyHandleFun> notifyHandlers;
	};
	typedef Client Pomelo;

	template<typename Function>
		void Client::AddEventHandler(const char * eventName,Function handler)
		{
			eventHandlers[eventName] = handler;
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