#ifndef _POMELO_MESSAGE_H__
#define _POMELO_MESSAGE_H__
/*!
 * Copyright (c) 2009-2013 Petri Lehtinen <petri@digip.org>
 *
 * Jansson is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */

#include <string>

namespace pomelo
{
	class Message
	{
	public:
		Message()
		{
		};
		Message(const char * str):
			msg(str)
		{
		}

	private:
		std::string msg;
	}


};



#endif //_POMELO_MESSAGE_H__