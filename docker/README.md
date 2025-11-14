# OpenTelemetry library builder

**How to use this build script:**

The build script `build.sh` can be used to build OpenTelemetry and
it's dependencies within the desired base image.
The final image contains the build of opentelemetry-cpp and its dependencies in `/opt/opentelemetry-cpp-install`.

```sh
bash build.sh -b alpine-latest -j ${nproc} -o main

# copy to current dir
docker create -ti --name otel otel-cpp-alpine-latest bash
docker cp otel:/opt/opentelemetry-cpp-install ./
docker rm -f otel
```
