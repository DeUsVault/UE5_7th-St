// Fill out your copyright notice in the Description page of Project Settings.


#include "TrafficPathControllerDetails.h"
#include "CivilFXCore/TrafficSim/TrafficPath.h"
#include "CivilFXCore/TrafficSim/TrafficPathController.h"
#include "PropertyEditing.h"
#include "PropertyEditorModule.h"
#include "PropertyCustomizationHelpers.h" //FDetailArrayBuilder

#include "Framework/MultiBox/MultiBoxBuilder.h"

#include "Widgets/Layout/SWidgetSwitcher.h"
#include "GameFramework/Actor.h"

#include "IPropertyTypeCustomization.h" //IPropertyTypeCustomizationUtils
#include "IPropertyUtilities.h" //IPropertyUtilities

#include "CivilFXToolbarEdStyle.h"

#include "Modules/ModuleManager.h"
#include "LevelEditor.h"
#include "IAssetViewport.h"
#include "EditorViewportClient.h"

#include "ScopedTransaction.h"

#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Input/SButton.h"

#define LOCTEXT_NAMESPACE "FRampCustomization"

TSharedRef<IPropertyTypeCustomization> FRampCustomization::MakeInstance()
{
	return MakeShareable(new FRampCustomization());
}

void FRampCustomization::CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle,
	FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	const bool bDisplayResetToDefault = false;
	FText DisplayNameOverride;
	const FText DisplayToolTipOverride = FText::GetEmpty();

	TSharedPtr<IPropertyHandle> ActorHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRamp, Actor));

	AActor* Actor = GetAssetFromProperty<AActor>(ActorHandle);
	if (Actor)
	{
		DisplayNameOverride = FText::FromString(Actor->GetName());
	}
	else
	{
		DisplayNameOverride = FText::FromString(TEXT("<Null>"));
	}
	
	//display just the header name of this struct (ramp)
	HeaderRow
		.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget(DisplayNameOverride, DisplayToolTipOverride, bDisplayResetToDefault)
		]
	.ValueContent()
		.MinDesiredWidth(1)
		.MaxDesiredWidth(4096)
		[
			StructPropertyHandle->CreatePropertyValueWidget(false)
		];
}


