// mssao demo

#pragma once

#include "deferred_demo.h"
#include "mssao_kit.h"

class mssao_demo : public deferred_demo {
public:
	mssao_demo();
	virtual ~mssao_demo();
	
	virtual bool init();
protected:
	virtual void draw();
	virtual void process_keys(const float factor);

	//members
	mssao_kit ao_kit;

};