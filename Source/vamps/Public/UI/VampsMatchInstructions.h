#pragma once

#include "Delegates/Delegate.h"
#include "Widgets/SCompoundWidget.h"

class SBorder;
class SButton;
class STextBlock;

class SVampsMatchInstructions : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SVampsMatchInstructions)
	{
	}
	SLATE_EVENT(FSimpleDelegate, OnQuit)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	void Close();

private:

	virtual FReply OnPreviewKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FNavigationReply OnNavigation(const FGeometry& MyGeometry,
	                                      const FNavigationEvent& InNavigationEvent) override;

	TSharedRef<SWidget> MakeControlRow(const TCHAR* Key, const TCHAR* Japanese, const TCHAR* English);
	TSharedRef<SWidget> MakeInstructions();
	static TSharedRef<STextBlock> MakeText(TAttribute<FText> Text, const FName& Weight, int32 Size);
	TAttribute<FText> LocalizedText(const TCHAR* Japanese, const TCHAR* English);
	FText SelectLanguage(FText Japanese, FText English) const;
	FReply OpenFromInput();
	FReply CloseFromInput();
	FReply ToggleLanguage();

	TSharedPtr<SButton> OpenButton;
	TSharedPtr<SButton> LanguageButton;
	TSharedPtr<SButton> CloseButton;
	TSharedPtr<SBorder> ModalBackdrop;
	bool bEnglish = false;
};
