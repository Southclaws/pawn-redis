#include "redis.inc"

#define RUN_TESTS

#include <a_samp>
#include <YSI\y_testing>


Test:ConnectDisconnect()
{
	new Redis:client;
	new ret;
	
	ret = Redis_Connect("localhost", 6379, "", Redis:client);
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
	ASSERT(ret == REDIS_ERROR_CONTEXT_INVALID_ID);
}


/*
	Simple ping
*/
new Redis:client_ping;
TestInit:Ping()
{
	client_ping = Redis_Connect("localhost", 6379, "");
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


/*
	Set a string key then check if it exists.
*/
new Redis:client_exists;
TestInit:CheckExists()
{
	client_exists = Redis_Connect("localhost", 6379, "");
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


/*
	Set a string key then get it and compare the value.
*/
new Redis:client_setgetstr;
TestInit:SetThenGetString()
{
	client_setgetstr = Redis_Connect("localhost", 6379, "");
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


/*
	Set an int key then get it and compare the value.
*/
new Redis:client_setgetint;
TestInit:SetThenGetInt()
{
	client_setgetint = Redis_Connect("localhost", 6379, "");
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


/*
	Set a float key then get it and compare the value.
*/
new Redis:client_setgetfloat;
TestInit:SetThenGetFloat()
{
	client_setgetfloat = Redis_Connect("localhost", 6379, "");
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


/*
	Bind a callback to a message channel
*/
new Redis:client_bind_1;
TestInit:MessageBindReply()
{
	client_bind_1 = Redis_Connect("localhost", 6379, "");
}

Test:MessageBindReply()
{
	new ret = Redis_BindMessage(client_bind_1, "samp.test.1", "Receive");
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	ret = Redis_SendMessage(client_bind_1, "samp.test.1", "hello world!");
	printf("ret: %d", ret);
	ASSERT(ret == 0);
}

TestClose:MessageBindReply()
{
	Redis_Disconnect(client_bind_1);
}

forward Receive(data[]);
public Receive(data[])
{
	if(!strcmp(data, "hello world!"))
		printf("\n\nPASS!\n\n*** Redis bind message callback 'Receive' returned the correct value: '%s' test passed!", data);

	else
		printf("\n\nFAIL!\n\n*** Redis bind message callback 'Receive' returned the incorrect value: '%s'", data);
}


/*
	Bind multiple callbacks to multiple message channels.
*/
new Redis:client_bind_2;
TestInit:MultiMessage()
{
	client_bind_2 = Redis_Connect("localhost", 6379, "");
}

Test:MultiMessage()
{
	new ret = Redis_BindMessage(client_bind_2, "samp.test.2", "Receive2");
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	ret = Redis_SendMessage(client_bind_2, "samp.test.2", "to receive2");
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	ret = Redis_SendMessage(client_bind_2, "samp.test.2", "to receive2");
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	ret = Redis_SendMessage(client_bind_2, "samp.test.2", "to receive2");
	printf("ret: %d", ret);
	ASSERT(ret == 0);
}

TestClose:MultiMessage()
{
	Redis_Disconnect(client_bind_2);
}

forward Receive2(data[]);
public Receive2(data[])
{
	if(!strcmp(data, "to receive2"))
		printf("\n\nPASS!\n\n*** Redis bind message callback 'Receive2' returned the correct value: '%s' test passed!", data);

	else
		printf("\n\nFAIL!\n\n*** Redis bind message callback 'Receive2' returned the incorrect value: '%s'", data);
}


/*
	Bind a callback to a message channel and wait a little for a reply.
*/
new Redis:client_bind_3;
TestInit:DeferredMessage()
{
	client_bind_3 = Redis_Connect("localhost", 6379, "");
}

Test:DeferredMessage()
{
	new ret = Redis_BindMessage(client_bind_3, "samp.test.3", "ReceiveLater");
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	SetTimer("SendLater", 3000, false);
}

forward SendLater();
public SendLater()
{
	new ret = Redis_SendMessage(client_bind_3, "samp.test.3", "is anyone there?");
	printf("ret: %d", ret);
	ASSERT(ret == 0);
}

forward ReceiveLater(data[]);
public ReceiveLater(data[])
{
	if(!strcmp(data, "is anyone there?"))
		printf("\n\nPASS!\n\n*** Redis bind message callback 'ReceiveLater' returned the correct value: '%s' test passed!", data);

	else
		printf("\n\nFAIL!\n\n*** Redis bind message callback 'ReceiveLater' returned the incorrect value: '%s'", data);
}


/*
	Set a hash and check the get value
*/
new Redis:client_hash_1;
TestInit:SetThenGetHashValue()
{
	client_hash_1 = Redis_Connect("localhost", 6379, "");
}

Test:SetThenGetHashValue()
{
	new ret;
	new want[32];
	want = "value";

	ret = Redis_SetHashValue(client_hash_1, "test.hash", "property", want);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	new got[32];

	ret = Redis_GetHashValue(client_hash_1, "test.hash", "property", got);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	printf("want: '%s' got: '%s'", want, got);
	ASSERT(!strcmp(want, got));
}

TestClose:SetThenGetHashValue()
{
	Redis_Disconnect(client_hash_1);
}
