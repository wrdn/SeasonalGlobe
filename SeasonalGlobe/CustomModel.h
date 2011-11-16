#pragma once

#include "Model.h"

class CustomModel
{
private:
	Model _model;

public:
	void Draw() { _model.Draw(); };
	Model& GetModel() const { return (Model&)_model; };
};