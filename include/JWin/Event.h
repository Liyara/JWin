#ifndef EVENT_H
#define EVENT_H

namespace jwin {
    struct Event {
        enum Key {
            UNKNOWN = -1,
            SPACE = 0x20,
            APOS = 0x27,
            COMMA = 0x2C,
            MINUS,
            PERIOD,
            SLASH,
            ZERO,
            ONE,
            TWO,
            THREE,
            FOUR,
            FIVE,
            SIX,
            SEVEN,
            EIGHT,
            NINE,
            SEMICOLON = 0x3B,
            EQUAL = 0x3D,
            A = 0x41,
            B,
            C,
            D,
            E,
            F,
            G,
            H,
            I,
            J,
            K,
            L,
            M,
            N,
            O,
            P,
            Q,
            R,
            S,
            T,
            U,
            V,
            W,
            X,
            Y,
            Z,
            BRACKET_OPEN,
            BACKSLASH,
            BRACKET_CLOSE,
            GRAVE  = 0x60,
            ESCAPE = 0x100,
            ENTER,
            TAB,
            BACKSPACE,
            INSERT,
            DELETE,
            ARROW_RIGHT,
            ARROW_LEFT,
            ARROW_DOWN,
            ARROW_UP,
            PAGE_UP,
            PAGE_DOWN,
            HOME,
            END,
            CAPS,
            SCROLL,
            NUM,
            PRINT,
            PAUSE,
            F1 = 0x122,
            F2,
            F3,
            F4,
            F5,
            F6,
            F7,
            F8,
            F9,
            F10,
            F11,
            F12,
            F13,
            F14,
            F15,
            F16,
            F17,
            F18,
            F19,
            F20,
            F21,
            F22,
            F23,
            F24,
            F25,
            LEFT_SHIFT = 0x154,
            LEFT_CONTROL,
            LEFT_ALT,
            LEFT_SUPER,
            RIGHT_SHIFT,
            RIGHT_CONTROL,
            RIGHT_ALT,
            RIGHT_SUPER,

            __KEY_COUNT__
        };

        enum Button {
            LEFT,
            RIGHT,
            MIDDLE,
            SPECIAL
        };
        enum Action {
            NONE = -1,
            UP,
            DOWN,
            REPEAT
        };
        enum Mod {
            SHIFT = 0x01,
            CONTROL = 0x02,
            ALT = 0x04,
            SUPER = 0x08,
            FUNCTION = 0x10
        };
        enum Type {
            NO_TYPE,
            KEY,
            MOUSE,
            MOUSE_SCROLL,
            CURSOR,
            RESIZE,
            CLOSE,
            MOVE,
            GAIN_FOCUS,
            LOSE_FOCUS,
            CURSOR_ENTER,
            CURSOR_LEAVE,
            EXPOSE
        };
        double x, y;
        int width, height;
        int code;
        Action action;
        int mods;
        unsigned long time;
        Type type;
    };
}

#endif // EVENT_H
