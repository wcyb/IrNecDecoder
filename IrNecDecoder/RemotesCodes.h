#pragma once
/*
 Name:		RemotesCodes.h
 Created:	15/02/2019 9:32:05 PM
 Author:	Wojciech Cybowski (github.com/wcyb)
 Editor:	http://www.visualmicro.com
 License:	GPL v2
*/

#ifndef ENUM_CONVERSION
#define ENUM_CONVERSION 1

template<typename Enumerator>
uint8_t getEnumValue(const Enumerator& enumType)
{
	return static_cast<uint8_t>(enumType);
}

#endif // ENUM_CONVERSION

enum class BlackRemote : uint8_t {
	Up = 98, Left = 34, Ok = 2, Right = 194, Down = 168, One = 104, Two = 152, Three = 176, Four = 48,
	Five = 24, Six = 122, Seven = 16, Eight = 56, Nine = 90, Asterix = 66, Zero = 74, Hash = 82
};

enum class MusicRemote : uint8_t {
	ChMinus = 162, Ch = 98, ChPlus = 226, Prev = 34, Next = 2, PlayPause = 194, VolMinus = 224, VolPlus = 168, Eq = 144, Zero = 104,
	OneHundredPlus = 152, TwoHundredPlus = 176, One = 48, Two = 24, Three = 122, Four = 16, Five = 56, Six = 90, Seven = 66, Eight = 74, Nine = 82
};