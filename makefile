ALL= tele_run_shanghai_index
LIB= -lcurl -lrdkafka -lpthread -lrt -lz  -lcrypto -ljsoncpp -lglog
env=/home/panghao/env
LIB_DIRS= -L. -Wl,-rpath=. -L/usr/local/lib -L$(env)/libs -Wl,-rpath=/usr/local/lib
Include = /usr/local/include/librdkafka 
Flags = -g -O2 -fPIC -Wall -Wsign-compare -Wfloat-equal -Wpointer-arith 

objects= main.o json_data.o json_value.o json_writer.o json_reader.o  basictypes.o base64.o KafkaWrapper.o CurlWrapper.o tele_compress.o

$(ALL) : $(objects)
	g++ -o  $@ $(objects) $(LIB_DIRS) $(LIB)
%.o:%.cpp
	g++  -I $(Include) -c   $< -o $@
.PHONY : clean

clean:
	rm $(objects)

