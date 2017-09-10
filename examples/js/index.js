var redis = require("redis"),
    client = redis.createClient();

client.on("error", (err) => {
    console.log("ERROR: " + err);
});

client.blpop("OnPlayerText", 0, (err, res) => {
    if (err != "") {
        console.log("ERROR: " + err)
    } else {
        console.log(res)
    }
})
