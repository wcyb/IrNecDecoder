/*
 Name:		IrNecDecoderDemo.ino
 Created:	15/02/2019 9:50:24 PM
 Author:	Wojciech Cybowski (github.com/wcyb)
 License:	GPL v2
*/

#include "IrNecDecoder.h"
#include "RemotesCodes.h"

IrNecDecoder* irDec = nullptr;
uint32_t irPacket = 0;

/// <summary>
/// Called when IR data was received.
/// </summary>
void* irDataReceived(const uint32_t& data);
/// <summary>
/// Called when IR data is incomming (interrupt starts) !do not change!
/// </summary>
void irInterrupt(void);

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	irDec = new IrNecDecoder(static_cast<uint8_t>(3), irInterrupt, irDataReceived);
	Serial.println("...IR NEC Decoder started...");
}

// the loop function runs over and over again until power down or reset
void loop() {
	if (!irDec->getPacketsProcessingState())//if processing has stopped, then packet was received
	{
		Serial.println("-------------");

		Serial.print("Raw packet: "); Serial.println(irPacket, HEX);//print raw data

		Serial.print("Received addres: 0x");
		Serial.print(irDec->getReceivedAddress(), HEX);
		if (irDec->checkIfAddressCorrect()) Serial.println(" (Correct)");//check if received data is correct (assuming we not use extended mode)
		else//although, it may happen that wrong check data will be received (or wrong data with good check), so we get false-positive result
		{//so it would be best to check if received code exist in appropriate enum
			irDec->clearReceivedData();
			Serial.println(" (Incorrect)");
		}

		Serial.print("Received data: 0x");
		Serial.print(irDec->getReceivedData(), HEX);
		if (irDec->checkIfDataCorrect()) Serial.println(" (Correct)");
		else
		{
			irDec->clearReceivedData();
			Serial.println(" (Incorrect)");
		}

		if (irDec->getReceivedData() == getEnumValue(BlackRemote::Ok))//example how you can use enums for determining which key was pressed
		{
			Serial.println("OK button pressed!");
		}

		Serial.println("-------------");
		irDec->enablePacketsProcessing();//remember to enable packet processing after you end manipulations on received packet
	}
}

void* irDataReceived(const uint32_t& data)
{
	//you can pass received data to some other code here or just do nothing, because further data processing will be halted util
	//you confirm that you did what you wanted to do with received packet by calling enablePacketsProcessing()
	//if you don't need this to be called right after data is received, then you can omit passing function pointer to handler in constructor
	//---also remember that below code must execute as fast as possible because it is called inside interrupt---
	irPacket = data;
}

void irInterrupt()
{
	irDec->signalDetected();
}
