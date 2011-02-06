#pragma once
/*
 *  TalkyMessage.h
 *  network protocol test
 *
 *  Created by Elliot Woods on 01/02/2011.
 *  Copyright 2011 Kimchi and Chips. All rights reserved.
 *
 */

#define OFXTALKY_ENDCHAR '\n'

#include <ctime>
#include <string>
#include <sstream>

#define CLOCKS_PER_MILLISEC CLOCKS_PER_SEC / 1000

using namespace std;

class TalkyMessage
{
public:
	TalkyMessage();
	
	bool	serialise(char* &message, int &remainingAvailableBytes);
	bool	deSerialise(char* &message, int &remainingBytesReceived);
	
	char	*getPayload(int &length) const;
	char	*getPayload() const;
	
	void	setPayload(char* const message, unsigned short length);
	int		getTotalLength();
	
	string	toString();
	void	setCompany(const char * s);
	void	setProtocol(const char * s);
	void	setTimestamp();
	
	//for sorting by timestamp
	bool operator<(const TalkyMessage& other) const;
	
	//contents
	char			Company[2];
	
	char			Protocol[2];
	unsigned short	Version;
	int				Timestamp;
	
	unsigned short	Type;
	
	string			remoteHost;
	
private:
	void			initPayload(unsigned short length);
	
	unsigned short	PayloadLength;
	char			*Payload;
	
	bool			hasPayload;
	
};