/**
 A set of variables that records mouse activity
*/

#ifndef _INPUTSTATE_H_
#define _INPUTSTATE_H_

class InputState
{
public:
	InputState(): lMousePressed(false), 
                  rMousePressed(false),
    	    	  prevX(0), prevY(0),
	    	      deltaX(0), deltaY(0) {}

	bool lMousePressed;
	bool rMousePressed;

	int prevX;
	int prevY;
	int deltaX;
	int deltaY;
};

#endif // _INPUTSTATE_H_
