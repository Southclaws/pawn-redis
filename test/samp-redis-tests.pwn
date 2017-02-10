#include <a_samp>
#define RUN_TESTS
#include <YSI\y_testing>

#include "../samp-redis.inc"

new Redis:context;


Test:ConnectDisconnect()
{
	context = Redis_Connect("localhost", 6379);
	printf("context: %d", _:context);
	ASSERT(context != Redis:-1);

	new ret = Redis_Disconnect(context);
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
TestInit:Ping()
{
	context = Redis_Connect("localhost", 6379);
}

Test:Ping()
{
	new ret = Redis_Command(context, "PING");
	printf("ret: %d", ret);
	ASSERT(ret == 0);
}

TestClose:Ping()
{
	Redis_Disconnect(context);
}


/*
	Set a string key then get it and compare the value.
*/
TestInit:SetThenGetString()
{
	context = Redis_Connect("localhost", 6379);
}

Test:SetThenGetString()
{
	new ret = Redis_SetString(context, "test", "hello world!");
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	new want[13] = {"hello world!"};
	new got[13];

	ret = Redis_GetString(context, "test", got, 13);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	printf("want: '%s' got: '%s'", want, got);
	ASSERT(strcmp(want, got) == 0);
}

TestClose:SetThenGetString()
{
	Redis_Disconnect(context);
}


/*
	Set a string key then get it and compare the value.
*/
TestInit:SetThenGetInt()
{
	context = Redis_Connect("localhost", 6379);
}

Test:SetThenGetInt()
{
	new ret = Redis_SetInt(context, "test", 42);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	new want = 42;
	new got;

	ret = Redis_GetInt(context, "test", got);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	printf("want: '%d' got: '%d'", want, got);
	ASSERT(want == got);
}

TestClose:SetThenGetInt()
{
	Redis_Disconnect(context);
}


/*
	Set a string key then get it and compare the value.
*/
TestInit:SetThenGetFloat()
{
	context = Redis_Connect("localhost", 6379);
}

Test:SetThenGetFloat()
{
	new ret = Redis_SetFloat(context, "test", 77.653);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	new Float:want = 77.653;
	new Float:got;

	ret = Redis_GetFloat(context, "test", got);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	printf("want: '%f' got: '%f'", want, got);
	ASSERT(want == got);
}

TestClose:SetThenGetFloat()
{
	Redis_Disconnect(context);
}


/*
	Bind a callback to a message channel
*/
TestInit:MessageBindReply()
{
	context = Redis_Connect("localhost", 6379);
}

Test:MessageBindReply()
{
	new ret = Redis_BindMessage(context, "samp.test.1", "Receive");
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	ret = Redis_SendMessage(context, "samp.test.1", "hello world!");
	printf("ret: %d", ret);
	ASSERT(ret == 0);
}

TestClose:MessageBindReply()
{
	Redis_Disconnect(context);
}

forward Receive(data[]);
public Receive(data[])
{
	if(!strcmp(data, "hello world!"))
		printf("\n\nPASS!\n\n*** Redis bind message callback returned the correct value: '%s' test passed!", data);

	else
		printf("\n\nFAIL!\n\n*** Redis bind message callback returned the incorrect value: '%s'", data);
}
