#include "ExperimentServer.h"
#include "EthernetInterface.h"

ExperimentServer::ExperimentServer() {
    _terminal = NULL;  
    _data_cnt = 0;
}

void ExperimentServer::attachTerminal( Serial & terminal) {
    _terminal = &terminal;
} 

void ExperimentServer::init() {
    // default configuration  
    char ip_address[] = "192.168.1.100";
    char subnet_mask[]= "255.255.255.0";
    char gateway[]    = "192.168.1.1";
    int server_port   = 11223;
    init(ip_address,subnet_mask,gateway, server_port);
}   
    
void ExperimentServer::init(const char * ip_addr, const char * subnet_mask, const char * gateway, unsigned int port) {
    if(_terminal!=NULL) {
        _terminal->printf("\n==============================\nStarting Server\n");
        _terminal->printf("...Intializing Ethernet\n");
    }
    _eth.init(ip_addr,subnet_mask,gateway);
    
    if(_terminal!=NULL)
        _terminal->printf("...Connecting\n");    
    _eth.connect();
    
    if(_terminal!=NULL)
        _terminal->printf("...Ethernet IP Address is %s\n", _eth.getIPAddress());
    
    _server.bind(port);
    if(_terminal!=NULL)
        _terminal->printf("...Listening on Port %d\n", port);
}

int ExperimentServer::getParams(float params[], int num_params) {
    if(_terminal!=NULL) {
        _terminal->printf("\n========================\nNew Experiment\n");
        _terminal->printf("...Waiting for parameters...\n");
    }
        
    int n = _server.receiveFrom(_client,(char *) params, sizeof(params)*sizeof(float));    
    if ( (n% 4) > 0 ) {
        if(_terminal!=NULL) {
            _terminal->printf("ERROR: input data bad size\n");
            _terminal->printf("ERROR: Expected %d got %d\n",4*num_params,n);
        }
        return false;    
    }
    if ( n / 4 != num_params) {
        if(_terminal!=NULL) {
            _terminal->printf("ERROR: input data bad size\n");
            _terminal->printf("ERROR: Expected %d got %d\n",4*num_params,n);
        }
        return false;    
    }
    
    if(_terminal!=NULL) {
        _terminal->printf("...Received input from: %s\n", _client.get_address());
        _terminal->printf("...Parameters: \n");
        for ( int j = 0 ; j < n/sizeof(float) ; j++) {
            _terminal->printf("      %d) %f\n",j+1,params[j]);
        }
    }
    return true;
}
void ExperimentServer::flushBuffer() {
    if(_data_cnt > 0) {
         _server.sendTo(_client,(char*) _buffer, 4*_data_cnt );    
        _data_cnt = 0;
    }
}

void ExperimentServer::sendData(float data_output[], int data_size) {
    if( data_size + _data_cnt > _MAX_BUFFER_SIZE) {
        flushBuffer();
    }
    for(int j = 0; j < data_size; j++) {
        _buffer[ _data_cnt++ ] = data_output[j];    
    }
}
void ExperimentServer::setExperimentComplete() {
    flushBuffer();
    char buff[] = {'0'};
    _server.sendTo(_client,buff,1);
}   