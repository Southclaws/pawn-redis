#include <a_samp>
#define RUN_TESTS
#include <YSI\y_testing>

#include "../redis.inc"

new auth[41] = "77fe172e874dd2259ee051378f7fed549ea84cc5";

Test:ConnectDisconnect()
{
	new Redis:context = Redis_Connect("localhost", 6379, auth);
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
new Redis:context_ping;
TestInit:Ping()
{
	context_ping = Redis_Connect("localhost", 6379, auth);
}

Test:Ping()
{
	new ret = Redis_Command(context_ping, "PING");
	printf("ret: %d", ret);
	ASSERT(ret == 0);
}

TestClose:Ping()
{
	Redis_Disconnect(context_ping);
}


/*
	Set a string key then check if it exists.
*/
new Redis:context_exists;
TestInit:CheckExists()
{
	context_exists = Redis_Connect("localhost", 6379, auth);
}

Test:CheckExists()
{
	new exists = Redis_Exists(context_exists, "test_exists");
	ASSERT(exists == 0);

	new ret = Redis_SetInt(context_exists, "test_exists", 42);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	exists = Redis_Exists(context_exists, "test_exists");
	ASSERT(exists == 1);

	Redis_Command(context_exists, "DEL test_exists");
}

TestClose:CheckExists()
{
	Redis_Disconnect(context_exists);
}


/*
	Set a string key then get it and compare the value.
*/
new Redis:context_setgetstr;
TestInit:SetThenGetString()
{
	context_setgetstr = Redis_Connect("localhost", 6379, auth);
}

Test:SetThenGetString()
{
	new ret = Redis_SetString(context_setgetstr, "test", "hello world!");
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	new want[13] = {"hello world!"};
	new got[13];

	ret = Redis_GetString(context_setgetstr, "test", got, 13);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	printf("want: '%s' got: '%s'", want, got);
	ASSERT(strcmp(want, got) == 0);
}

TestClose:SetThenGetString()
{
	Redis_Disconnect(context_setgetstr);
}


/*
	Set an int key then get it and compare the value.
*/
new Redis:context_setgetint;
TestInit:SetThenGetInt()
{
	context_setgetint = Redis_Connect("localhost", 6379, auth);
}

Test:SetThenGetInt()
{
	new ret = Redis_SetInt(context_setgetint, "test", 42);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	new want = 42;
	new got;

	ret = Redis_GetInt(context_setgetint, "test", got);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	printf("want: '%d' got: '%d'", want, got);
	ASSERT(want == got);
}

TestClose:SetThenGetInt()
{
	Redis_Disconnect(context_setgetint);
}


/*
	Set a float key then get it and compare the value.
*/
new Redis:context_setgetfloat;
TestInit:SetThenGetFloat()
{
	context_setgetfloat = Redis_Connect("localhost", 6379, auth);
}

Test:SetThenGetFloat()
{
	new ret = Redis_SetFloat(context_setgetfloat, "test", 77.653);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	new Float:want = 77.653;
	new Float:got;

	ret = Redis_GetFloat(context_setgetfloat, "test", got);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	printf("want: '%f' got: '%f'", want, got);
	ASSERT(want == got);
}

TestClose:SetThenGetFloat()
{
	Redis_Disconnect(context_setgetfloat);
}


/*
	Bind a callback to a message channel
*/
new Redis:context_bind_1;
TestInit:MessageBindReply()
{
	context_bind_1 = Redis_Connect("localhost", 6379, auth);
}

Test:MessageBindReply()
{
	new ret = Redis_BindMessage(context_bind_1, "samp.test.1", "Receive");
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	ret = Redis_SendMessage(context_bind_1, "samp.test.1", "hello world!");
	printf("ret: %d", ret);
	ASSERT(ret == 0);
}

TestClose:MessageBindReply()
{
	Redis_Disconnect(context_bind_1);
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
new Redis:context_bind_2;
TestInit:MultiMessage()
{
	context_bind_2 = Redis_Connect("localhost", 6379, auth);
}

Test:MultiMessage()
{
	new ret = Redis_BindMessage(context_bind_2, "samp.test.2", "Receive2");
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	ret = Redis_SendMessage(context_bind_2, "samp.test.2", "to receive2");
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	ret = Redis_SendMessage(context_bind_2, "samp.test.2", "to receive2");
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	ret = Redis_SendMessage(context_bind_2, "samp.test.2", "to receive2");
	printf("ret: %d", ret);
	ASSERT(ret == 0);
}

TestClose:MultiMessage()
{
	Redis_Disconnect(context_bind_2);
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
new Redis:context_bind_3;
TestInit:DeferredMessage()
{
	context_bind_3 = Redis_Connect("localhost", 6379, auth);
}

Test:DeferredMessage()
{
	new ret = Redis_BindMessage(context_bind_3, "samp.test.3", "ReceiveLater");
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	SetTimer("SendLater", 3000, false);
}

forward SendLater();
public SendLater()
{
	new ret = Redis_SendMessage(context_bind_3, "samp.test.3", "is anyone there?");
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
new Redis:context_hash_1;
TestInit:SetThenGetHashValue()
{
	context_hash_1 = Redis_Connect("localhost", 6379, auth);
}

Test:SetThenGetHashValue()
{
	new ret;
	new want[32];
	want = "value";

	ret = Redis_SetHashValue(context_hash_1, "test.hash", "property", want);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	new got[32];

	ret = Redis_GetHashValue(context_hash_1, "test.hash", "property", got);
	printf("ret: %d", ret);
	ASSERT(ret == 0);

	printf("want: '%s' got: '%s'", want, got);
	ASSERT(!strcmp(want, got));
}

TestClose:SetThenGetHashValue()
{
	Redis_Disconnect(context_hash_1);
}
