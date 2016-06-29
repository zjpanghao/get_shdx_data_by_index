ALL= tele_run_shanghai_index
LIB= -lcurl -lrdkafka -lpthread -lrt -lz -lssl -lcrypto -ljsoncpp -lglog
LIB_DIRS= -L. -Wl,-rpath=. -L/usr/local/lib
Include = -I/usr/local/include/librdkafka -I/usr/local/include/zdb 
Flags = -g -O2 -fPIC -Wall -Wsign-compare -Wfloat-equal -Wpointer-arith 

objects= main.o json_data.o json_value.o json_writer.o json_reader.o  basictypes.o base64.o KafkaWrapper.o CurlWrapper.o tele_compress.o

$(ALL) : $(objects)
	g++ -o $(Iclude) $@ $(objects) $(LIB_DIRS) $(LIB)
%.o:%.cpp
	g++  -c $(Include)  $< -o $@
.PHONY : clean

clean:
	rm $(objects)

