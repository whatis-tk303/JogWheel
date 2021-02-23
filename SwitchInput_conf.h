/* @file	SwitchInput_conf.h
 * @date	2021-02-06
 * @author	TK303
 */

#if !defined(_SWITCHINPUT_CONF_H_)
#define _SWITCHINPUT_CONF_H_


/*************** CONSTANTS ******************************/

/* user defined button ID List */
enum {
	BTN_IDX_BLUE,
	BTN_IDX_ORANGE,
	BTN_IDX_PINK,
	BTN_IDX_YELLOW,
	BTN_IDX_RED,
	BTN_IDX_GREEN,

	/* !!! DON'T CHANGE !!!  place NUM_INPUT_PORTS at last of enum list */
	NUM_INPUT_PORTS		/* (number of input ports) */
};


#endif /* _SWITCHINPUT_CONF_H_ */
