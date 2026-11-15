#pragma once

#include "Delegates/Delegate.h"
#include "Widgets/SCompoundWidget.h"

class SButton;

class SVampsPauseMenu : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SVampsPauseMenu)
	{
	}
	SLATE_EVENT(FSimpleDelegate, OnResume)
	SLATE_EVENT(FSimpleDelegate, OnQuit)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	TSharedRef<SWidget> GetResumeButton() const;

private:

	virtual FReply OnPreviewKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FNavigationReply OnNavigation(const FGeometry& MyGeometry,
	                                      const FNavigationEvent& InNavigationEvent) override;

	FReply ResumeFromInput();
	FReply QuitFromInput();

	TSharedPtr<SButton> ResumeButton;
	TSharedPtr<SButton> QuitButton;
	FSimpleDelegate ResumeAction;
	FSimpleDelegate QuitAction;
};
