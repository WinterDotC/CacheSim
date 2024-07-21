# Introduction
A simple Cache Simulator for Cache Design Space Exploration. It is a byproduct of participating in the [One Student One Chip](www.ysyx.org)

# Instruction
```
make app
```

# How to run
For example, to simulate the Cache with 8 sets, 8 ways and the size of cache line is 8 bytes. The simulator will run with the least recently used algorithm (LRU) as replacement algorithm.
```
./CacheSim -f"bintrace.bin" -s3 -w3 -c3 -r"lru"
```