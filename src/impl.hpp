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

#ifndef PAWN_REDIS_IMPL_H
#define PAWN_REDIS_IMPL_H

#include <iterator>
#include <map>
#include <mutex>
#include <sstream>
#include <stack>
#include <string>
#include <thread>
#include <vector>

using std::string;
using std::vector;

#include <cpp_redis/cpp_redis>
#include <amx/amx2.h>

#include "common.hpp"

namespace Impl {

enum E_REDIS_ERROR {
	REDIS_ERROR_CONNECT_GENERIC = -1,
	REDIS_ERROR_CONNECT_FAIL = -2,
	REDIS_ERROR_CONNECT_AUTH = -3,
	REDIS_ERROR_CONTEXT_INVALID_ID = 10,
	REDIS_ERROR_CONTEXT_MISSING_POINTER = 20,
	REDIS_ERROR_COMMAND_BAD_REPLY = 30,
	REDIS_ERROR_COMMAND_NO_REPLY = 40,
	REDIS_ERROR_SUBSCRIBE_THREAD_ERROR = 50,
	REDIS_ERROR_UNEXPECTED_RESULT_TYPE = 60,
	REDIS_ERROR_INTERNAL_ERROR = 70
};

struct clientData {
	cpp_redis::client* client;
	std::string host;
	int port;
	std::string auth;
	bool isPubSub;
	cpp_redis::subscriber* subscriber;
};

struct subscription {
    string channel;
    string callback;
};

struct message {
    string channel;
    string msg;
    string callback;
};

int Connect(string hostname, int port, string auth);
int Disconnect(int client_id);

int Command(int client_id, string command);
int Exists(int client_id, string key);
int SetString(int client_id, string key, string value);
int GetString(int client_id, string key, string& value);
int SetInt(int client_id, string key, int value);
int GetInt(int client_id, string key, int& value);
int SetFloat(int client_id, string key, float value);
int GetFloat(int client_id, string key, float& value);

int SetHashValue(int client_id, string key, string inner, string value);
int GetHashValue(int client_id, string key, string inner, string& value);

int Subscribe(string host, int port, string auth, string channel, string callback);
int Publish(int client_id, string channel, string message);

int clientFromID(int client_id, cpp_redis::client*& client);
int clientDataFromID(int client_id, clientData& client);
void amx_tick(AMX* amx);
std::vector<std::string> split(const std::string& s);

extern int context_count;
extern std::map<int, clientData> clients;
extern std::map<string, string> subscriptions;
extern std::stack<Impl::message> message_stack;
extern std::mutex message_stack_mutex;
}

#endif
