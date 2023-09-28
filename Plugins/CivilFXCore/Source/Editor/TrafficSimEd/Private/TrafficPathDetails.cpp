// Fill out your copyright notice in the Description page of Project Settings.


#include "TrafficPathDetails.h"
#include "CivilFXCore/TrafficSim/TrafficPath.h"
#include "PropertyEditing.h"
#include "PropertyEditorModule.h"

#define LOCTEXT_NAMESPACE "TrafficPathDetails"

void FTrafficPathDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory(TEXT("TrafficPath Actions"));
	
	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailBuilder.GetObjectsBeingCustomized(Objects);

	TWeakObjectPtr<UTrafficPath> TrafficPathComp = Cast<UTrafficPath>(Objects[0].Get());
	auto OnRegenerate = [TrafficPathComp]
	{
		if (TrafficPathComp.IsValid())
		{
			TrafficPathComp->ReverseNodes();
		}
		return FReply::Handled();
	};

	Category.AddCustomRow(LOCTEXT("111", "222"))
		.WholeRowContent()
		[
			SNew(SButton)
			.Text(LOCTEXT("Reversing", "Reverse Nodes"))
			.OnClicked_Lambda(OnRegenerate)

		];
}

TSharedRef<IDetailCustomization> FTrafficPathDetails::MakeInstance()
{
	return MakeShareable(new FTrafficPathDetails);
}

#undef LOCTEXT_NAMESPACE