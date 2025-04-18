/* Generated by re2c 3.1 on Wed Apr 10 11:01:14 2024 */
#line 1 "re2c_functions.re"
#line 5 "re2c_functions.re"

#line 94 "re2c_functions.re"


#include "re2c_generated_functions.h"

#define YYCTYPE    QChar

#line 103 "re2c_functions.re"


namespace re2c {
    QStringView decimal(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        
#line 22 "re2c_generated_functions.cpp"
{
	YYCTYPE yych;
	yych = *YYCURSOR;
	if (yych <= '-') {
		if (yych == '+') goto yy3;
		if (yych >= '-') goto yy3;
	} else {
		if (yych <= '.') goto yy4;
		if (yych <= '/') goto yy1;
		if (yych <= '9') goto yy5;
	}
yy1:
	++YYCURSOR;
yy2:
#line 112 "re2c_functions.re"
	{ return QStringView(); }
#line 39 "re2c_generated_functions.cpp"
yy3:
	yych = *(YYMARKER = ++YYCURSOR);
	if (yych == '.') goto yy7;
	if (yych <= '/') goto yy2;
	if (yych <= '9') goto yy5;
	goto yy2;
yy4:
	yych = *++YYCURSOR;
	if (yych <= '/') goto yy2;
	if (yych <= '9') goto yy9;
	goto yy2;
yy5:
	yych = *++YYCURSOR;
	if (yych == '.') goto yy9;
	if (yych <= '/') goto yy6;
	if (yych <= '9') goto yy5;
yy6:
#line 111 "re2c_functions.re"
	{ return QStringView(input.cbegin(), YYCURSOR); }
#line 59 "re2c_generated_functions.cpp"
yy7:
	yych = *++YYCURSOR;
	if (yych <= '/') goto yy8;
	if (yych <= '9') goto yy9;
yy8:
	YYCURSOR = YYMARKER;
	goto yy2;
yy9:
	yych = *++YYCURSOR;
	if (yych <= '/') goto yy6;
	if (yych <= '9') goto yy9;
	goto yy6;
}
#line 113 "re2c_functions.re"

    }

    QStringView snbtNumber(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        
#line 82 "re2c_generated_functions.cpp"
{
	YYCTYPE yych;
	unsigned int yyaccept = 0;
	yych = *YYCURSOR;
	if (yych <= '-') {
		if (yych == '+') goto yy13;
		if (yych >= '-') goto yy13;
	} else {
		if (yych <= '.') goto yy14;
		if (yych <= '/') goto yy11;
		if (yych <= '9') goto yy15;
	}
yy11:
	++YYCURSOR;
yy12:
#line 125 "re2c_functions.re"
	{ return QStringView(); }
#line 100 "re2c_generated_functions.cpp"
yy13:
	yyaccept = 0;
	yych = *(YYMARKER = ++YYCURSOR);
	if (yych == '.') goto yy17;
	if (yych <= '/') goto yy12;
	if (yych <= '9') goto yy15;
	goto yy12;
yy14:
	yych = *++YYCURSOR;
	if (yych <= '/') goto yy12;
	if (yych <= '9') goto yy19;
	goto yy12;
yy15:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	if (yych <= '9') {
		if (yych == '.') goto yy19;
		if (yych >= '0') goto yy15;
	} else {
		if (yych <= 'E') {
			if (yych >= 'E') goto yy20;
		} else {
			if (yych == 'e') goto yy20;
		}
	}
yy16:
#line 124 "re2c_functions.re"
	{ return QStringView(input.cbegin(), YYCURSOR); }
#line 129 "re2c_generated_functions.cpp"
yy17:
	yych = *++YYCURSOR;
	if (yych <= '/') goto yy18;
	if (yych <= '9') goto yy19;
yy18:
	YYCURSOR = YYMARKER;
	if (yyaccept == 0) {
		goto yy12;
	} else {
		goto yy16;
	}
yy19:
	yyaccept = 1;
	yych = *(YYMARKER = ++YYCURSOR);
	if (yych <= 'D') {
		if (yych <= '/') goto yy16;
		if (yych <= '9') goto yy19;
		goto yy16;
	} else {
		if (yych <= 'E') goto yy20;
		if (yych != 'e') goto yy16;
	}
yy20:
	yych = *++YYCURSOR;
	if (yych <= ',') {
		if (yych != '+') goto yy18;
	} else {
		if (yych <= '-') goto yy21;
		if (yych <= '/') goto yy18;
		if (yych <= '9') goto yy22;
		goto yy18;
	}
yy21:
	yych = *++YYCURSOR;
	if (yych <= '/') goto yy18;
	if (yych >= ':') goto yy18;
yy22:
	yych = *++YYCURSOR;
	if (yych <= '/') goto yy16;
	if (yych <= '9') goto yy22;
	goto yy16;
}
#line 126 "re2c_functions.re"

    }

