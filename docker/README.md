# OpenTelemetry library builder

**How to use this build script:**

The build script `build.sh` can be used to build OpenTelemetry and
it's dependencies within the desired base image.
The final image only contains the necessary files in `/`.

```sh
bash build.sh -b alpine-latest -j ${nproc} -o main

# copy to current dir
docker create -ti --name otel otel-cpp-alpine-latest bash
docker cp otel:/ ./
docker rm -f otel
```
