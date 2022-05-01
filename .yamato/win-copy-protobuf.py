import os
import shutil
import sys

extDir = sys.argv[1]
destDir = "trick/google/protobuf"

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
            os.makedirs(destDir + "/" | dir_sans_prefix, exist_ok=True)
            dest_file = destDir + "/" + file
            shutil.copyfile(root + "/" + file, dest_file)
