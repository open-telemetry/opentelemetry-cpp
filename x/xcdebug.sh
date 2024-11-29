pwd
ls -l
#/usr/bin/xcdebug $(pwd)/../../$*
export PATH=$(pwd):$PATH
/usr/bin/xcdebug $(pwd)/$*
