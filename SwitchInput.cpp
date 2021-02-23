/* @file	SwitchInput.cpp
 * @date	2021-02-06
 * @author	TK303
 *
 * <example>
 *	  int pin_stat;
 *	  pin_stat = digitalRead(PORT);
 *	  input = (pin_stat == LOW) ? 1 : 0;
 *
 */

#include <cstdlib>	/* for NULL */
#include "SwitchInput.h"

/* instance of static menber variables */
SwitchInputAttr_t * SwitchInput::root;
SwitchInputAttr_t SwitchInput::attr[NUM_INPUT_PORTS];


/* the bit-mask for continuously same input counts */
#define MASK_CONT_INPUT_COUNT		((1 << CAHTTER_REDUCTION_STAGE) - 1)


/********************************************************************************
 * @brief	intialize SwitchInput module
 */
void SwitchInput::Init()
{
	static SwitchInputAttr_t empty =
	{
		.next = NULL,
		.index = NUM_INPUT_PORTS,
		.input_buffer = 0,
		.port_input_stub = NULL,
		.switch_input_notifier = NULL
	};

	for (int i = 0; i < NUM_INPUT_PORTS; i++)
	{
		attr[i] = empty;
	}

	root = &empty;
}


/********************************************************************************
 * @brief	register switch input attribute
 * @param [in]	index - index of switch (user defined)
 * @param [in]	port_input_stub       - stub function of GPIO input
 * @param [in]	switch_input_notifier - notifier function for switch input
 */
bool SwitchInput::Register(int index, PortInputStub_t port_input_stub, SwitchInputNotifier_t switch_input_notifier)
{
	if (NUM_INPUT_PORTS <= index)
	{
		return false;	/* wrong index ! */
	}

	/* search last of list with reject same index */
	SwitchInputAttr_t *cur = root;
	SwitchInputAttr_t *prev = NULL;
	while(cur != NULL)
	{
		if (cur->index == index)
		{
			return false;	/* already registerd this index ! */
		}

		prev = cur;
		cur = cur->next;
	}

	/* register new item at last of list */
	if (prev != NULL)
	{
		cur = &attr[index];
		prev->next = cur;
		cur->next = NULL;
		cur->index = index;
		cur->input_buffer = 0;
		cur->port_input_stub = port_input_stub;
		cur->switch_input_notifier = switch_input_notifier;

		return true;
	}
	else
	{
		return false;	/* wrong list ! */
	}
}


/********************************************************************************
 * @brief	interval polling for switch input
 *				- to reduce chattering, updating input status
 *				- call this with interval constantly and continuously
 */
void SwitchInput::PollingPort(void)
{
	PortInput_t raw_input;
	uint8_t input;

	/* input from raw ports and notify input state reduced chattering */
	//for (int index = 0; index < NUM_INPUT_PORTS; index++)

	SwitchInputAttr_t *cur = root->next;
	while(cur != NULL)
	{
		/* input from raw port */
		uint8_t prev_input_buffer = cur->input_buffer;
		raw_input = cur->port_input_stub(cur->index);
		input = raw_input == INPUT_ON ? 1 : 0;
		cur->input_buffer = (cur->input_buffer << 1) | input;

		/* issue event if changed input state */
		bool changed_input_state = ( 0 != ((prev_input_buffer ^ cur->input_buffer) & MASK_CONT_INPUT_COUNT));

		/* confirm if same input counts are the number of stages continuously */
		if (changed_input_state)
		{
			uint8_t input_bits = cur->input_buffer & MASK_CONT_INPUT_COUNT;
			
			if (input_bits == MASK_CONT_INPUT_COUNT)
			{ /* ON input is determined and notify */
				cur->switch_input_notifier(cur->index, INPUT_ON);
			}
			else if (input_bits == 0)
			{ /* OFF input is determined and notify */
				cur->switch_input_notifier(cur->index, INPUT_OFF);
			}
			else
			{
				/* (no updating) */
			}
		}

		cur = cur->next;
	}
}
