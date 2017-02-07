#include <a_samp>

#include "../samp-redis.inc"


public OnFilterScriptInit()
{
	new Redis:context;

	print("Connecting to Redis at localhost:6379...");
	context = Redis_Connect("localhost", 6379, 1);

	printf("Connected, context ID: %d", _:context);

	print("Disconnecting, bye!");
	new ret;
	ret = Redis_Disconnect(context);
	printf("returned: %d", ret);

	printf("attempting to disconnect from same context, this should return an error");
	ret = Redis_Disconnect(context);
	printf("returned: %d", ret);
}
