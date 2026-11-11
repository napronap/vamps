#include "UI/VampsCombatTestHUD.h"
#include "UI/VampsHUDWidget.h"
#include "Combat/VampsCombatLog.h"
#include "GameFramework/PlayerController.h"

void AVampsCombatTestHUD::BeginPlay()
{
	Super::BeginPlay();
	if (!PlayerOwner || !PlayerOwner->IsLocalController())
	{
		return;
	}
	if (!HUDWidgetClass)
	{
		UE_LOG(LogVampsCombat, Error, TEXT("%hs: '%s' requires HUDWidgetClass."), __FUNCTION__, *GetPathName());
		return;
	}
	HUDWidget = CreateWidget<UVampsHUDWidget>(PlayerOwner, HUDWidgetClass);
	if (!HUDWidget)
	{
		UE_LOG(LogVampsCombat,
		       Error,
		       TEXT("%hs: '%s' could not create '%s'."),
		       __FUNCTION__,
		       *GetPathName(),
		       *GetPathNameSafe(HUDWidgetClass));
		return;
	}
	HUDWidget->AddToViewport();
}

void AVampsCombatTestHUD::EndPlay(const EEndPlayReason::Type Reason)
{
	if (HUDWidget)
	{
		HUDWidget->RemoveFromParent();
	}
	Super::EndPlay(Reason);
}