void FRampCustomization::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle,
	IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	this->StructUtils = StructCustomizationUtils.GetPropertyUtilities();

	TSharedPtr<IPropertyHandle> ActorHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRamp, Actor));
	TSharedPtr<IPropertyHandle> PathControllerHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRamp, NewPathController));
	TSharedPtr<IPropertyHandle> ColorHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRamp, Color));
	TSharedPtr<IPropertyHandle> TypeHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRamp, Type));
	TSharedPtr<IPropertyHandle> ModeHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRamp, Mode));
	TSharedPtr<IPropertyHandle> DirectionHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRamp, Direction));
	TSharedPtr<IPropertyHandle> UMinNewPathHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRamp, CurvePosOnNewPath));
	TSharedPtr<IPropertyHandle> UMinHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRamp, CurvePosMin));
	TSharedPtr<IPropertyHandle> UMaxHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRamp, CurvePosMax));
	TSharedPtr<IPropertyHandle> FromLanesHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRamp, FromLanes));
	TSharedPtr<IPropertyHandle> ToLanesHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRamp, ToLanes));

	//force a complete redraw if user select a new actor
	FSimpleDelegate OnActorSelectionChangedDelegate = FSimpleDelegate::CreateSP(this, &FRampCustomization::OnActorSelectionChanged, ActorHandle, PathControllerHandle);
	ActorHandle->SetOnPropertyValueChanged(OnActorSelectionChangedDelegate);


	StructBuilder.AddProperty(ActorHandle.ToSharedRef());
	//StructBuilder.AddProperty(PathControllerHandle.ToSharedRef()).IsEnabled(false).ShowPropertyButtons(false);

	AActor* Actor = GetAssetFromProperty<AActor>(ActorHandle);
	
	if (Actor == nullptr)
	{
		//return;
	}
	
	//EVisibility ShouldBeVisible = Actor == nullptr ? EVisibility::Hidden : EVisibility::Visible;

	StructBuilder.AddProperty(ColorHandle.ToSharedRef());

	TArray<UObject*> Outers;
	StructPropertyHandle->GetOuterObjects(Outers);
	UTrafficPathController* ThisPathController = Cast<UTrafficPathController>(Outers[0]);
	UTrafficPathController* NewPathController = PathControllerHandle.IsValid() ? GetAssetFromProperty<UTrafficPathController>(PathControllerHandle) : nullptr;
	uint8 ThisLanesCount = ThisPathController->GetTrafficPath()->GetLanesCount();
	uint8 NewLanesCount = NewPathController == nullptr ? 100 : NewPathController->GetTrafficPath()->GetLanesCount();

	float ThisPathCurveLength = ThisPathController->GetTrafficPath()->GetPathLength();

	//whenever switching to different type, just set the max value to max
	FSimpleDelegate OnTypeValueChangedDelegate = FSimpleDelegate::CreateSP(this, &FRampCustomization::OnValueChanged, ThisPathCurveLength, UMaxHandle);
	TypeHandle->SetOnPropertyValueChanged(OnTypeValueChangedDelegate);

	StructBuilder.AddProperty(TypeHandle.ToSharedRef());
	StructBuilder.AddProperty(ModeHandle.ToSharedRef());


	TAttribute<EVisibility> DirectionPropVisibilityAttr = TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FRampCustomization::GetGenericPropertyVisibility, ModeHandle, false));
	StructBuilder.AddProperty(DirectionHandle.ToSharedRef()).Visibility(DirectionPropVisibilityAttr);
	
	/************/
	TSharedPtr<IPropertyHandleArray> FromLanesArray = FromLanesHandle->AsArray();
	TSharedPtr<IPropertyHandleArray> ToLanesArray = ToLanesHandle->AsArray();
	TAttribute<EVisibility> FromToLanesPropVisibilityAttr = TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FRampCustomization::GetGenericPropertyVisibility, ModeHandle, true));
	StructBuilder.AddCustomRow(FText::GetEmpty())
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.Padding(FMargin(5.0f, 5.0f, 5.0f, 0.f))
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.MaxWidth(80.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("From Lanes Label", "From Lanes"))
				.Font(StructCustomizationUtils.GetRegularFont())
			]

			+SHorizontalBox::Slot()
			.MaxWidth(25.0f)
			.Padding(FMargin(20.f, 0.0f, 80.0f, 0.0f))
			[
				SNew(SButton)		
				.OnClicked(FOnClicked::CreateSP(this, &FRampCustomization::OnAddLane, FromLanesArray, ToLanesArray, ThisLanesCount, NewLanesCount))
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Text(LOCTEXT("Add Lanes", "+"))
			]

			+ SHorizontalBox::Slot()
			.MaxWidth(80.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("To Lanes Label", "To Lanes"))
				.Font(StructCustomizationUtils.GetRegularFont())
			]
		]
	]
	.Visibility(FromToLanesPropVisibilityAttr);

	/**draw from to lanes*/
	uint32 FromToLanesNumChildren;
	FromLanesArray->GetNumElements(FromToLanesNumChildren);

	//first make sure we always have at lease 1 element;
	if (FromToLanesNumChildren == 0)
	{
		FromLanesArray->AddItem();
		ToLanesArray->AddItem();
		++FromToLanesNumChildren;
	}

	//auto set/clamp values for arrays
	GLog->Log(FString::Printf(TEXT("%d <-> %d"), ThisLanesCount, NewLanesCount));

	TSharedRef<IPropertyHandle> FromLanesFirstElementHandle = FromLanesArray->GetElement(0);
	uint8 FromLanesFirstElementValue;
	FromLanesFirstElementHandle->GetValue(FromLanesFirstElementValue);
	FromLanesFirstElementValue = FMath::Clamp(FromLanesFirstElementValue, (uint8)0, (uint8)(ThisLanesCount - FromToLanesNumChildren));
	FromLanesFirstElementHandle->SetValue(FromLanesFirstElementValue);

	TSharedRef<IPropertyHandle> ToLanesFirstElementHandle = ToLanesArray->GetElement(0);
	uint8 ToLanesFirstElementValue;
	ToLanesFirstElementHandle->GetValue(ToLanesFirstElementValue);
	ToLanesFirstElementValue = FMath::Clamp(ToLanesFirstElementValue, (uint8)0, (uint8)(NewLanesCount - FromToLanesNumChildren));
	ToLanesFirstElementHandle->SetValue(ToLanesFirstElementValue);

	for (uint8 i = 1; i < FromToLanesNumChildren; ++i)
	{
		TSharedRef<IPropertyHandle> FromLanesElementHandle = FromLanesArray->GetElement(i);
		check(FromLanesElementHandle->SetValue((uint8)(FromLanesFirstElementValue + i)) == FPropertyAccess::Success);

		TSharedRef<IPropertyHandle> ToLanesElementHandle = ToLanesArray->GetElement(i);
		ToLanesElementHandle->SetValue((uint8)(ToLanesFirstElementValue + i));
	}

	StructBuilder.AddCustomRow(FText::GetEmpty())
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(FMargin(10.0f, 0.0f, 10.0f, 0.0f))
		.MaxWidth(80.f)
		[
			SNew(SNumericEntryBox<uint8>)
			.Value(this, &FRampCustomization::GetLaneValue, FromLanesFirstElementHandle)
			.OnValueCommitted(this, &FRampCustomization::OnLaneValueCommitted, FromLanesFirstElementHandle, ThisLanesCount, FromToLanesNumChildren)
			.MinValue(0)
			.MaxValue(100) //arbitrary limit
		]
		+ SHorizontalBox::Slot()
		.Padding(FMargin(120.f, 0.0f, 10.0f, 0.0f))
		.MaxWidth(80.f)
		[
			SNew(SNumericEntryBox<uint8>)
			.Value(this, &FRampCustomization::GetLaneValue, ToLanesFirstElementHandle)
			.OnValueCommitted(this, &FRampCustomization::OnLaneValueCommitted, ToLanesFirstElementHandle, NewLanesCount, FromToLanesNumChildren)
			.MinValue(0)
			.MaxValue(100) //arbitrary limit
		]
	]
	.Visibility(FromToLanesPropVisibilityAttr);
	
	//draw the rest of the elements
	for (uint32 i = 1; i < FromToLanesNumChildren; ++i)
	{
		TSharedRef<IPropertyHandle> FromLanesElementHandle = FromLanesArray->GetElement(i);
		TSharedRef<IPropertyHandle> ToLanesElementHandle = ToLanesArray->GetElement(i);

		uint8 FromLanesElementValue, ToLanesElementValue;
		FromLanesElementHandle->GetValue(FromLanesElementValue);
		ToLanesElementHandle->GetValue(ToLanesElementValue);

		//We use SEditableTextBox here to make it look like the entry box.
		//but it won't be editable
		StructBuilder.AddCustomRow(FText::GetEmpty())
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.Padding(FMargin(10.0f, 0.0f, 10.0f, 0.0f))
			.MaxWidth(80.f)
			[
				SNew(SEditableTextBox)
				.Text(FText::FromString(FString::FromInt(FromLanesElementValue)))
				.IsEnabled(false)
			]

			+ SHorizontalBox::Slot()
			.Padding(FMargin(120.f, 0.0f, 10.0f, 0.0f))
			.MaxWidth(80.f)
			[
				SNew(SEditableTextBox)
				.Text(FText::FromString(FString::FromInt(ToLanesElementValue)))
				.IsEnabled(false)
			]

			//delete button
			+ SHorizontalBox::Slot()
			.MaxWidth(25.f)
			[
				SNew(SButton)
				.OnClicked(FOnClicked::CreateSP(this, &FRampCustomization::OnDeleteLane, FromLanesArray, ToLanesArray, (int32)i))
				.Text(LOCTEXT("Delete from to lanes", "-"))
			]
		]
		.Visibility(FromToLanesPropVisibilityAttr);
	}
	
	/*************/
	StructBuilder.AddProperty(UMinNewPathHandle.ToSharedRef()).IsEnabled(false);
	if (GetEnumValueFromProperty<ERampType>(TypeHandle) == ERampType::Merge)
	{
		UMaxHandle->SetValue(ThisPathCurveLength);
	}

	StructBuilder.AddCustomRow(FText::GetEmpty())
	.NameContent()
	[
		UMaxHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	.MinDesiredWidth(400.f)
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.FillWidth(1)
		[
			SNew(SSlider)
			.Value(this, &FRampCustomization::OnGetValue, UMaxHandle)
			.OnValueChanged(this, &FRampCustomization::OnValueChanged, UMaxHandle)
			.MinValue(0)
			.MaxValue(ThisPathCurveLength)	
			.IsEnabled_Lambda([TypeHandle]()
			{
				return GetEnumValueFromProperty<ERampType>(TypeHandle) == ERampType::Diverge;
			})			
		]
		+SHorizontalBox::Slot()
		.MaxWidth(50.f)
		.Padding(5)
		[
			SNew(SEditableTextBox)

			.Text(this, &FRampCustomization::GetTextValue<float>, UMaxHandle)
			.IsEnabled(false)
		]

		+SHorizontalBox::Slot()
		.MaxWidth(20.f)
		.Padding(2.5)
		.HAlign(HAlign_Center)
		[	
			SNew(SButton)
			.ContentPadding(0)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked_Lambda([ThisPathController, UMaxHandle]()
			{
				float CurvePos;
				UMaxHandle->GetValue(CurvePos);
				TSharedPtr<FSplineBuilder> Spline = ThisPathController->GetTrafficPath()->GetSplineBuilder();

				FVector Position = Spline->GetPointOnPathSegment(CurvePos);
				FVector Direction = Spline->GetTangentOnPathSegment(CurvePos);

				FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
				TSharedPtr< IAssetViewport > ViewportWindow = LevelEditorModule.GetFirstActiveViewport();
				if (ViewportWindow.IsValid())
				{
					FEditorViewportClient& Viewport = ViewportWindow->GetAssetViewportClient();
					//Viewport.SetViewLocationForOrbiting(Position, 4096.0f);		
					Viewport.FocusViewportOnBox(FBox::BuildAABB(Position, FVector::OneVector * 1500.f));
				}

				return FReply::Handled();
			})
			.Content()
			[
				SNew(SImage)	
				.Image_Lambda([]()
				{
					return FCivilFXToolbarEdStyle::Get().GetBrush("TrafficSimEd.FocusIcon");
				})
				
			]
		]
	];
	///

	/** ************************************ */
	/*
	StructBuilder.AddCustomRow(FText::GetEmpty())
		.NameContent()
		[
			SNew(STextBlock)
			.Text(FText::GetEmpty())
		]
	.ValueContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("AdvancedLabel", "Advanced"))
		];
	*/
	//draw connect button
	auto OnConnectButtonDelegate = [](TSharedRef<IPropertyHandle> StructPropertyHandle)
	{
		GLog->Log(FString::Printf(TEXT("Button clicked: %d"), StructPropertyHandle->GetIndexInArray()));

		//get the
		TArray<UObject*> Outers;
		StructPropertyHandle->GetOuterObjects(Outers);

		UTrafficPathController* ThisPathController = Cast<UTrafficPathController>(Outers[0]);

		if (ThisPathController)
		{
			GLog->Log(ThisPathController->GetOwner()->GetName());
		}
		else
		{
			GLog->Log("Failed to get outer");
		}

		return FReply::Handled();
	};
	StructBuilder.AddCustomRow(FText::GetEmpty())
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(5)
		.HAlign(HAlign_Center)
		[
			SNew(SButton)
			.OnClicked(FOnClicked::CreateLambda(OnConnectButtonDelegate, StructPropertyHandle))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.Text(LOCTEXT("Connect to this ramp", "Connect"))
		]
	];
}

