// Scintilla source code edit control
// Encoding: UTF-8
/** @file CaseConvert.cxx
 ** Case fold characters and convert them to upper or lower case.
 ** Tables automatically regenerated by scripts/GenerateCharacterCategory.py
 ** Should only be rarely regenerated for new versions of Unicode.
 **/
// Copyright 2013 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstring>

#include <vector>
#include <algorithm>

#include "CaseConvert.h"
#include "UniConversion.h"
#include "UnicodeFromUTF8.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

namespace {
	// Use an unnamed namespace to protect the declarations from name conflicts

// Unicode code points are ordered by groups and follow patterns.
// Most characters (pitch==1) are in ranges for a particular alphabet and their
// upper case forms are a fixed distance away.
// Another pattern (pitch==2) is where each lower case letter is preceded by
// the upper case form. These are also grouped into ranges.

int symmetricCaseConversionRanges[] = {
//lower, upper, range length, range pitch
//++Autogenerated -- start of section automatically generated
//**\(\*\n\)
97,65,26,1, 
224,192,23,1, 
248,216,7,1, 
257,256,24,2, 
314,313,8,2, 
331,330,23,2, 
462,461,8,2, 
479,478,9,2, 
505,504,20,2, 
547,546,9,2, 
583,582,5,2, 
945,913,17,1, 
963,931,9,1, 
985,984,12,2, 
1072,1040,32,1, 
1104,1024,16,1, 
1121,1120,17,2, 
1163,1162,27,2, 
1218,1217,7,2, 
1233,1232,44,2, 
1377,1329,38,1, 
7681,7680,75,2, 
7841,7840,48,2, 
7936,7944,8,1, 
7952,7960,6,1, 
7968,7976,8,1, 
7984,7992,8,1, 
8000,8008,6,1, 
8032,8040,8,1, 
8560,8544,16,1, 
9424,9398,26,1, 
11312,11264,47,1, 
11393,11392,50,2, 
11520,4256,38,1, 
42561,42560,23,2, 
42625,42624,12,2, 
42787,42786,7,2, 
42803,42802,31,2, 
42879,42878,5,2, 
42913,42912,5,2, 
65345,65313,26,1, 
66600,66560,40,1, 

//--Autogenerated -- end of section automatically generated
};

// Code points that are symmetric but don't fit into a range of similar characters
// are listed here.

int symmetricCaseConversions[] = {
//lower, upper
//++Autogenerated -- start of section automatically generated
//**1 \(\*\n\)
255,376, 
307,306, 
309,308, 
311,310, 
378,377, 
380,379, 
382,381, 
384,579, 
387,386, 
389,388, 
392,391, 
396,395, 
402,401, 
405,502, 
409,408, 
410,573, 
414,544, 
417,416, 
419,418, 
421,420, 
424,423, 
429,428, 
432,431, 
436,435, 
438,437, 
441,440, 
445,444, 
447,503, 
454,452, 
457,455, 
460,458, 
477,398, 
499,497, 
501,500, 
572,571, 
575,11390, 
576,11391, 
578,577, 
592,11375, 
593,11373, 
594,11376, 
595,385, 
596,390, 
598,393, 
599,394, 
601,399, 
603,400, 
608,403, 
611,404, 
613,42893, 
614,42922, 
616,407, 
617,406, 
619,11362, 
623,412, 
625,11374, 
626,413, 
629,415, 
637,11364, 
640,422, 
643,425, 
648,430, 
649,580, 
650,433, 
651,434, 
652,581, 
658,439, 
881,880, 
883,882, 
887,886, 
891,1021, 
892,1022, 
893,1023, 
940,902, 
941,904, 
942,905, 
943,906, 
972,908, 
973,910, 
974,911, 
983,975, 
1010,1017, 
1016,1015, 
1019,1018, 
1231,1216, 
7545,42877, 
7549,11363, 
8017,8025, 
8019,8027, 
8021,8029, 
8023,8031, 
8048,8122, 
8049,8123, 
8050,8136, 
8051,8137, 
8052,8138, 
8053,8139, 
8054,8154, 
8055,8155, 
8056,8184, 
8057,8185, 
8058,8170, 
8059,8171, 
8060,8186, 
8061,8187, 
8112,8120, 
8113,8121, 
8144,8152, 
8145,8153, 
8160,8168, 
8161,8169, 
8165,8172, 
8526,8498, 
8580,8579, 
11361,11360, 
11365,570, 
11366,574, 
11368,11367, 
11370,11369, 
11372,11371, 
11379,11378, 
11382,11381, 
11500,11499, 
11502,11501, 
11507,11506, 
11559,4295, 
11565,4301, 
42874,42873, 
42876,42875, 
42892,42891, 
42897,42896, 
42899,42898, 

//--Autogenerated -- end of section automatically generated
};

// Characters that have complex case conversions are listed here.
// This includes cases where more than one character is needed for a conversion,
// folding is different to lowering, or (as appropriate) upper(lower(x)) != x or
// lower(upper(x)) != x.

const char *complexCaseConversions =
// Original | Folded | Upper | Lower |
//++Autogenerated -- start of section automatically generated
//**2 \(\*\n\)
"µ|μ|Μ||"
"ß|ss|SS||"
"İ|i̇||i̇|"
"ı||I||"
"ŉ|ʼn|ʼN||"
"ſ|s|S||"
"ǅ|ǆ|Ǆ|ǆ|"
"ǈ|ǉ|Ǉ|ǉ|"
"ǋ|ǌ|Ǌ|ǌ|"
"ǰ|ǰ|J̌||"
"ǲ|ǳ|Ǳ|ǳ|"
"ͅ|ι|Ι||"
"ΐ|ΐ|Ϊ́||"
"ΰ|ΰ|Ϋ́||"
"ς|σ|Σ||"
"ϐ|β|Β||"
"ϑ|θ|Θ||"
"ϕ|φ|Φ||"
"ϖ|π|Π||"
"ϰ|κ|Κ||"
"ϱ|ρ|Ρ||"
"ϴ|θ||θ|"
"ϵ|ε|Ε||"
"և|եւ|ԵՒ||"
"ẖ|ẖ|H̱||"
"ẗ|ẗ|T̈||"
"ẘ|ẘ|W̊||"
"ẙ|ẙ|Y̊||"
"ẚ|aʾ|Aʾ||"
"ẛ|ṡ|Ṡ||"
"ẞ|ss||ß|"
"ὐ|ὐ|Υ̓||"
"ὒ|ὒ|Υ̓̀||"
"ὔ|ὔ|Υ̓́||"
"ὖ|ὖ|Υ̓͂||"
"ᾀ|ἀι|ἈΙ||"
"ᾁ|ἁι|ἉΙ||"
"ᾂ|ἂι|ἊΙ||"
"ᾃ|ἃι|ἋΙ||"
"ᾄ|ἄι|ἌΙ||"
"ᾅ|ἅι|ἍΙ||"
"ᾆ|ἆι|ἎΙ||"
"ᾇ|ἇι|ἏΙ||"
"ᾈ|ἀι|ἈΙ|ᾀ|"
"ᾉ|ἁι|ἉΙ|ᾁ|"
"ᾊ|ἂι|ἊΙ|ᾂ|"
"ᾋ|ἃι|ἋΙ|ᾃ|"
"ᾌ|ἄι|ἌΙ|ᾄ|"
"ᾍ|ἅι|ἍΙ|ᾅ|"
"ᾎ|ἆι|ἎΙ|ᾆ|"
"ᾏ|ἇι|ἏΙ|ᾇ|"
"ᾐ|ἠι|ἨΙ||"
"ᾑ|ἡι|ἩΙ||"
"ᾒ|ἢι|ἪΙ||"
"ᾓ|ἣι|ἫΙ||"
"ᾔ|ἤι|ἬΙ||"
"ᾕ|ἥι|ἭΙ||"
"ᾖ|ἦι|ἮΙ||"
"ᾗ|ἧι|ἯΙ||"
"ᾘ|ἠι|ἨΙ|ᾐ|"
"ᾙ|ἡι|ἩΙ|ᾑ|"
"ᾚ|ἢι|ἪΙ|ᾒ|"
"ᾛ|ἣι|ἫΙ|ᾓ|"
"ᾜ|ἤι|ἬΙ|ᾔ|"
"ᾝ|ἥι|ἭΙ|ᾕ|"
"ᾞ|ἦι|ἮΙ|ᾖ|"
"ᾟ|ἧι|ἯΙ|ᾗ|"
"ᾠ|ὠι|ὨΙ||"
"ᾡ|ὡι|ὩΙ||"
"ᾢ|ὢι|ὪΙ||"
"ᾣ|ὣι|ὫΙ||"
"ᾤ|ὤι|ὬΙ||"
"ᾥ|ὥι|ὭΙ||"
"ᾦ|ὦι|ὮΙ||"
"ᾧ|ὧι|ὯΙ||"
"ᾨ|ὠι|ὨΙ|ᾠ|"
"ᾩ|ὡι|ὩΙ|ᾡ|"
"ᾪ|ὢι|ὪΙ|ᾢ|"
"ᾫ|ὣι|ὫΙ|ᾣ|"
"ᾬ|ὤι|ὬΙ|ᾤ|"
"ᾭ|ὥι|ὭΙ|ᾥ|"
"ᾮ|ὦι|ὮΙ|ᾦ|"
"ᾯ|ὧι|ὯΙ|ᾧ|"
"ᾲ|ὰι|ᾺΙ||"
"ᾳ|αι|ΑΙ||"
"ᾴ|άι|ΆΙ||"
"ᾶ|ᾶ|Α͂||"
"ᾷ|ᾶι|Α͂Ι||"
"ᾼ|αι|ΑΙ|ᾳ|"
"ι|ι|Ι||"
"ῂ|ὴι|ῊΙ||"
"ῃ|ηι|ΗΙ||"
"ῄ|ήι|ΉΙ||"
"ῆ|ῆ|Η͂||"
"ῇ|ῆι|Η͂Ι||"
"ῌ|ηι|ΗΙ|ῃ|"
"ῒ|ῒ|Ϊ̀||"
"ΐ|ΐ|Ϊ́||"
"ῖ|ῖ|Ι͂||"
"ῗ|ῗ|Ϊ͂||"
"ῢ|ῢ|Ϋ̀||"
"ΰ|ΰ|Ϋ́||"
"ῤ|ῤ|Ρ̓||"
"ῦ|ῦ|Υ͂||"
"ῧ|ῧ|Ϋ͂||"
"ῲ|ὼι|ῺΙ||"
"ῳ|ωι|ΩΙ||"
"ῴ|ώι|ΏΙ||"
"ῶ|ῶ|Ω͂||"
"ῷ|ῶι|Ω͂Ι||"
"ῼ|ωι|ΩΙ|ῳ|"
"Ω|ω||ω|"
"K|k||k|"
"Å|å||å|"
"ﬀ|ff|FF||"
"ﬁ|fi|FI||"
"ﬂ|fl|FL||"
"ﬃ|ffi|FFI||"
"ﬄ|ffl|FFL||"
"ﬅ|st|ST||"
"ﬆ|st|ST||"
"ﬓ|մն|ՄՆ||"
"ﬔ|մե|ՄԵ||"
"ﬕ|մի|ՄԻ||"
"ﬖ|վն|ՎՆ||"
"ﬗ|մխ|ՄԽ||"

//--Autogenerated -- end of section automatically generated
;

class CaseConverter : public ICaseConverter {
	// Maximum length of a case conversion result is 6 bytes in UTF-8
	enum { maxConversionLength=6 };
	struct ConversionString {
		char conversion[maxConversionLength+1];
	};
	// Conversions are initially store in a vector of structs but then decomposed into
	// parallel arrays as that is about 10% faster to search.
	struct CharacterConversion {
		int character;
		ConversionString conversion;
		CharacterConversion(int character_=0, const char *conversion_="") : character(character_) {
			strcpy(conversion.conversion, conversion_);
		}
		bool operator<(const CharacterConversion &other) const {
			return character < other.character;
		}
	};
	typedef std::vector<CharacterConversion> CharacterToConversion;
	CharacterToConversion characterToConversion;
	// The parallel arrays 
	std::vector<int> characters;
	std::vector<ConversionString> conversions;

public:
	CaseConverter() {
	}
	bool Initialised() const {
		return characters.size() > 0;
	}
	void Add(int character, const char *conversion) {
		characterToConversion.push_back(CharacterConversion(character, conversion));
	}
	const char *Find(int character) {
		const std::vector<int>::iterator it = std::lower_bound(characters.begin(), characters.end(), character);
		if (it == characters.end())
			return 0;
		else if (*it == character)
			return conversions[it - characters.begin()].conversion;
		else
			return 0;
	}
	size_t CaseConvertString(char *converted, size_t sizeConverted, const char *mixed, size_t lenMixed) {
		size_t lenConverted = 0;
		size_t mixedPos = 0;
		unsigned char bytes[UTF8MaxBytes + 1];
		while (mixedPos < lenMixed) {
			const unsigned char leadByte = static_cast<unsigned char>(mixed[mixedPos]);
			const char *caseConverted = 0;
			size_t lenMixedChar = 1;
			if (UTF8IsAscii(leadByte)) {
				caseConverted = Find(leadByte);
			} else {
				bytes[0] = leadByte;
				const int widthCharBytes = UTF8BytesOfLead[leadByte];
				for (int b=1; b<widthCharBytes; b++) {
					bytes[b] = (mixedPos+b < lenMixed) ? mixed[mixedPos+b] : 0;
				}
				int classified = UTF8Classify(bytes, widthCharBytes);
				if (!(classified & UTF8MaskInvalid)) {
					// valid UTF-8
					lenMixedChar = classified & UTF8MaskWidth;
					int character = UnicodeFromUTF8(bytes);
					caseConverted = Find(character);
				}
			}
			if (caseConverted) {
				// Character has a conversion so copy that conversion in
				while (*caseConverted) {
					converted[lenConverted++] = *caseConverted++;
					if (lenConverted >= sizeConverted)
						return 0;
				}
			} else {
				// Character has no conversion so copy the input to output
				for (size_t i=0; i<lenMixedChar; i++) {
					converted[lenConverted++] = mixed[mixedPos+i];
					if (lenConverted >= sizeConverted)
						return 0;
				}
			}
			mixedPos += lenMixedChar;
		}
		return lenConverted;
	}
	void FinishedAdding() {
		std::sort(characterToConversion.begin(), characterToConversion.end());
		characters.reserve(characterToConversion.size());
		conversions.reserve(characterToConversion.size());
		for (CharacterToConversion::iterator it = characterToConversion.begin(); it != characterToConversion.end(); ++it) {
			characters.push_back(it->character);
			conversions.push_back(it->conversion);
		}
		// Empty the original calculated data completely
		CharacterToConversion().swap(characterToConversion);
	}
};

CaseConverter caseConvFold;
CaseConverter caseConvUp;
CaseConverter caseConvLow;

void UTF8FromUTF32Character(int uch, char *putf) {
	size_t k = 0;
	if (uch < 0x80) {
		putf[k++] = static_cast<char>(uch);
	} else if (uch < 0x800) {
		putf[k++] = static_cast<char>(0xC0 | (uch >> 6));
		putf[k++] = static_cast<char>(0x80 | (uch & 0x3f));
	} else if (uch < 0x10000) {
		putf[k++] = static_cast<char>(0xE0 | (uch >> 12));
		putf[k++] = static_cast<char>(0x80 | ((uch >> 6) & 0x3f));
		putf[k++] = static_cast<char>(0x80 | (uch & 0x3f));
	} else {
		putf[k++] = static_cast<char>(0xF0 | (uch >> 18));
		putf[k++] = static_cast<char>(0x80 | ((uch >> 12) & 0x3f));
		putf[k++] = static_cast<char>(0x80 | ((uch >> 6) & 0x3f));
		putf[k++] = static_cast<char>(0x80 | (uch & 0x3f));
	}
	putf[k] = 0;
}

void AddSymmetric(enum CaseConversion conversion, int lower,int upper) {
	char lowerUTF8[UTF8MaxBytes+1];
	UTF8FromUTF32Character(lower, lowerUTF8);
	char upperUTF8[UTF8MaxBytes+1];
	UTF8FromUTF32Character(upper, upperUTF8);

	switch (conversion) {
	case CaseConversionFold:
		caseConvFold.Add(upper, lowerUTF8);
		break;
	case CaseConversionUpper:
		caseConvUp.Add(lower, upperUTF8);
		break;
	case CaseConversionLower:
		caseConvLow.Add(upper, lowerUTF8);
		break;
	}
}

void SetupConversions(enum CaseConversion conversion) {
	// First initialize for the symmetric ranges
	for (size_t i=0; i<sizeof(symmetricCaseConversionRanges)/sizeof(symmetricCaseConversionRanges[0]);) {
		int lower = symmetricCaseConversionRanges[i++];
		int upper = symmetricCaseConversionRanges[i++];
		int length = symmetricCaseConversionRanges[i++];
		int pitch = symmetricCaseConversionRanges[i++];
		for (int j=0;j<length*pitch;j+=pitch) {
			AddSymmetric(conversion, lower+j, upper+j);
		}
	}
	// Add the symmetric singletons
	for (size_t i=0; i<sizeof(symmetricCaseConversions)/sizeof(symmetricCaseConversions[0]);) {
		int lower = symmetricCaseConversions[i++];
		int upper = symmetricCaseConversions[i++];
		AddSymmetric(conversion, lower, upper);
	}
	// Add the complex cases
	const char *sComplex = complexCaseConversions;
	while (*sComplex) {
		// Longest ligature is 3 character so 5 for safety
		const size_t lenUTF8 = 5*UTF8MaxBytes+1;
		char originUTF8[lenUTF8];
		char foldedUTF8[lenUTF8];
		char lowerUTF8[lenUTF8];
		char upperUTF8[lenUTF8];
		size_t i = 0;
		while (*sComplex && *sComplex != '|') {
			originUTF8[i++] = *sComplex;
			sComplex++;
		}
		sComplex++;
		originUTF8[i] = 0;
		i = 0;
		while (*sComplex && *sComplex != '|') {
			foldedUTF8[i++] = *sComplex;
			sComplex++;
		}
		sComplex++;
		foldedUTF8[i] = 0;
		i = 0;
		while (*sComplex && *sComplex != '|') {
			upperUTF8[i++] = *sComplex;
			sComplex++;
		}
		sComplex++;
		upperUTF8[i] = 0;
		i = 0;
		while (*sComplex && *sComplex != '|') {
			lowerUTF8[i++] = *sComplex;
			sComplex++;
		}
		sComplex++;
		lowerUTF8[i] = 0;

		int character = UnicodeFromUTF8(reinterpret_cast<unsigned char *>(originUTF8));

		if (conversion == CaseConversionFold && foldedUTF8[0]) {
			caseConvFold.Add(character, foldedUTF8);
		}

		if (conversion == CaseConversionUpper && upperUTF8[0]) {
			caseConvUp.Add(character, upperUTF8);
		}

		if (conversion == CaseConversionLower && lowerUTF8[0]) {
			caseConvLow.Add(character, lowerUTF8);
		}
	}

	switch (conversion) {
	case CaseConversionFold:
		caseConvFold.FinishedAdding();
		break;
	case CaseConversionUpper:
		caseConvUp.FinishedAdding();
		break;
	case CaseConversionLower:
		caseConvLow.FinishedAdding();
		break;
	}
}

CaseConverter *ConverterForConversion(enum CaseConversion conversion) {
	switch (conversion) {
	case CaseConversionFold:
		return &caseConvFold;
	case CaseConversionUpper:
		return &caseConvUp;
	case CaseConversionLower:
		return &caseConvLow;
	}
	return 0;
}

}

#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif

ICaseConverter *ConverterFor(enum CaseConversion conversion) {
	CaseConverter *pCaseConv = ConverterForConversion(conversion);
	if (!pCaseConv->Initialised())
		SetupConversions(conversion);
	return pCaseConv;
}

const char *CaseConvert(int character, enum CaseConversion conversion) {
	CaseConverter *pCaseConv = ConverterForConversion(conversion);
	if (!pCaseConv->Initialised())
		SetupConversions(conversion);
	return pCaseConv->Find(character);
}

size_t CaseConvertString(char *converted, size_t sizeConverted, const char *mixed, size_t lenMixed, enum CaseConversion conversion) {
	CaseConverter *pCaseConv = ConverterForConversion(conversion);
	if (!pCaseConv->Initialised())
		SetupConversions(conversion);
	return pCaseConv->CaseConvertString(converted, sizeConverted, mixed, lenMixed);
}

#ifdef SCI_NAMESPACE
}
#endif
