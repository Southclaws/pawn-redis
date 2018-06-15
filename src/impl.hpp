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
#include <iomanip>

#include <amx/amx2.h>
#include <cpp_redis/cpp_redis>
#include <cpp_redis/misc/logger.hpp>

#include "common.hpp"

namespace Impl {

struct clientData {
    cpp_redis::client* client;
    std::string host;
    int port;
    std::string auth;
    bool isPubSub;
    cpp_redis::subscriber* subscriber;
};

struct subscription {
    std::string channel;
    std::string callback;
};

struct message {
    std::string channel;
    std::string msg;
    std::string callback;
};

int Connect(std::string hostname, int port, std::string auth, int& id);
int Disconnect(int client_id);

int Command(int client_id, std::string command);
int Exists(int client_id, std::string key);
int SetString(int client_id, std::string key, std::string value);
int GetString(int client_id, std::string key, std::string& value);
int SetInt(int client_id, std::string key, int value);
int GetInt(int client_id, std::string key, int& value);
int SetFloat(int client_id, std::string key, float value);
int GetFloat(int client_id, std::string key, float& value);

int Subscribe(std::string host, int port, std::string auth, std::string channel, std::string callback, int& id);
int Publish(int client_id, std::string channel, std::string message);

int clientFromID(int client_id, cpp_redis::client*& client);
int clientDataFromID(int client_id, clientData& client);
void amx_tick(AMX* amx);
std::vector<std::string> split(const std::string s);

extern int context_count;
extern std::map<int, clientData> clients;
extern std::map<std::string, std::string> subscriptions;
extern std::stack<Impl::message> message_stack;
extern std::mutex message_stack_mutex;
}

#endif