void FRampCustomization::OnActorSelectionChanged(TSharedPtr<IPropertyHandle> ActorHandle, 
	TSharedPtr<IPropertyHandle> PathControllerHandle)
{
	AActor* Actor = GetAssetFromProperty<AActor>(ActorHandle);
	if (Actor)
	{
		UTrafficPathController* PathController = Cast<UTrafficPathController>(Actor->GetComponentByClass(UTrafficPathController::StaticClass()));
		if (PathController)
		{
			
			FScopedTransaction Transaction(LOCTEXT("SetPathController", "Set PathController"));
			PathControllerHandle->NotifyPreChange();

			TArray<UObject*> Objects;
			PathControllerHandle->GetOuterObjects(Objects);
			for (UObject* Obj : Objects)
			{
				Obj->Modify();
			}
			TArray<void*> RawData;
			PathControllerHandle->AccessRawData(RawData);
			for (void* RawPtr : RawData)
			{
				(*(UTrafficPathController**)RawPtr) = PathController;
			}
			PathControllerHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
			PathControllerHandle->NotifyFinishedChangingProperties();
			
			//check(PathControllerHandle->SetValue(PathController) == FPropertyAccess::Success);
		}
		else
		{
			PathControllerHandle->ResetToDefault();
			//this handle needs to be called last
			//because there is a delegate called to this
			ActorHandle->ResetToDefault();
		}
	}
	StructUtils->ForceRefresh();
}