    QStringView itemSlot(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        
#line 181 "re2c_generated_functions.cpp"
{
	YYCTYPE yych;
	yych = *YYCURSOR;
	if (yych <= '9') {
		if (yych == '.') goto yy25;
		if (yych >= '0') goto yy25;
	} else {
		if (yych <= '_') {
			if (yych >= '_') goto yy25;
		} else {
			if (yych <= '`') goto yy24;
			if (yych <= 'z') goto yy25;
		}
	}
yy24:
	++YYCURSOR;
#line 137 "re2c_functions.re"
	{ return QStringView(); }
#line 200 "re2c_generated_functions.cpp"
yy25:
	yych = *++YYCURSOR;
	if (yych <= '9') {
		if (yych == '.') goto yy25;
		if (yych >= '0') goto yy25;
	} else {
		if (yych <= '_') {
			if (yych >= '_') goto yy25;
		} else {
			if (yych <= '`') goto yy26;
			if (yych <= 'z') goto yy25;
		}
	}
yy26:
#line 136 "re2c_functions.re"
	{ return QStringView(input.cbegin(), YYCURSOR); }
#line 217 "re2c_generated_functions.cpp"
}
#line 138 "re2c_functions.re"

    }

    QStringView itemSlots(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        
#line 228 "re2c_generated_functions.cpp"
{
	YYCTYPE yych;
	yych = *YYCURSOR;
	if (yych <= '/') {
		if (yych <= '*') {
			if (yych >= '*') goto yy29;
		} else {
			if (yych == '.') goto yy29;
		}
	} else {
		if (yych <= '_') {
			if (yych <= '9') goto yy29;
			if (yych >= '_') goto yy29;
		} else {
			if (yych <= '`') goto yy28;
			if (yych <= 'z') goto yy29;
		}
	}
yy28:
	++YYCURSOR;
#line 149 "re2c_functions.re"
	{ return QStringView(); }
#line 251 "re2c_generated_functions.cpp"
yy29:
	yych = *++YYCURSOR;
	if (yych <= '/') {
		if (yych <= '*') {
			if (yych >= '*') goto yy29;
		} else {
			if (yych == '.') goto yy29;
		}
	} else {
		if (yych <= '_') {
			if (yych <= '9') goto yy29;
			if (yych >= '_') goto yy29;
		} else {
			if (yych <= '`') goto yy30;
			if (yych <= 'z') goto yy29;
		}
	}
yy30:
#line 148 "re2c_functions.re"
	{ return QStringView(input.cbegin(), YYCURSOR); }
#line 272 "re2c_generated_functions.cpp"
}
#line 150 "re2c_functions.re"

    }

    QStringView nbtPathKey(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        
#line 283 "re2c_generated_functions.cpp"
{
	YYCTYPE yych;
	yych = *YYCURSOR;
	if (yych <= '\\') {
		if (yych <= '-') {
			if (yych == '!') goto yy33;
			if (yych >= '#') goto yy33;
		} else {
			if (yych <= '.') goto yy32;
			if (yych != '[') goto yy33;
		}
	} else {
		if (yych <= '|') {
			if (yych <= ']') goto yy32;
			if (yych != '{') goto yy33;
		} else {
			if (yych == '~') goto yy33;
			if (yych >= 0x0080) goto yy33;
		}
	}
yy32:
	++YYCURSOR;
#line 161 "re2c_functions.re"
	{ return QStringView(); }
#line 308 "re2c_generated_functions.cpp"
yy33:
	yych = *++YYCURSOR;
	if (yych <= '\\') {
		if (yych <= '-') {
			if (yych == '!') goto yy33;
			if (yych >= '#') goto yy33;
		} else {
			if (yych <= '.') goto yy34;
			if (yych != '[') goto yy33;
		}
	} else {
		if (yych <= '|') {
			if (yych <= ']') goto yy34;
			if (yych != '{') goto yy33;
		} else {
			if (yych == '~') goto yy33;
			if (yych >= 0x0080) goto yy33;
		}
	}
yy34:
#line 160 "re2c_functions.re"
	{ return QStringView(input.cbegin(), YYCURSOR); }
#line 331 "re2c_generated_functions.cpp"
}
#line 162 "re2c_functions.re"

    }

