#pragma once

#include "ctypes.h"
#include "Namable.h"

#include <memory>

#ifdef __linux__
#include <tr1/memory> // for std::tr1::shared_ptr
#endif

// Generic game resource, inherit from this to allow objects to be managed by the ResourceManager
// Used for meshes, shaders, textures and render targets. To allow the object to be more descriptive
// (especially when debugging), the object is also namable
class Resource : public Namable
{
private:
	i32 id; // resource ID (usually set from resourcemanager). This can be used to delete an object from the resource manager by passing the object resource id
public:
	Resource() : id(-1) {}; // invalidate id by default
	Resource(int _id) : id(_id) {};
	virtual ~Resource() {}; // virtual destructor so things inheriting Resource can have own destructor

	// Accessors and Mutators
	void SetResourceID(i32 _id) { id = _id; };
	i32 GetResourceID() const { return id; };

	// All Resources forced to overload this function
	// This function should be added to the destructor of each Resource
	virtual void Unload()=0;
};

// Generic resource handle (shared pointer to resource)
// In order to simplify the code for resources created, use the following
// typedef format to create a nice *Handle type. E.g. ShaderHandle, TextureHandle
typedef std::tr1::shared_ptr<Resource> ResourceHandle;