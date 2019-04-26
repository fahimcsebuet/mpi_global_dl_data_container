import os
import subprocess
import sys
import time
from pprint import pprint

def main():

    nvme_supports = [0, 1]
    memory_capacities = [100, 200, 300]

    print("NVM, MEM_CAP, LATENCY (NS)")
    for nvme_support in sorted(nvme_supports):
        for memory_capacity in sorted(memory_capacities):
            for iter in range(100):
                command_args = []
                command_args.append("./baseline")
                command_args.append("--capacity")
                command_args.append(str(memory_capacity))
                if nvme_support:
                    command_args.append("--enable-nvme")

                subprocess.call(command_args)


if __name__ == "__main__":
    main()
