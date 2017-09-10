#include <a_samp>
#include <redis>

static Redis:context

main() {
    context = Redis_Connect("localhost", 6379, "");
}

public OnPlayerText(playerid, text[]) {
    new
        name[MAX_PLAYER_NAME],
        str[MAX_PLAYER_NAME + 3 + 128];
    
    GetPlayerName(playerid, name, MAX_PLAYER_NAME);
    format(str, sizeof(str), "%s: %s", name, text);

    Redis_SendMessage(context, "OnPlayerText", str);

    return 1;
}
