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
		This file contains the actual Redis implementation code including the
		message binding, threading and callback mechanism.


==============================================================================*/

#include "impl.hpp"

int Impl::context_count;
std::map<int, Impl::clientData> Impl::clients;
std::map<std::string, std::string> Impl::subscriptions;
std::stack<Impl::message> Impl::message_stack;
std::mutex Impl::message_stack_mutex;

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
int Impl::Connect(std::string host, int port, std::string auth, int& id)
{
    cpp_redis::client* client = new cpp_redis::client();
    try {
        client->connect(host, port);
    } catch (cpp_redis::redis_error e) {
        logprintf("ERROR: %s", e.what());
        return 1;
    }

    if (auth.length() > 0) {
        auto r = client->auth(auth).get();
        if (r.is_error()) {
            logprintf("ERROR: %s", r.error().c_str());
            return 2;
        }
    }

    clientData cd;
    cd.client = client;
    cd.host = host;
    cd.port = port;
    cd.auth = auth;
    clients[context_count] = cd;

    id = context_count++;

    return 0;
}

int Impl::Disconnect(int client_id)
{
    cpp_redis::client* client;
    int err = clientFromID(client_id, client);
    if (err) {
        return err;
    }

    clients.erase(client_id);

    return 0;
}

int Impl::Command(int client_id, std::string command)
{
    cpp_redis::client* client;
    int err = clientFromID(client_id, client);
    if (err) {
        return 1;
    }

	std::vector<std::string> cmd = split(command);
    auto r = client->send(cmd).get();
    if (r.is_error()) {
        logprintf("ERROR: %s", r.error().c_str());
        return 2;
    }

    return 0;
}

int Impl::Exists(int client_id, std::string key)
{
    cpp_redis::client* client;
    int err = clientFromID(client_id, client);
    if (err) {
        return 0;
    }

    logprintf("creating req");
    auto req = client->exists(std::vector<std::string>{ key });

    logprintf("waiting on req");
    auto r = req.get();

    logprintf("checking err");
    if (r.is_error()) {
        logprintf("ERROR: %s", r.error().c_str());
        return 0;
    }

    return static_cast<int>(r.as_integer());
}

int Impl::SetString(int client_id, std::string key, std::string value)
{
    cpp_redis::client* client;
    int err = clientFromID(client_id, client);
    if (err) {
        return 1;
    }

    auto r = client->set(key, value).get();
    if (r.is_error()) {
        logprintf("ERROR: %s", r.error().c_str());
        return 1;
    }

    return 0;
}

int Impl::GetString(int client_id, std::string key, std::string& value)
{
    cpp_redis::client* client;
    int err = clientFromID(client_id, client);
    if (err) {
        return 1;
    }

    auto r = client->get(key).get();
    if (r.is_error()) {
        logprintf("ERROR: %s", r.error().c_str());
        return 1;
    } else if (r.get_type() == cpp_redis::reply::type::null) {
        return 2;
    } else if (r.get_type() != cpp_redis::reply::type::bulk_string) {
        return 3;
    } else {
        value = r.as_string();
    }

    return 0;
}

int Impl::SetInt(int client_id, std::string key, int value)
{
    cpp_redis::client* client;
    int err = clientFromID(client_id, client);
    if (err) {
        return 1;
    }

    auto r = client->set(key, std::to_string(value)).get();
    if (r.is_error()) {
        logprintf("ERROR: %s", r.error().c_str());
        return 2;
    }

    return 0;
}

int Impl::GetInt(int client_id, std::string key, int& value)
{
    cpp_redis::client* client;
    int err = clientFromID(client_id, client);
    if (err) {
        return 1;
    }

    auto r = client->get(key).get();
    if (r.is_error()) {
        logprintf("ERROR: %s", r.error().c_str());
        return 2;
    } else if (r.get_type() == cpp_redis::reply::type::null) {
        return 3;
    } else if (r.get_type() != cpp_redis::reply::type::bulk_string) {
        return 4;
    } else {
        value = std::atoi(r.as_string().c_str());
    }

    return 0;
}

int Impl::SetFloat(int client_id, std::string key, float value)
{
    cpp_redis::client* client;
    int err = clientFromID(client_id, client);
    if (err) {
        return 1;
    }

    auto r = client->set(key, std::to_string(value)).get();
    if (r.is_error()) {
        logprintf("ERROR: %s", r.error().c_str());
        return 1;
    }

    return 0;
}

