#include "CivilFXToolbarEdStyle.h"

// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr< FSlateStyleSet > FCivilFXToolbarEdStyle::StyleInstance = NULL;

void FCivilFXToolbarEdStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FCivilFXToolbarEdStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FCivilFXToolbarEdStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("CivilFXToolbarEdStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef< FSlateStyleSet > FCivilFXToolbarEdStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("CivilFXToolbarEdStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("CivilFXCore")->GetBaseDir() / TEXT("Resources"));

	Style->Set("CivilFXToolbarEdStyle.CivilFXLogo", new IMAGE_BRUSH(TEXT("CFXLogo"), Icon40x40));
	Style->Set("TrafficSimEd.FocusIcon", new IMAGE_BRUSH(TEXT("Focus64"), Icon16x16));

	Style->Set("CivilFXEdMode.CameraIcon", new IMAGE_BRUSH(TEXT("Camera128"), Icon40x40));
	Style->Set("CivilFXEdMode.CameraIcon.Small", new IMAGE_BRUSH(TEXT("Camera128"), Icon16x16));

	Style->Set("CivilFXEdMode.TreeIcon", new IMAGE_BRUSH(TEXT("Tree128"), Icon40x40));
	Style->Set("CivilFXEdMode.TreeIcon.Small", new IMAGE_BRUSH(TEXT("Tree128"), Icon16x16));


	Style->Set("CivilFXEdMode.StillCameraIcon.Small", new IMAGE_BRUSH(TEXT("StillCamera128"), Icon40x40));
	Style->Set("CivilFXEdMode.AnimatedCameraIcon.Small", new IMAGE_BRUSH(TEXT("AnimatedCamera128"), Icon40x40));



	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FCivilFXToolbarEdStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FCivilFXToolbarEdStyle::Get()
{
	return *StyleInstance;
}
