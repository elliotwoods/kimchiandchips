/*
 *  TalkyMessage.cpp
 *  network protocol test
 *
 *  Created by Elliot Woods on 01/02/2011.
 *  Copyright 2011 Kimchi and Chips. All rights reserved.
 *
 */

#include "TalkyMessage.h"

TalkyMessage::TalkyMessage() :
hasPayload(false),
PayloadLength(0),
Version(0),
Timestamp(clock() / CLOCKS_PER_MILLISEC),
Type(0)
{
}

bool TalkyMessage::serialise(char* &message, int &remainingAvailableBytes)
{
	
	int len = getTotalLength();
	if (len > remainingAvailableBytes)
		return false;
	
	memcpy(message, Company, 2);
	
	memcpy(message+2, Protocol, 2);
	memcpy(message+4, &Version, 2);
	memcpy(message+6, &Timestamp, 4);
	memcpy(message+10, &Type, 2);
	
	memcpy(message+12, &PayloadLength, 2);
	memcpy(message+14, Payload, PayloadLength);
	
	message[len-1] = OFXTALKY_ENDCHAR;
	
	message += len;
	remainingAvailableBytes -= len;
	
	return true;	
}

bool TalkyMessage::deSerialise(char* &message, int &remainingBytesReceived)
{
	try {
		memcpy(Company, message, 2);
		
		memcpy(Protocol, message+2, 2);
		memcpy(&Version, message+4, 2);
		memcpy(&Timestamp, message+6, 4);
		memcpy(&Type, message+10, 2);
		
		unsigned short tempLength;
		memcpy(&tempLength, message+12, 2);
		initPayload(tempLength);
		
		if (getTotalLength() > remainingBytesReceived)
			return false;
		
		memcpy(Payload, message+14, PayloadLength);
		
		if (message[getTotalLength()-1] != OFXTALKY_ENDCHAR)
		{
			throw("TalkyMessage.cpp: End charachter of message is wrong when deserialising");
			return false;
		}
		
		
	}
	catch (string e) {
		throw("TalkyMessage.cpp: Failed to deserialise due to exception, likely memory overflow");
		return false;
	}
	
	message += getTotalLength();
	remainingBytesReceived -= getTotalLength();
	
	return true;
}

char *TalkyMessage::getPayload(int &length) const
{
	length = PayloadLength;
	return getPayload();
}

char *TalkyMessage::getPayload() const
{
	return Payload;
}

void TalkyMessage::setPayload(char* const message, unsigned short length)
{
	initPayload(length);
	memcpy(Payload, message, length);
}

int TalkyMessage::getTotalLength()
{
	return 2 + 2 + 2 + 2 + 2 + 4 + PayloadLength + 1;
}

string TalkyMessage::toString()
{
	
	stringstream out;

	out << "Company:\t" << string(Company, 2) << "\n";
	out << "Protocol:\t" << string(Protocol, 2) << "\n";
	out << "Version:\t" << Version << "\n";
	out << "Timestamp:\t" << Timestamp << "\n";
	out << "Type:\t" << Type << "\n";
	out << "PayloadLength:\t" << PayloadLength << "\n";
	
	out << "Payload:\n";
	
	for (int i=0; i<PayloadLength; i++)
	{
		if (Payload[i] > 32)
			out << Payload[i];
		else
			out << ".";
	}
	
	out << "\n\n";
	
	return out.str();
}

void TalkyMessage::setCompany(const char * s)
{
	memcpy(Company, s, 2);
}

void TalkyMessage::setProtocol(const char * s)
{
	memcpy(Protocol, s, 2);
}

void TalkyMessage::setTimestamp()
{
	Timestamp = clock() / CLOCKS_PER_MILLISEC;
}

bool TalkyMessage::operator<(const TalkyMessage& other) const
{
	return Timestamp < other.Timestamp;
}

void TalkyMessage::initPayload(unsigned short length)
{
	if (hasPayload && PayloadLength != length)
	{
		delete[] Payload;
		hasPayload = false;
	}
	
	if (!hasPayload)
	{
		Payload = new char[length];
		PayloadLength = length;
	}
}