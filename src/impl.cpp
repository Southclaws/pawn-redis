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
		message binding, threading and callback mechanism.


==============================================================================*/


#include <string>
#include <cstring>
#include <map>
#include <stack>
#include <thread>
#include <mutex>

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
	Stores a list of active subscriptions
*/
std::map<string, string> Redisamp::subscriptions;

/*
	Note:
	Contains a list of responses that have finished processing. When a thread
	has obtained a message from a queue, it will store the return value on this
	stack. When the AMX calls ProcessTick, it will process whatever available
	items are stored in it. This is the bread & butter of thread-safe plugins.
*/
std::stack<Redisamp::message> Redisamp::message_stack;

/*
	Note:
	This mutex protects the message_stack from race conditions. Since each bind
	await runs in a thread, two scripts could finished at the same time and try
	to write their responses into the stack. This is standard when working with
	threads!
*/
std::mutex Redisamp::message_stack_mutex;

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

int Redisamp::Exists(int context_id, string key)
{
	redisContext* context = NULL;
	int err = contextFromId(context_id, context);
	if(err)
		return err;

	redisReply *reply = redisCommand(context, "EXISTS %s", key.c_str());
	int result = 0;

	if(reply == NULL)
	{
		logprintf("Redis context error: %s", context->errstr);
		result = context->err;
	}
	else if(reply->type != REDIS_REPLY_INT)
	{
		logprintf("expected int reply but got %d", reply->type);
		result = REDIS_ERROR_COMMAND_BAD_REPLY;
	}
	else
	{
		value = reply->integer;
	}

	freeReplyObject(reply);

	return result;
}

int Redisamp::SetString(int context_id, string key, string value)
{
	redisContext* context = NULL;
	int err = contextFromId(context_id, context);
	if(err)
		return err;

	redisReply *reply = redisCommand(context, "SET %s %s", key.c_str(), value.c_str());
	int result = 0;

	if(reply == NULL)
	{
		logprintf("Redis context error: %s", context->errstr);
		result = context->err;
	}
	if(reply->type != REDIS_REPLY_STATUS)
	{
		logprintf("Redis reply error: %s", reply->str);
		result = REDIS_ERROR_COMMAND_BAD_REPLY;
	}

	freeReplyObject(reply);

	return result;
}

int Redisamp::GetString(int context_id, string key, string& value)
{
	redisContext* context = NULL;
	int err = contextFromId(context_id, context);
	if(err)
		return err;

	redisReply *reply = redisCommand(context, "GET %s", key.c_str());
	int result = 0;

	if(reply == NULL)
	{
		logprintf("Redis context error: %s", context->errstr);
		result = context->err;
	}
	else if(reply->type == REDIS_REPLY_NIL)
	{
		logprintf("expected string reply but got nil");
		result = REDIS_ERROR_COMMAND_BAD_REPLY;
	}
	else if(reply->type != REDIS_REPLY_STRING)
	{
		logprintf("expected string reply but got %d", reply->type);
		result = REDIS_ERROR_COMMAND_BAD_REPLY;
	}
	else if(reply->len <= 0)
	{
		result = REDIS_ERROR_COMMAND_NO_REPLY;
	}
	else
	{
		value = string(reply->str);
	}

	freeReplyObject(reply);

	return result;
}

int Redisamp::SetInt(int context_id, string key, int value)
{
	redisContext* context = NULL;
	int err = contextFromId(context_id, context);
	if(err)
		return err;

	redisReply *reply = redisCommand(context, "SET %s %d", key.c_str(), value);
	int result = 0;

	if(reply == NULL)
	{
		logprintf("Redis error: %s", context->errstr);
		result = context->err;
	}
	if(reply->type != REDIS_REPLY_STATUS)
	{
		logprintf("Redis reply error: %s", reply->str);
		result = REDIS_ERROR_COMMAND_BAD_REPLY;
	}

	freeReplyObject(reply);

	return result;
}

