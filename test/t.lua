--

local thread = require "thread"
local trace = require "trace"
local zmq = require "zmq"

--

print("worker #"..thread.get_id().." started")

--

local ctx = assert(zmq.context())
local sock = assert(ctx:socket(zmq.f.ZMQ_PULL))
assert(sock:connect("inproc://tasks"))

--

while true do
    local msg = assert(sock:recv(0))
    print("worker #"..thread.get_id().." received task: "..msg)
end
