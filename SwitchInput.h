/* @file	SwitchInput.h
 * @date	2021-02-06
 * @author	TK303
 */

#if !defined(_SWITCHINPUT_H_)
#define SWITCHINPUT_H_

#include <stdint.h>

/* user defined switch configuration */
#include "SwitchInput_conf.h"


/*************** CONSTANTS ******************************/

#define CAHTTER_REDUCTION_STAGE		(3)		/* number of counts for determining input aquisition (upto 8) */


/*************** DEFINITIONS ******************************/

/**/
typedef enum {
   INPUT_OFF,	/* input none  */
   INPUT_ON,	/* exist input */
} PortInput_t;


/* protorype of stub function for input port */
typedef PortInput_t (*PortInputStub_t)(int index);

/* protorype of callback to notify function for switch input */
typedef void (*SwitchInputNotifier_t)(int index, PortInput_t input);


/* (use internal) for input status management */
typedef struct tag_SwitchInputAttr_t
{
   tag_SwitchInputAttr_t   *next;
   int index;
   uint8_t                 input_buffer;
   PortInputStub_t         port_input_stub;
   SwitchInputNotifier_t   switch_input_notifier;
} SwitchInputAttr_t;


/*************** CLASSES ******************************/
class SwitchInput
{
public:
   /* intialize SwitchInput module */
   static void Init();

   /* register attribute for switch inupt */
   static bool Register(int index, PortInputStub_t port_input_stub, SwitchInputNotifier_t switch_input_notifier);

   /* interval polling for switch input */
   static void PollingPort(void);

private:
   /* root of list for management input state */
   static SwitchInputAttr_t *root;

   /* attribute for every switch input state */
   static SwitchInputAttr_t attr[NUM_INPUT_PORTS];

   /* for forbidden to call constructor and destructor, operator = */
   SwitchInput();                               /* default constructor */
   SwitchInput(const SwitchInput&);             /* copy constructor */
   SwitchInput& operator=(const SwitchInput&);  /* copy operator */
   ~SwitchInput();                              /* destructor */
};


#endif /* _SWITCHINPUT_H_ */
