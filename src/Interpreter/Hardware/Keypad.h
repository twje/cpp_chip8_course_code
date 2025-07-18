#pragma once

//  Includes
//--------------------------------------------------------------------------------
// Interpreter
#include "Interfaces/IKeyInputProvider.h"

// System
#include <optional>
#include <array>
#include <cassert>

//--------------------------------------------------------------------------------
class Keypad
{
public:
    enum class Key : uint8_t
    {
        Key0, Key1, Key2, Key3,
        Key4, Key5, Key6, Key7,
        Key8, Key9, KeyA, KeyB,
        KeyC, KeyD, KeyE, KeyF,
    };

    explicit Keypad()
        : mKeyStates{ }
        , mKeyMapping{ }
		, mInputProvider(nullptr)
    { }

    void SetInputProvider(std::unique_ptr<IKeyInputProvider> inputProvider)
    {
        mInputProvider = std::move(inputProvider);
    }

    void SetKeyBinding(Key key, uint8_t physicalKey)
    {
        mKeyMapping[KeyToIndex(key)] = physicalKey;
    }

    uint8_t GetKeyBinding(Key key) const
    {
        return mKeyMapping[KeyToIndex(key)];
    }

    void PollKeypad()
    {
        for (uint8_t i = 0; i < mKeyStates.size(); ++i)
        {
            const Key key = IndexToKey(i);
            const uint8_t physicalKey = mKeyMapping[i];            
            const bool isHeld = mInputProvider->IsKeyPressed(physicalKey);

            SetKeyPressed(key, isHeld);
        }
    }

    void SetKeyPressed(Key key, bool isPressed)
    {
        mKeyStates[KeyToIndex(key)] = isPressed;
    }

    bool IsKeyPressed(Key key) const
    {
        return mKeyStates[KeyToIndex(key)];
    }

    std::optional<Keypad::Key> GetFirstKeyPressed() const
    {
        for (uint8_t i = 0; i < mKeyStates.size(); ++i)
        {
            if (mKeyStates[i])
            {
                return IndexToKey(i);
            }
        }
        return std::nullopt;
    }

    static Key IndexToKey(uint8_t index)
    {
        assert(index < 16 && "Index must be less than 16 for Keypad keys.");
        return static_cast<Key>(index);
    }

    static uint8_t KeyToIndex(Key key)
    {
        return static_cast<uint8_t>(key);
    }

    static std::string_view GetLabelForKey(Key key)
    {
        switch (key)
        {
            case Key::Key0: return "0";
            case Key::Key1: return "1";
            case Key::Key2: return "2";
            case Key::Key3: return "3";
            case Key::Key4: return "4";
            case Key::Key5: return "5";
            case Key::Key6: return "6";
            case Key::Key7: return "7";
            case Key::Key8: return "8";
            case Key::Key9: return "9";
            case Key::KeyA: return "A";
            case Key::KeyB: return "B";
            case Key::KeyC: return "C";
            case Key::KeyD: return "D";
            case Key::KeyE: return "E";
            case Key::KeyF: return "F";
            default: return "?";
        }
    }

private:
    std::array<bool, 16> mKeyStates;
    std::array<uint8_t, 16> mKeyMapping;    
    std::unique_ptr<IKeyInputProvider> mInputProvider;
};