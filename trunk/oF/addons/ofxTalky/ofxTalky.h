/*
 *  ofxTalky.h
 *
 *  Created by Elliot Woods on 01/02/2011.
 *  Copyright 2011 Kimchi and Chips. All rights reserved.
 *
 */

#include "TalkyBase.h"

#include "ofxThread.h"
#include "ofxNetwork.h"
#include "ofMain.h"

class ofxTalky : public ofxThread, public TalkyBase
{
public:
	~ofxTalky();
    
    //setup as client
	void setup(string remoteHost, int remotePort);
	
	//setup as server
	void setup(int localPort);
	
	bool	getIsServerBound();
	int		getNumClients();
	
protected:
    void threadedFunction();
    
    void beginThread();  
    bool lockThread();
    void unlockThread();
    bool lockServer();
    void unlockServer();
    
    void startClient();
    void startServer();
    
    bool isClientConnected();
    bool isServerConnected();
    bool isServersClientConnected(int iClient);
	
    int getNumServerClients();
    
    int rxServer(int iClient, char *buffer, int bufferSize);
    int rxClient(char *buffer, int bufferSize);
    
    void txServer(int iClient, char *buffer, int messageSize);
    void txClient(char *buffer, int messageSize);
    
	ofxTCPServer	*tcpServer;
	ofxTCPClient	*tcpClient;
};