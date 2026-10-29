#include "UI/VampsEnemyWidget.h"
#include "Characters/VampsCharacter.h"
#include "Characters/VampsLockOnComponent.h"
#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"

void UVampsEnemyWidget::UpdateEnemy(
    const AVampsCharacter* Viewer, const AVampsCharacter* Enemy, int32 EnemyIndex, int32 VisibleRow, bool bSelected)
{
	const FVampsCombatState& EnemyState = Enemy->CombatState;
	const bool bIncoming =
	    Viewer->CombatState.bIsAlive && Enemy->LockOn && Enemy->LockOn->GetCurrentTarget() == Viewer &&
	    (EnemyState.Phase == EVampsActionPhase::Preparation || EnemyState.Phase == EVampsActionPhase::Contacts);
	const FVector Offset = Enemy->GetActorLocation() - Viewer->GetActorLocation();
	const FRotationMatrix CameraAxes(FRotator(0.f, GetOwningPlayer()->GetControlRotation().Yaw, 0.f));
	const FVector2D Direction = FVector2D(FVector::DotProduct(Offset, CameraAxes.GetUnitAxis(EAxis::Y)),
	                                      -FVector::DotProduct(Offset, CameraAxes.GetUnitAxis(EAxis::X)))
	                                .GetSafeNormal();
	const float Angle = FMath::RadiansToDegrees(FMath::Atan2(Direction.X, -Direction.Y));
	const FLinearColor RowColor =
	    bIncoming ? FLinearColor::Red : (bSelected ? FLinearColor::Yellow : FLinearColor::White);
	EnemyRow->SetVisibility(VisibleRow >= 0 ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	Cast<UCanvasPanelSlot>(EnemyRow->Slot)->SetPosition(FVector2D(-320.f, 224.f + VisibleRow * 26.f));
	const double DistanceMeters = Offset.Size2D() / 100.0;
	EnemyText->SetText(FText::FromString(FString::Printf(TEXT("%s %d | %.1fm%s%s"),
	                                                     bSelected ? TEXT(">") : TEXT(" "),
	                                                     EnemyIndex + 1,
	                                                     DistanceMeters,
	                                                     DistanceMeters <= 15.0 ? TEXT(" NEAR") : TEXT(""),
	                                                     bIncoming ? TEXT(" ! INCOMING") : TEXT(""))));
	EnemyText->SetColorAndOpacity(RowColor);
	DirectionArrow->SetContentColorAndOpacity(RowColor);
	DirectionArrow->SetRenderTransformAngle(Angle);
	DirectionArrow->SetVisibility(Direction.IsNearlyZero() ? ESlateVisibility::Collapsed
	                                                       : ESlateVisibility::HitTestInvisible);
	ThreatMarker->SetVisibility(bIncoming && !Direction.IsNearlyZero() ? ESlateVisibility::HitTestInvisible
	                                                                   : ESlateVisibility::Collapsed);
	Cast<UCanvasPanelSlot>(ThreatMarker->Slot)->SetPosition(Direction * 100.f);
	ThreatArrow->SetRenderTransformAngle(Angle);
}