int Impl::GetFloat(int client_id, std::string key, float& value)
{
    cpp_redis::client* client;
    int err = clientFromID(client_id, client);
    if (err) {
        return 1;
    }

    auto r = client->get(key).get();
    if (r.is_error()) {
        logprintf("ERROR: %s", r.error().c_str());
        return 2;
    } else if (r.get_type() == cpp_redis::reply::type::null) {
        return 3;
    } else if (r.get_type() != cpp_redis::reply::type::bulk_string) {
        return 4;
    } else {
        value = static_cast<float>(std::atof(r.as_string().c_str()));
    }

    return 0;
}

int Impl::SetHashValue(int client_id, std::string key, std::string inner, std::string value)
{
    cpp_redis::client* client;
    int err = clientFromID(client_id, client);
    if (err) {
        return 1;
    }

    auto r = client->hset(key, inner, value).get();
    if (r.is_error()) {
        logprintf("ERROR: %s", r.error().c_str());
        return 2;
    }
    if (r.get_type() != cpp_redis::reply::type::integer) {
        return 3;
    }

    return 0;
}

int Impl::GetHashValue(int client_id, std::string key, std::string inner, std::string& value)
{
    cpp_redis::client* client;
    int err = clientFromID(client_id, client);
    if (err) {
        return 1;
    }

    auto r = client->hget(key, inner).get();
    if (r.is_error()) {
        logprintf("ERROR: %s", r.error().c_str());
        return 2;
    } else if (r.get_type() == cpp_redis::reply::type::null) {
        return 3;
    } else if (r.get_type() != cpp_redis::reply::type::simple_string) {
        return 4;
    } else {
        value = r.as_string();
    }

    return 0;
}

int Impl::Subscribe(std::string host, int port, std::string auth, std::string channel, std::string callback)
{
    cpp_redis::subscriber* sub = new cpp_redis::subscriber();
    sub->connect(host, port);
    sub->auth(auth);

    sub->subscribe(channel, [callback](const std::string& chan, const std::string& msg) {
        message m;
        m.channel = chan;
        m.msg = msg;
        m.callback = callback;

        message_stack_mutex.lock();
        message_stack.push(m);
        message_stack_mutex.unlock();
    });

    clientData cd;
    cd.subscriber = sub;
    cd.host = host;
    cd.port = port;
    cd.auth = auth;
    cd.isPubSub = true;
    clients[context_count] = cd;

    return context_count++;
}

int Impl::Publish(int client_id, std::string channel, std::string data)
{
    cpp_redis::client* client;
    int err = clientFromID(client_id, client);
    if (err) {
        return 1;
    }

    auto r = client->publish(channel, data).get();
    if (r.is_error()) {
        logprintf("ERROR: %s", r.error().c_str());
        return 2;
    }

    return 0;
}

void Impl::amx_tick(AMX* amx)
{
    if (message_stack_mutex.try_lock()) {
        message m;
        int error = 0;
        int amx_idx = -1;
        cell amx_addr;
        cell amx_ret;
        cell* phys_addr;

        while (!message_stack.empty()) {
            m = message_stack.top();

            error = amx_FindPublic(amx, m.callback.c_str(), &amx_idx);

            if (error == AMX_ERR_NONE) {
                /*
                Note:
                This is the part that calls the Pawn callback!
                */
                amx_Push(amx, m.msg.length());
                amx_PushString(amx, &amx_addr, &phys_addr, m.msg.c_str(), 0, 0);

                amx_Exec(amx, &amx_ret, amx_idx);
                amx_Release(amx, amx_addr);

                if (amx_ret > 0) {
                    // todo: something clever with the return value...
                    // logprintf("return from amx was %d", amx_ret);
                }
            } else {
                logprintf("ERROR: Redis amx_FindPublic returned %d for callback '%s' channel '%s'",
                    error,
                    m.callback.c_str(),
                    m.channel.c_str());
            }

            message_stack.pop();
        }
        message_stack_mutex.unlock();
    }

    return;
}

int Impl::clientFromID(int client_id, cpp_redis::client*& client)
{
    try {
        auto cd = clients.at(client_id);
        client = cd.client;
    } catch (...) {
        return 1;
    }

    return 0;
}

int Impl::clientDataFromID(int client_id, clientData& cd)
{
    try {
        cd = clients.at(client_id);
    } catch (...) {
        return 1;
    }

    return 0;
}

std::vector<std::string> Impl::split(const std::string& s)
{
    std::vector<std::string> result;
    std::istringstream iss(s);
    std::copy(std::istream_iterator<std::string>(iss),
        std::istream_iterator<std::string>(),
        back_inserter(result));

    return result;
}
