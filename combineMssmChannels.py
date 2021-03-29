import os
import re
import sys

def compare(x):
    result = re.search("\.([0-9]{1,3})\.", x)
    if result: result = int(result.group(1))
    # print result,
    return result


dir = sys.argv[1]

for file in sorted(os.listdir(dir), key=compare):
    if not file.endswith(".err"):
        continue

    path = os.path.join(dir, file)
    with open(path) as f:
        if "Warning in <ROOT::Math::GSLInterpolator::Eval>: input domain error" in f.read():
            print path, " -> Failed"
            continue
    print path, " -> Successful"
