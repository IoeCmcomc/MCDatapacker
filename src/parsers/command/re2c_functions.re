/*!re2c
    re2c:yyfill:enable = 0;
    re2c:encoding:utf16 = 1;
    re2c:header = re2c_generated_functions.h;
 */
/*!header:re2c:on*/
#ifndef RE2C_GENERATED_FUNCTIONS_H
#define RE2C_GENERATED_FUNCTIONS_H

#include <QStringView>
#include <QUuid>

namespace re2c {
    template < typename T >
    T strToUHex(QStringView v, bool &ok) {
        static_assert(std::is_unsigned < T > ::value,
                      "T must be an unsigned type");

        T value = 0;

        ok = false;
        for (int i = 0; i < v.length(); ++i) {
            const char ch = v[i].toLatin1();
            if (value > std::numeric_limits < T > ::max() / 16) {
                ok = false;
                return 0;
            }
            value *= 16;
            uchar digit = 0;
            switch (ch) {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9': {
                    digit = ch - '0';
                    ok    = true;
                    break;
                }
                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f': {
                    digit = 10 + (ch - 'a');
                    ok    = true;
                    break;
                }
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F': {
                    digit = 10 + (ch - 'A');
                    ok    = true;
                    break;
                }
                default: {
                    ok = false;
                    return 0;
                }
            }

            if (value > (std::numeric_limits < T > ::max() - digit)) {
                ok = false;
                return 0;
            } else {
                value += digit;
            }
        }
        return value;
    }

    QStringView decimal(QStringView input);
    QStringView snbtNumber(QStringView input);
    QStringView itemSlot(QStringView input);
    QStringView itemSlots(QStringView input);
    QStringView nbtPathKey(QStringView input);
    QStringView resLocPart(QStringView input);
    QStringView objective(QStringView input);
    QStringView objectiveCriteria(QStringView input);
    QStringView realPlayerName(QStringView input);
    QStringView uuid(QStringView input, QUuid &result);
}

#endif // RE2C_GENERATED_FUNCTIONS_H
/*!header:re2c:off*/

#include "re2c_generated_functions.h"

#define YYCTYPE    QChar

/*!rules:re2c:decimal
    d = [0-9];
    decimal = ( [+-]? ( d+ '.' d+ | '.' d+ | d+ '.' | d+ ) );
 */

namespace re2c {
    QStringView decimal(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        /*!use:re2c:decimal
            decimal { return QStringView(input.cbegin(), YYCURSOR); }
         *       { return QStringView(); }
         */
    }

    QStringView snbtNumber(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        /*!local:re2c
            !use:decimal;
            number = decimal ( [eE] [-+]? d+ )?;

            number { return QStringView(input.cbegin(), YYCURSOR); }
         *       { return QStringView(); }
         */
    }

    QStringView itemSlot(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        /*!local:re2c
            id = [a-z0-9._]+;

            id { return QStringView(input.cbegin(), YYCURSOR); }
         *       { return QStringView(); }
         */
    }

    QStringView itemSlots(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        /*!local:re2c
            id = [a-z0-9._*]+;

            id { return QStringView(input.cbegin(), YYCURSOR); }
         *       { return QStringView(); }
         */
    }

    QStringView nbtPathKey(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        /*!local:re2c
            key = [^ "[\].{}\x00-\x1f\x7f]+;

            key { return QStringView(input.cbegin(), YYCURSOR); }
         *       { return QStringView(); }
         */
    }

    QStringView objective(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        /*!local:re2c
            id = [0-9a-zA-Z-+_.:#]+;

            id { return QStringView(input.cbegin(), YYCURSOR); }
         *       { return QStringView(); }
         */
    }

    QStringView objectiveCriteria(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        /*!local:re2c
            id = [0-9a-zA-Z-_.:]+;

            id { return QStringView(input.cbegin(), YYCURSOR); }
         *       { return QStringView(); }
         */
    }

    QStringView realPlayerName(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        /*!local:re2c
            id = [0-9a-zA-Z_]+;

            id { return QStringView(input.cbegin(), YYCURSOR); }
         *       { return QStringView(); }
         */
    }

    QStringView resLocPart(QStringView input) {
        const QChar *YYCURSOR = input.cbegin();
        const QChar *YYMARKER = nullptr;

        /*!local:re2c
            id = [0-9a-z-_\\/.]+;

            id { return QStringView(input.cbegin(), YYCURSOR); }
         *       { return QStringView(); }
         */
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

        /*!stags:re2c format = 'const QChar *@@;\n'; */

        /*!local:re2c
            re2c:tags = 1;

            hex = [0-9a-fA-F];
            uuid = @_p1 hex+ @p1_ '-' @_p2 hex+ @p2_ '-'  @_p3 hex+ @p3_ '-' @_p4 hex+ @p4_ '-' @_p5 hex+ @p5_;

            uuid {
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
         *      { return QStringView(); }
         */

    #undef GET_PART
    }
}


#undef YYCTYPE
