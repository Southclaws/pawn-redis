#include <a_samp>

#include "../samp-redis.inc"


public OnFilterScriptInit()
{
	new Redis:context;

	print("Connecting to Redis at localhost:6379...");
	context = Redis_Connect("localhost", 6379, 1);

	printf("Connected, context ID: %d", _:context);

	print("Sleeping for a second...");

	new now = GetTickCount();
	while(now + 1000 > GetTickCount())
		continue;

	print("Disconnecting, bye!");
	Redis_Disconnect(context);
}