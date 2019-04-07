/*
 Name:		IrNecDecoder.h
 Created:	15/02/2019 9:32:05 PM
 Author:	Wojciech Cybowski (github.com/wcyb)
 Editor:	http://www.visualmicro.com
 License:	GPL v2
*/

#ifndef _IrNecDecoder_h
#define _IrNecDecoder_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#define NEC_ONE 2250 //logical 1 in ms
#define NEC_ZERO 1125 //logical 0 in ms
#define NEC_AGC 9000 //AGC burst time in ms
#define NEC_SPACE 4500 //space time in ms after AGC burst
#define NEC_REPEAT 100000 //length of repeat command in ms
#define NEC_DEVIATION 10 //percent of acceptable deviation from standard !only thing you can change here!
#define NEC_DATA_BITS 32 //number of bits in one packet

constexpr unsigned long NEC_ONE_MAX = NEC_ONE + (NEC_ONE * (NEC_DEVIATION * 0.01));//max time of logical 1 in ms
constexpr unsigned long NEC_ONE_MIN = NEC_ONE - (NEC_ONE * (NEC_DEVIATION * 0.01));//min time of logical 1 in ms
constexpr unsigned long NEC_ZERO_MAX = NEC_ZERO + (NEC_ZERO * (NEC_DEVIATION * 0.01));//max time of logical 0 in ms
constexpr unsigned long NEC_ZERO_MIN = NEC_ZERO - (NEC_ZERO * (NEC_DEVIATION * 0.01));//min time of logical 0 in ms
constexpr unsigned long NEC_REPEAT_MAX = NEC_REPEAT + (NEC_REPEAT * (NEC_DEVIATION * 0.01));//max time of repeat command in ms
constexpr unsigned long NEC_REPEAT_MIN = NEC_REPEAT - (NEC_REPEAT * (NEC_DEVIATION * 0.01));//min time of repeat command in ms

class IrNecDecoder final
{
public:
	/// <summary>
	/// Possible decoder states.
	/// </summary>
	enum class decoderState : uint8_t { enabled, disabled };

	/// <summary>
	/// Sets decoder state to enabled or disabled.
	/// </summary>
	/// <param name="stateToSet">Enable or disable</param>
	/// <param name="externalHandler">Function that will be called when interrupt happens, it must call signalDetected(). This parameter can be omitted when disabling decoder.</param>
	void setDecoderState(const decoderState& stateToSet, void(*externalHandler)() = nullptr);
	/// <summary>
	/// Returns current decoder state (enabled or disabled), but packet processing state may be different.
	/// </summary>
	/// <returns>Decoder state</returns>
	const decoderState& getDecoderState(void) { return m_state; }

	/// <summary>
	/// Function that must be called by interrupt.
	/// </summary>
	void signalDetected(void);

	/// <summary>
	/// Returns received address from last packet.
	/// </summary>
	/// <returns>last received address</returns>
	uint8_t getReceivedAddress(void) { return (m_decodedData & 0xFF000000) >> 24; }
	/// <summary>
	/// Returns received extended address from last packet.
	/// </summary>
	/// <returns>last received extended address</returns>
	uint16_t getReceivedExtendedAddress(void) { return (m_decodedData & 0xFFFF0000) >> 16; }

	/// <summary>
	/// Returns received data from last packet.
	/// </summary>
	/// <returns>last received data</returns>
	uint8_t getReceivedData(void) { return (m_decodedData & 0x0000FF00) >> 8; }
	/// <summary>
	/// Returns received extended data from last packet.
	/// </summary>
	/// <returns>last received extended data</returns>
	uint16_t getReceivedExtendedData(void) { return (m_decodedData & 0x0000FFFF); }

	/// <summary>
	/// Returns raw packet.
	/// </summary>
	/// <returns>received packet</returns>
	const uint32_t& getRawPacket(void) { return m_decodedData; }

	/// <summary>
	/// Clears received data.
	/// </summary>
	void clearReceivedData(void) { m_receivedBits = 0; m_decodedData = 0; }

	/// <summary>
	/// Enables IR packet processing.
	/// </summary>
	void enablePacketsProcessing(void) { m_irPacketsProcessingEnabled = true; }
	/// <summary>
	/// Disables IR packet processing, but not disabes interrupt handling.
	/// </summary>
	void disablePacketsProcessing(void) { m_irPacketsProcessingEnabled = false; }
	/// <summary>
	/// Returns packet processing state. All received data will be ignored when packet processing is disabled.
	/// </summary>
	/// <returns>Current packet processing state.</returns>
	const bool& getPacketsProcessingState(void) { return m_irPacketsProcessingEnabled; }

	/// <summary>
	/// Checks whether received address is correct (assuming that address is not extended).
	/// </summary>
	/// <returns>true if address is correct, false otherwise</returns>
	bool checkIfAddressCorrect(void) { return !((m_decodedData & 0xFF000000) & ((m_decodedData & 0x00FF0000) << 8)); }
	/// <summary>
	/// Checks whether received data is correct (assuming that data is not extended).
	/// </summary>
	/// <returns>true if data is correct, false otherwise</returns>
	bool checkIfDataCorrect(void) { return !(((m_decodedData & 0x0000FF00) >> 8) & (m_decodedData & 0x000000FF)); }

public:
	/// <summary>
	/// Constructor for IR NEC Decoder object.
	/// </summary>
	/// <param name="receiverPin">Pin on which IR receiver is connected. Refer to documentation of attachInterrupt() to know what pin you can use.</param>
	/// <param name="externalInterruptStarter">Function that will call signalDetected() from this class.</param>
	/// <param name="dataCallback">Function that will be called when data packet will be received. May be omitted if not needed.</param>
	IrNecDecoder(const uint8_t& receiverPin, void(*externalInterruptStarter)(), void*(*dataCallback)(const uint32_t&) = nullptr)
		: m_receiverPin(receiverPin), externalStarter(externalInterruptStarter), dataCallbackFunction(dataCallback)
	{
		pinMode(m_receiverPin, INPUT);
		setDecoderState(decoderState::enabled, externalStarter);
	};

	IrNecDecoder(const IrNecDecoder&) = delete;
	IrNecDecoder& operator=(const IrNecDecoder&) = delete;

	/// <summary>
	/// Destructor for IR NEC Decoder object. It will deattach interrupt handler.
	/// </summary>
	~IrNecDecoder() { setDecoderState(decoderState::disabled); }

private:
	/// <summary>
	/// Current decoder state.
	/// </summary>
	decoderState m_state;
	/// <summary>
	/// Current IR packets processing state.
	/// </summary>
	bool m_irPacketsProcessingEnabled = true;

	/// <summary>
	/// Time count used to extract bits from received data.
	/// </summary>
	unsigned long m_usTimeCount = 0;
	/// <summary>
	/// Decoded data.
	/// </summary>
	uint32_t m_decodedData = 0;
	/// <summary>
	/// Received bits count.
	/// </summary>
	uint8_t m_receivedBits = 0;

	/// <summary>
	/// Function called when received IR data processing ends.
	/// </summary>
	void*(*dataCallbackFunction)(const uint32_t&);
	/// <summary>
	/// Interrupt handler function.
	/// </summary>
	void(*externalStarter)();

	/// <summary>
	/// IR input data pin.
	/// </summary>
	const uint8_t m_receiverPin;

private:
	/// <summary>
	/// Processes received IR signal.
	/// </summary>
	void risingEdgeDetected(void);
};

#endif
