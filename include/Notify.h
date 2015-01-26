#ifndef __POMELO_NOTIFY_H_
#define __POMELO_NOTIFY_H_

#include "pomelo.h"

namespace pomelo
{
	class Client;
	
	/// libpomelo2 pc_notify_t 简单包装，携带Notify请求的相应数据
	class Notify
	{
	public:

		/// 构造函数
		Notify(const pc_notify_t* nf):notify(nf)
		{
		}

		/// 发出Nofify请求的Client对象指针
		pomelo::Client * Client() const
		{
			pc_client_t* cli = pc_notify_client(notify);
			return (pomelo::Client *)pc_client_ex_data(cli);
		}

		/// 请求Route地址
		const char * Route() const
		{
			return pc_notify_route(notify);
		}

		/// 请求携带的数据
		const char * Message() const
		{
			return pc_notify_msg(notify);
		}

		/// 请求超时时间
		int TimeOut() const
		{
			return pc_notify_timeout(notify);
		}

		/// 该Notify请求的唯一识别ID，用于查找回调函数
		int GetId() const
		{
			return (int)pc_notify_ex_data(notify);
		}

	private:
		const pc_notify_t* notify; 
	};

}




#endif //__POMELO_NOTIFY_H_
