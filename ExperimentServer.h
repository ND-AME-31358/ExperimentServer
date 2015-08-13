#include "EthernetInterface.h"
#include "mbed.h"

#define _MAX_BUFFER_SIZE 200

typedef void (*p_handler)(float[]);

class ExperimentServer
{
public:
     /**
     * Constructor.
     *
     * Constructs an empty experiment server. Server objects require further
     * initialization before use.
     */
    ExperimentServer();
    
     /**
     *
     * Links a serial terminal with the server for debugging.
     *
     * @param terminal Serial terminal to be used for debug messages
     */
    void attachTerminal( Serial & terminal); 
    
    void init(const char * addr, const char * subnet, const char * host, unsigned int port);
    
    int getParams(float params[], int num_params);
    void sendData(float data_output[], int data_size);
    void setExperimentComplete();
    
private:
    
    void flushBuffer();
    
    EthernetInterface _eth;
    Endpoint _client;
    UDPSocket _server;
    Serial * _terminal;
    
    float _buffer[_MAX_BUFFER_SIZE];
    int _data_cnt;
};