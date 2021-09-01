TARGET = bin/demo
SRCS = \
src/demo/main.cc \
src/demo/proto/c2s.pb.cc \
src/demo/c2s_login_message_handler.cc \
src/demo/c2s.cc \
src/demo/server_app.cc \


LINK_TYPE = exec
INCLUDE = -Isrc -Isrc/lib
BUILD_DIR = build
DEPFILE = \
build/libzbluenet.a \


LIB = \
-Lbuild -lzbluenet \
-Llib-bin -lprotobuf -lpthread \
-L/usr/lib64/mysql -lmysqlclient -lpthread -lrt \

include mak/main.mak