#pragma once

#include "Utils/StreamFactory.hpp"
#include "Devices/Window.hpp"
#include "InputAxis.hpp"

namespace acid {
/**
 * @brief Interface for a binary input device.
 */
class ACID_EXPORT InputButton : public StreamFactory<InputButton>, public virtual Observer {
public:
	virtual ~InputButton() = default;

	/**
	 * Returns whether this button is currently pressed.
	 * @return True if the button is pressed, false otherwise.
	 */
	virtual bool IsDown() const { return false; }

	/**
	 * Gets if the key is down and was not down before. Key press recognized as one click.
	 * @return Is the key down and was not down before?
	 */
	bool WasDown() {
		auto stillDown = wasDown && IsDown();
		wasDown = IsDown();
		return wasDown == !stillDown;
	}

	virtual InputAxis::ArgumentDescription GetArgumentDescription() const { return {}; }

	/**
	 * Called when the button changes state.
	 * @return The delegate.
	 */
	Delegate<void(InputAction, BitMask<InputMod>)> &OnButton() { return onButton; }

	bool IsInverted() const { return inverted; }
	void SetInverted(bool inverted) { this->inverted = inverted; }

protected:
	Delegate<void(InputAction, BitMask<InputMod>)> onButton;
	bool inverted = false;

private:
	bool wasDown = false;
};
}
