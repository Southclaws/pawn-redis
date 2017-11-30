/*==============================================================================


	Redis for SA:MP

		Copyright (C) 2016 Barnaby "Southclaws" Keene

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
		This header and it's counterpart .cpp are the only two files that don't
		really contain generic SA:MP plugin boilerplate code. See the .cpp for
		implementation details.


==============================================================================*/

#ifndef SAMP_REDIS_IMPL_H
#define SAMP_REDIS_IMPL_H

#include <map>
#include <mutex>
#include <stack>
#include <string>
#include <thread>
#include <vector>

using std::string;
using std::vector;

#include "hiredis/hiredis.h"

#include "main.hpp"

#define REDIS_ERROR_CONNECT_GENERIC (-1)
#define REDIS_ERROR_CONNECT_FAIL (-2)
#define REDIS_ERROR_CONNECT_AUTH (-3)
#define REDIS_ERROR_CONTEXT_INVALID_ID (10)
#define REDIS_ERROR_CONTEXT_MISSING_POINTER (20)
#define REDIS_ERROR_COMMAND_BAD_REPLY (30)
#define REDIS_ERROR_COMMAND_NO_REPLY (40)
#define REDIS_ERROR_SUBSCRIBE_THREAD_ERROR (50)
#define REDIS_ERROR_UNEXPECTED_RESULT_TYPE (60)

namespace Redisamp {

/*
	Note:
	A subscription represents an active waiting channel.
	Since a new context must exist for each thread, I decided not to offload
	that responsibility to the user so the plugin internally derives a new
	context from the parent context upon subscription creation. It's likely
	that most users will only have a single connection anyway so it's best to
	keep it all under one single context ID in Pawn space.

	Since subscriptions are mapped by channel and multiple subscriptions to the
	same channel is illogical, this makes finding an existing subscription
	trivial.
*/
struct subscription {
    string channel;
    string callback;
};

struct message {
    string channel;
    string message;
    string callback;
};

int Connect(string hostname, int port, string auth);
int Disconnect(int context_id);

int Command(int context_id, string command);
int Exists(int context_id, string key);
int SetString(int context_id, string key, string value);
int GetString(int context_id, string key, string& value);
int SetInt(int context_id, string key, int value);
int GetInt(int context_id, string key, int& value);
int SetFloat(int context_id, string key, float value);
int GetFloat(int context_id, string key, float& value);

int SetHashValue(int context_id, string key, string inner, string value);
int GetHashValue(int context_id, string key, string inner, string& value);
int SetHashValues(int context_id, string key, string inner, vector<string> value);
int GetHashValues(int context_id, string key, string inner, vector<string>& value);

int BindMessage(int context_id, string channel, string callback);
int SendMessage(int context_id, string channel, string message);

/*
	Note:
	I'm a Golang fanboy so I'm using Go's universal style:
	Public exports begin with UpperCase, privates begin with lowerCase
*/
void await(const redisContext* parent, string auth, const string channel, const string callback);
void processMessages(const redisReply* reply, const string channel, const string callback);
void processMessage(const redisReply* reply, const string channel, const string callback);
void amx_tick(AMX* amx);
int contextFromId(int context_id, redisContext*& context);

extern int context_count;
extern std::map<int, redisContext*> contexts;
extern std::map<int, string> auths;
extern std::map<string, string> subscriptions;
extern std::stack<Redisamp::message> message_stack;
extern std::mutex message_stack_mutex;
}

#endif