    QStringView objective(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        
#line 342 "re2c_generated_functions.cpp"
{
	YYCTYPE yych;
	yych = *YYCURSOR;
	if (yych <= '/') {
		if (yych <= '*') {
			if (yych == '#') goto yy37;
		} else {
			if (yych == ',') goto yy36;
			if (yych <= '.') goto yy37;
		}
	} else {
		if (yych <= '^') {
			if (yych <= ':') goto yy37;
			if (yych <= '@') goto yy36;
			if (yych <= 'Z') goto yy37;
		} else {
			if (yych == '`') goto yy36;
			if (yych <= 'z') goto yy37;
		}
	}
yy36:
	++YYCURSOR;
#line 173 "re2c_functions.re"
	{ return QStringView(); }
#line 367 "re2c_generated_functions.cpp"
yy37:
	yych = *++YYCURSOR;
	if (yych <= '/') {
		if (yych <= '*') {
			if (yych == '#') goto yy37;
		} else {
			if (yych == ',') goto yy38;
			if (yych <= '.') goto yy37;
		}
	} else {
		if (yych <= '^') {
			if (yych <= ':') goto yy37;
			if (yych <= '@') goto yy38;
			if (yych <= 'Z') goto yy37;
		} else {
			if (yych == '`') goto yy38;
			if (yych <= 'z') goto yy37;
		}
	}
yy38:
#line 172 "re2c_functions.re"
	{ return QStringView(input.cbegin(), YYCURSOR); }
#line 390 "re2c_generated_functions.cpp"
}
#line 174 "re2c_functions.re"

    }

    QStringView objectiveCriteria(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        
#line 401 "re2c_generated_functions.cpp"
{
	YYCTYPE yych;
	yych = *YYCURSOR;
	if (yych <= '@') {
		if (yych <= '.') {
			if (yych >= '-') goto yy41;
		} else {
			if (yych <= '/') goto yy40;
			if (yych <= ':') goto yy41;
		}
	} else {
		if (yych <= '_') {
			if (yych <= 'Z') goto yy41;
			if (yych >= '_') goto yy41;
		} else {
			if (yych <= '`') goto yy40;
			if (yych <= 'z') goto yy41;
		}
	}
yy40:
	++YYCURSOR;
#line 185 "re2c_functions.re"
	{ return QStringView(); }
#line 425 "re2c_generated_functions.cpp"
yy41:
	yych = *++YYCURSOR;
	if (yych <= '@') {
		if (yych <= '.') {
			if (yych >= '-') goto yy41;
		} else {
			if (yych <= '/') goto yy42;
			if (yych <= ':') goto yy41;
		}
	} else {
		if (yych <= '_') {
			if (yych <= 'Z') goto yy41;
			if (yych >= '_') goto yy41;
		} else {
			if (yych <= '`') goto yy42;
			if (yych <= 'z') goto yy41;
		}
	}
yy42:
#line 184 "re2c_functions.re"
	{ return QStringView(input.cbegin(), YYCURSOR); }
#line 447 "re2c_generated_functions.cpp"
}
#line 186 "re2c_functions.re"

    }

    QStringView realPlayerName(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        
#line 458 "re2c_generated_functions.cpp"
{
	YYCTYPE yych;
	yych = *YYCURSOR;
	if (yych <= 'Z') {
		if (yych <= '/') goto yy44;
		if (yych <= '9') goto yy45;
		if (yych >= 'A') goto yy45;
	} else {
		if (yych <= '_') {
			if (yych >= '_') goto yy45;
		} else {
			if (yych <= '`') goto yy44;
			if (yych <= 'z') goto yy45;
		}
	}
yy44:
	++YYCURSOR;
#line 197 "re2c_functions.re"
	{ return QStringView(); }
#line 478 "re2c_generated_functions.cpp"
yy45:
	yych = *++YYCURSOR;
	if (yych <= 'Z') {
		if (yych <= '/') goto yy46;
		if (yych <= '9') goto yy45;
		if (yych >= 'A') goto yy45;
	} else {
		if (yych <= '_') {
			if (yych >= '_') goto yy45;
		} else {
			if (yych <= '`') goto yy46;
			if (yych <= 'z') goto yy45;
		}
	}
yy46:
#line 196 "re2c_functions.re"
	{ return QStringView(input.cbegin(), YYCURSOR); }
#line 496 "re2c_generated_functions.cpp"
}
#line 198 "re2c_functions.re"

    }

