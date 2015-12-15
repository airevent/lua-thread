--

local thread = require "thread"
local trace = require "trace"
local zmq = require "zmq"

--

--print("sink started")

local ctx = assert(zmq.context())

local sock = assert(ctx:socket(zmq.f.ZMQ_PULL))
assert(sock:bind("inproc://tasksback"))

--

local n = 0
local ts = zmq.microtime()

while true do
    local msg = assert(sock:recv())

    if msg=="kill" then
        break
    else
        n = n + 1
    end
end

print("sink ended with stat: ts="..(zmq.microtime()-ts)..", n="..n)
