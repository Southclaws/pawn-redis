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
#include <thread>

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
std::map<string, Redisamp::subscription> Redisamp::subscriptions;

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


int Redisamp::Subscribe(int context_id, string channel, string callback)
{
	redisContext* context = NULL;
	int err = contextFromId(context_id, context);
	if(err)
		return err;

	int result;

	std::thread* thr = nullptr;
	subscription s;
	s.parent = context;
	s.context = nullptr;
	s.channel = channel;
	s.callback = callback;

	thr = new std::thread(subscribe, s);
	logprintf("thread started");

	if(thr == nullptr)
	{
		logprintf("unable to create thread for Redis subscribe");
		result = REDIS_ERROR_SUBSCRIBE_THREAD_ERROR;
	}
	else
	{
		logprintf("sub created, channel %s", channel.c_str());
		s.thread_id = thr->get_id();

		subscriptions[channel] = s;

		thr->detach();

		delete thr;
		result = 0;
	}
	logprintf("finished");
	return result;
}

void Redisamp::subscribe(subscription sub)
{
	struct timeval timeout_val = {1, 0};

	sub.context = redisConnectWithTimeout(sub.parent->tcp.host, sub.parent->tcp.port, timeout_val);

	if (sub.context == NULL || sub.context->err)
	{
		if (sub.context)
		{
			logprintf("Redis subscribe error: %s", sub.context->errstr);
			redisFree(sub.context);
			return;
		}
		else
		{
			return;
		}
		exit(1);
	}

	redisReply *reply = redisCommand(sub.context, "SUBSCRIBE %s", sub.channel.c_str());

	if(reply == NULL)
	{
		logprintf("Redis subscribe error: %s", sub.context->errstr);
		return;
	}

	freeReplyObject(reply);

	struct timeval timeval = {1, 0};
	redisSetTimeout(sub.context, timeval);

	logprintf("subscribed to %s", sub.channel.c_str());

	int ret;

	while(true)
	{
		ret = redisGetReply(sub.context, &reply);

		if(ret == -1)
		{
			logprintf("context error: %s", sub.context->errstr);
			continue;
		}

		logprintf("got reply from subscription: %d", ret);

		// logprintf(reply->type);
		// logprintf(reply->str);

		if(ret == REDIS_OK)
		{
			freeReplyObject(reply);
		}
	}

	return;
}

int Redisamp::Unsubscribe(int context_id, string channel)
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