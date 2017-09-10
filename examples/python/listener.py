import redis

r = redis.StrictRedis(host='localhost', port=6379, db=0)

while True:
    response, options = r.blpop("OnPlayerText", 0)
    print(response)
