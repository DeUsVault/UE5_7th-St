
#pragma once

#include "CoreMinimal.h"
#include "EdMode.h"

class FCivilFXEdMode : public FEdMode
{
public:
	const static FEditorModeID EM_CivilFXEdModeEdModeId;
public:
	FCivilFXEdMode();
	virtual ~FCivilFXEdMode();

	// FEdMode interface
	virtual void Enter() override;
	virtual void Exit() override;
	//virtual void Tick(FEditorViewportClient* ViewportClient, float DeltaTime) override;
	//virtual void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) override;
	//virtual void ActorSelectionChangeNotify() override;
	bool UsesToolkits() const override;
	// End of FEdMode interface

	TSharedRef<FUICommandList> GetUICommandList() const;
};