int Redisamp::GetInt(int context_id, string key, int &value)
{
	redisContext* context = NULL;
	int err = contextFromId(context_id, context);
	if(err)
		return err;

	redisReply *reply = redisCommand(context, "GET %s", key.c_str());
	int result = 0;

	if(reply == NULL)
	{
		logprintf("Redis error: %s", context->errstr);
		result = context->err;
	}
	else if(reply->type == REDIS_REPLY_NIL)
	{
		logprintf("expected string reply but got nil");
		result = REDIS_ERROR_COMMAND_BAD_REPLY;
	}
	else if(reply->type != REDIS_REPLY_STRING)
	{
		logprintf("expected string reply but got %d", reply->type);
		result = REDIS_ERROR_COMMAND_BAD_REPLY;
	}
	else if(reply->len <= 0)
	{
		result = REDIS_ERROR_COMMAND_NO_REPLY;
	}
	else
	{
		value = atoi(reply->str);
	}

	freeReplyObject(reply);

	return result;
}

int Redisamp::SetFloat(int context_id, string key, float value)
{
	redisContext* context = NULL;
	int err = contextFromId(context_id, context);
	if(err)
		return err;

	redisReply *reply = redisCommand(context, "SET %s %f", key.c_str(), value);
	int result = 0;

	if(reply == NULL)
	{
		logprintf("Redis error: %s", context->errstr);
		result = context->err;
	}
	if(reply->type != REDIS_REPLY_STATUS)
	{
		logprintf("Redis reply error: %s", reply->str);
		result = REDIS_ERROR_COMMAND_BAD_REPLY;
	}

	freeReplyObject(reply);

	return result;
}

int Redisamp::GetFloat(int context_id, string key, float &value)
{
	redisContext* context = NULL;
	int err = contextFromId(context_id, context);
	if(err)
		return err;

	redisReply *reply = redisCommand(context, "GET %s", key.c_str());
	int result = 0;

	if(reply == NULL)
	{
		logprintf("Redis error: %s", context->errstr);
		result = context->err;
	}
	else if(reply->type == REDIS_REPLY_NIL)
	{
		logprintf("expected string reply but got nil");
		result = REDIS_ERROR_COMMAND_BAD_REPLY;
	}
	else if(reply->type != REDIS_REPLY_STRING)
	{
		logprintf("expected string reply but got %d", reply->type);
		result = REDIS_ERROR_COMMAND_BAD_REPLY;
	}
	else if(reply->len <= 0)
	{
		result = REDIS_ERROR_COMMAND_NO_REPLY;
	}
	else
	{
		value = atof(reply->str);
	}

	freeReplyObject(reply);

	return result;
}

int Redisamp::SetHashValue(int context_id, string key, string inner, string value)
{
	redisContext* context = NULL;
	int err = contextFromId(context_id, context);
	if(err)
		return err;

	redisReply *reply = redisCommand(context, "HSET %s %s %s", key.c_str(), inner.c_str(), value.c_str());
	int result = 0;

	if(reply == NULL)
	{
		logprintf("Redis context error: %s", context->errstr);
		result = context->err;
	}
	if(reply->type != REDIS_REPLY_INTEGER)
	{
		logprintf("Redis reply error: %s", reply->str);
		result = REDIS_ERROR_COMMAND_BAD_REPLY;
	}

	freeReplyObject(reply);

	return result;
}

int Redisamp::GetHashValue(int context_id, string key, string inner, string& value)
{
	redisContext* context = NULL;
	int err = contextFromId(context_id, context);
	if(err)
		return err;

	redisReply *reply = redisCommand(context, "HGET %s %s", key.c_str(), inner.c_str());
	int result = 0;

	if(reply == NULL)
	{
		logprintf("Redis context error: %s", context->errstr);
		result = context->err;
		value = "";
	}
	else if(reply->type == REDIS_REPLY_NIL)
	{
		logprintf("expected string reply but got nil");
		result = REDIS_ERROR_COMMAND_BAD_REPLY;
		value = "";
	}
	else if(reply->type != REDIS_REPLY_STRING)
	{
		logprintf("expected string reply but got %d", reply->type);
		result = REDIS_ERROR_COMMAND_BAD_REPLY;
		value = "";
	}
	else if(reply->len <= 0)
	{
		result = REDIS_ERROR_COMMAND_NO_REPLY;
		value = "";
	}
	else
	{
		value = string(reply->str);
	}

	freeReplyObject(reply);

	return result;
}

int Redisamp::SetHashValues(int context_id, string key, string inner, vector<string> values)
{
	return 1;
}

int Redisamp::GetHashValues(int context_id, string key, string inner, vector<string>& values)
{
	return 1;
}


