#pragma once

#include "ievent.h"

namespace flounder 
{
	template<typename t>
	class eventchange : 
		public ievent
	{
	private:
		t *m_reference;
		t m_current;
		std::function<void()> m_onEvent;
	public:
		/// <summary>
		/// Creates a new change event.
		/// </summary>
		/// <param name="reference"> The reference to listen to. </param>
		/// <param name="onEvent"> A function called when the event is triggered. </param>
		eventchange(t *reference, const std::function<void()> &onEvent);

		bool eventTriggered() override;

		void onEvent() override;

		inline bool removeAfterEvent() override { return false; }
	};
}