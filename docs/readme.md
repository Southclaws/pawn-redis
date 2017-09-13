# Redis Plugin Documentation

This is an introduction to Redis+SA:MP, you do not need to know anything about Redis but this documentation assumes an intermediate understanding of Pawn, SA:MP servers and the basics of external databases (i.e.: not SQLite).

## What The Hell is a Redis?

Redis, at the most basic level, is a key-value database. It's like a glorified `.ini` file. You can store data like this:

```
password=mycatpatrick
ip-address=192.168.1.1
```

Pretty impressive right? Okay, maybe not so much. How about this:

I'm assuming you know what a *global variable* is:

```c
#include <a_samp>

new gMyFavouriteColour = 0xFF3200FF;

main() {
    printf("My favourite colour in decimal notation: %d", gMyFavouriteColour);
}

public OnPlayerUpdate() {
    printf("Just in case you forgot my favourite colour: %d", gMyFavouriteColour);
}
```

Every function in this script can access `gMyFavouriteColour` - yeah I know, this is basic stuff - but it will help you understand the power of Redis:

### Redis is like global variables on steroids.

If `gMyFavouriteColour` was stored in Redis instead of the SA:MP server's heap memory, instead of every function having access to the value, every *application* on the machine now has access to it!

What's more, if you set Redis up correctly, you can have a server running on the other side of the world that also has access to `gMyFavouriteColour`, now if that ain't a *"global variable"* I don't know what is!

---

## Further Examples

- [User Control Panel](ucp/readme.md)
- [Scavenge and Survive](ss/readme.md)