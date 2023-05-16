/* Generated by re2c 3.0 on Sat May 13 10:39:57 2023 */
#line 1 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"
#line 5 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"

#line 93 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"


#include "re2c_generated_functions.h"

#define YYCTYPE    QChar

#line 102 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"


namespace re2c {
    QStringView decimal(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        
#line 22 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
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
#line 111 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"
	{ return QStringView(); }
#line 39 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
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
#line 110 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"
	{ return QStringView(input.cbegin(), YYCURSOR); }
#line 59 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
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
#line 112 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"

    }

    QStringView snbtNumber(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        
#line 82 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
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
#line 124 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"
	{ return QStringView(); }
#line 100 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
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
#line 123 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"
	{ return QStringView(input.cbegin(), YYCURSOR); }
#line 129 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
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
#line 125 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"

    }

    QStringView itemSlot(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        
#line 181 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
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
#line 136 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"
	{ return QStringView(); }
#line 200 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
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
#line 135 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"
	{ return QStringView(input.cbegin(), YYCURSOR); }
#line 217 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
}
#line 137 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"

    }

    QStringView nbtPathKey(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        
#line 228 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
{
	YYCTYPE yych;
	yych = *YYCURSOR;
	if (yych <= 'Z') {
		if (yych <= '!') {
			if (yych <= 0x0000) goto yy28;
			if (yych != ' ') goto yy29;
		} else {
			if (yych <= '"') goto yy28;
			if (yych != '.') goto yy29;
		}
	} else {
		if (yych <= 'z') {
			if (yych == '\\') goto yy29;
			if (yych >= '^') goto yy29;
		} else {
			if (yych == '|') goto yy29;
			if (yych >= '~') goto yy29;
		}
	}
yy28:
	++YYCURSOR;
#line 148 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"
	{ return QStringView(); }
#line 253 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
yy29:
	yych = *++YYCURSOR;
	if (yych <= 'Z') {
		if (yych <= '!') {
			if (yych <= 0x0000) goto yy30;
			if (yych != ' ') goto yy29;
		} else {
			if (yych <= '"') goto yy30;
			if (yych != '.') goto yy29;
		}
	} else {
		if (yych <= 'z') {
			if (yych == '\\') goto yy29;
			if (yych >= '^') goto yy29;
		} else {
			if (yych == '|') goto yy29;
			if (yych >= '~') goto yy29;
		}
	}
yy30:
#line 147 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"
	{ return QStringView(input.cbegin(), YYCURSOR); }
#line 276 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
}
#line 149 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"

    }

    QStringView objective(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        
#line 287 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
{
	YYCTYPE yych;
	yych = *YYCURSOR;
	if (yych <= '/') {
		if (yych <= '*') {
			if (yych == '#') goto yy33;
		} else {
			if (yych == ',') goto yy32;
			if (yych <= '.') goto yy33;
		}
	} else {
		if (yych <= '^') {
			if (yych <= ':') goto yy33;
			if (yych <= '@') goto yy32;
			if (yych <= 'Z') goto yy33;
		} else {
			if (yych == '`') goto yy32;
			if (yych <= 'z') goto yy33;
		}
	}
yy32:
	++YYCURSOR;
#line 160 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"
	{ return QStringView(); }
#line 312 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
yy33:
	yych = *++YYCURSOR;
	if (yych <= '/') {
		if (yych <= '*') {
			if (yych == '#') goto yy33;
		} else {
			if (yych == ',') goto yy34;
			if (yych <= '.') goto yy33;
		}
	} else {
		if (yych <= '^') {
			if (yych <= ':') goto yy33;
			if (yych <= '@') goto yy34;
			if (yych <= 'Z') goto yy33;
		} else {
			if (yych == '`') goto yy34;
			if (yych <= 'z') goto yy33;
		}
	}
yy34:
#line 159 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"
	{ return QStringView(input.cbegin(), YYCURSOR); }
#line 335 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
}
#line 161 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"

    }

    QStringView objectiveCriteria(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        
#line 346 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
{
	YYCTYPE yych;
	yych = *YYCURSOR;
	if (yych <= '@') {
		if (yych <= '.') {
			if (yych >= '-') goto yy37;
		} else {
			if (yych <= '/') goto yy36;
			if (yych <= ':') goto yy37;
		}
	} else {
		if (yych <= '_') {
			if (yych <= 'Z') goto yy37;
			if (yych >= '_') goto yy37;
		} else {
			if (yych <= '`') goto yy36;
			if (yych <= 'z') goto yy37;
		}
	}
yy36:
	++YYCURSOR;
#line 172 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"
	{ return QStringView(); }
#line 370 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
yy37:
	yych = *++YYCURSOR;
	if (yych <= '@') {
		if (yych <= '.') {
			if (yych >= '-') goto yy37;
		} else {
			if (yych <= '/') goto yy38;
			if (yych <= ':') goto yy37;
		}
	} else {
		if (yych <= '_') {
			if (yych <= 'Z') goto yy37;
			if (yych >= '_') goto yy37;
		} else {
			if (yych <= '`') goto yy38;
			if (yych <= 'z') goto yy37;
		}
	}
yy38:
#line 171 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"
	{ return QStringView(input.cbegin(), YYCURSOR); }
#line 392 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
}
#line 173 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"

    }

    QStringView realPlayerName(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        
#line 403 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
{
	YYCTYPE yych;
	yych = *YYCURSOR;
	if (yych <= 'Z') {
		if (yych <= '/') goto yy40;
		if (yych <= '9') goto yy41;
		if (yych >= 'A') goto yy41;
	} else {
		if (yych <= '_') {
			if (yych >= '_') goto yy41;
		} else {
			if (yych <= '`') goto yy40;
			if (yych <= 'z') goto yy41;
		}
	}
yy40:
	++YYCURSOR;
#line 184 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"
	{ return QStringView(); }
#line 423 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
yy41:
	yych = *++YYCURSOR;
	if (yych <= 'Z') {
		if (yych <= '/') goto yy42;
		if (yych <= '9') goto yy41;
		if (yych >= 'A') goto yy41;
	} else {
		if (yych <= '_') {
			if (yych >= '_') goto yy41;
		} else {
			if (yych <= '`') goto yy42;
			if (yych <= 'z') goto yy41;
		}
	}
yy42:
#line 183 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"
	{ return QStringView(input.cbegin(), YYCURSOR); }
#line 441 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
}
#line 185 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"

    }

    QStringView resLocPart(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        
#line 452 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
{
	YYCTYPE yych;
	yych = *YYCURSOR;
	if (yych <= '\\') {
		if (yych <= ',') goto yy44;
		if (yych <= '9') goto yy45;
		if (yych >= '\\') goto yy45;
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
#line 196 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"
	{ return QStringView(); }
#line 472 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
yy45:
	yych = *++YYCURSOR;
	if (yych <= '\\') {
		if (yych <= ',') goto yy46;
		if (yych <= '9') goto yy45;
		if (yych >= '\\') goto yy45;
	} else {
		if (yych <= '_') {
			if (yych >= '_') goto yy45;
		} else {
			if (yych <= '`') goto yy46;
			if (yych <= 'z') goto yy45;
		}
	}
yy46:
#line 195 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"
	{ return QStringView(input.cbegin(), YYCURSOR); }
#line 490 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
}
#line 197 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"

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

        
#line 512 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
const QChar *yyt1;
const QChar *yyt2;
const QChar *yyt3;
const QChar *yyt4;
const QChar *yyt5;
#line 215 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"


        
#line 522 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
{
	YYCTYPE yych;
	yych = *YYCURSOR;
	if (yych <= '@') {
		if (yych <= '/') goto yy48;
		if (yych <= '9') {
			yyt1 = YYCURSOR;
			goto yy50;
		}
	} else {
		if (yych <= 'F') {
			yyt1 = YYCURSOR;
			goto yy50;
		}
		if (yych <= '`') goto yy48;
		if (yych <= 'f') {
			yyt1 = YYCURSOR;
			goto yy50;
		}
	}
yy48:
	++YYCURSOR;
yy49:
#line 250 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"
	{ return QStringView(); }
#line 548 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
yy50:
	yych = *(YYMARKER = ++YYCURSOR);
	if (yych <= '9') {
		if (yych == '-') goto yy51;
		if (yych <= '/') goto yy49;
		goto yy53;
	} else {
		if (yych <= 'F') {
			if (yych <= '@') goto yy49;
			goto yy53;
		} else {
			if (yych <= '`') goto yy49;
			if (yych <= 'f') goto yy53;
			goto yy49;
		}
	}
yy51:
	yych = *++YYCURSOR;
	if (yych <= '@') {
		if (yych <= '/') goto yy52;
		if (yych <= '9') {
			yyt2 = YYCURSOR;
			goto yy54;
		}
	} else {
		if (yych <= 'F') {
			yyt2 = YYCURSOR;
			goto yy54;
		}
		if (yych <= '`') goto yy52;
		if (yych <= 'f') {
			yyt2 = YYCURSOR;
			goto yy54;
		}
	}
yy52:
	YYCURSOR = YYMARKER;
	goto yy49;
yy53:
	yych = *++YYCURSOR;
	if (yych <= '9') {
		if (yych == '-') goto yy51;
		if (yych <= '/') goto yy52;
		goto yy55;
	} else {
		if (yych <= 'F') {
			if (yych <= '@') goto yy52;
			goto yy55;
		} else {
			if (yych <= '`') goto yy52;
			if (yych <= 'f') goto yy55;
			goto yy52;
		}
	}
yy54:
	yych = *++YYCURSOR;
	if (yych <= '9') {
		if (yych == '-') goto yy56;
		if (yych <= '/') goto yy52;
		goto yy57;
	} else {
		if (yych <= 'F') {
			if (yych <= '@') goto yy52;
			goto yy57;
		} else {
			if (yych <= '`') goto yy52;
			if (yych <= 'f') goto yy57;
			goto yy52;
		}
	}
yy55:
	yych = *++YYCURSOR;
	if (yych <= '9') {
		if (yych == '-') goto yy51;
		if (yych <= '/') goto yy52;
		goto yy58;
	} else {
		if (yych <= 'F') {
			if (yych <= '@') goto yy52;
			goto yy58;
		} else {
			if (yych <= '`') goto yy52;
			if (yych <= 'f') goto yy58;
			goto yy52;
		}
	}
yy56:
	yych = *++YYCURSOR;
	if (yych <= '@') {
		if (yych <= '/') goto yy52;
		if (yych <= '9') {
			yyt3 = YYCURSOR;
			goto yy59;
		}
		goto yy52;
	} else {
		if (yych <= 'F') {
			yyt3 = YYCURSOR;
			goto yy59;
		}
		if (yych <= '`') goto yy52;
		if (yych <= 'f') {
			yyt3 = YYCURSOR;
			goto yy59;
		}
		goto yy52;
	}
yy57:
	yych = *++YYCURSOR;
	if (yych <= '9') {
		if (yych == '-') goto yy56;
		if (yych <= '/') goto yy52;
		goto yy60;
	} else {
		if (yych <= 'F') {
			if (yych <= '@') goto yy52;
			goto yy60;
		} else {
			if (yych <= '`') goto yy52;
			if (yych <= 'f') goto yy60;
			goto yy52;
		}
	}
yy58:
	yych = *++YYCURSOR;
	if (yych <= '9') {
		if (yych == '-') goto yy51;
		if (yych <= '/') goto yy52;
		goto yy61;
	} else {
		if (yych <= 'F') {
			if (yych <= '@') goto yy52;
			goto yy61;
		} else {
			if (yych <= '`') goto yy52;
			if (yych <= 'f') goto yy61;
			goto yy52;
		}
	}
yy59:
	yych = *++YYCURSOR;
	if (yych <= '9') {
		if (yych == '-') goto yy62;
		if (yych <= '/') goto yy52;
		goto yy63;
	} else {
		if (yych <= 'F') {
			if (yych <= '@') goto yy52;
			goto yy63;
		} else {
			if (yych <= '`') goto yy52;
			if (yych <= 'f') goto yy63;
			goto yy52;
		}
	}
yy60:
	yych = *++YYCURSOR;
	if (yych <= '9') {
		if (yych == '-') goto yy56;
		if (yych <= '/') goto yy52;
		goto yy64;
	} else {
		if (yych <= 'F') {
			if (yych <= '@') goto yy52;
			goto yy64;
		} else {
			if (yych <= '`') goto yy52;
			if (yych <= 'f') goto yy64;
			goto yy52;
		}
	}
yy61:
	yych = *++YYCURSOR;
	if (yych <= '9') {
		if (yych == '-') goto yy51;
		if (yych <= '/') goto yy52;
		goto yy65;
	} else {
		if (yych <= 'F') {
			if (yych <= '@') goto yy52;
			goto yy65;
		} else {
			if (yych <= '`') goto yy52;
			if (yych <= 'f') goto yy65;
			goto yy52;
		}
	}
yy62:
	yych = *++YYCURSOR;
	if (yych <= '@') {
		if (yych <= '/') goto yy52;
		if (yych <= '9') {
			yyt4 = YYCURSOR;
			goto yy66;
		}
		goto yy52;
	} else {
		if (yych <= 'F') {
			yyt4 = YYCURSOR;
			goto yy66;
		}
		if (yych <= '`') goto yy52;
		if (yych <= 'f') {
			yyt4 = YYCURSOR;
			goto yy66;
		}
		goto yy52;
	}
yy63:
	yych = *++YYCURSOR;
	if (yych <= '9') {
		if (yych == '-') goto yy62;
		if (yych <= '/') goto yy52;
		goto yy67;
	} else {
		if (yych <= 'F') {
			if (yych <= '@') goto yy52;
			goto yy67;
		} else {
			if (yych <= '`') goto yy52;
			if (yych <= 'f') goto yy67;
			goto yy52;
		}
	}
yy64:
	yych = *++YYCURSOR;
	if (yych == '-') goto yy56;
	goto yy52;
yy65:
	yych = *++YYCURSOR;
	if (yych <= '9') {
		if (yych == '-') goto yy51;
		if (yych <= '/') goto yy52;
		goto yy68;
	} else {
		if (yych <= 'F') {
			if (yych <= '@') goto yy52;
			goto yy68;
		} else {
			if (yych <= '`') goto yy52;
			if (yych <= 'f') goto yy68;
			goto yy52;
		}
	}
yy66:
	yych = *++YYCURSOR;
	if (yych <= '9') {
		if (yych == '-') goto yy69;
		if (yych <= '/') goto yy52;
		goto yy70;
	} else {
		if (yych <= 'F') {
			if (yych <= '@') goto yy52;
			goto yy70;
		} else {
			if (yych <= '`') goto yy52;
			if (yych <= 'f') goto yy70;
			goto yy52;
		}
	}
yy67:
	yych = *++YYCURSOR;
	if (yych <= '9') {
		if (yych == '-') goto yy62;
		if (yych <= '/') goto yy52;
		goto yy71;
	} else {
		if (yych <= 'F') {
			if (yych <= '@') goto yy52;
			goto yy71;
		} else {
			if (yych <= '`') goto yy52;
			if (yych <= 'f') goto yy71;
			goto yy52;
		}
	}
yy68:
	yych = *++YYCURSOR;
	if (yych <= '9') {
		if (yych == '-') goto yy51;
		if (yych <= '/') goto yy52;
		goto yy72;
	} else {
		if (yych <= 'F') {
			if (yych <= '@') goto yy52;
			goto yy72;
		} else {
			if (yych <= '`') goto yy52;
			if (yych <= 'f') goto yy72;
			goto yy52;
		}
	}
yy69:
	yych = *++YYCURSOR;
	if (yych <= '@') {
		if (yych <= '/') goto yy52;
		if (yych <= '9') {
			yyt5 = YYCURSOR;
			goto yy73;
		}
		goto yy52;
	} else {
		if (yych <= 'F') {
			yyt5 = YYCURSOR;
			goto yy73;
		}
		if (yych <= '`') goto yy52;
		if (yych <= 'f') {
			yyt5 = YYCURSOR;
			goto yy73;
		}
		goto yy52;
	}
yy70:
	yych = *++YYCURSOR;
	if (yych <= '9') {
		if (yych == '-') goto yy69;
		if (yych <= '/') goto yy52;
		goto yy75;
	} else {
		if (yych <= 'F') {
			if (yych <= '@') goto yy52;
			goto yy75;
		} else {
			if (yych <= '`') goto yy52;
			if (yych <= 'f') goto yy75;
			goto yy52;
		}
	}
yy71:
	yych = *++YYCURSOR;
	if (yych == '-') goto yy62;
	goto yy52;
yy72:
	yych = *++YYCURSOR;
	if (yych == '-') goto yy51;
	goto yy52;
yy73:
	yych = *++YYCURSOR;
	if (yych <= '@') {
		if (yych <= '/') goto yy74;
		if (yych <= '9') goto yy76;
	} else {
		if (yych <= 'F') goto yy76;
		if (yych <= '`') goto yy74;
		if (yych <= 'f') goto yy76;
	}
yy74:
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
#line 223 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"
	{
                bool              ok;
                constexpr static uint8_t last2HexDigitMask = 0xff;

                const auto l             = strToUHex<uint32_t>(GET_PART(1), ok);
                const auto w1            = strToUHex<uint16_t>(GET_PART(2), ok);
                const auto w2            = strToUHex<uint16_t>(GET_PART(3), ok);
                const auto clock_seq_low = strToUHex<uint16_t>(GET_PART(4), ok);

                const uint8_t b1   = clock_seq_low >> 8;
                const uint8_t b2   = clock_seq_low & last2HexDigitMask;
                auto          node = strToUHex<uint64_t>(GET_PART(5), ok);
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
#line 935 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_generated_functions.cpp"
yy75:
	yych = *++YYCURSOR;
	if (yych <= '9') {
		if (yych == '-') goto yy69;
		if (yych <= '/') goto yy52;
		goto yy77;
	} else {
		if (yych <= 'F') {
			if (yych <= '@') goto yy52;
			goto yy77;
		} else {
			if (yych <= '`') goto yy52;
			if (yych <= 'f') goto yy77;
			goto yy52;
		}
	}
yy76:
	yych = *++YYCURSOR;
	if (yych <= '@') {
		if (yych <= '/') goto yy74;
		if (yych <= '9') goto yy78;
		goto yy74;
	} else {
		if (yych <= 'F') goto yy78;
		if (yych <= '`') goto yy74;
		if (yych <= 'f') goto yy78;
		goto yy74;
	}
yy77:
	yych = *++YYCURSOR;
	if (yych == '-') goto yy69;
	goto yy52;
yy78:
	yych = *++YYCURSOR;
	if (yych <= '@') {
		if (yych <= '/') goto yy74;
		if (yych >= ':') goto yy74;
	} else {
		if (yych <= 'F') goto yy79;
		if (yych <= '`') goto yy74;
		if (yych >= 'g') goto yy74;
	}
yy79:
	yych = *++YYCURSOR;
	if (yych <= '@') {
		if (yych <= '/') goto yy74;
		if (yych >= ':') goto yy74;
	} else {
		if (yych <= 'F') goto yy80;
		if (yych <= '`') goto yy74;
		if (yych >= 'g') goto yy74;
	}
yy80:
	yych = *++YYCURSOR;
	if (yych <= '@') {
		if (yych <= '/') goto yy74;
		if (yych >= ':') goto yy74;
	} else {
		if (yych <= 'F') goto yy81;
		if (yych <= '`') goto yy74;
		if (yych >= 'g') goto yy74;
	}
yy81:
	yych = *++YYCURSOR;
	if (yych <= '@') {
		if (yych <= '/') goto yy74;
		if (yych >= ':') goto yy74;
	} else {
		if (yych <= 'F') goto yy82;
		if (yych <= '`') goto yy74;
		if (yych >= 'g') goto yy74;
	}
yy82:
	yych = *++YYCURSOR;
	if (yych <= '@') {
		if (yych <= '/') goto yy74;
		if (yych >= ':') goto yy74;
	} else {
		if (yych <= 'F') goto yy83;
		if (yych <= '`') goto yy74;
		if (yych >= 'g') goto yy74;
	}
yy83:
	yych = *++YYCURSOR;
	if (yych <= '@') {
		if (yych <= '/') goto yy74;
		if (yych >= ':') goto yy74;
	} else {
		if (yych <= 'F') goto yy84;
		if (yych <= '`') goto yy74;
		if (yych >= 'g') goto yy74;
	}
yy84:
	yych = *++YYCURSOR;
	if (yych <= '@') {
		if (yych <= '/') goto yy74;
		if (yych >= ':') goto yy74;
	} else {
		if (yych <= 'F') goto yy85;
		if (yych <= '`') goto yy74;
		if (yych >= 'g') goto yy74;
	}
yy85:
	yych = *++YYCURSOR;
	if (yych <= '@') {
		if (yych <= '/') goto yy74;
		if (yych >= ':') goto yy74;
	} else {
		if (yych <= 'F') goto yy86;
		if (yych <= '`') goto yy74;
		if (yych >= 'g') goto yy74;
	}
yy86:
	yych = *++YYCURSOR;
	if (yych <= '@') {
		if (yych <= '/') goto yy74;
		if (yych >= ':') goto yy74;
	} else {
		if (yych <= 'F') goto yy87;
		if (yych <= '`') goto yy74;
		if (yych >= 'g') goto yy74;
	}
yy87:
	++YYCURSOR;
	goto yy74;
}
#line 251 "D:\\C++\\Qt\\MCDatapacker\\src\\parsers\\command\\re2c_functions.re"


    #undef GET_PART
    }
}


#undef YYCTYPE