int Redisamp::BindMessage(int context_id, string channel, string callback)
{
	redisContext* context = NULL;
	int err = contextFromId(context_id, context);
	if(err)
		return err;

	int result;

	std::thread* thr = nullptr;

	thr = new std::thread(await, context, channel, callback);

	if(thr == nullptr)
	{
		logprintf("unable to create thread for Redis await");
		result = REDIS_ERROR_SUBSCRIBE_THREAD_ERROR;
	}
	else
	{
		subscriptions[channel] = callback;
		thr->detach();
		delete thr;
		result = 0;
	}

	return result;
}

int Redisamp::SendMessage(int context_id, string channel, string data)
{
	redisContext* context = NULL;
	int err = contextFromId(context_id, context);
	if(err)
		return err;

	redisReply *reply = redisCommand(context, "LPUSH %s %s", channel.c_str(), data.c_str());
	int result = 0;

	if(reply == NULL)
	{
		logprintf("Redis error: %s", context->errstr);
		result = context->err;
	}
	if(reply->type != REDIS_REPLY_INTEGER)
	{
		logprintf("Redis SendMessage did not return an integer as expected");
		result = REDIS_ERROR_COMMAND_BAD_REPLY;
	}

	freeReplyObject(reply);

	return result;
}


/*
	Note:
	Internal functions not exposed to Pawn.
*/

void Redisamp::await(const redisContext *parent, const string channel, const string callback)
{
	struct timeval timeout_val = {1, 0};

	redisContext *context = redisConnectWithTimeout(parent->tcp.host, parent->tcp.port, timeout_val);

	if (context == NULL || context->err)
	{
		if (context)
		{
			logprintf("Redis await error on channel '%s': %s", channel.c_str(), context->errstr);
			redisFree(context);
			return;
		}
		else
		{
			return;
		}
		exit(1);
	}

	redisReply *reply;

	while(true)
	{
		reply = redisCommand(context, "BLPOP %s 0", channel.c_str());

		if(reply == NULL)
		{
			logprintf("Redis await error on channel '%s': reply null, context error: '%s'", channel.c_str(), context->errstr);
			return;
		}

		if(reply->type != REDIS_REPLY_ARRAY)
		{
			logprintf("Redis await error on channel '%s': reply type was not array", channel.c_str());
			continue;
		}

		if(reply->elements < 2)
		{
			logprintf("Redis await error on channel '%s': reply elements is %d", reply->elements);
			continue;
		}

		processMessages(reply, channel, callback);
		freeReplyObject(reply);
	}

	redisFree(context);

	logprintf("ERROR: Redis await on channel '%s' has stopped", channel.c_str());

	return;
}

void Redisamp::processMessages(const redisReply *reply, const string channel, const string callback)
{
	if(strcmp(channel.c_str(), reply->element[0]->str))
	{
		logprintf("Redis processMessages error on channel '%s': reply channel '%s' does not match", channel.c_str(), reply->element[0]->str);
		return;
	}

	for(int i = 1; i < reply->elements; ++i)
	{
		processMessage(reply->element[i], channel, callback);
	}
}

void Redisamp::processMessage(const redisReply *reply, const string channel, const string callback)
{
	message m;
	m.channel = channel;
	m.message = string(reply->str);
	m.callback = callback;

	message_stack_mutex.lock();
	message_stack.push(m);
	message_stack_mutex.unlock();
}

void Redisamp::amx_tick(AMX* amx)
{
	if(message_stack_mutex.try_lock())
	{
		message m;
		int error = 0;
		int amx_idx = -1;
		cell amx_addr;
		cell amx_ret;
		cell *phys_addr; 

		while(!message_stack.empty())
		{
			m = message_stack.top();

			error = amx_FindPublic(amx, m.callback.c_str(), &amx_idx);

			if(error == AMX_ERR_NONE)
			{
				/*
					Note:
					This is the part that calls the Pawn callback!
				*/
				amx_Push(amx, m.message.length());
				amx_PushString(amx, &amx_addr, &phys_addr, m.message.c_str(), 0, 0);

				amx_Exec(amx, &amx_ret, amx_idx);
				amx_Release(amx, amx_addr);

				if(amx_ret > 0)
				{
					// todo: something clever with the return value...
					logprintf("return from amx was %d", amx_ret);
				}
			}
			else
			{
				logprintf("ERROR: amx_FindPublic returned %d for callback '%s' channel '%s'", error, m.callback.c_str(), m.channel.c_str());
			}

			message_stack.pop();
		}
		message_stack_mutex.unlock();
	}

	return;
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