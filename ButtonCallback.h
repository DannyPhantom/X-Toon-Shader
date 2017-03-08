/*
 * ButtonCallback.h
 *
 *  Created on: 2016-10-29
 *      Author: dannyphantom
 */

#ifndef BUTTONCALLBACK_H_
#define BUTTONCALLBACK_H_

class Button;

/**
 * Interface that represents a button callback.
 * Whenever a button is pressed or released, this
 * interface methods will be called
 */
class ButtonCallback {
public:
	ButtonCallback() {}
	virtual ~ButtonCallback() {}

	virtual void onButtonClick(Button *button) = 0;
	virtual void onButtonRelease(Button *button) = 0;
};

#endif /* BUTTONCALLBACK_H_ */