EVisibility FRampCustomization::GetGenericPropertyVisibility(TSharedPtr<IPropertyHandle> ModeHandle, bool bReverse)
{
	uint8 ModeAsInt;
	ModeHandle->GetValue(ModeAsInt);
	bool bModeAuto = (ERampMode)ModeAsInt == ERampMode::Auto;
	bool bVisible = bReverse ? !bModeAuto : bModeAuto;
	return bVisible ? EVisibility::Visible : EVisibility::Hidden;
}

FReply FRampCustomization::OnAddLane(TSharedPtr<IPropertyHandleArray> FromLanesArray, TSharedPtr<IPropertyHandleArray> ToLanesArray, 
	uint8 ThisLanesCount, uint8 NewLanesCount)
{
	uint32 FromLanesNumChildren, ToLanesNumChildren;
	check(FromLanesArray->GetNumElements(FromLanesNumChildren) == FPropertyAccess::Success);
	check(ToLanesArray->GetNumElements(ToLanesNumChildren) == FPropertyAccess::Success);

	if (FromLanesNumChildren < ThisLanesCount && ToLanesNumChildren < NewLanesCount)
	{
		FromLanesArray->AddItem();
		ToLanesArray->AddItem();
		StructUtils->ForceRefresh();
	}
	return FReply::Handled();
}

