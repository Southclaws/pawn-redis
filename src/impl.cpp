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
    client->connect(host, port);

    if (auth.length() > 0) {
        auto req = client->auth(auth);
        client->sync_commit();
        auto r = req.get();

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
    auto req = client->send(cmd);
    client->sync_commit();
    auto r = req.get();

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

    auto req = client->exists(std::vector<std::string>{ key });
    client->sync_commit();
    auto r = req.get();

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

    auto req = client->set(key, value);
    client->sync_commit();
    auto r = req.get();

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

    auto req = client->get(key);
    client->sync_commit();
    auto r = req.get();

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

    auto req = client->set(key, std::to_string(value));
    client->sync_commit();
    auto r = req.get();

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

    auto req = client->get(key);
    client->sync_commit();
    auto r = req.get();

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

    auto req = client->set(key, std::to_string(value));
    client->sync_commit();
    auto r = req.get();

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

    auto req = client->get(key);
    client->sync_commit();
    auto r = req.get();

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

int Impl::SetHString(int client_id, std::string key, std::string field, std::string value)
{
    cpp_redis::client* client;
    int err = clientFromID(client_id, client);
    if (err) {
        return 1;
    }

    auto req = client->hset(key, field, value);
    client->sync_commit();
    auto r = req.get();

    if (r.is_error()) {
        logprintf("ERROR: %s", r.error().c_str());
        return 1;
    }

    return 0;
}

int Impl::GetHString(int client_id, std::string key, std::string field, std::string& value)
{
    cpp_redis::client* client;
    int err = clientFromID(client_id, client);
    if (err) {
        return 1;
    }

    auto req = client->hget(key, field);
    client->sync_commit();
    auto r = req.get();

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

int Impl::HDel(int client_id, std::string key, std::string field)
{
    cpp_redis::client* client;
    int err = clientFromID(client_id, client);
    if (err) {
        return 1;
    }

    auto req = client->hdel(key, std::vector<std::string>{ field });
    client->sync_commit();
    auto r = req.get();
    
    if (r.is_error()) {
        logprintf("ERROR: %s", r.error().c_str());
        return 2;
    }

    return 0;
}

int Impl::HExists(int client_id, std::string key, std::string field)
{
    cpp_redis::client* client;
    int err = clientFromID(client_id, client);
    if (err) {
        return 1;
    }

    auto req = client->hexists(key, field);
    client->sync_commit();
    auto r = req.get();

    if (r.is_error()) {
        logprintf("ERROR: %s", r.error().c_str());
        return 0;
    }

    return static_cast<int>(r.as_integer());
}

int Impl::HIncrBy(int client_id, std::string key, std::string field, int incr)
{
    cpp_redis::client* client;
    int err = clientFromID(client_id, client);
    if (err) {
        return 1;
    }

    auto req = client->hincrby(key, field, incr);
    client->sync_commit();
    auto r = req.get();

    if (r.is_error()) {
        logprintf("ERROR: %s", r.error().c_str());
        return 1;
    }

    return 0;
}

int Impl::HIncrByFloat(int client_id, std::string key, std::string field, float incr)
{
    cpp_redis::client* client;
    int err = clientFromID(client_id, client);
    if (err) {
        return 1;
    }

    auto req = client->hincrbyfloat(key, field, incr);
    client->sync_commit();
    auto r = req.get();

    if (r.is_error()) {
        logprintf("ERROR: %s", r.error().c_str());
        return 1;
    }

    return 0;
}

int Impl::Subscribe(std::string host, int port, std::string auth, std::string channel, std::string callback, int& id)
{
    cpp_redis::subscriber* sub = new cpp_redis::subscriber();
    sub->connect(host, port);

    if (auth.length() > 0) {
        sub->auth(auth);
    }

    sub->subscribe(channel, [callback](const std::string& chan, const std::string& msg) {
        message m;
        m.channel = chan;
        m.msg = msg;
        m.callback = callback;

        message_stack_mutex.lock();
        message_stack.push(m);
        message_stack_mutex.unlock();
    });

    sub->commit();

    clientData cd;
    cd.subscriber = sub;
    cd.channel = channel;
    cd.host = host;
    cd.port = port;
    cd.auth = auth;
    cd.isPubSub = true;
    clients[context_count] = cd;

    id = context_count++;

    return 0;
}

int Impl::Unsubscribe(int client_id)
{
    clientData cd;

    int err = clientDataFromID(client_id, cd);
    if (err) {
        return 1;
    }

    cd.subscriber->unsubscribe(cd.channel);
    cd.subscriber->commit();
    
    clients.erase(client_id);

    return 0;
}

int Impl::Publish(int client_id, std::string channel, std::string data)
{
    cpp_redis::client* client;
    int err = clientFromID(client_id, client);
    if (err) {
        return 1;
    }

    auto req = client->publish(channel, data);
    client->sync_commit();
    auto r = req.get();
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

std::vector<std::string> Impl::split(const std::string s)
{
    std::vector<std::string> result;
    std::istringstream iss(s);
    std::string tmp;

    while (iss >> std::quoted(tmp)) {
        result.push_back(tmp);
    }

    return result;
}