    QStringView resLocPart(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        
#line 507 "re2c_generated_functions.cpp"
{
	YYCTYPE yych;
	yych = *YYCURSOR;
	if (yych <= '\\') {
		if (yych <= ',') goto yy48;
		if (yych <= '9') goto yy49;
		if (yych >= '\\') goto yy49;
	} else {
		if (yych <= '_') {
			if (yych >= '_') goto yy49;
		} else {
			if (yych <= '`') goto yy48;
			if (yych <= 'z') goto yy49;
		}
	}
yy48:
	++YYCURSOR;
#line 209 "re2c_functions.re"
	{ return QStringView(); }
#line 527 "re2c_generated_functions.cpp"
yy49:
	yych = *++YYCURSOR;
	if (yych <= '\\') {
		if (yych <= ',') goto yy50;
		if (yych <= '9') goto yy49;
		if (yych >= '\\') goto yy49;
	} else {
		if (yych <= '_') {
			if (yych >= '_') goto yy49;
		} else {
			if (yych <= '`') goto yy50;
			if (yych <= 'z') goto yy49;
		}
	}
yy50:
#line 208 "re2c_functions.re"
	{ return QStringView(input.cbegin(), YYCURSOR); }
#line 545 "re2c_generated_functions.cpp"
}
#line 210 "re2c_functions.re"

    }

    QStringView uuid(QStringView input, QUuid &result) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

    #define DECLARE_PART_POINTERS(part)    const QChar *_p ## part, \
            *p ## part ## _;
        DECLARE_PART_POINTERS(1);
        DECLARE_PART_POINTERS(2);
        DECLARE_PART_POINTERS(3);
        DECLARE_PART_POINTERS(4);
        DECLARE_PART_POINTERS(5);
    #undef DECLARE_PART_POINTERS

