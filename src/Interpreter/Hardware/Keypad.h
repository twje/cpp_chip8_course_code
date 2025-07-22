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
class Key
{
public:
    static constexpr uint8_t Key0 = 0;
    static constexpr uint8_t Key1 = 1;
    static constexpr uint8_t Key2 = 2;
    static constexpr uint8_t Key3 = 3;
    static constexpr uint8_t Key4 = 4;
    static constexpr uint8_t Key5 = 5;
    static constexpr uint8_t Key6 = 6;
    static constexpr uint8_t Key7 = 7;
    static constexpr uint8_t Key8 = 8;
    static constexpr uint8_t Key9 = 9;
    static constexpr uint8_t KeyA = 10;
    static constexpr uint8_t KeyB = 11;
    static constexpr uint8_t KeyC = 12;
    static constexpr uint8_t KeyD = 13;
    static constexpr uint8_t KeyE = 14;
    static constexpr uint8_t KeyF = 15;

    static constexpr uint8_t Count = 16;

    explicit Key(uint8_t key)
        : mKey(key)
    {
        assert(key >= 0 && key < Count && "Key must be in range [0, 15]");
	}

    uint8_t GetValue() const { return mKey; }
    std::string_view GetLabel() const { return GetLabel(mKey);}

private:
    static std::string_view GetLabel(int key)
    {
        static constexpr std::string_view labels[Count] = {
            "0", "1", "2", "3", "4", "5", "6", "7",
            "8", "9", "A", "B", "C", "D", "E", "F"
        };

        return (key >= 0 && key < Count) ? labels[key] : "?";
    }

    uint8_t mKey;
};

//--------------------------------------------------------------------------------
class Keypad
{
public:
    explicit Keypad()
        : mCurrKeyStates{}
        , mPrevKeyStates{}
        , mKeyMapping{}
        , mInputProvider(nullptr)
    { }

    void SetInputProvider(std::unique_ptr<IKeyInputProvider> inputProvider)
    {
        mInputProvider = std::move(inputProvider);
    }

    void SetKeyBinding(Key key, uint8_t physicalKey)
    {
        mKeyMapping[key.GetValue()] = physicalKey;
    }

    void PollKeypad()
    {
        mPrevKeyStates = mCurrKeyStates;

        for (uint8_t i = 0; i < mCurrKeyStates.size(); ++i)
        {            
            const uint8_t physicalKey = mKeyMapping[i];
            const bool isHeld = mInputProvider->IsKeyPressed(physicalKey);

            mCurrKeyStates[i] = isHeld;
        }
    }

    bool IsKeyPressed(Key key) const
    {
        return mCurrKeyStates[key.GetValue()];
    }    

    std::optional<Key> GetFirstKeyReleased() const
    {
        for (uint8_t i = 0; i < Key::Count; ++i)
        {
            if (mPrevKeyStates[i] && !mCurrKeyStates[i])
            {
                return Key(i);
            }
        }
        return std::nullopt;
    }

    void SetKeyPressed(Key key, bool isPressed)
    {
        uint8_t index = key.GetValue();
        mPrevKeyStates[index] = mCurrKeyStates[index];
        mCurrKeyStates[index] = isPressed;
    }
   
private:
    std::array<bool, 16> mCurrKeyStates;
    std::array<bool, 16> mPrevKeyStates;
    std::array<uint8_t, 16> mKeyMapping;
    std::unique_ptr<IKeyInputProvider> mInputProvider;
};