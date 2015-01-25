#ifndef _POMELO_REQUEST_H__
#define _POMELO_REQUEST_H__

#include "pomelo.h"

namespace pomelo
{
	class Client;

	/// 包装libpomelo2 pc_request_t结构
	class Request
	{
	public:
		/// 构造函数
		Request(const pc_request_t* req): request(req)
		{
		}

		/// 发出Request请求的Client对象指针
		pomelo::Client * Client() const
		{
			pc_client_t* cli = pc_request_client(request);
			return (pomelo::Client *)pc_client_ex_data(cli);
		}

		/// 请求Route地址
		const char * Route() const
		{
			return pc_request_route(request);
		}

		/// 请求数据信息
		const char * Message() const
		{
			return  pc_request_msg(request);
		}

		/// 请求超时时间
		int TimeOut() const
		{
			return pc_request_timeout(request);
		}

		/// 该Request请求的唯一识别ID，用于查找回调函数
		int GetId() const
		{
			return (int)pc_request_ex_data(request);
		}
	private:
		const pc_request_t* request;
	};
}



#endif //_POMELO_REQUEST_H__