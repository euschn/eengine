// cgse depth of field demo
// author: Eugen Jiresch
// 2012

#pragma once

#include "deferred_demo.h"
#include "fbo.h"

class dof_demo : public deferred_demo {
public:

	virtual bool init();

protected:
	virtual void draw();
	virtual void process_keys(const float factor);

	//dof fbos
	fbo blur_fbo[2]; //fbo for blurring
	fbo coc_fbo;	 //fbo to hold the coc
	fbo scene_fbo;   //fbo for the (not post-processed) scene
};