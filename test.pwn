#include "redis.inc"

#define RUN_TESTS

#include <a_samp>
#include <YSI_Core\y_testing>


Test:ConnectDisconnect()
{
	new Redis:client;
	new ret;

	ret = Redis_Connect("localhost", 6379, "", client);
	printf("client: %d", _:client);
	ASSERT(client != Redis:-1);

	ret = Redis_Disconnect(client);
	printf("ret: %d", ret);
	ASSERT(ret == 0);
}

Test:DisconnectInvalid()
{
	new ret = Redis_Disconnect(Redis:2835672);
	printf("ret: %d", ret);
	ASSERT(ret == 1);
}


// -
// Simple ping
// -

new Redis:client_ping;
TestInit:Ping()
{
	new ret = Redis_Connect("localhost", 6379, "", client_ping);
	ASSERT(ret == 0);
}

Test:Ping()
{
	new ret = Redis_Command(client_ping, "PING");
	printf("ret: %d", ret);
	ASSERT(ret == 0);
}

TestClose:Ping()
{
	Redis_Disconnect(client_ping);
}


// -
// Set a string key then check if it exists.
// -

new Redis:client_exists;
TestInit:CheckExists()
{
	new ret = Redis_Connect("localhost", 6379, "", client_exists);
	ASSERT(ret == 0);
}

Test:CheckExists()
{
	new exists = Redis_Exists(client_exists, "test_exists");
	ASSERT(exists == 0);

	new ret = Redis_SetInt(client_exists, "test_exists", 42);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	exists = Redis_Exists(client_exists, "test_exists");
	ASSERT(exists == 1);

	Redis_Command(client_exists, "DEL test_exists");
}

TestClose:CheckExists()
{
	Redis_Disconnect(client_exists);
}


// -
// Set a string key then get it and compare the value.
// -

new Redis:client_setgetstr;
TestInit:SetThenGetString()
{
	new ret = Redis_Connect("localhost", 6379, "", client_setgetstr);
	ASSERT(ret == 0);
}

Test:SetThenGetString()
{
	new ret = Redis_SetString(client_setgetstr, "test", "hello world!");
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	new want[13] = {"hello world!"};
	new got[13];

	ret = Redis_GetString(client_setgetstr, "test", got, 13);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	printf("want: '%s' got: '%s'", want, got);
	ASSERT(strcmp(want, got) == 0);
}

TestClose:SetThenGetString()
{
	Redis_Disconnect(client_setgetstr);
}


// -
// Set an int key then get it and compare the value.
// -

new Redis:client_setgetint;
TestInit:SetThenGetInt()
{
	new ret = Redis_Connect("localhost", 6379, "", client_setgetint);
	ASSERT(ret == 0);
}

Test:SetThenGetInt()
{
	new ret = Redis_SetInt(client_setgetint, "test", 42);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	new want = 42;
	new got;

	ret = Redis_GetInt(client_setgetint, "test", got);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	printf("want: '%d' got: '%d'", want, got);
	ASSERT(want == got);
}

TestClose:SetThenGetInt()
{
	Redis_Disconnect(client_setgetint);
}


// -
// Set a float key then get it and compare the value.
// -

new Redis:client_setgetfloat;
TestInit:SetThenGetFloat()
{
	new ret = Redis_Connect("localhost", 6379, "", client_setgetfloat);
	ASSERT(ret == 0);
}

Test:SetThenGetFloat()
{
	new ret = Redis_SetFloat(client_setgetfloat, "test", 77.653);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	new Float:want = 77.653;
	new Float:got;

	ret = Redis_GetFloat(client_setgetfloat, "test", got);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	printf("want: '%f' got: '%f'", want, got);
	ASSERT(want == got);
}

TestClose:SetThenGetFloat()
{
	Redis_Disconnect(client_setgetfloat);
}


// -
// Run A Command
// -

new Redis:client_runCommand;
TestInit:RunCommand()
{
	new ret = Redis_Connect("localhost", 6379, "", client_runCommand);
	ASSERT(ret == 0);
}

Test:RunCommand()
{
	new ret = Redis_Command(client_runCommand, "LPUSH test_list \"Hello World\"");
	printf("ret: %d", ret);
	ASSERT(ret == 0);
}

TestClose:RunCommand()
{
	Redis_Disconnect(client_runCommand);
}


// -
// Bind a callback to a message channel
// -

new PubSub:pubsub_1;
new Redis:client_pubsub_1;
TestInit:MessageBindReply()
{
	new ret = Redis_Subscribe("localhost", 6379, "", "samp.test.1", "Receive", pubsub_1);
	ASSERT(ret == 0);

	ret = Redis_Connect("localhost", 6379, "", client_pubsub_1);
	ASSERT(ret == 0);
}

Test:MessageBindReply()
{
	new ret = Redis_Publish(client_pubsub_1, "samp.test.1", "hello world!");
	printf("ret: %d", ret);
	ASSERT(ret == 0);
}

TestClose:MessageBindReply()
{
	// Redis_Unsubscribe(pubsub_1);
}

forward Receive(data[]);
public Receive(data[])
{
	if(!strcmp(data, "hello world!")) {
		printf("\n\nPASS!\n\n*** Redis bind message callback 'Receive' returned the correct value: '%s' test passed!", data);
	} else {
		printf("\n\nFAIL!\n\n*** Redis bind message callback 'Receive' returned the incorrect value: '%s'", data);
	}
}


// -
// Bind multiple callbacks to multiple message channels.
// -

new PubSub:pubsub_2;
new Redis:client_pubsub_2;
TestInit:MultiMessage()
{
	new ret = Redis_Subscribe("localhost", 6379, "", "samp.test.2", "Receive2", pubsub_2);
	ASSERT(ret == 0);

	ret = Redis_Connect("localhost", 6379, "", client_pubsub_2);
	ASSERT(ret == 0);
}

Test:MultiMessage()
{
	new ret;

	ret = Redis_Publish(client_pubsub_2, "samp.test.2", "to receive2");
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	ret = Redis_Publish(client_pubsub_2, "samp.test.2", "to receive2");
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	ret = Redis_Publish(client_pubsub_2, "samp.test.2", "to receive2");
	printf("ret: %d", ret);
	ASSERT(ret == 0);
}

TestClose:MultiMessage()
{
	// Redis_Unsubscribe(pubsub_2);
}

forward Receive2(data[]);
public Receive2(data[])
{
	if(!strcmp(data, "to receive2"))
		printf("\n\nPASS!\n\n*** Redis bind message callback 'Receive2' returned the correct value: '%s' test passed!", data);

	else
		printf("\n\nFAIL!\n\n*** Redis bind message callback 'Receive2' returned the incorrect value: '%s'", data);
}
