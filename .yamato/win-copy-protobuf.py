import os
import shutil
import sys

extDir = sys.argv[1]
destDir = "installhere\\google\\protobuf"

skipDirs = ["java", "csharp", "objectivec", "ruby", "unittest"]

for root, dirs, files in os.walk(extDir):
    skip = False
    for sd in skipDirs:
        if sd in root:
            skip = True

    if skip:
        continue

    for file in files:
        if file.endswith(".h") or file.endswith(".inc"):
            dir_sans_prefix = root[len(extDir)+1:]
            dd = destDir + "\\" + dir_sans_prefix
            os.makedirs(dd, exist_ok=True)
            dst_file = dd + "\\" + file
            src_file = root + "\\" + file
            shutil.copyfile(src_file, dst_file)
