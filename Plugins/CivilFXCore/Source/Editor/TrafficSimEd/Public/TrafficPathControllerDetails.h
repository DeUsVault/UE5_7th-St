// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "IDetailCustomization.h"


//for struct
class TRAFFICSIMED_API FRampCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

private:

	//TSharedRef<class IPropertyHandle> StructHandle;
	void OnActorSelectionChanged(TSharedPtr<IPropertyHandle> ActorHandle, TSharedPtr<IPropertyHandle> PathControllerHandle);

	template<typename T>
	void OnValueChanged(T NewValue, TSharedPtr<IPropertyHandle> Handle)
	{
		check(Handle->SetValue(NewValue) == FPropertyAccess::Success);
	}

	template<typename T>
	void OnValueCommitted(T NewValue, TSharedPtr<IPropertyHandle> Handle)
	{
		OnValueChanged(NewValue, Handle);
	}

	template<typename T>
	T OnGetValue(TSharedPtr<IPropertyHandle> Handle) const
	{
		T Value;
		check(Handle->GetValue(Value) == FPropertyAccess::Success);
		return Value;
	}

	template<typename T>
	FText GetTextValue(TSharedPtr<IPropertyHandle> Handle) const
	{
		T Value = OnGetValue<T>(Handle);
#define LOCTEXT_NAMESPACE "GetValueAsText"
		return FText::Format(LOCTEXT("GetTextValueFmt", "{0}"), Value);
#undef LOCTEXT_NAMESPACE
	}

	EVisibility GetGenericPropertyVisibility(TSharedPtr<IPropertyHandle> ModeHandle, bool bReverse);

	FReply OnAddLane(TSharedPtr<IPropertyHandleArray> FromLanesArray, TSharedPtr<IPropertyHandleArray> ToLanesArray, uint8 ThisLanesCount, uint8 NewLanesCount);
	FReply OnDeleteLane(TSharedPtr<IPropertyHandleArray> FromLanesArray, TSharedPtr<IPropertyHandleArray> ToLanesArray, int32 ElementIndex);
	TOptional<uint8> GetLaneValue(TSharedRef<IPropertyHandle> LaneHandle) const;
	void OnLaneValueCommitted(uint8 NewValue, ETextCommit::Type/*unused*/, TSharedRef<IPropertyHandle> LaneHandle, uint8 LanesCount, uint32 ChildrenNum);

	template<typename T>
	static T* GetAssetFromProperty(TSharedPtr<IPropertyHandle> PropertyHandle)
	{
		UObject* ObjectRef = nullptr;
		check(PropertyHandle->GetValue(ObjectRef) == FPropertyAccess::Success);
		return Cast<T>(ObjectRef);
	}

	template<typename T>
	static T GetEnumValueFromProperty(TSharedPtr<IPropertyHandle> PropertyHandle)
	{
		uint8 EnumAsInt;
		//If this assert is triggered, make sure your enum is of type uint8
		check(PropertyHandle->GetValue(EnumAsInt) == FPropertyAccess::Success);
		return (T)EnumAsInt;
	}

	TSharedPtr<class IPropertyUtilities> StructUtils;
};

/**
 * This class detail is current NOT registered, so you can ignore it
 */
class TRAFFICSIMED_API FTrafficPathControllerDetails : public IDetailCustomization
{
public:
	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	void OnGenerateRampChild(TSharedRef<IPropertyHandle> StructPropertyHandle, int32 ElementIndex, IDetailChildrenBuilder& ChildrenBuilder);
	//

	static TSharedRef< IDetailCustomization > MakeInstance();

private:
	template<typename T>
	static T* GetAssetFromProperty(TSharedPtr<IPropertyHandle> PropertyHandle)
	{
		UObject* ObjectRef = nullptr;
		PropertyHandle->GetValue(ObjectRef);
		return Cast<T>(ObjectRef);
	}
};