FReply FRampCustomization::OnDeleteLane(TSharedPtr<IPropertyHandleArray> FromLanesArray, TSharedPtr<IPropertyHandleArray> ToLanesArray, int32 ElementIndex)
{
	FromLanesArray->DeleteItem(ElementIndex);
	ToLanesArray->DeleteItem(ElementIndex);
	StructUtils->ForceRefresh();
	return FReply::Handled();
}

TOptional<uint8> FRampCustomization::GetLaneValue(TSharedRef<IPropertyHandle> LaneHandle) const
{
	uint8 Value;
	check(LaneHandle->GetValue(Value) == FPropertyAccess::Success);
	return TOptional<uint8>(Value);
}

void FRampCustomization::OnLaneValueCommitted(uint8 NewValue, ETextCommit::Type/*unused*/, TSharedRef<IPropertyHandle> LaneHandle, uint8 LanesCount, uint32 ChildrenNum)
{
	NewValue = FMath::Clamp(NewValue, (uint8)0, (uint8)(LanesCount - ChildrenNum));
	GLog->Log(FString::Printf(TEXT("New Value: %d"), NewValue));
	check(LaneHandle->SetValue(NewValue) == FPropertyAccess::Success);
	StructUtils->ForceRefresh();

}

#undef LOCTEXT_NAMESPACE




