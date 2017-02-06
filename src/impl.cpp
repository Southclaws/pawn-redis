/*==============================================================================


	Redis for SA:MP

		Copyright (C) 2016 Barnaby "Southclaw" Keene

		This program is free software: you can redistribute it and/or modify it
		under the terms of the GNU General Public License as published by the
		Free Software Foundation, either version 3 of the License, or (at your
		option) any later version.

		This program is distributed in the hope that it will be useful, but
		WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
		See the GNU General Public License for more details.

		You should have received a copy of the GNU General Public License along
		with this program.  If not, see <http://www.gnu.org/licenses/>.

	Note:
		This file contains the actual Python calling code, thread code and all
		other features specific to the plugin.


==============================================================================*/


#include <string>
#include <vector>

using std::string;
using std::vector;

#include <sdk.hpp>

#include "impl.hpp"


int Redisamp::Connect(string hostname, int port, int timeout)
{
	struct timeval timeout_val = {timeout, 0};

	redisContext *context = redisConnectWithTimeout(hostname.c_str(), port, timeout_val);

	if (context == NULL || context->err)
	{
		if (context)
		{
			redisFree(context);
			return -1;
		}
		else
		{
			return -2;
		}
		exit(1);
	}

	contexts.push_back(context);

	// Position in the vector is our "ID", a common Pawn idiom since it doesn't
	// have any concept of pointers so entities are generally referred to with
	// a 0-n integer.
	return ((int)contexts.size()) - 1;
}

int Redisamp::Disconnect(int context_id)
{
	if(!(context_id < contexts.size()))
		return 1;

	redisContext *context = contexts[context_id];

	// hiredis is C so no nullptr
	if(context == NULL)
		return 2;

	redisFree(context);

	return 0;
}

int Redisamp::Command(string command)
{
	return 0;
}

int Redisamp::Subscribe(int context, string channel, string callback)
{
	return 0;
}

int Redisamp::Publish(int context, string channel, string data)
{
	return 0;
}

void Redisamp::amx_tick(AMX* amx)
{
	//
}