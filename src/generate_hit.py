#!/usr/bin/python
import argparse
samples_per_us = 0.2647
mvolts_per_bit = 3400.0 / 256 # should be 3300 but this bbb measures at 3.42v

parser = argparse.ArgumentParser(description="generate hit test data")
parser.add_argument('--zero-val', action='store', type=int, help="zero value (mv)", default=0)
parser.add_argument('--hit-max', action='store', type=int, help="max hit value (mv)", default=2000)
parser.add_argument('--hit-len', action='store', type=int, help="hit length (us)", default=60)
parser.add_argument('--hit-slope', action='store', type=int, help="slope of hit (mv/us)", default=100)

args = parser.parse_args()

# do conversions from us and mv to samples and bits
args.zero_val /= mvolts_per_bit # mvolts converted to byte
args.hit_max /= mvolts_per_bit # mvolts converted to byte
args.hit_len /= samples_per_us # us converted to samples
args.hit_slope /= (mvolts_per_bit/samples_per_us)


val = args.zero_val
samples = []

while True:
    if val < args.hit_max and len(samples) < args.hit_len:
        val += args.hit_slope
    elif len(samples) > args.hit_len:
        val -= args.hit_slope
        if val < args.zero_val:
            break
    # clamp
    if val > 255:
        val = 255
    if val < 0:
        val = 0
    samples.append(val)
	
# pad with zeros, and ensure
# samples must be even number long
for pad in range(len(samples) % 2 + 50):
    samples.append(args.zero_val)

# check signal is short enough
assert len(samples) < 7500 
with open("data.txt", 'w') as fh:
    for val in samples:
        fh.write("%d\n" % val)
print("hit file generated, %d samples long" % len(samples))