/**
 * This class detail is current NOT registered, so you can ignore it
 */
#define LOCTEXT_NAMESPACE "TrafficPathControllerDetails"

TSharedRef<IDetailCustomization> FTrafficPathControllerDetails::MakeInstance()
{
	return MakeShareable(new FTrafficPathControllerDetails);
}

void FTrafficPathControllerDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	GLog->Log("Custom..");

	IDetailCategoryBuilder& EditingCategory = DetailBuilder.EditCategory(TEXT("Ramp Settings Custom"));

	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailBuilder.GetObjectsBeingCustomized(Objects);

	//disable multiple editing
	if (Objects.Num() > 1)
	{
		return;
	}
	UTrafficPathController* EditingPathController = Cast<UTrafficPathController>(Objects[0].Get());
	/*
	TSharedPtr<IPropertyHandle> RampsProp = DetailBuilder.GetProperty(TEXT("Ramps"));

	//Add builder for children to handle array changes
	TSharedRef<FDetailArrayBuilder> RampsBuilder = MakeShareable(new FDetailArrayBuilder(RampsProp.ToSharedRef()));
	RampsBuilder->OnGenerateArrayElementWidget(FOnGenerateArrayElementWidget::CreateSP(this, &FTrafficPathControllerDetails::OnGenerateRampChild));

	//Add array to category
	EditingCategory.AddCustomBuilder(RampsBuilder);
	*/
}

void FTrafficPathControllerDetails::OnGenerateRampChild(TSharedRef<IPropertyHandle> StructPropertyHandle, int32 ElementIndex, IDetailChildrenBuilder& ChildrenBuilder)
{
	GLog->Log(FString::FromInt(ElementIndex));

	IDetailPropertyRow& PropertyArrayRow = ChildrenBuilder.AddProperty(StructPropertyHandle);

	TSharedPtr<IPropertyHandle> ActorHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRamp, Actor));
	//Get name of the element 
	//Name of element will be that of the selected Actor
	//or <null> is non selected
	UObject* ObjectRef = nullptr;
	ActorHandle->GetValue(ObjectRef);
	AActor* Actor = Cast<AActor>(ObjectRef);
	FText DisplayNameOverride;
	if (Actor)
	{
		UTrafficPath* Path = Cast<UTrafficPath>(Actor->GetComponentByClass(UTrafficPath::StaticClass()));
		UTrafficPathController* PathController = Cast<UTrafficPathController>(Actor->GetComponentByClass(UTrafficPathController::StaticClass()));
		if (Path && PathController)
		{
			DisplayNameOverride = FText::FromString(Actor->GetName());
		}
		else
		{
			//ActorHandle->ResetToDefault();
			DisplayNameOverride = FText::FromString(TEXT("<Null>"));
		}
	}
	else
	{
		DisplayNameOverride = FText::FromString(TEXT("<Null>"));
	}

	//Setting header
	PropertyArrayRow.CustomWidget()
		.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget(DisplayNameOverride, FText::GetEmpty(), false)
		]
	.ValueContent()
		.MinDesiredWidth(1)
		.MaxDesiredWidth(4096)
		[
			StructPropertyHandle->CreatePropertyValueWidget(false)
			/*
			SNew(STextBlock)
			.Text(FText::FromString("a"))
			*/

		];
	//Setting property for child of struct

}


#undef LOCTEXT_NAMESPACE