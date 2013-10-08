#ifndef __HEIRARCHICALSTORAGEATTRIBUTE_H__
#define __HEIRARCHICALSTORAGEATTRIBUTE_H__
#include "HeirarchicalStorageInterface/HeirarchicalStorageInterface.pkg"
#include "Stream/Stream.pkg"

class HeirarchicalStorageAttribute :public IHeirarchicalStorageAttribute
{
public:
	//Constructors
	HeirarchicalStorageAttribute();
	explicit HeirarchicalStorageAttribute(const std::wstring& aname);

	//Name functions
	inline std::wstring GetName() const;
	inline void SetName(const std::wstring& aname);

protected:
	//Name functions
	virtual void GetNameInternal(const InteropSupport::ISTLObjectTarget<std::wstring>& marshaller) const;
	virtual void SetNameInternal(const InteropSupport::ISTLObjectSource<std::wstring>& marshaller);

	//Stream functions
	virtual void ResetInternalStreamPosition() const;
	virtual void EmptyInternalStream();
	virtual Stream::IStream& GetInternalStream() const;

private:
	std::wstring name;
	mutable Stream::Buffer buffer;
};

#include "HeirarchicalStorageAttribute.inl"
#endif
