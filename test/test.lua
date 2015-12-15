--

local thread = require "thread"
local trace = require "trace"
local zmq = require "zmq"

--

local ctx = assert(zmq.context())
local sock = assert(ctx:socket(zmq.f.ZMQ_PUSH))
assert(sock:bind("inproc://tasks"))

--

local workers = {}
local workers_n = 8

for i=1,workers_n do
    local t, tid = assert(thread.start("t.lua", {}, { "_zmq_ctx" }))
    workers[tid] = t
end

local n = 0
while true do
    n = n + 1
    sock:send("task #"..n, 0)
    zmq.sleep(1)
end

for tid,t in pairs(workers) do
    thread.join(t)
end
