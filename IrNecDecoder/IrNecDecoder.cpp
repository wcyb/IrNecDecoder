/*
 Name:		IrNecDecoder.cpp
 Created:	15/02/2019 9:32:05 PM
 Author:	Wojciech Cybowski (github.com/wcyb)
 Editor:	http://www.visualmicro.com
 License:	GPL v2
*/

#include "IrNecDecoder.h"

void IrNecDecoder::setDecoderState(const decoderState& stateToSet, void(*externalHandler)() = nullptr)
{
	if (stateToSet == decoderState::disabled) { detachInterrupt(digitalPinToInterrupt(m_receiverPin)); m_state = stateToSet; }
	else if (externalHandler != nullptr) { attachInterrupt(digitalPinToInterrupt(m_receiverPin), externalHandler, RISING); m_state = stateToSet; }
}

void IrNecDecoder::signalDetected(void)
{
	risingEdgeDetected();
}

void IrNecDecoder::risingEdgeDetected(void)
{
	if (!m_irPacketsProcessingEnabled) return;
	unsigned long timePeriod = micros() - m_usTimeCount;

	if (timePeriod >= NEC_ONE_MIN && timePeriod <= NEC_ONE_MAX)
	{
		m_decodedData = (m_decodedData << 1) | 1;
		m_receivedBits++;
	}
	else if (timePeriod >= NEC_ZERO_MIN && timePeriod <= NEC_ZERO_MAX)
	{
		m_decodedData = (m_decodedData << 1);
		m_receivedBits++;
	}
	else if (timePeriod >= NEC_REPEAT_MIN && timePeriod <= NEC_REPEAT_MAX)
	{
		m_receivedBits = 0;
		disablePacketsProcessing();
		if (dataCallbackFunction) dataCallbackFunction(m_decodedData);//or return other code (like 0xFFFFFFFF) to specify that REPEAT was received
	}

	if (m_receivedBits == NEC_DATA_BITS)
	{
		m_receivedBits = 0;
		disablePacketsProcessing();
		if (dataCallbackFunction) dataCallbackFunction(m_decodedData);
	}

	m_usTimeCount = micros();
}