    #define GET_PART(part)    QStringView(_p ## part, p ## part ## _)

        
#line 567 "re2c_generated_functions.cpp"
const QChar *yyt1;
const QChar *yyt2;
const QChar *yyt3;
const QChar *yyt4;
const QChar *yyt5;
#line 228 "re2c_functions.re"


        
#line 577 "re2c_generated_functions.cpp"
{
	YYCTYPE yych;
	yych = *YYCURSOR;
	if (yych <= '@') {
		if (yych <= '/') goto yy52;
		if (yych <= '9') {
			yyt1 = YYCURSOR;
			goto yy54;
		}
	} else {
		if (yych <= 'F') {
			yyt1 = YYCURSOR;
			goto yy54;
		}
		if (yych <= '`') goto yy52;
		if (yych <= 'f') {
			yyt1 = YYCURSOR;
			goto yy54;
		}
	}
yy52:
	++YYCURSOR;
yy53:
#line 266 "re2c_functions.re"
	{ return QStringView(); }
#line 603 "re2c_generated_functions.cpp"
yy54:
	yych = *(YYMARKER = ++YYCURSOR);
	if (yych <= '9') {
		if (yych == '-') goto yy55;
		if (yych <= '/') goto yy53;
		goto yy57;
	} else {
		if (yych <= 'F') {
			if (yych <= '@') goto yy53;
			goto yy57;
		} else {
			if (yych <= '`') goto yy53;
			if (yych <= 'f') goto yy57;
			goto yy53;
		}
	}
yy55:
	yych = *++YYCURSOR;
	if (yych <= '@') {
		if (yych <= '/') goto yy56;
		if (yych <= '9') {
			yyt2 = YYCURSOR;
			goto yy58;
		}
	} else {
		if (yych <= 'F') {
			yyt2 = YYCURSOR;
			goto yy58;
		}
		if (yych <= '`') goto yy56;
		if (yych <= 'f') {
			yyt2 = YYCURSOR;
			goto yy58;
		}
	}
yy56:
	YYCURSOR = YYMARKER;
	goto yy53;
yy57:
	yych = *++YYCURSOR;
	if (yych <= '9') {
		if (yych == '-') goto yy55;
		if (yych <= '/') goto yy56;
		goto yy57;
	} else {
		if (yych <= 'F') {
			if (yych <= '@') goto yy56;
			goto yy57;
		} else {
			if (yych <= '`') goto yy56;
			if (yych <= 'f') goto yy57;
			goto yy56;
		}
	}
yy58:
	yych = *++YYCURSOR;
	if (yych <= '9') {
		if (yych == '-') goto yy59;
		if (yych <= '/') goto yy56;
		goto yy58;
	} else {
		if (yych <= 'F') {
			if (yych <= '@') goto yy56;
			goto yy58;
		} else {
			if (yych <= '`') goto yy56;
			if (yych <= 'f') goto yy58;
			goto yy56;
		}
	}
yy59:
	yych = *++YYCURSOR;
	if (yych <= '@') {
		if (yych <= '/') goto yy56;
		if (yych >= ':') goto yy56;
		yyt3 = YYCURSOR;
	} else {
		if (yych <= 'F') {
			yyt3 = YYCURSOR;
			goto yy60;
		}
		if (yych <= '`') goto yy56;
		if (yych >= 'g') goto yy56;
		yyt3 = YYCURSOR;
	}
yy60:
	yych = *++YYCURSOR;
	if (yych <= '9') {
		if (yych == '-') goto yy61;
		if (yych <= '/') goto yy56;
		goto yy60;
	} else {
		if (yych <= 'F') {
			if (yych <= '@') goto yy56;
			goto yy60;
		} else {
			if (yych <= '`') goto yy56;
			if (yych <= 'f') goto yy60;
			goto yy56;
		}
	}
yy61:
	yych = *++YYCURSOR;
	if (yych <= '@') {
		if (yych <= '/') goto yy56;
		if (yych >= ':') goto yy56;
		yyt4 = YYCURSOR;
	} else {
		if (yych <= 'F') {
			yyt4 = YYCURSOR;
			goto yy62;
		}
		if (yych <= '`') goto yy56;
		if (yych >= 'g') goto yy56;
		yyt4 = YYCURSOR;
	}
yy62:
	yych = *++YYCURSOR;
	if (yych <= '9') {
		if (yych == '-') goto yy63;
		if (yych <= '/') goto yy56;
		goto yy62;
	} else {
		if (yych <= 'F') {
			if (yych <= '@') goto yy56;
			goto yy62;
		} else {
			if (yych <= '`') goto yy56;
			if (yych <= 'f') goto yy62;
			goto yy56;
		}
	}
yy63:
	yych = *++YYCURSOR;
	if (yych <= '@') {
		if (yych <= '/') goto yy56;
		if (yych >= ':') goto yy56;
		yyt5 = YYCURSOR;
	} else {
		if (yych <= 'F') {
			yyt5 = YYCURSOR;
			goto yy64;
		}
		if (yych <= '`') goto yy56;
		if (yych >= 'g') goto yy56;
		yyt5 = YYCURSOR;
	}
yy64:
	yych = *++YYCURSOR;
	if (yych <= '@') {
		if (yych <= '/') goto yy65;
		if (yych <= '9') goto yy64;
	} else {
		if (yych <= 'F') goto yy64;
		if (yych <= '`') goto yy65;
		if (yych <= 'f') goto yy64;
	}
yy65:
	_p1 = yyt1;
	_p2 = yyt2;
	_p3 = yyt3;
	_p4 = yyt4;
	_p5 = yyt5;
	p1_ = yyt2 - 1;
	p2_ = yyt3 - 1;
	p3_ = yyt4 - 1;
	p4_ = yyt5 - 1;
	p5_ = YYCURSOR;
#line 236 "re2c_functions.re"
	{
            if (YYCURSOR - input.cbegin() > 36) {
                return QStringView();
            }
            bool              ok;
            constexpr static uint8_t last2HexDigitMask = 0xff;

            const auto l             = strToUHex<uint32_t>(GET_PART(1).right(8), ok);
            const auto w1            = strToUHex<uint16_t>(GET_PART(2).right(4), ok);
            const auto w2            = strToUHex<uint16_t>(GET_PART(3).right(4), ok);
            const auto clock_seq_low = strToUHex<uint16_t>(GET_PART(4).right(4), ok);

            const uint8_t b1   = clock_seq_low >> 8;
            const uint8_t b2   = clock_seq_low & last2HexDigitMask;
            auto          node = strToUHex<uint64_t>(GET_PART(5).right(12), ok);
            const uint8_t b8   = node & last2HexDigitMask;
            node >>= 8;
            const uint8_t b7 = node & last2HexDigitMask;
            node >>= 8;
            const uint8_t b6 = node & last2HexDigitMask;
            node >>= 8;
            const uint8_t b5 = node & last2HexDigitMask;
            node >>= 8;
            const uint8_t b4 = node & last2HexDigitMask;
            node >>= 8;
            const uint8_t b3 = node;

            result = QUuid{l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8};
            return QStringView(input.cbegin(), YYCURSOR);
            }
#line 803 "re2c_generated_functions.cpp"
}
#line 267 "re2c_functions.re"


    #undef GET_PART
    }
}


#undef YYCTYPE
