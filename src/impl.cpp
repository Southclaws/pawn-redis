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
		This file contains the actual Redis implementation code including the
		subscribe threading and callback mechanism.


==============================================================================*/


#include <string>
#include <map>

using std::string;

#include <sdk.hpp>

#include "impl.hpp"


/*
	Note:
	Slightly hacky but it exposes a simple incrementing integer ID to Pawn
	similar to the way SetTimer IDs are handled.
*/
int Redisamp::context_count;
std::map<int, redisContext*> Redisamp::contexts;

/*
	Note:
	Connects to the redis server. Returns negative values on errors, if
	successful the returned value will represent a pseudo-ID which maps
	internally to a Redis context.

	Parameters:
	- `host[]`: hostname or ip of redis server
	- `port`: port number for redis server
	- `timeout`: connection timeout window

	Return values:
	- `0...`: Redis context ID
	- `-1`: generic error
	- `-2`: cannot allocate redis context
*/
int Redisamp::Connect(string hostname, int port, int timeout)
{
	struct timeval timeout_val = {timeout, 0};

	redisContext *context = redisConnectWithTimeout(hostname.c_str(), port, timeout_val);

	if (context == NULL || context->err)
	{
		if (context)
		{
			logprintf("Redis error: %s", context->errstr);
			redisFree(context);
			return REDIS_ERROR_CONNECT_GENERIC;
		}
		else
		{
			return REDIS_ERROR_CONNECT_FAIL;
		}
		exit(1);
	}

	contexts[context_count] = context;

	return context_count++;
}

int Redisamp::Disconnect(int context_id)
{
	redisContext* context = NULL;
	int err = contextFromId(context_id, context);
	if(err)
		return err;

	redisFree(context);

	contexts.erase(context_id);

	return 0;
}

int Redisamp::Command(int context_id, string command)
{
	redisContext* context = NULL;
	int err = contextFromId(context_id, context);
	if(err)
		return err;

	redisReply *reply = redisCommand(context, command.c_str());
	int result = 0;

	if(reply == NULL)
	{
		logprintf("Redis error: %s", context->errstr);
		result = context->err;
	}

    freeReplyObject(reply);

	return result;
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

int Redisamp::contextFromId(int context_id, redisContext *& context)
{
	try
	{
		context = contexts.at(context_id);
	}
	catch(const std::out_of_range& e)
	{
		return REDIS_ERROR_CONTEXT_INVALID_ID;
	}

	if(context == NULL)
		return REDIS_ERROR_CONTEXT_MISSING_POINTER;

	return 0;
}